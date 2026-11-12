#include <windows.h>
#include <iostream>

int main() {
    const wchar_t* path = L"share_demo.txt";

    // Экземпляр Б: GENERIC_WRITE + FILE_SHARE_READ
    HANDLE file = CreateFileW(path, GENERIC_WRITE,
        FILE_SHARE_READ, nullptr, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        std::cout << "Process B: Open failed. Code: " << GetLastError() << '\n';
        std::cout << "Press Enter to exit...\n";
        std::cin.get();
        return 1;
    }

    std::cout << "Process B: File opened for WRITE. Press Enter to close...\n";
    std::cin.get();

    if (!CloseHandle(file)) {
        std::cout << "Close failed. Code: " << GetLastError() << '\n';
        return 1;
    }

    std::cout << "Closed.\n";
    return 0;
}
