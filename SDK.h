#pragma once
#include "UE4/Containers.h"
#include "UE4/UE4.hpp"
#include "UE4/Classes.hpp"

#include <iostream>
#include <fstream>

#include "Memory.h"

using namespace std;

namespace UE4
{
	namespace Utils
	{
		void DumpClasses(string Path)
		{
			std::ofstream outputFile(Path + "Classes.txt", std::ios::app);

			if (outputFile.is_open())
			{
				outputFile << "Dumped by NX-Finder.\n\n" << std::endl;
				outputFile << "                                  " << endl;
				outputFile << "==================================== CLASSES ====================================" << endl;
				outputFile << "                                  " << endl;

				bool bOldGObj = UObject::GObjectsNew->GetByIndex(0) ? false : true;
				int ObjectNum = bOldGObj ? UObject::GObjects->Num() : UObject::GObjectsNew->Num();

				for (int i = 0; i < ObjectNum; ++i)
				{
					UObject* Object = nullptr;

					if (bOldGObj)
						Object = UObject::GObjects->GetByIndex(i);
					else
						Object = UObject::GObjectsNew->GetByIndex(i);

					if (!Object || !Object->GetFullName().starts_with("Class "))
						continue;

					outputFile << "[" << i << "]: " << "(0x" << hex << __int64(Object) << dec << ") " << Object->GetFullName() << std::endl;
				}

				outputFile.close();
			}
		}

		void DumpObjects(string Path)
		{
			std::ofstream outputFile(Path + "Objects.txt", std::ios::app);

			if (outputFile.is_open())
			{
				outputFile << "Dumped by NX-Finder.\n\n" << std::endl;
				outputFile << "                                  " << endl;
				outputFile << "==================================== OBJECTS ====================================" << endl;
				outputFile << "                                  " << endl;

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

					outputFile << "[" << to_string(i) << "]: " << "(0x" << hex << __int64(Object) << dec << ") " << Object->GetFullName() << std::endl;
				}

				outputFile.close();
			}	
		}

		void DumpFunctions(string Path)
		{
			std::ofstream outputFile(Path + "Functions.txt", std::ios::app);

			if (outputFile.is_open())
			{
				outputFile << "Dumped by NX-Finder.\n\n" << std::endl;

				outputFile << "                                  " << endl;
				outputFile << "==================================== FUNCTIONS ====================================" << endl;
				outputFile << "                                  " << endl;
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

					if (!Object->GetFullName().starts_with("Function "))
						continue;

					UFunction* Func = (UFunction*)Object;

					outputFile << "---------------------------------------------" << endl;
					outputFile << Func->GetFullName() << endl;
					outputFile << format("Exec: 0x{:x}", Memory::GetOffset(Func->ExecFunction)).c_str() << endl;
					vector<UProperty*> FuncProps = Func->GetFuncProperties();
					for (UProperty* Prop : FuncProps)
					{
						if (!Prop)
							continue;

						outputFile << format("Offset: 0x{:x}", Prop->Offset).c_str() << endl;
						outputFile << format("ArrayDim: 0x{:x}", Prop->ArrayDim).c_str() << endl;
						outputFile << format("ElementSize: 0x{:x}", Prop->ElementSize).c_str() << endl;
						outputFile << format("PropertyFlags: {}", Prop->PropertyFlags).c_str() << endl;
					}
					outputFile << "---------------------------------------------" << endl;
				}

				outputFile.close();
			}
		}

		void DumpClass(string Path, std::string ClassName, std::vector<class UProperty*> ClassProps, std::vector<class UFunction*> ClassFuncs)
		{
			std::ofstream outputFile(Path + ClassName + ".txt", std::ios::app);

			if (outputFile.is_open())
			{
				outputFile << "Dumped by NX-Finder.\n\n" << std::endl;
				if (!ClassProps.empty())
				{
					outputFile << "                                  " << endl;
					outputFile << "==================================== PROPERTIES ====================================" << endl;
					outputFile << "                                  " << endl;

					for (UProperty* Prop : ClassProps)
					{
						outputFile << "---------------------------------------------" << endl;
						outputFile << Prop->GetFullName() << endl;
						if (!Prop)
							continue;

						outputFile << format("{}:Offset: 0x{:x}", Prop->GetName(), Prop->Offset).c_str() << endl;
						outputFile << format("{}:ArrayDim: 0x{:x}", Prop->GetName(), Prop->ArrayDim).c_str() << endl;
						outputFile << format("{}:ElementSize: 0x{:x}", Prop->GetName(), Prop->ElementSize).c_str() << endl;
						outputFile << format("{}:PropertyFlags: {}", Prop->GetName(), Prop->PropertyFlags).c_str() << endl;
					}
					outputFile << "---------------------------------------------" << endl;
				}

				if (!ClassFuncs.empty())
				{
					outputFile << "                                  " << endl;
					outputFile << "==================================== FUNCTIONS ====================================" << endl;
					outputFile << "                                  " << endl;
					for (UFunction* Func : ClassFuncs)
					{

						if (!Func)
							continue;

						outputFile << "---------------------------------------------" << endl;
						outputFile << Func->GetFullName() << endl;
						outputFile << format("Exec: 0x{:x}", Memory::GetOffset(Func->ExecFunction)).c_str() << endl;
						vector<UProperty*> FuncProps = Func->GetFuncProperties();
						for (UProperty* Prop : FuncProps)
						{
							if (!Prop)
								continue;

							outputFile << format("Offset: 0x{:x}", Prop->Offset).c_str() << endl;
							outputFile << format("ArrayDim: 0x{:x}", Prop->ArrayDim).c_str() << endl;
							outputFile << format("ElementSize: 0x{:x}", Prop->ElementSize).c_str() << endl;
							outputFile << format("PropertyFlags: {}", Prop->PropertyFlags).c_str() << endl;
						}
						outputFile << "---------------------------------------------" << endl;
					}
				}

				outputFile.close();
			}
		}

		UObject* GetDefaultObj(std::string Name)
		{
			auto Obj = UObject::FindObject(Name);
			if (!Obj)
				Obj = UObject::FindObjectFast(Name);

			if (!Obj)
				return nullptr;

			if (Obj->IsA(UClass::StaticClass()))
				return Obj->GetDefaultObj();

			if (!Obj->IsDefaultObject())
				return Obj->GetDefaultObj();
			else
				return Obj;

			return nullptr;
		}
	}
}
