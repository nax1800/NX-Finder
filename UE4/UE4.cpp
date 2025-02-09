#include "UE4.hpp"

#include "Classes.hpp"

namespace UE4
{
		class UClass* BasicFilesImpleUtils::FindClassByName(const std::string& Name)
		{
			return UObject::FindClassFast(Name);
		}

		class UClass* BasicFilesImpleUtils::FindClassByFullName(const std::string& Name)
		{
			return UObject::FindClass(Name);
		}

		std::string BasicFilesImpleUtils::GetObjectName(class UClass* Class)
		{
			return Class->GetName();
		}

		int32 BasicFilesImpleUtils::GetObjectIndex(class UClass* Class)
		{
			return Class->Index;
		}

		class UObject* BasicFilesImpleUtils::GetObjectByIndex(int32 Index)
		{
			return UObject::GObjects->GetByIndex(Index);
		}

		UFunction* BasicFilesImpleUtils::FindFunctionByFName(const FName* Name)
		{
			for (int i = 0; i < UObject::GObjects->Num(); ++i)
			{
				UObject* Object = UObject::GObjects->GetByIndex(i);

				if (!Object)
					continue;

				if (Object->Name == *Name)
					return static_cast<UFunction*>(Object);
			}

			return nullptr;
		}
}