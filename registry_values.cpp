#include <windows.h>
#include <iostream>

int wmain() {
    HKEY key = nullptr;
    const wchar_t* subKey = L"Environment"; // HKCU\Environment

    if (RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &key) != ERROR_SUCCESS) {
        std::wcout << L"Subkey not found.\n";
        return 1;
    }

    std::wcout << L"Values from HKCU\\" << subKey << L":\n\n";

    for (DWORD i = 0;; ++i) {
        wchar_t name[256] = {};
        BYTE data[1024] = {};
        DWORD nameLen = 256;
        DWORD dataLen = 1024;
        DWORD type = 0;

        LONG rc = RegEnumValueW(key, i, name, &nameLen, nullptr, &type, data, &dataLen);
        if (rc == ERROR_NO_MORE_ITEMS) break;
        if (rc != ERROR_SUCCESS) continue;

        std::wcout << L"Name: " << (nameLen ? name : L"(Default)") << L"\n";
        std::wcout << L"Type: " << type << L"\n";
        std::wcout << L"Data: ";

        if ((type == REG_SZ || type == REG_EXPAND_SZ) && dataLen >= sizeof(wchar_t)) {
            std::wcout << reinterpret_cast<wchar_t*>(data);
        } else if (type == REG_DWORD && dataLen >= sizeof(DWORD)) {
            std::wcout << *reinterpret_cast<DWORD*>(data);
        } else {
            std::wcout << L"[not shown]";
        }

        std::wcout << L"\n--------------------\n";
    }

    RegCloseKey(key);
    return 0;
}
