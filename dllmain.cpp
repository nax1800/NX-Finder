#include "framework.h"

DWORD Initialize(LPVOID)
{
    AllocConsole();
    FILE* File;
    freopen_s(&File, "CONOUT$", "w+", stdout);

    UE4::Offsets::GObjects = OffsetFinder::GetGObjects();
    LogInfo("GObjects: 0x{:x}", UE4::Offsets::GObjects);

    if (UE4::Offsets::GObjects == 0)
    {
        LogError("GObjects Offset is 0, aborting!");
        Sleep(2000);
        exit(1);
    }

    UE4::Offsets::AppendString = OffsetFinder::GetAppendString();
    LogInfo("AppendString: 0x{:x}", UE4::Offsets::AppendString);

    if (UE4::Offsets::AppendString == 0)
    {
        LogError("AppendString Offset is 0, aborting!");
        Sleep(2000);
        exit(1);
    }

    UE4::UObject::GObjectsNew = reinterpret_cast<UE4::TUObjectArrayNew*>(Memory::GetAddress(UE4::Offsets::GObjects));
    if (UE4::UObject::GObjectsNew->GetByIndex(0) == nullptr)
    {
        UE4::UObject::GObjects.GObjectsAddress = reinterpret_cast<void*>(Memory::GetAddress(UE4::Offsets::GObjects));
        if (UE4::UObject::GObjects.GetTypedPtr()->GetByIndex(0) == nullptr)
        {
            LogInfo("GObjects failed, aborting.");
            Sleep(2000);
            exit(1);
        }
        else
            LogInfo("Using Old GObjects system.");
    }
    else
    {
        LogInfo("Using New GObjects system.");
    }

    pair<uintptr_t, uintptr_t> ProcessEventPair;
    ProcessEventPair = OffsetFinder::GetProcessEventOffAndIdx();

    UE4::Offsets::ProcessEvent = ProcessEventPair.first;
    UE4::Offsets::ProcessEventIdx = ProcessEventPair.second;

    FString GameName;
    FString EngineVersion;
    auto KismetLibrary = UObject::FindObject("KismetSystemLibrary Engine.Default__KismetSystemLibrary");

    auto Func_GetGameName = UObject::FindObject<UFunction>("Function Engine.KismetSystemLibrary.GetGameName");
    auto Func_GetEngineVersion = UObject::FindObject<UFunction>("Function Engine.KismetSystemLibrary.GetEngineVersion");

    KismetLibrary->ProcessEvent(Func_GetGameName, &GameName);
    KismetLibrary->ProcessEvent(Func_GetEngineVersion, &EngineVersion);

    LogInfo("GameName: {}", GameName.ToString());
    LogInfo("EngineVersion: {}", EngineVersion.ToString());

    Globals::GameName = GameName.ToString();
    Globals::EngineVersion = EngineVersion.ToString();

    CreateThread(nullptr, 0, Overlay::MainThread, 0, 0, nullptr);


    UE4::Offsets::CreateNetDriver = OffsetFinder::FindCreateNetDriver();
    UE4::Offsets::SetWorld = OffsetFinder::FindSetWorld();
    UE4::Offsets::InitListen = OffsetFinder::FindInitListen();

    filesystem::path p = Globals::StartPath;

    if (!filesystem::exists(p)) 
    {
        filesystem::create_directories(p);
    }

    Globals::GamePath = Globals::StartPath + Globals::GameName + "-" + Globals::EngineVersion + "/";

    filesystem::path pp = Globals::GamePath;

    if (!filesystem::exists(pp))
    {
        filesystem::create_directories(pp);
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReason, LPVOID lpReserved)
{
    switch (ulReason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Initialize, 0, 0, 0);
    case DLL_PROCESS_DETACH:
        filesystem::path pp = Globals::GamePath;

        if (filesystem::exists(pp) && filesystem::is_empty(pp))
            filesystem::remove(pp);
        break;
    }
    return TRUE;
}

