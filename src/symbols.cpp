#include <iostream>
#include <windows.h>
#include <dbghelp.h>
#include <dia2.h>


const char* GetTagString(DWORD tag) {
    switch (tag) {
        case SymTagNull:
            return "SymTagNull";
        case SymTagExe:
            return "SymTagExe";
        case SymTagCompiland:
            return "SymTagCompiland";
        case SymTagCompilandDetails:
            return "SymTagCompilandDetails";
        case SymTagCompilandEnv:
            return "SymTagCompilandEnv";
        case SymTagFunction:
            return "SymTagFunction";
        case SymTagBlock:
            return "SymTagBlock";
        case SymTagData:
            return "SymTagData";
        case SymTagAnnotation:
            return "SymTagAnnotation";
        case SymTagLabel:
            return "SymTagLabel";
        case SymTagPublicSymbol:
            return "SymTagPublicSymbol";
        case SymTagUDT:
            return "SymTagUDT";
        case SymTagEnum:
            return "SymTagEnum";
        case SymTagFunctionType:
            return "SymTagFunctionType";
        case SymTagPointerType:
            return "SymTagPointerType";
        case SymTagArrayType:
            return "SymTagArrayType";
        case SymTagBaseType:
            return "SymTagBaseType";
        case SymTagTypedef:
            return "SymTagTypedef";
        case SymTagBaseClass:
            return "SymTagBaseClass";
        case SymTagFriend:
            return "SymTagFriend";
        case SymTagFunctionArgType:
            return "SymTagFunctionArgType";
        case SymTagFuncDebugStart:
            return "SymTagFuncDebugStart";
        case SymTagFuncDebugEnd:
            return "SymTagFuncDebugEnd";
        case SymTagUsingNamespace:
            return "SymTagUsingNamespace";
        case SymTagVTableShape:
            return "SymTagVTableShape";
        case SymTagVTable:
            return "SymTagVTable";
        case SymTagCustom:
            return "SymTagCustom";
        case SymTagThunk:
            return "SymTagThunk";
        case SymTagCustomType:
            return "SymTagCustomType";
        case SymTagManagedType:
            return "SymTagManagedType";
        case SymTagDimension:
            return "SymTagDimension";
        default:
            return "Unknown";
    }
}



void get_symbole(const char* path) {
    HANDLE hProc = GetCurrentProcess();
    if (!SymInitialize(hProc, nullptr, TRUE)) {
        std::cerr << "SymInit failed. Error: " << GetLastError() << std::endl;
        return;
    }

    HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    DWORD64 baseAddr = SymLoadModuleEx(hProc, hFile, nullptr, nullptr, 0, 0, nullptr, 0);

    if (baseAddr == 0) {
        std::cerr << "File load failed. Error: " << GetLastError() << std::endl;
        SymCleanup(hProc);
        return;
    }

    IMAGEHLP_MODULE modInfo;
    modInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);

    if (SymGetModuleInfo(hProc, baseAddr, &modInfo)) {
        std::cout << "Module: " << modInfo.ModuleName << std::endl;
        std::cout << "  Base: 0x" << std::hex << modInfo.BaseOfImage << std::dec << std::endl;
        std::cout << "  Size: " << modInfo.ImageSize << " bytes" << std::endl;
        std::cout << "  Loaded: " << modInfo.LoadedImageName << std::endl;
        std::cout << "  Type: " << modInfo.SymType << "\n" << std::endl;

        if (!SymEnumSymbols(hProc, baseAddr, nullptr, [](PSYMBOL_INFO pInfo, ULONG Size, PVOID Context) -> BOOL {
            std::cout << "\n Name: " << pInfo->Name << std::endl;
            std::cout << "     Address: 0x" << std::hex << pInfo->Address << std::endl;
            std::cout << "     Size: " << pInfo->Size << " bytes" << " - Value: " << pInfo->Value << std::endl;
            std::cout << "     TagStr: " << GetTagString(pInfo->Tag) << std::endl;
            std::cout << "     Reserved: " << pInfo->Reserved << std::endl;
            std::cout << "     Scope: 0x" << pInfo->Scope << " - NameLen: 0x" << pInfo->NameLen << std::endl;
            std::cout << "     Flags: " << pInfo->Flags << " - Tag: 0x" << pInfo->Tag << std::endl;
            std::cout << std::endl;
            return TRUE;
        }, const_cast<PVOID>(static_cast<const void*>(path)))) {
            std::cerr << "Enum symbols failed. Error: " << GetLastError() << std::endl;
        }
        std::cout << std::endl;
    } else {
        std::cerr << "Module info failed. Error: " << GetLastError() << std::endl;
    }

    SymUnloadModule64(hProc, baseAddr);
    SymCleanup(hProc);
}