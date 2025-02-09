
#include "UE4.hpp"

#include "Classes.hpp"

namespace UE4
{
	class UObject* UObject::FindObjectFastImpl(const std::string& Name, EClassCastFlags RequiredType)
	{
		bool bOldGObj = GObjectsNew->GetByIndex(0) ? false : true;
		int ObjectNum = bOldGObj ? GObjects->Num() : GObjectsNew->Num();

		for (int i = 0; i < ObjectNum; ++i)
		{
			UObject* Object = nullptr;

			if (bOldGObj)
				Object = GObjects->GetByIndex(i);
			else
				Object = GObjectsNew->GetByIndex(i);

			if (!Object)
				continue;

			if (Object->HasTypeFlag(RequiredType) && Object->GetName().contains(Name))
				return Object;
		}

		return nullptr;
	}

	class UObject* UObject::FindObjectImpl(const std::string& FullName, EClassCastFlags RequiredType)
	{
		bool bOldGObj = GObjectsNew->GetByIndex(0) ? false : true;
		int ObjectNum = bOldGObj ? GObjects->Num() : GObjectsNew->Num();

		for (int i = 0; i < ObjectNum; ++i)
		{
			UObject* Object = nullptr;

			if (bOldGObj)
				Object = GObjects->GetByIndex(i);
			else
				Object = GObjectsNew->GetByIndex(i);

			if (!Object)
				continue;

			if (Object->HasTypeFlag(RequiredType) && Object->GetFullName() == FullName)
				return Object;
		}

		return nullptr;
	}

	std::string UObject::GetFullName() const
	{
		if (this && Class)
		{
			std::string Temp;

			for (UObject* NextOuter = Outer; NextOuter; NextOuter = NextOuter->Outer)
			{
				Temp = NextOuter->GetName() + "." + Temp;
			}

			std::string Name = Class->GetName();
			Name += " ";
			Name += Temp;
			Name += GetName();

			return Name;
		}

		return "None";
	}

	std::string UObject::GetName() const
	{
		return this ? Name.ToString() : "None";
	}

	bool UObject::HasTypeFlag(EClassCastFlags TypeFlags) const
	{
		return (Class->CastFlags & TypeFlags);
	}

	bool UObject::IsA(EClassCastFlags TypeFlags) const
	{
		return (Class->CastFlags & TypeFlags);
	}

	bool UObject::IsA(class UClass* TypeClass) const
	{
		return Class->IsSubclassOf(TypeClass);
	}

	bool UObject::IsDefaultObject() const
	{
		return (Flags & EObjectFlags::ClassDefaultObject);
	}

	std::vector<UFunction*> UObject::GetFunctions() const
	{
		std::vector<UFunction*> Functions;
		for (auto idk = Class; idk; idk = (UClass*)idk->Super)
		{
			for (UField* Field = idk->Children; Field; Field = Field->Next)
			{
				if (Field->IsA(EClassCastFlags::Function) && Field)
					Functions.push_back((UFunction*)Field);
			}
		}

		return Functions;
	}

	std::vector<UProperty*> UObject::GetFuncProperties() const
	{
		std::vector<UProperty*> Properties;
		for (UField* Field = Class->Children; Field; Field = Field->Next)
		{
			if (Field->IsA(EClassCastFlags::Property))
				Properties.push_back((UProperty*)Field);
		}

		return Properties;
	}

	std::vector<UProperty*> UObject::GetProperties() const
	{
		std::vector<UProperty*> Properties;
		for (auto idk = Class; idk; idk = (UClass*)idk->Super)
		{
			for (UField* Field = idk->Children; Field; Field = Field->Next)
			{
				if (Field->IsA(EClassCastFlags::Property))
					Properties.push_back((UProperty*)Field);
			}
		}

		return Properties;
	}

	int UObject::FindOffset(const char* varName) const
	{
		auto Properties = GetProperties();
		for (UProperty* Property : Properties)
		{
			if (Property->GetName() == varName)
				return Property->Offset;
		}

		return 0;
	}

	bool UStruct::IsSubclassOf(const UStruct* Base) const
	{
		if (!Base)
			return false;

		for (const UStruct* Struct = this; Struct; Struct = Struct->Super)
		{
			if (Struct == Base)
				return true;
		}

		return false;
	}

	UField* UStruct::GetChild() const
	{
		return *(UField**)(__int64(this) + Offsets::Children);
	}

	UField* UStruct::GetChildProperties() const
	{
		return (*(UField**)this + Offsets::ChildProperties);
	}

	UField* UField::GetNext() const
	{
		if(Offsets::Next != 0)
			return *(UField**)(__int64(this) + Offsets::Next);

		return this->Next;
	}

	class UFunction* UClass::GetFunction(const std::string& ClassName, const std::string& FuncName) const
	{
		for (const UStruct* Clss = this; Clss; Clss = Clss->Super)
		{
			if (Clss->GetName() != ClassName)
				continue;

			for (UField* Field = Clss->Children; Field; Field = Field->Next)
			{
				if (Field->HasTypeFlag(EClassCastFlags::Function) && Field->GetName() == FuncName)
					return static_cast<class UFunction*>(Field);
			}
		}

		return nullptr;
	}
}