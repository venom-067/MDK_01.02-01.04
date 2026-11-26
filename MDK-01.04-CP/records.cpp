#include <windows.h>
#include <iostream>
#include <string>
#include <limits>

bool isValidRecordText(const std::string& text) {
    if (text.size() != 8) {
        return false;
    }
    for (char ch : text) {
        if (ch < 'A' || ch > 'Z') {
            return false;
        }
    }
    return true;
}

int main() {
    int record = 0;
    std::string newData;

    std::cout << "Введите номер записи (1 или 2): ";
    if (!(std::cin >> record) || (record != 1 && record != 2)) {
        std::cerr << "Ошибка ввода: номер записи должен быть 1 или 2." << std::endl;
        return 1;
    }

    std::cout << "Введите строку из 8 заглавных латинских букв: ";
    std::cin >> newData;
    if (!isValidRecordText(newData)) {
        std::cerr << "Ошибка ввода: строка должна содержать ровно 8 заглавных латинских букв (A-Z)." << std::endl;
        return 1;
    }

    HANDLE file = CreateFileW(
        L"records.txt",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (file == INVALID_HANDLE_VALUE) {
        std::cerr << "Ошибка открытия файла CreateFileW. Код ошибки: " << GetLastError() << std::endl;
        return 1;
    }

    const DWORD offset = static_cast<DWORD>((record - 1) * 8);
    OVERLAPPED region{};
    region.Offset = offset;

    const BOOL locked = LockFileEx(
        file,
        LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY,
        0,
        8,
        0,
        &region
    );

    if (!locked) {
        const DWORD err = GetLastError();
        std::cerr << "Не удалось заблокировать запись " << record
                  << ". Код ошибки: " << err << std::endl;
        CloseHandle(file);
        return 1;
    }

    std::cout << "Запись " << record << ": LOCKED" << std::endl;
    std::cout << "Нажмите Enter для выполнения записи и снятия блокировки...";
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    std::cin.get();

    bool writeSuccess = true;
    LARGE_INTEGER movePos{};
    movePos.QuadPart = offset;

    if (!SetFilePointerEx(file, movePos, nullptr, FILE_BEGIN)) {
        std::cerr << "Ошибка SetFilePointerEx. Код ошибки: " << GetLastError() << std::endl;
        writeSuccess = false;
    } else {
        DWORD bytesWritten = 0;
        const BOOL written = WriteFile(
            file,
            newData.data(),
            8,
            &bytesWritten,
            nullptr
        );

        if (!written || bytesWritten != 8) {
            std::cerr << "Ошибка WriteFile. Код ошибки: " << GetLastError() << std::endl;
            writeSuccess = false;
        }
    }

    OVERLAPPED unlockRegion{};
    unlockRegion.Offset = offset;
    if (!UnlockFileEx(file, 0, 8, 0, &unlockRegion)) {
        std::cerr << "Ошибка UnlockFileEx. Код ошибки: " << GetLastError() << std::endl;
        CloseHandle(file);
        return 1;
    }

    CloseHandle(file);

    if (!writeSuccess) {
        return 1;
    }

    std::cout << "Запись " << record << " успешно обновлена, блокировка снята." << std::endl;
    return 0;
}