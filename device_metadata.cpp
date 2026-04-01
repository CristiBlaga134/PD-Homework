#include <windows.h>
#include <setupapi.h>
#include <iostream>

#pragma comment(lib, "Setupapi.lib")

int wmain() {
    HDEVINFO list = SetupDiGetClassDevsW(nullptr, nullptr, nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (list == INVALID_HANDLE_VALUE) return 1;

    SP_DEVINFO_DATA dev{};
    dev.cbSize = sizeof(dev);
    if (!SetupDiEnumDeviceInfo(list, 0, &dev)) {
        SetupDiDestroyDeviceInfoList(list);
        return 1;
    }

    struct P { DWORD id; const wchar_t* name; } props[] = {
        {SPDRP_DEVICEDESC, L"DeviceDesc"},
        {SPDRP_HARDWAREID, L"HardwareID"},
        {SPDRP_CLASS, L"Class"},
        {SPDRP_MFG, L"Manufacturer"},
        {SPDRP_FRIENDLYNAME, L"FriendlyName"},
        {SPDRP_LOCATION_INFORMATION, L"LocationInfo"}
    };

    std::wcout << L"First connected device metadata:\n\n";

    for (auto p : props) {
        BYTE buf[1024] = {};
        DWORD type = 0, need = 0;
        if (SetupDiGetDeviceRegistryPropertyW(list, &dev, p.id, &type, buf, sizeof(buf), &need)) {
            std::wcout << p.name << L": ";
            if (type == REG_SZ || type == REG_EXPAND_SZ) std::wcout << reinterpret_cast<wchar_t*>(buf);
            else std::wcout << L"[available]";
            std::wcout << L"\n";
        }
    }

    SetupDiDestroyDeviceInfoList(list);
    return 0;
}
