#pragma once
#include "framework.h"

namespace Globals
{
	string GameName = "";
	string EngineVersion = "";

	vector<UClass*> GameClasses{};

	string StartPath = "C:/NX-Finder/";
	string GamePath = "";

	UObject* GetEngine()
	{
		static UObject* Engine = nullptr;
		static UClass* EngineClass = UObject::FindObject<UClass>("Class Engine.Engine");
		if (!Engine)
		{
			bool bOldGObj = UObject::GObjectsNew->GetByIndex(0) ? false : true;
			int ObjectNum = bOldGObj ? UObject::GObjects->Num() : UObject::GObjectsNew->Num();

			for (int i = 0; i < ObjectNum; ++i)
			{
				UObject* Object = nullptr;

				if (bOldGObj)
					Object = UObject::GObjects->GetByIndex(i);
				else
					Object = UObject::GObjectsNew->GetByIndex(i);

				if (!Object)
					continue;

				if (Object->IsA(EngineClass) && !Object->IsDefaultObject())
				{
					Engine = Object;
					break;
				}
			}
		}

		return Engine;
	}

	UObject* GetWorld()
	{
		UObject* Engine = GetEngine();
		if (!Engine)
			return nullptr;

		UObject* GameViewport = Engine->Get(Engine->FindOffset("GameViewport"));
		if (!GameViewport)
			return nullptr;

		UObject* World = GameViewport->Get(GameViewport->FindOffset("World"));
		if (!World)
			return nullptr;

		return World;
	}

	TArray<UObject*> GetAllActors()
	{
		TArray<UObject*> OutActors;
		static auto ActorClass = UObject::FindObject<UClass>("Class Engine.Actor");
		if (!ActorClass)
			return {};

		static UObject* GameplayStatics = UObject::FindObject("GameplayStatics Engine.Default__GameplayStatics");
		if (!GameplayStatics)
			return  {};

		static UFunction* GetAllActorsOfClass = UObject::FindObject<UFunction>("Function Engine.GameplayStatics.GetAllActorsOfClass");
		if (!GetAllActorsOfClass)
			return  {};

		struct GetAllActorsOfClassParms final
		{
		public:
			const UObject* WorldContextObject;
			UClass* ActorClass;
			TArray<UObject*> OutActors;
		};

		GetAllActorsOfClassParms Params{};
		Params.ActorClass = ActorClass;
		Params.OutActors = OutActors;
		Params.WorldContextObject = GetWorld();

		GameplayStatics->ProcessEvent(GetAllActorsOfClass, &Params);

		return Params.OutActors;
	}
}