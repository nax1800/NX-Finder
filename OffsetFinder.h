#pragma once
#include "framework.h"

namespace OffsetFinder
{
	struct FChunkedFixedUObjectArray
	{
		void** ObjectsAbove;
		uint8_t Pad_0[0x08];
		int32_t MaxElements;
		int32_t NumElements;
		int32_t MaxChunks;
		int32_t NumChunks;
		void** ObjectsBelow;

		inline int32 IsValid(int32& OutObjectsPtrOffset)
		{
			void** ObjectsAboveButDecrypted = (void**)Memory::DecryptPtr(ObjectsAbove);
			void** ObjectsBelowButDecrypted = (void**)Memory::DecryptPtr(ObjectsBelow);

			if (NumChunks > 0x14 || NumChunks < 0x1)
				return false;

			if (MaxChunks > 0x22F || MaxChunks < 0x6)
				return false;

			if (NumElements > MaxElements || NumChunks > MaxChunks)
				return false;

			if (((NumElements / 0x10000) + 1) != NumChunks || (MaxElements / 0x10000) != MaxChunks)
				return false;

			const bool bAreObjectsAboveValid = (ObjectsAboveButDecrypted && !Memory::IsBadReadPtr(ObjectsAboveButDecrypted));
			const bool bAreObjectsBewlowValid = (ObjectsBelowButDecrypted && !Memory::IsBadReadPtr(ObjectsBelowButDecrypted));

			if (!bAreObjectsAboveValid && !bAreObjectsBewlowValid)
				return false;

			for (int i = 0; i < NumChunks; i++)
			{
#pragma warning(disable:6011)
				const bool bIsCurrentIndexValidAbove = bAreObjectsAboveValid ? !Memory::IsBadReadPtr(ObjectsAboveButDecrypted[i]) : false;
				const bool bIsCurrentIndexValidBelow = bAreObjectsBewlowValid ? !Memory::IsBadReadPtr(ObjectsBelowButDecrypted[i]) : false;
#pragma pop

				if (!bIsCurrentIndexValidAbove && !bIsCurrentIndexValidBelow)
					return false;
			}

			OutObjectsPtrOffset = 0x00;

			if (!bAreObjectsAboveValid && bAreObjectsBewlowValid)
				OutObjectsPtrOffset = 0x20;

			return true;
		}
	};

	struct FFixedUObjectArray
	{
		struct FUObjectItem
		{
			void* Object;
			uint8_t Pad[0x10];
		};

		FUObjectItem* Objects;
		int32_t Max;
		int32_t Num;

		inline bool IsValid()
		{
			FUObjectItem* ObjectsButDecrypted = (FUObjectItem*)Memory::DecryptPtr(Objects);

			if (Num > Max)
				return false;

			if (Max > 0x400000)
				return false;

			if (Num < 0x1000)
				return false;

			if (Memory::IsBadReadPtr(ObjectsButDecrypted))
				return false;

			if (Memory::IsBadReadPtr(ObjectsButDecrypted[5].Object))
				return false;

			if (*(int32_t*)(uintptr_t(ObjectsButDecrypted[5].Object) + 0xC) != 5)
				return false;

			return true;
		}
	};

	template<bool bCheckForVft = true>
	inline int32_t GetValidPointerOffset(uint8_t* ObjA, uint8_t* ObjB, int32_t StartingOffset, int32_t MaxOffset)
	{
		if (Memory::IsBadReadPtr(ObjA) || Memory::IsBadReadPtr(ObjB))
			return -1;

		for (int j = StartingOffset; j <= MaxOffset; j += 0x8)
		{
			const bool bIsAValid = !Memory::IsBadReadPtr(*reinterpret_cast<void**>(ObjA + j)) && (bCheckForVft ? !Memory::IsBadReadPtr(**reinterpret_cast<void***>(ObjA + j)) : true);
			const bool bIsBValid = !Memory::IsBadReadPtr(*reinterpret_cast<void**>(ObjB + j)) && (bCheckForVft ? !Memory::IsBadReadPtr(**reinterpret_cast<void***>(ObjB + j)) : true);

			if (bIsAValid && bIsBValid)
				return j;
		}

		return -1;
	};

	int32 FUObjectArrayPtr;

	uintptr_t GetGObjects()
	{
		bool bScanAllMemory = false;
		const auto [ImageBase, ImageSize] = Memory::GetImageBaseAndSize();

		uintptr_t SearchBase = ImageBase;
		DWORD SearchRange = ImageSize;

		if (!bScanAllMemory)
		{
			const auto [DataSection, DataSize] = Memory::GetSectionByName(ImageBase, ".data");

			if (DataSection != 0x0 && DataSize != 0x0)
			{
				SearchBase = DataSection;
				SearchRange = DataSize;
			}
			else
			{
				bScanAllMemory = true;
			}
		}

		SearchRange -= 0x50;

		for (int i = 0; i < SearchRange; i += 0x4)
		{
			auto FixedArray = reinterpret_cast<FFixedUObjectArray*>(SearchBase + i);
			auto ChunkedArray = reinterpret_cast<FChunkedFixedUObjectArray*>(SearchBase + i);

			if (FixedArray->IsValid())
			{
				return (SearchBase + i) - ImageBase;
			}
			else if (ChunkedArray->IsValid(FUObjectArrayPtr))
			{
				return (SearchBase + i) - ImageBase;
			}
		}

		return 0;
	}

	uintptr_t GetAppendString()
	{
		static std::vector<const char*> PossibleSigs =
		{
			"48 8D ? ? 48 8D ? ? E8",
			"48 8D ? ? ? 48 8D ? ? E8",
			"48 8D ? ? 49 8B ? E8",
			"48 8D ? ? ? 49 8B ? E8",
			"48 8D ? ? 48 8B ? E8"
			"48 8D ? ? ? 48 8B ? E8",
		};

		Memory::MemAddress StringRef = Memory::FindByStringInAllSections("ForwardShadingQuality_");


		int i = 0;
		while (!UE4::FName::AppendString && i < PossibleSigs.size())
		{
			UE4::FName::AppendString = static_cast<void(*)>(StringRef.RelativePattern(PossibleSigs[i], 0x50, -1 /* auto */));

			i++;
		}

		return Memory::GetOffset(UE4::FName::AppendString);
	}

	inline int32_t FindFunctionFlagsOffset()
	{
		std::vector<std::pair<void*, UE4::EFunctionFlags>> Infos;

		Infos.push_back({ UE4::UObject::FindObjectFast("WasInputKeyJustPressed"), UE4::EFunctionFlags::Final | UE4::EFunctionFlags::Native | UE4::EFunctionFlags::Public | UE4::EFunctionFlags::BlueprintCallable | UE4::EFunctionFlags::BlueprintPure | UE4::EFunctionFlags::Const });
		Infos.push_back({ UE4::UObject::FindObjectFast("ToggleSpeaking"), UE4::EFunctionFlags::Exec | UE4::EFunctionFlags::Native | UE4::EFunctionFlags::Public });
		Infos.push_back({ UE4::UObject::FindObjectFast("SwitchLevel"), UE4::EFunctionFlags::Exec | UE4::EFunctionFlags::Native | UE4::EFunctionFlags::Public });

		int32_t Ret = Memory::FindOffset(Infos);

		if (Ret == -1)
		{
			for (auto& [_, Flags] : Infos)
				Flags = Flags | UE4::EFunctionFlags::RequiredAPI;
		}

		return Memory::FindOffset(Infos);
	}

	pair<uintptr_t, uintptr_t> GetProcessEventOffAndIdx()
	{
		bool bOldGObj = UE4::UObject::GObjectsNew->GetByIndex(0) ? false : true;
		void** Vft = bOldGObj ? *(void***)UE4::UObject::GObjects->GetByIndex(0) : *(void***)UE4::UObject::GObjectsNew->GetByIndex(0);

		static int32_t FunctionFlags = FindFunctionFlagsOffset();

		/* Primary, and more reliable, check for ProcessEvent */
		auto IsProcessEvent = [](const uint8_t* FuncAddress, [[maybe_unused]] int32_t Index) -> bool
			{
				return Memory::FindPatternInRange({ 0xF7, -0x1, FunctionFlags, 0x0, 0x0, 0x0, 0x0, 0x04, 0x0, 0x0 }, FuncAddress, 0x400)
					&& Memory::FindPatternInRange({ 0xF7, -0x1, FunctionFlags, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0 }, FuncAddress, 0xF00);
			};

		const void* ProcessEventAddr = nullptr;
		int32_t ProcessEventIdx = 0;

		auto [FuncPtr, FuncIdx] = Memory::IterateVTableFunctions(Vft, IsProcessEvent);

		ProcessEventAddr = FuncPtr;
		ProcessEventIdx = FuncIdx;

		if (!FuncPtr)
		{
			void* PossiblePEAddr = (void*)Memory::FindByWStringInAllSections(L"Accessed None").FindNextFunctionStart();

			auto IsSameAddr = [PossiblePEAddr](const uint8_t* FuncAddress, [[maybe_unused]] int32_t Index) -> bool
				{
					return FuncAddress == PossiblePEAddr;
				};

			auto [FuncPtr2, FuncIdx2] = Memory::IterateVTableFunctions(Vft, IsSameAddr);
			ProcessEventAddr = FuncPtr2;
			ProcessEventIdx = FuncIdx2;
		}

		if (ProcessEventAddr)
			return make_pair(Memory::GetOffset(ProcessEventAddr), ProcessEventIdx);
	}

	inline int32_t FindSuperOffset()
	{
		std::vector<std::pair<void*, void*>> Infos;

		Infos.push_back({ UObject::FindObjectFast("Struct"), UObject::FindObjectFast("Field") });
		Infos.push_back({ UObject::FindObjectFast("Class"), UObject::FindObjectFast("Struct") });

		// Thanks to the ue4 dev who decided UStruct should be spelled Ustruct
		if (Infos[0].first == nullptr)
			Infos[0].first = Infos[1].second = UObject::FindObjectFast("struct");

		return Memory::FindOffset(Infos);
	}

	inline int32_t FindChildOffset()
	{
		std::vector<std::pair<void*, void*>> Infos;

		Infos.push_back({ UObject::FindObjectFast("PlayerController"), UObject::FindObjectFastInOuter("WasInputKeyJustReleased", "PlayerController") });
		Infos.push_back({ UObject::FindObjectFast("Controller"), UObject::FindObjectFastInOuter("UnPossess", "Controller") });

		if (Memory::FindOffset(Infos) == -1)
		{
			Infos.clear();

			Infos.push_back({ UObject::FindObjectFast("Vector"), UObject::FindObjectFastInOuter("X", "Vector") });
			Infos.push_back({ UObject::FindObjectFast("Vector4"), UObject::FindObjectFastInOuter("X", "Vector4") });
			Infos.push_back({ UObject::FindObjectFast("Vector2D"), UObject::FindObjectFastInOuter("X", "Vector2D") });
			Infos.push_back({ UObject::FindObjectFast("Guid"), UObject::FindObjectFastInOuter("A","Guid") });

			return Memory::FindOffset(Infos);
		}

		return Memory::FindOffset(Infos);
	}

	inline int32_t FindChildPropertiesOffset()
	{
		uint8* ObjA = (uint8*)UObject::FindObjectFast("Color");
		uint8* ObjB = (uint8*)UObject::FindObjectFast("Guid");

		return GetValidPointerOffset(ObjA, ObjB, Offsets::Children + 0x08, 0x80);
	}

	inline int32_t FindUFieldNextOffset()
	{
		uint8_t* KismetSystemLibraryChild = reinterpret_cast<uint8_t*>(UObject::FindObjectFast<UStruct>("KismetSystemLibrary")->GetChild());
		uint8_t* KismetStringLibraryChild = reinterpret_cast<uint8_t*>(UObject::FindObjectFast<UStruct>("KismetStringLibrary")->GetChild());

		uintptr_t Outer = 0x20;

		return GetValidPointerOffset(KismetSystemLibraryChild, KismetStringLibraryChild, Outer + 0x08, 0x60);
	}

	//todo
	inline uintptr_t FindCreateNetDriver()
	{
		auto StringRefAddr = Memcury::Scanner::FindStringRef(L"CreateNamedNetDriver failed to create driver %s from definition %s", true);

		if (!StringRefAddr.IsValid())
		{
			printf("String not found!\n");
			return 0;
		}

		auto FunctionAddr = Memcury::Scanner(StringRefAddr.Get()).ScanFor({ 0x48, 0x89, 0x5C, 0x24 }, false);

		if (!FunctionAddr.IsValid())
		{
			LogInfo("CreateNetDriver could not be found.");
			return 0;
		}

		return 0;
	}

	inline uintptr_t FindSetWorld()
	{
		auto StringRefAddr = Memcury::Scanner::FindStringRef(L"AOnlineBeaconHost::InitHost failed");
		if (!StringRefAddr.IsValid())
		{
			return 0;
		}

		return Memory::GetOffset(StringRefAddr.ScanFor({ 0x48, 0x8B, 0xD0, 0xE8 }, false).RelativeOffset(4).GetAs<void*>());
	}

	inline uintptr_t FindInitListen()
	{
		auto StringRefAddr = Memcury::Scanner::FindStringRef(L"%s IpNetDriver listening on port %i");
		if (!StringRefAddr.IsValid())
		{
			return 0;
		}

		return Memory::GetOffset(StringRefAddr.ScanFor({ 0x48, 0x89, 0x5C }, false).RelativeOffset(4).GetAs<void*>());
	}
}