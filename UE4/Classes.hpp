#pragma once
#include "Containers.h"
#include "UE4.hpp"

namespace UE4
{
		class UObject
		{
		public:
			static inline class TUObjectArrayWrapper GObjects;
			static inline class TUObjectArrayNew* GObjectsNew;

			void* VTable;
			EObjectFlags Flags;
			int32 Index;
			class UClass* Class;
			class FName  Name;
			class UObject* Outer;

		public:
			static class UObject* FindObjectFastImpl(const std::string& Name, EClassCastFlags RequiredType = EClassCastFlags::None);
			static class UObject* FindObjectImpl(const std::string& FullName, EClassCastFlags RequiredType = EClassCastFlags::None);

			std::string GetFullName() const;
			std::string GetName() const;
			bool HasTypeFlag(EClassCastFlags TypeFlags) const;
			bool IsA(EClassCastFlags TypeFlags) const;
			bool IsA(class UClass* TypeClass) const;
			bool IsDefaultObject() const;

			std::vector<class UFunction*> GetFunctions() const;
			std::vector<class UProperty*> GetProperties() const;
			std::vector<class UProperty*> GetFuncProperties() const;
			int FindOffset(const char* varName) const;

			template <typename T = UObject*>
			inline T& Get(int Offset) const { return *(T*)(__int64(this) + Offset); }

		public:
			static class UClass* FindClass(const std::string& ClassFullName)
			{
				return FindObject<class UClass>(ClassFullName, EClassCastFlags::Class);
			}
			static class UClass* FindClassFast(const std::string& ClassName)
			{
				return FindObjectFast<class UClass>(ClassName, EClassCastFlags::Class);
			}

			template<typename UEType = UObject>
			static UEType* FindObject(const std::string& Name, EClassCastFlags RequiredType = EClassCastFlags::None)
			{
				return static_cast<UEType*>(FindObjectImpl(Name, RequiredType));
			}
			template<typename UEType = UObject>
			static UEType* FindObjectFast(const std::string& Name, EClassCastFlags RequiredType = EClassCastFlags::None)
			{
				return static_cast<UEType*>(FindObjectFastImpl(Name, RequiredType));
			}

			template<typename UEType = UObject>
			static UEType* FindObjectFastInOuter(const char* Name, const char* Outer)
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


					if (Object->GetName() == Name && Object->Outer->GetName() == Outer)
					{
						return static_cast<UEType*>(Object);
					}
				}

				return nullptr;
			}

			void ProcessEvent(class UFunction* Function, void* Parms) const
			{
				InSDKUtils::CallGameFunction(InSDKUtils::GetVirtualFunction<void(*)(const UObject*, class UFunction*, void*)>(this, Offsets::ProcessEventIdx), this, Function, Parms);
			}

			static class UClass* StaticClass()
			{
				return StaticClassImpl<"Object">();
			}
			static class UObject* GetDefaultObj()
			{
				return GetDefaultObjImpl<UObject>();
			}
		};


		class UField : public UObject
		{
		public:
			class UField* Next;

			class UField* GetNext() const;

		public:
			static class UClass* StaticClass()
			{
				return StaticClassImpl<"Field">();
			}
			static class UField* GetDefaultObj()
			{
				return GetDefaultObjImpl<UField>();
			}
		};

		class UProperty : public UField
		{
		public:
			int32 ArrayDim;
			int32 ElementSize;
			uint64 PropertyFlags;
			uint8 Pad_20[0x4]; 
			int32 Offset; 
			uint8 Pad_21[0x28];

		public:
			static class UClass* StaticClass()
			{
				return StaticClassImpl<"Property">();
			}
			static class UProperty* GetDefaultObj()
			{
				return GetDefaultObjImpl<UProperty>();
			}
		};

		class UStruct : public UField
		{
		public:
			class UStruct* Super;
			class UField* Children;
			int32 Size;
			int32 MinAlignemnt; 
			uint8 Pad_1D[0x40];

		public:
			bool IsSubclassOf(const UStruct* Base) const;

			class UField* GetChild() const;
			class UField* GetChildProperties() const;

		public:
			static class UClass* StaticClass()
			{
				return StaticClassImpl<"Struct">();
			}
			static class UStruct* GetDefaultObj()
			{
				return GetDefaultObjImpl<UStruct>();
			}
		};

		class UClass : public UStruct
		{
		public:
			uint8 Pad_24[0x30];
			enum class EClassCastFlags                    CastFlags;
			uint8 Pad_25[0x38];
			class UObject* DefaultObject;
			uint8 Pad_26[0xF8];
		public:
			class UFunction* GetFunction(const std::string& ClassName, const std::string& FuncName) const;

		public:
			static class UClass* StaticClass()
			{
				return StaticClassImpl<"Class">();
			}
			static class UClass* GetDefaultObj()
			{
				return GetDefaultObjImpl<UClass>();
			}
		};

		class UFunction : public UStruct
		{
		public:
			using FNativeFuncPtr = void (*)(void* Context, void* TheStack, void* Result);

			uint32                                        FunctionFlags;
			uint8 Pad_27[0x20];
			FNativeFuncPtr                                ExecFunction;

		public:
			static class UClass* StaticClass()
			{
				return StaticClassImpl<"Function">();
			}
			static class UFunction* GetDefaultObj()
			{
				return GetDefaultObjImpl<UFunction>();
			}
		};


		struct FLinearColor final
		{
		public:
			float                                         R;                              
			float                                         G;                              
			float                                         B;  
			float                                         A;  
		};
}