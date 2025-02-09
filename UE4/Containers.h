#pragma once
#include <string>
#include <stdexcept>
#include <cmath>
#include <Windows.h>

namespace UE4
{
	namespace Containers
	{
		typedef int8_t  int8;
		typedef int16_t int16;
		typedef int32_t int32;
		typedef int64_t int64;

		typedef uint8_t  uint8;
		typedef uint16_t uint16;
		typedef uint32_t uint32;
		typedef uint64_t uint64;


		namespace FMemory
		{
			inline void* (*Realloc)(void* Memory, int64_t NewSize, uint32_t Alignment);
			inline void* (*Free)(void* Memory);
		}

		template<typename ArrayElementType>
		class TArray;

		template<typename SparseArrayElementType>
		class TSparseArray;

		template<typename SetElementType>
		class TSet;

		template<typename KeyElementType, typename ValueElementType>
		class TMap;

		template<typename KeyElementType, typename ValueElementType>
		class TPair;

		namespace Iterators
		{
			class FSetBitIterator;

			template<typename ArrayType>
			class TArrayIterator;

			template<class ContainerType>
			class TContainerIterator;

			template<typename SparseArrayElementType>
			using TSparseArrayIterator = TContainerIterator<TSparseArray<SparseArrayElementType>>;

			template<typename SetElementType>
			using TSetIterator = TContainerIterator<TSet<SetElementType>>;

			template<typename KeyElementType, typename ValueElementType>
			using TMapIterator = TContainerIterator<TMap<KeyElementType, ValueElementType>>;
		}


		namespace ContainerImpl
		{
			namespace HelperFunctions
			{
				inline uint32 FloorLog2(uint32 Value)
				{
					uint32 pos = 0;
					if (Value >= 1 << 16) { Value >>= 16; pos += 16; }
					if (Value >= 1 << 8) { Value >>= 8; pos += 8; }
					if (Value >= 1 << 4) { Value >>= 4; pos += 4; }
					if (Value >= 1 << 2) { Value >>= 2; pos += 2; }
					if (Value >= 1 << 1) { pos += 1; }
					return pos;
				}

				inline uint32 CountLeadingZeros(uint32 Value)
				{
					if (Value == 0)
						return 32;

					return 31 - FloorLog2(Value);
				}
			}

			template<int32 Size, uint32 Alignment>
			struct TAlignedBytes
			{
				alignas(Alignment) uint8 Pad[Size];
			};

			template<uint32 NumInlineElements>
			class TInlineAllocator
			{
			public:
				template<typename ElementType>
				class ForElementType
				{
				private:
					static constexpr int32 ElementSize = sizeof(ElementType);
					static constexpr int32 ElementAlign = alignof(ElementType);

					static constexpr int32 InlineDataSizeBytes = NumInlineElements * ElementSize;

				private:
					TAlignedBytes<ElementSize, ElementAlign> InlineData[NumInlineElements];
					ElementType* SecondaryData;

				public:
					ForElementType()
						: InlineData{ 0x0 }, SecondaryData(nullptr)
					{
					}

					ForElementType(ForElementType&&) = default;
					ForElementType(const ForElementType&) = default;

				public:
					ForElementType& operator=(ForElementType&&) = default;
					ForElementType& operator=(const ForElementType&) = default;

				public:
					inline const ElementType* GetAllocation() const { return SecondaryData ? SecondaryData : reinterpret_cast<const ElementType*>(&InlineData); }

					inline uint32 GetNumInlineBytes() const { return NumInlineElements; }
				};
			};

			class FBitArray
			{
			protected:
				static constexpr int32 NumBitsPerDWORD = 32;
				static constexpr int32 NumBitsPerDWORDLogTwo = 5;

			private:
				TInlineAllocator<4>::ForElementType<int32> Data;
				int32 NumBits;
				int32 MaxBits;

			public:
				FBitArray()
					: NumBits(0), MaxBits(Data.GetNumInlineBytes()* NumBitsPerDWORD)
				{
				}

				FBitArray(const FBitArray&) = default;

				FBitArray(FBitArray&&) = default;

			public:
				FBitArray& operator=(FBitArray&&) = default;

				FBitArray& operator=(const FBitArray& Other) = default;

			private:
				inline void VerifyIndex(int32 Index) const { if (!IsValidIndex(Index)) throw std::out_of_range("Index was out of range!"); }

			public:
				inline int32 Num() const { return NumBits; }
				inline int32 Max() const { return MaxBits; }

				inline const uint32* GetData() const { return reinterpret_cast<const uint32*>(Data.GetAllocation()); }

				inline bool IsValidIndex(int32 Index) const { return Index >= 0 && Index < NumBits; }

				inline bool IsValid() const { return GetData() && NumBits > 0; }

			public:
				inline bool operator[](int32 Index) const { VerifyIndex(Index); return GetData()[Index / NumBitsPerDWORD] & (1 << (Index & (NumBitsPerDWORD - 1))); }

				inline bool operator==(const FBitArray& Other) const { return NumBits == Other.NumBits && GetData() == Other.GetData(); }
				inline bool operator!=(const FBitArray& Other) const { return NumBits != Other.NumBits || GetData() != Other.GetData(); }

			public:
				friend Iterators::FSetBitIterator begin(const FBitArray& Array);
				friend Iterators::FSetBitIterator end(const FBitArray& Array);
			};

			template<typename SparseArrayType>
			union TSparseArrayElementOrFreeListLink
			{
				SparseArrayType ElementData;

				struct
				{
					int32 PrevFreeIndex;
					int32 NextFreeIndex;
				};
			};

			template<typename SetType>
			class SetElement
			{
			private:
				template<typename SetDataType>
				friend class TSet;

			private:
				SetType Value;
				int32 HashNextId;
				int32 HashIndex;
			};
		}


		template <typename KeyType, typename ValueType>
		class TPair
		{
		private:
			KeyType First;
			ValueType Second;

		public:
			TPair(KeyType Key, ValueType Value)
				: First(Key), Second(Value)
			{
			}

		public:
			inline       KeyType& Key() { return First; }
			inline const KeyType& Key() const { return First; }

			inline       ValueType& Value() { return Second; }
			inline const ValueType& Value() const { return Second; }
		};

		template<typename ArrayElementType>
		class TArray
		{
		private:
			template<typename ArrayElementType>
			friend class TAllocatedArray;

			template<typename SparseArrayElementType>
			friend class TSparseArray;

		protected:
			static constexpr uint64 ElementAlign = alignof(ArrayElementType);
			static constexpr uint64 ElementSize = sizeof(ArrayElementType);

		public:
			ArrayElementType* Data;
			int32 NumElements;
			int32 MaxElements;

			TArray()
				: Data(nullptr), NumElements(0), MaxElements(0)
			{
			}

			TArray(const TArray&) = default;

			TArray(TArray&&) = default;

		public:
			TArray& operator=(TArray&&) = default;
			TArray& operator=(const TArray&) = default;

		private:
			inline int32 GetSlack() const { return MaxElements - NumElements; }

			inline void VerifyIndex(int32 Index) const { if (!IsValidIndex(Index)) throw std::out_of_range("Index was out of range!"); }

			inline       ArrayElementType& GetUnsafe(int32 Index) { return Data[Index]; }
			inline const ArrayElementType& GetUnsafe(int32 Index) const { return Data[Index]; }

		public:
			inline void Reserve(const int Num)
			{
				Data = (ArrayElementType*)realloc(Data, (MaxElements = Num + NumElements) * sizeof(ArrayElementType));
			}

			inline ArrayElementType& Add(const ArrayElementType& InData)
			{
				Reserve(1);

				Data[NumElements] = InData;
				++NumElements;

				return Data[NumElements - 1];
			}

			inline void Remove(int Index)
			{
				TArray<ArrayElementType> NewArray;
				for (int i = 0; i < this->NumElements; i++)
				{
					if (i == Index)
						continue;

					NewArray.Add(this->Data[i]);
				}
				this->Data = (ArrayElementType*)realloc(NewArray.Data, sizeof(ArrayElementType) * (NewArray.NumElements));
				this->NumElements = NewArray.NumElements;
				this->MaxElements = NewArray.NumElements;
			}

			inline void Clear()
			{
				NumElements = 0;

				if (!Data)
					memset(Data, 0, NumElements * ElementSize);
			}

		public:
			inline int32 Num() const { return NumElements; }
			inline int32 Max() const { return MaxElements; }

			inline bool IsValidIndex(int32 Index) const { return Data && Index >= 0 && Index < NumElements; }

			inline bool IsValid() const { return Data && NumElements > 0 && MaxElements >= NumElements; }

		public:
			inline       ArrayElementType& operator[](int32 Index) { VerifyIndex(Index); return Data[Index]; }
			inline const ArrayElementType& operator[](int32 Index) const { VerifyIndex(Index); return Data[Index]; }

			inline bool operator==(const TArray<ArrayElementType>& Other) const { return Data == Other.Data; }
			inline bool operator!=(const TArray<ArrayElementType>& Other) const { return Data != Other.Data; }

			inline explicit operator bool() const { return IsValid(); };

		public:
			template<typename T> friend Iterators::TArrayIterator<T> begin(const TArray& Array);
			template<typename T> friend Iterators::TArrayIterator<T> end(const TArray& Array);
		};

		class FString : public TArray<wchar_t>
		{
		public:
			using TArray::TArray;

			FString(const wchar_t* Str)
			{
				const uint32 NullTerminatedLength = static_cast<uint32>(wcslen(Str) + 0x1);

				Data = const_cast<wchar_t*>(Str);
				NumElements = NullTerminatedLength;
				MaxElements = NullTerminatedLength;
			}

		public:
			inline std::string ToString() const
			{
				if (*this)
				{
					std::wstring WData(Data);
#pragma warning(suppress: 4244)
					return std::string(WData.begin(), WData.end());
				}

				return "";
			}

			inline std::wstring ToWString() const
			{
				if (*this)
					return std::wstring(Data);

				return L"";
			}

		public:
			inline       wchar_t* CStr() { return Data; }
			inline const wchar_t* CStr() const { return Data; }

		public:
			inline bool operator==(const FString& Other) const { return Other ? NumElements == Other.NumElements && wcscmp(Data, Other.Data) == 0 : false; }
			inline bool operator!=(const FString& Other) const { return Other ? NumElements != Other.NumElements || wcscmp(Data, Other.Data) != 0 : true; }
		};
		template<typename ArrayElementType>
		class TAllocatedArray : public TArray<ArrayElementType>
		{
		public:
			TAllocatedArray() = delete;

		public:
			TAllocatedArray(int32 Size)
			{
				this->Data = static_cast<ArrayElementType*>(malloc(Size * sizeof(ArrayElementType)));
				this->NumElements = 0x0;
				this->MaxElements = Size;
			}

			~TAllocatedArray()
			{
				if (this->Data)
					free(this->Data);

				this->NumElements = 0x0;
				this->MaxElements = 0x0;
			}

		public:
			inline operator TArray<ArrayElementType>() { return *reinterpret_cast<TArray<ArrayElementType>*>(this); }
			inline operator const TArray<ArrayElementType>() const { return *reinterpret_cast<const TArray<ArrayElementType>*>(this); }
		};
		class FAllocatedString : public FString
		{
		public:
			FAllocatedString() = delete;

		public:
			FAllocatedString(int32 Size)
			{
				Data = static_cast<wchar_t*>(malloc(Size * sizeof(wchar_t)));
				NumElements = 0x0;
				MaxElements = Size;
			}

			~FAllocatedString()
			{
				if (Data)
					free(Data);

				NumElements = 0x0;
				MaxElements = 0x0;
			}

		public:
			inline operator FString() { return *reinterpret_cast<FString*>(this); }
			inline operator const FString() const { return *reinterpret_cast<const FString*>(this); }
		};
		template<typename SparseArrayElementType>
		class TSparseArray
		{
		private:
			static constexpr uint32 ElementAlign = alignof(SparseArrayElementType);
			static constexpr uint32 ElementSize = sizeof(SparseArrayElementType);

		private:
			using FElementOrFreeListLink = ContainerImpl::TSparseArrayElementOrFreeListLink<ContainerImpl::TAlignedBytes<ElementSize, ElementAlign>>;

		private:
			TArray<FElementOrFreeListLink> Data;
			ContainerImpl::FBitArray AllocationFlags;
			int32 FirstFreeIndex;
			int32 NumFreeIndices;

		public:
			TSparseArray()
				: FirstFreeIndex(-1), NumFreeIndices(0)
			{
			}

			TSparseArray(TSparseArray&&) = default;
			TSparseArray(const TSparseArray&) = default;

		public:
			TSparseArray& operator=(TSparseArray&&) = default;
			TSparseArray& operator=(const TSparseArray&) = default;

		private:
			inline void VerifyIndex(int32 Index) const { if (!IsValidIndex(Index)) throw std::out_of_range("Index was out of range!"); }

		public:
			inline int32 NumAllocated() const { return Data.Num(); }

			inline int32 Num() const { return NumAllocated() - NumFreeIndices; }
			inline int32 Max() const { return Data.Max(); }

			inline bool IsValidIndex(int32 Index) const { return Data.IsValidIndex(Index) && AllocationFlags[Index]; }

			inline bool IsValid() const { return Data.IsValid() && AllocationFlags.IsValid(); }

		public:
			const ContainerImpl::FBitArray& GetAllocationFlags() const { return AllocationFlags; }

		public:
			inline       SparseArrayElementType& operator[](int32 Index) { VerifyIndex(Index); return *reinterpret_cast<SparseArrayElementType*>(&Data.GetUnsafe(Index).ElementData); }
			inline const SparseArrayElementType& operator[](int32 Index) const { VerifyIndex(Index); return *reinterpret_cast<SparseArrayElementType*>(&Data.GetUnsafe(Index).ElementData); }

			inline bool operator==(const TSparseArray<SparseArrayElementType>& Other) const { return Data == Other.Data; }
			inline bool operator!=(const TSparseArray<SparseArrayElementType>& Other) const { return Data != Other.Data; }

		public:
			template<typename T> friend Iterators::TSparseArrayIterator<T> begin(const TSparseArray& Array);
			template<typename T> friend Iterators::TSparseArrayIterator<T> end(const TSparseArray& Array);
		};

		template<typename SetElementType>
		class TSet
		{
		private:
			static constexpr uint32 ElementAlign = alignof(SetElementType);
			static constexpr uint32 ElementSize = sizeof(SetElementType);

		private:
			using SetDataType = ContainerImpl::SetElement<SetElementType>;
			using HashType = ContainerImpl::TInlineAllocator<1>::ForElementType<int32>;

		private:
			TSparseArray<SetDataType> Elements;
			HashType Hash;
			int32 HashSize;

		public:
			TSet()
				: HashSize(0)
			{
			}

			TSet(TSet&&) = default;
			TSet(const TSet&) = default;

		public:
			TSet& operator=(TSet&&) = default;
			TSet& operator=(const TSet&) = default;

		private:
			inline void VerifyIndex(int32 Index) const { if (!IsValidIndex(Index)) throw std::out_of_range("Index was out of range!"); }

		public:
			inline int32 NumAllocated() const { return Elements.NumAllocated(); }

			inline int32 Num() const { return Elements.Num(); }
			inline int32 Max() const { return Elements.Max(); }

			inline bool IsValidIndex(int32 Index) const { return Elements.IsValidIndex(Index); }

			inline bool IsValid() const { return Elements.IsValid(); }

		public:
			const ContainerImpl::FBitArray& GetAllocationFlags() const { return Elements.GetAllocationFlags(); }

		public:
			inline       SetElementType& operator[] (int32 Index) { return Elements[Index].Value; }
			inline const SetElementType& operator[] (int32 Index) const { return Elements[Index].Value; }

			inline bool operator==(const TSet<SetElementType>& Other) const { return Elements == Other.Elements; }
			inline bool operator!=(const TSet<SetElementType>& Other) const { return Elements != Other.Elements; }

		public:
			template<typename T> friend Iterators::TSetIterator<T> begin(const TSet& Set);
			template<typename T> friend Iterators::TSetIterator<T> end(const TSet& Set);
		};

		template<typename KeyElementType, typename ValueElementType>
		class TMap
		{
		public:
			using ElementType = TPair<KeyElementType, ValueElementType>;

		private:
			TSet<ElementType> Elements;

		private:
			inline void VerifyIndex(int32 Index) const { if (!IsValidIndex(Index)) throw std::out_of_range("Index was out of range!"); }

		public:
			inline int32 NumAllocated() const { return Elements.NumAllocated(); }

			inline int32 Num() const { return Elements.Num(); }
			inline int32 Max() const { return Elements.Max(); }

			inline bool IsValidIndex(int32 Index) const { return Elements.IsValidIndex(Index); }

			inline bool IsValid() const { return Elements.IsValid(); }

		public:
			const ContainerImpl::FBitArray& GetAllocationFlags() const { return Elements.GetAllocationFlags(); }

		public:
			inline decltype(auto) Find(const KeyElementType& Key, bool(*Equals)(const KeyElementType& LeftKey, const KeyElementType& RightKey))
			{
				for (auto It = begin(*this); It != end(*this); ++It)
				{
					if (Equals(It->Key(), Key))
						return It;
				}

				return end(*this);
			}

		public:
			inline       ElementType& operator[] (int32 Index) { return Elements[Index]; }
			inline const ElementType& operator[] (int32 Index) const { return Elements[Index]; }

			inline bool operator==(const TMap<KeyElementType, ValueElementType>& Other) const { return Elements == Other.Elements; }
			inline bool operator!=(const TMap<KeyElementType, ValueElementType>& Other) const { return Elements != Other.Elements; }

		public:
			template<typename KeyType, typename ValueType> friend Iterators::TMapIterator<KeyType, ValueType> begin(const TMap& Map);
			template<typename KeyType, typename ValueType> friend Iterators::TMapIterator<KeyType, ValueType> end(const TMap& Map);
		};

		namespace Iterators
		{
			class FRelativeBitReference
			{
			protected:
				static constexpr int32 NumBitsPerDWORD = 32;
				static constexpr int32 NumBitsPerDWORDLogTwo = 5;

			public:
				inline explicit FRelativeBitReference(int32 BitIndex)
					: WordIndex(BitIndex >> NumBitsPerDWORDLogTwo)
					, Mask(1 << (BitIndex & (NumBitsPerDWORD - 1)))
				{
				}

				int32  WordIndex;
				uint32 Mask;
			};

			class FSetBitIterator : public FRelativeBitReference
			{
			private:
				const ContainerImpl::FBitArray& Array;

				uint32 UnvisitedBitMask;
				int32 CurrentBitIndex;
				int32 BaseBitIndex;

			public:
				explicit FSetBitIterator(const ContainerImpl::FBitArray& InArray, int32 StartIndex = 0)
					: FRelativeBitReference(StartIndex)
					, Array(InArray)
					, UnvisitedBitMask((~0U) << (StartIndex & (NumBitsPerDWORD - 1)))
					, CurrentBitIndex(StartIndex)
					, BaseBitIndex(StartIndex & ~(NumBitsPerDWORD - 1))
				{
					if (StartIndex != Array.Num())
						FindFirstSetBit();
				}

			public:
				inline FSetBitIterator& operator++()
				{
					UnvisitedBitMask &= ~this->Mask;

					FindFirstSetBit();

					return *this;
				}

				inline explicit operator bool() const { return CurrentBitIndex < Array.Num(); }

				inline bool operator==(const FSetBitIterator& Rhs) const { return CurrentBitIndex == Rhs.CurrentBitIndex && &Array == &Rhs.Array; }
				inline bool operator!=(const FSetBitIterator& Rhs) const { return CurrentBitIndex != Rhs.CurrentBitIndex || &Array != &Rhs.Array; }

			public:
				inline int32 GetIndex() { return CurrentBitIndex; }

				void FindFirstSetBit()
				{
					const uint32* ArrayData = Array.GetData();
					const int32   ArrayNum = Array.Num();
					const int32   LastWordIndex = (ArrayNum - 1) / NumBitsPerDWORD;

					uint32 RemainingBitMask = ArrayData[this->WordIndex] & UnvisitedBitMask;
					while (!RemainingBitMask)
					{
						++this->WordIndex;
						BaseBitIndex += NumBitsPerDWORD;
						if (this->WordIndex > LastWordIndex)
						{
							CurrentBitIndex = ArrayNum;
							return;
						}

						RemainingBitMask = ArrayData[this->WordIndex];
						UnvisitedBitMask = ~0;
					}

					const uint32 NewRemainingBitMask = RemainingBitMask & (RemainingBitMask - 1);

					this->Mask = NewRemainingBitMask ^ RemainingBitMask;

					CurrentBitIndex = BaseBitIndex + NumBitsPerDWORD - 1 - ContainerImpl::HelperFunctions::CountLeadingZeros(this->Mask);

					if (CurrentBitIndex > ArrayNum)
						CurrentBitIndex = ArrayNum;
				}
			};

			template<typename ArrayType>
			class TArrayIterator
			{
			private:
				TArray<ArrayType>& IteratedArray;
				int32 Index;

			public:
				TArrayIterator(const TArray<ArrayType>& Array, int32 StartIndex = 0x0)
					: IteratedArray(const_cast<TArray<ArrayType>&>(Array)), Index(StartIndex)
				{
				}

			public:
				inline int32 GetIndex() { return Index; }

				inline int32 IsValid() { return IteratedArray.IsValidIndex(GetIndex()); }

			public:
				inline TArrayIterator& operator++() { ++Index; return *this; }
				inline TArrayIterator& operator--() { --Index; return *this; }

				inline       ArrayType& operator*() { return IteratedArray[GetIndex()]; }
				inline const ArrayType& operator*() const { return IteratedArray[GetIndex()]; }

				inline       ArrayType* operator->() { return &IteratedArray[GetIndex()]; }
				inline const ArrayType* operator->() const { return &IteratedArray[GetIndex()]; }

				inline bool operator==(const TArrayIterator& Other) const { return &IteratedArray == &Other.IteratedArray && Index == Other.Index; }
				inline bool operator!=(const TArrayIterator& Other) const { return &IteratedArray != &Other.IteratedArray || Index != Other.Index; }
			};

			template<class ContainerType>
			class TContainerIterator
			{
			private:
				ContainerType& IteratedContainer;
				FSetBitIterator BitIterator;

			public:
				TContainerIterator(const ContainerType& Container, const ContainerImpl::FBitArray& BitArray, int32 StartIndex = 0x0)
					: IteratedContainer(const_cast<ContainerType&>(Container)), BitIterator(BitArray, StartIndex)
				{
				}

			public:
				inline int32 GetIndex() { return BitIterator.GetIndex(); }

				inline int32 IsValid() { return IteratedContainer.IsValidIndex(GetIndex()); }

			public:
				inline TContainerIterator& operator++() { ++BitIterator; return *this; }
				inline TContainerIterator& operator--() { --BitIterator; return *this; }

				inline       auto& operator*() { return IteratedContainer[GetIndex()]; }
				inline const auto& operator*() const { return IteratedContainer[GetIndex()]; }

				inline       auto* operator->() { return &IteratedContainer[GetIndex()]; }
				inline const auto* operator->() const { return &IteratedContainer[GetIndex()]; }

				inline bool operator==(const TContainerIterator& Other) const { return &IteratedContainer == &Other.IteratedContainer && BitIterator == Other.BitIterator; }
				inline bool operator!=(const TContainerIterator& Other) const { return &IteratedContainer != &Other.IteratedContainer || BitIterator != Other.BitIterator; }
			};
		}

		inline Iterators::FSetBitIterator begin(const ContainerImpl::FBitArray& Array) { return Iterators::FSetBitIterator(Array, 0); }
		inline Iterators::FSetBitIterator end(const ContainerImpl::FBitArray& Array) { return Iterators::FSetBitIterator(Array, Array.Num()); }

		template<typename T> inline Iterators::TArrayIterator<T> begin(const TArray<T>& Array) { return Iterators::TArrayIterator<T>(Array, 0); }
		template<typename T> inline Iterators::TArrayIterator<T> end(const TArray<T>& Array) { return Iterators::TArrayIterator<T>(Array, Array.Num()); }

		template<typename T> inline Iterators::TSparseArrayIterator<T> begin(const TSparseArray<T>& Array) { return Iterators::TSparseArrayIterator<T>(Array, Array.GetAllocationFlags(), 0); }
		template<typename T> inline Iterators::TSparseArrayIterator<T> end(const TSparseArray<T>& Array) { return Iterators::TSparseArrayIterator<T>(Array, Array.GetAllocationFlags(), Array.NumAllocated()); }

		template<typename T> inline Iterators::TSetIterator<T> begin(const TSet<T>& Set) { return Iterators::TSetIterator<T>(Set, Set.GetAllocationFlags(), 0); }
		template<typename T> inline Iterators::TSetIterator<T> end(const TSet<T>& Set) { return Iterators::TSetIterator<T>(Set, Set.GetAllocationFlags(), Set.NumAllocated()); }

		template<typename T0, typename T1> inline Iterators::TMapIterator<T0, T1> begin(const TMap<T0, T1>& Map) { return Iterators::TMapIterator<T0, T1>(Map, Map.GetAllocationFlags(), 0); }
		template<typename T0, typename T1> inline Iterators::TMapIterator<T0, T1> end(const TMap<T0, T1>& Map) { return Iterators::TMapIterator<T0, T1>(Map, Map.GetAllocationFlags(), Map.NumAllocated()); }


		struct FVector
		{
		public:
			using UnderlayingType = float;

			float                                         X;
			float                                         Y;
			float                                         Z;

		public:
			FVector& Normalize()
			{
				*this /= Magnitude();
				return *this;
			}
			FVector& operator*=(const FVector& Other)
			{
				*this = *this * Other;
				return *this;
			}
			FVector& operator*=(float Scalar)
			{
				*this = *this * Scalar;
				return *this;
			}
			FVector& operator+=(const FVector& Other)
			{
				*this = *this + Other;
				return *this;
			}
			FVector& operator-=(const FVector& Other)
			{
				*this = *this - Other;
				return *this;
			}
			FVector& operator/=(const FVector& Other)
			{
				*this = *this / Other;
				return *this;
			}
			FVector& operator/=(float Scalar)
			{
				*this = *this / Scalar;
				return *this;
			}

			UnderlayingType Dot(const FVector& Other) const
			{
				return (X * Other.X) + (Y * Other.Y) + (Z * Other.Z);
			}
			UnderlayingType GetDistanceTo(const FVector& Other) const
			{
				FVector DiffVector = Other - *this;
				return DiffVector.Magnitude();
			}
			UnderlayingType GetDistanceToInMeters(const FVector& Other) const
			{
				return GetDistanceTo(Other) * 0.01;
			}
			FVector GetNormalized() const
			{
				return *this / Magnitude();
			}
			bool IsZero() const
			{
				return X == 0.0 && Y == 0.0 && Z == 0.0;
			}
			UnderlayingType Magnitude() const
			{
				return std::sqrt((X * X) + (Y * Y) + (Z * Z));
			}
			bool operator!=(const FVector& Other) const
			{
				return X != Other.X || Y != Other.Y || Z != Other.Z;
			}
			FVector operator*(const FVector& Other) const
			{
				return { X * Other.X, Y * Other.Y, Z * Other.Z };
			}
			FVector operator*(float Scalar) const
			{
				return { X * Scalar, Y * Scalar, Z * Scalar };
			}
			FVector operator+(const FVector& Other) const
			{
				return { X + Other.X, Y + Other.Y, Z + Other.Z };
			}
			FVector operator-(const FVector& Other) const
			{
				return { X - Other.X, Y - Other.Y, Z - Other.Z };
			}
			FVector operator/(const FVector& Other) const
			{
				if (Other.X == 0.0f || Other.Y == 0.0f || Other.Z == 0.0f)
					return *this;

				return { X / Other.X, Y / Other.Y, Z / Other.Z };
			}
			FVector operator/(float Scalar) const
			{
				if (Scalar == 0.0f)
					return *this;

				return { X / Scalar, Y / Scalar, Z / Scalar };
			}
			bool operator==(const FVector& Other) const
			{
				return X == Other.X && Y == Other.Y && Z == Other.Z;
			}
		};

		struct FRotator final
		{
		public:
			float                                         Pitch;
			float                                         Yaw;
			float                                         Roll;
		};

		struct FQuat final
		{
		public:
			float                                         X;                                                 // 0x0000(0x0004)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			float                                         Y;                                                 // 0x0004(0x0004)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			float                                         Z;                                                 // 0x0008(0x0004)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			float                                         W;                                                 // 0x000C(0x0004)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		};

		struct FTransform final
		{
		public:
			struct FQuat                                  Rotation;                                          // 0x0000(0x0010)(Edit, BlueprintVisible, SaveGame, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
			struct FVector                                Translation;                                       // 0x0010(0x000C)(Edit, BlueprintVisible, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8                                         Pad_16[0x4];                                       // 0x001C(0x0004)(Fixing Size After Last Property [ Dumper-7 ])
			struct FVector                                Scale3D;                                           // 0x0020(0x000C)(Edit, BlueprintVisible, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8                                         Pad_17[0x4];                                       // 0x002C(0x0004)(Fixing Struct Size After Last Property [ Dumper-7 ])
		};

		struct FGuid final
		{
		public:
			int32                                         A;                                                 // 0x0000(0x0004)(Edit, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			int32                                         B;                                                 // 0x0004(0x0004)(Edit, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			int32                                         C;                                                 // 0x0008(0x0004)(Edit, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			int32                                         D;                                                 // 0x000C(0x0004)(Edit, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		};

		struct FVector_NetQuantize10 : public FVector
		{
		public:
		};
	}
}