#pragma once


namespace Auxiliary
{
	//-------------------------------------------------------------------------------------------------
	enum class MemoryOwnage : int32_t
	{
		OwnsMemory		= 0,			// container owns it's memory
		ExternalMemory	= -1,			// container uses external memory
		AdaptMemory		= -2,			// container may own or may use external memory
	};

	//-------------------------------------------------------------------------------------------------
	/// FixedArrayBase
	//-------------------------------------------------------------------------------------------------
	template <typename DataType>
	class FixedArrayBase
	{
	protected:
		DataType*					m_value;
		uint32_t					m_size;

	public:
		//---------------------------------------------------------------------------------------------
		inline FixedArrayBase() : m_value(nullptr), m_size(0)
		{
		}

		//---------------------------------------------------------------------------------------------
		inline uint32_t size() const
		{
			return m_size;
		}

		//---------------------------------------------------------------------------------------------
		inline DataType* data()
		{
			return m_value;
		}

		//---------------------------------------------------------------------------------------------
		inline const DataType* data() const
		{
			return m_value;
		}

		//---------------------------------------------------------------------------------------------
		template <typename IndexType>
		inline DataType& at(IndexType index)
		{
			assert(index < static_cast<IndexType>(m_size));
			return m_value[index];
		}

		//---------------------------------------------------------------------------------------------
		template <typename IndexType>
		inline const DataType& at(IndexType index) const
		{
			assert(index < static_cast<IndexType>(m_size));
			return m_value[index];
		}

		//---------------------------------------------------------------------------------------------
		template <typename IndexType>
		inline DataType& operator[] (IndexType index)
		{
			assert(index < static_cast<IndexType>(m_size));
			return m_value[index];
		}

		//---------------------------------------------------------------------------------------------
		template <typename IndexType>
		inline const DataType& operator[] (IndexType index) const
		{
			assert(index < static_cast<IndexType>(m_size));
			return m_value[index];
		}
	};

	//-------------------------------------------------------------------------------------------------
	template <typename DataType, MemoryOwnage MemoryOwnageKind = MemoryOwnage::OwnsMemory>
	class FixedArray { };

	//-------------------------------------------------------------------------------------------------
	template <typename DataType>
	class FixedArray<DataType, MemoryOwnage::OwnsMemory> : public FixedArrayBase<DataType>
	{
	public:
		//---------------------------------------------------------------------------------------------
		inline FixedArray()
		{
		}

		//---------------------------------------------------------------------------------------------
		inline FixedArray(uint32_t size)
		{
			this->m_size = size;
			this->m_value = size ? new DataType[size] : nullptr;
		}

		//---------------------------------------------------------------------------------------------
		inline FixedArray(const FixedArrayBase<DataType>& source)
		{
			this->size = source.size;
			this->m_value = source.size ? new DataType[source.size] : nullptr;

			if (std::is_pod<DataType>::value)
			{
				memcpy_s(this->m_value, sizeof(DataType) * source.size, source.m_value, sizeof(DataType) * source.size);
			}
			else
			{
				for (uint32_t i = 0; i != source.size; ++i)
				{
					this->m_value[i] = source.m_value[i];
				}
			}
		}

		//---------------------------------------------------------------------------------------------
		inline FixedArray(const FixedArray& source) : FixedArray(static_cast<const FixedArrayBase<DataType>&>(source))
		{
		}

		//---------------------------------------------------------------------------------------------
		inline FixedArray(FixedArray&& source)
		{
			this->m_value = source.m_value;
			this->m_size = source.m_size;
			source.m_value = nullptr;
			source.m_size = 0;
		}

		//---------------------------------------------------------------------------------------------
		inline ~FixedArray()
		{
			Deallocate();
		}

		//---------------------------------------------------------------------------------------------
		inline FixedArray& Allocate(uint32_t size)
		{
			if (this->m_size != size)
			{
				if (this->m_value)
				{
					delete[] this->m_value;
				}
				this->m_size = size;
				this->m_value = size ? new DataType[size] : nullptr;
			}

			return *this;
		}

		//---------------------------------------------------------------------------------------------
		inline void Deallocate()
		{
			if (this->m_value)
			{
				delete[] this->m_value;
				this->m_value = nullptr;
			}

			this->m_size = 0;
		}

		//---------------------------------------------------------------------------------------------
		inline bool IsAllocated() const
		{
			return this->m_value != nullptr;
		}

		//---------------------------------------------------------------------------------------------
		inline FixedArray& operator=(const FixedArrayBase<DataType>& source)
		{
			if (this == &source)
			{
				return *this;
			}

			if (this->m_size != source.m_size)
			{
				if (this->m_value)
				{
					delete[] this->m_value;
				}

				this->m_size = source.size;
				this->m_value = source.size ? new DataType[source.size] : nullptr;
			}

			if (std::is_pod<DataType>::value)
			{
				memcpy_s(this->m_value, sizeof(DataType) * source.size, source.m_value, sizeof(DataType) * source.size);
			}
			else
			{
				for (uint32_t i = 0; i != source.size; ++i)
				{
					this->m_value[i] = source.m_value[i];
				}
			}

			return *this;
		}

		//---------------------------------------------------------------------------------------------
		inline FixedArray<DataType>& operator= (const FixedArray<DataType>& source)
		{
			return *this = static_cast<const FixedArrayBase<DataType>&>(source);
		}

		//---------------------------------------------------------------------------------------------
		inline FixedArray& operator= (FixedArray&& source)
		{
			if (this == &source)
			{
				return *this;
			}

			if (this->m_value)
			{
				delete[] this->m_value;
			}

			this->m_value = source.m_value;
			this->m_size = source.m_size;
			source.m_value = nullptr;
			source.m_size = 0;

			return *this;
		}
	};
/*
	//---------------------------------------------------------------------
	template <typename T, typename TAllocator>
	class FixedArray<T, EXT_MEM, TAllocator> : public FixedArrayBase<T>, public ISerializableValue
	{
	public:
		//---------------------------------------------------------------------
		/// Default constructor.
		inline FixedArray() { }

		//---------------------------------------------------------------------
		/// Constructor for external memory version.
		inline FixedArray(T* extMem, u32 extMemSize)
		{
			this->size = extMemSize;
			this->v = extMem;
		}

		//---------------------------------------------------------------------
		/// Constructor for external memory version.
		inline FixedArray(T* extMem, u32 extMemSize, const T& filler)
		{
			this->size = extMemSize;
			this->v = extMem;
			for (u32 i = 0; i < this->size; ++i)
				this->v[i] = filler;
		}

		//---------------------------------------------------------------------
		/// Destructor.
		inline ~FixedArray()
		{
		}

		//---------------------------------------------------------------------
		/// Sets memory for non-owning version.
		inline FixedArray& SetMemory(T* extMem, u32 extMemSize)
		{
			this->v = extMem;
			this->size = extMemSize;
			return *this;
		}

		//---------------------------------------------------------------------
		/// Copies content of another array.
		template <typename TOther, EMemoryOwnage TMemoryOwnageOther, typename TAllocatorOther>
		inline FixedArray& Copy(const FixedArray<TOther, TMemoryOwnageOther, TAllocatorOther>& src)
		{
			return Copy(src.Data(), src.Size());
		}

		//---------------------------------------------------------------------
		/// Copies content of another array.
		template <typename TOther>
		inline FixedArray& Copy(const TOther* src, u32 srcNumElements)
		{
			u32 numToCopy = Min(srcNumElements, this->size);
			if (std::is_pod<TOther>::value && std::is_same<T, TOther>::value)
				memcpy(this->v, src, sizeof(T) * numToCopy);
			else
			{
				for (u32 i = 0; i < numToCopy; ++i)
					this->v[i] = T(src[i]);
			}

			return *this;
		}

		//---------------------------------------------------------------------
		/// Assignment operator copies content of another array.
		template <typename TOther, EMemoryOwnage TMemoryOwnageOther, typename TAllocatorOther>
		inline FixedArray& operator =(const FixedArray<TOther, TMemoryOwnageOther, TAllocatorOther>& src)
		{
			return Copy(src);
		}

		//---------------------------------------------------------------------
		/// Copies pointer from another array.
		inline FixedArray& CopyPointer(const FixedArray& op)
		{
			this->v = op.v;
			this->size = op.size;
			return *this;
		}

		//---------------------------------------------------------------------
		/// Serialization.
		inline bool SerializeValue(ISerialize* ser, Id32 self, ESerializationFlags flags)
		{
			if (ser->IsReading())
			{
				assert(false); // do not use external allocated version of FixedArray for reading into
				return false;
			}

			u32 size = this->Size();
			if (!ser->BeginArray(self, size, flags))
				return false;

			bool result = true;
			for (u32 i = 0; i < this->size; ++i)
				result &= ser->Value(i, this->At(i), SERIALIZE_MANDATORY);

			ser->EndArray();
			return result;
		}

		//---------------------------------------------------------------------
		/// Checks if fixed array is initialized.
		inline operator bool() const
		{
			return this->v != nullptr;
		}
	};

	//---------------------------------------------------------------------
	template <typename T, typename TAllocator>
	class FixedArray<T, ADAPT_MEM, TAllocator> : public FixedArray<T, OWNS_MEM, TAllocator>
	{
		using Base = FixedArray<T, OWNS_MEM, TAllocator>;

	protected:
		bool ownsMem = false;

	protected:
		//---------------------------------------------------------------------
		/// Deallocates own memory.
		inline void InternalDeallocateOwn() { if (ownsMem) { Deallocate(); } }
		inline void InternalSetToOwn() { if (!ownsMem) { this->v = nullptr; this->size = 0; ownsMem = true; } }

	public:
		//---------------------------------------------------------------------
		/// Adapted overloads for owning memory version.
		//---------------------------------------------------------------------
		inline FixedArray() { }
		//---------------------------------------------------------------------
		inline FixedArray(u32 _size) : Base(_size) { ownsMem = true; }
		//---------------------------------------------------------------------
		inline FixedArray(u32 _size, const T& filler) : Base(_size, filler) { ownsMem = true; }
		//---------------------------------------------------------------------
		inline FixedArray(const FixedArray& op) : Base(op) { ownsMem = true; }
		//---------------------------------------------------------------------
		inline FixedArray& operator=(const FixedArray& op) { Base::operator =(op); ownsMem = true; return *this; }
		//---------------------------------------------------------------------
		template <typename F>
		inline FixedArray(const eastl::vector<F>& op) : Base(op) { ownsMem = true; }
		//---------------------------------------------------------------------
		template <typename F>
		inline FixedArray& operator=(const eastl::vector<F>& op) { Base::operator =(op); ownsMem = true; return *this; }
		//---------------------------------------------------------------------
		template <typename F>
		inline FixedArray(const F* begin, const F* end) : Base(begin, end) { ownsMem = true; }
		//---------------------------------------------------------------------
		inline FixedArray(FixedArray&& op) : Base(std::move(op)) { ownsMem = op.ownsMem; }
		//---------------------------------------------------------------------
		inline FixedArray& operator=(FixedArray&& op) { Base::operator =(std::move(op)); ownsMem = op.ownsMem; return *this; }
		//---------------------------------------------------------------------
		template <typename F>
		inline FixedArray(std::initializer_list<F> list) : Base(list.begin(), list.end()) { ownsMem = true; }

		//---------------------------------------------------------------------
		/// Constructor for external memory version.
		inline FixedArray(T* extMem, u32 extMemSize)
		{
			this->size = extMemSize;
			this->v = extMem;
			ownsMem = false;
		}

		//---------------------------------------------------------------------
		/// Constructor for external memory version.
		inline FixedArray(T* extMem, u32 extMemSize, const T& filler)
		{
			this->size = extMemSize;
			this->v = extMem;
			Fill(filler);
			ownsMem = false;
		}

		//---------------------------------------------------------------------
		/// Destructor.
		inline ~FixedArray() { InternalDeallocateOwn(); }

		//---------------------------------------------------------------------
		/// Adapted overloads for owning memory version.
		//---------------------------------------------------------------------
		inline FixedArray& Allocate(u32 _size) { InternalSetToOwn(); Base::Allocate(_size); return *this; }
		//---------------------------------------------------------------------
		inline FixedArray& Allocate(u32 _size, const T& filler) { InternalSetToOwn(); Base::Allocate(_size, filler); return *this; }
		//---------------------------------------------------------------------
		inline FixedArray& Reallocate(u32 _size) { InternalSetToOwn(); Base::Reallocate(_size); return *this; }
		//---------------------------------------------------------------------
		inline FixedArray& Reallocate(u32 _size, const T& filler) { InternalSetToOwn(); Base::Reallocate(_size, filler); return *this; }
		//---------------------------------------------------------------------
		inline void Deallocate() { InternalSetToOwn(); Base::Deallocate(); }
		//---------------------------------------------------------------------
		inline bool IsAllocated() const { if (ownsMem) return this->v != nullptr; return false; }
		//---------------------------------------------------------------------
		inline FixedArray& Swap(FixedArray& op) { std::swap(ownsMem, op.ownsMem); Base::Swap(op); return *this; }
		//---------------------------------------------------------------------
		template <typename F>
		inline FixedArray& Copy(const FixedArrayBase<F>& source) { InternalSetToOwn(); Base::Copy(source); return *this; }
		//---------------------------------------------------------------------
		template <typename F>
		inline FixedArray& Copy(const FixedArrayBase<F>& source, u32 sourceStart, u32 sourceLength) { InternalSetToOwn(); Base::Copy(source, sourceStart, sourceLength); return *this; }
		//---------------------------------------------------------------------
		inline FixedArray& Copy(const T* src, u32 srcNumElements) { InternalSetToOwn(); Base::Copy(src, srcNumElements); }
		//---------------------------------------------------------------------
		template <typename F>
		inline FixedArray& Append(const FixedArrayBase<F>& source) { InternalSetToOwn(); Base::Append(source); return *this; }
		//---------------------------------------------------------------------
		template <typename F>
		inline FixedArray& Append(const FixedArrayBase<F>& source, u32 sourceStart, u32 sourceLength, u32 thisPosition = -1) { InternalSetToOwn(); Base::Append(source, sourceStart, sourceLength, thisPosition); return *this; }

		//---------------------------------------------------------------------
		/// Sets memory for non-owning version.
		inline FixedArray& SetMemory(T* extMem, u32 extMemSize)
		{
			InternalDeallocateOwn();
			this->v = extMem;
			this->size = extMemSize;
			ownsMem = false;

			return *this;
		}
	};
*/
}
