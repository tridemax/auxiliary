#pragma once


namespace aux
{
	//-------------------------------------------------------------------------------------------------
	/// Hash
	//-------------------------------------------------------------------------------------------------
	template <typename HashType>
	class Hash
	{
	private:
		HashType					m_value;

	public:
		//---------------------------------------------------------------------------------------------
		inline Hash() : m_value(1u)
		{
		}

		//---------------------------------------------------------------------------------------------
		inline HashType GetInternalValue() const
		{
			return m_value;
		}

		//---------------------------------------------------------------------------------------------
		inline Hash& Add(const void* buffer, uint64_t bufferSize)
		{
			m_value = Calculate(buffer, bufferSize, m_value);
			return *this;
		}

		//---------------------------------------------------------------------------------------------
		inline Hash& Add(const char* value)
		{
			m_value = Calculate(value, strlen(value), m_value);
			return *this;
		}

		//---------------------------------------------------------------------------------------------
		inline Hash& Add(const std::string& value)
		{
			m_value = Calculate(value.data(), value.size(), m_value);
			return *this;
		}

		//---------------------------------------------------------------------------------------------
		template <typename Hashee>
		inline Hash& Add(const Hashee& value)
		{
			m_value = Calculate(&value, sizeof(Hashee), m_value);
			return *this;
		}

	private:
		//---------------------------------------------------------------------------------------------
		inline static uint32_t rotl32(uint32_t x, int8_t r)
		{
			return (x << r) | (x >> (32 - r));
		}

		//---------------------------------------------------------------------------------------------
		inline static uint64_t rotl64(uint64_t x, int8_t r)
		{
			return (x << r) | (x >> (64 - r));
		}

		//---------------------------------------------------------------------------------------------
		inline static uint32_t fmix32(uint32_t h)
		{
			h ^= h >> 16;
			h *= 0x85ebca6b;
			h ^= h >> 13;
			h *= 0xc2b2ae35;
			h ^= h >> 16;

			return h;
		}

		//---------------------------------------------------------------------------------------------
		inline static uint64_t fmix64(uint64_t k)
		{
			k ^= k >> 33;
			k *= 0xff51afd7ed558ccd;
			k ^= k >> 33;
			k *= 0xc4ceb9fe1a85ec53;
			k ^= k >> 33;

			return k;
		}

		//---------------------------------------------------------------------------------------------
		inline HashType Calculate(const void* buffer, uint64_t bufferLength, HashType startValue) const;
	};

	//-------------------------------------------------------------------------------------------------
	template <>
	inline uint32_t Hash<uint32_t>::Calculate(const void* buf, const uint64_t bufLen, uint32_t startValue) const
	{
		const uint8_t* data = (const uint8_t*)buf;
		const uint64_t nblocks = bufLen / 4;

		uint32_t h1 = startValue;

		const uint32_t c1 = 0xcc9e2d51;
		const uint32_t c2 = 0x1b873593;

		// body
		const uint32_t* blocks = (const uint32_t*)(data);
		for (uint64_t i = 0; i < nblocks; ++i)
		{
			uint32_t k1 = blocks[i];
			
			k1 *= c1;
			k1 = rotl32(k1, 15);
			k1 *= c2;
			
			h1 ^= k1;
			h1 = rotl32(h1, 13); 
			h1 = h1 * 5 + 0xe6546b64;
		}

		// tail
		const uint8_t* tail = (const uint8_t*)(data + nblocks * 4);

		uint32_t k1 = 0;

		switch (bufLen & 3)
		{
		case 3: k1 ^= tail[2] << 16;
		case 2: k1 ^= tail[1] << 8;
		case 1: k1 ^= tail[0];
				k1 *= c1; k1 = rotl32(k1, 15); k1 *= c2; h1 ^= k1;
		};

		// finalization
		h1 ^= (uint32_t)bufLen;
		
		h1 = fmix32(h1);
		
		return h1;
	}

	//-------------------------------------------------------------------------------------------------
	template <>
	inline uint64_t Hash<uint64_t>::Calculate(const void* buf, uint64_t bufLen, uint64_t startValue) const
	{
		const uint8_t* data = (const uint8_t*)buf;
		const uint64_t nblocks = bufLen / 16;

		uint64_t h1 = startValue >> 32;
		uint64_t h2 = startValue & 0xFFFFFFFF;

		uint64_t c1 = 0x87c37b91114253d5;
		uint64_t c2 = 0x4cf5ad432745937f;

		// body
		const uint64_t* blocks = (const uint64_t*)(data);
		for (uint64_t i = 0; i < nblocks; ++i)
		{
			uint64_t k1 = blocks[i * 2 + 0];
			uint64_t k2 = blocks[i * 2 + 1];

			k1 *= c1; k1 = rotl64(k1, 31); k1 *= c2; h1 ^= k1;
			h1 = rotl64(h1, 27); h1 += h2; h1 = h1 * 5 + 0x52dce729;
			k2 *= c2; k2 = rotl64(k2, 33); k2 *= c1; h2 ^= k2;
			h2 = rotl64(h2, 31); h2 += h1; h2 = h2 * 5 + 0x38495ab5;
		}

		// tail
		const uint8_t* tail = (const uint8_t*)(data + nblocks * 16);

		uint64_t k1 = 0;
		uint64_t k2 = 0;

		switch (bufLen & 15)
		{
		case 15: k2 ^= uint64_t(tail[14]) << 48;
		case 14: k2 ^= uint64_t(tail[13]) << 40;
		case 13: k2 ^= uint64_t(tail[12]) << 32;
		case 12: k2 ^= uint64_t(tail[11]) << 24;
		case 11: k2 ^= uint64_t(tail[10]) << 16;
		case 10: k2 ^= uint64_t(tail[ 9]) << 8;
		case  9: k2 ^= uint64_t(tail[ 8]) << 0;
				 k2 *= c2; k2 = rotl64(k2, 33); k2 *= c1; h2 ^= k2;

		case 8: k1 ^= uint64_t(tail[7]) << 56;
		case 7: k1 ^= uint64_t(tail[6]) << 48;
		case 6: k1 ^= uint64_t(tail[5]) << 40;
		case 5: k1 ^= uint64_t(tail[4]) << 32;
		case 4: k1 ^= uint64_t(tail[3]) << 24;
		case 3: k1 ^= uint64_t(tail[2]) << 16;
		case 2: k1 ^= uint64_t(tail[1]) << 8;
		case 1: k1 ^= uint64_t(tail[0]) << 0;
				k1 *= c1; k1 = rotl64(k1,31); k1 *= c2; h1 ^= k1;
		};

		// finalization
		h1 ^= bufLen; h2 ^= bufLen;

		h1 += h2;
		h2 += h1;

		h1 = fmix64(h1);
		h2 = fmix64(h2);

		h1 += h2;
		h2 += h1;

		return h1; // h2 contains bunch of equally mixed bits - can be used to output 128 bit value
	}

	//-------------------------------------------------------------------------------------------------
	inline uint32_t Hash32(const void* buffer, uint64_t bufferLength)
	{
		Hash<uint32_t> hasher;

		return hasher.Add(buffer, bufferLength).GetInternalValue();
	}

	//-------------------------------------------------------------------------------------------------
	inline uint64_t Hash64(const void* buffer, uint64_t bufferLength)
	{
		Hash<uint64_t> hasher;

		return hasher.Add(buffer, bufferLength).GetInternalValue();
	}
}
