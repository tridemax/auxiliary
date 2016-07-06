#pragma once


namespace std
{
	//-------------------------------------------------------------------------------------------------
	/// int24_t
	//-------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
	class int24_t
	{
	public:
		byte						m_internal[3];

	public:
		constexpr int24_t() : m_internal{ 0u, 0u, 0u }
		{
		}

		constexpr int24_t(int32_t input) : m_internal { static_cast<byte>(input), static_cast<byte>(input >> 8), static_cast<byte>(input >> 16) }
		{
		}

		constexpr int24_t(float input) : int24_t(static_cast<int32_t>(input))
		{
		}

		constexpr int24_t(double input) : int24_t(static_cast<int32_t>(input))
		{
		}

		forceinline int24_t& operator= (int32_t input)
		{
			m_internal[0] = static_cast<byte>(input);
			m_internal[1] = static_cast<byte>(input >> 8);
			m_internal[2] = static_cast<byte>(input >> 16);

			return *this;
		}

		forceinline int24_t& operator =(float input)
		{
			return operator=(static_cast<int32_t>(input));
		}

		forceinline int24_t& operator =(double input)
		{
			return operator=(static_cast<int32_t>(input));
		}

		constexpr int32_t scaleUpIntoInt32() const
		{
			return (m_internal[2] << 24) | (m_internal[1] << 16) | (m_internal[0] << 8) | (~(static_cast<int8_t>(m_internal[2]) >> 7) & 0x000000FF);
		}

		constexpr operator int32_t() const
		{
			return ((static_cast<int8_t>(m_internal[2]) >> 7) << 24) | (m_internal[2] << 16) | (m_internal[1] << 8) | m_internal[0];
		}

		constexpr operator float() const
		{
			return static_cast<float>(operator int32_t());
		}

		constexpr operator double() const
		{
			return static_cast<double>(operator int32_t());
		}
	};
#pragma pack(pop)

	static_assert(sizeof(int24_t) == 3u, "Size of \"int24_t\" class not equal to 24 bit.");

	//-------------------------------------------------------------------------------------------------
	/// Limits
	//-------------------------------------------------------------------------------------------------
	template <>
	constexpr int24_t numeric_limits<int24_t>::min() noexcept
	{
		return -0x7FFFFF - 1;
	}

	template <>
	constexpr int24_t numeric_limits<int24_t>::max() noexcept
	{
		return 0x7FFFFF;
	}
}

using std::int24_t;

namespace aux
{
	//-------------------------------------------------------------------------------------------------
	/// DummyHash
	//-------------------------------------------------------------------------------------------------
	template <class KeyType>
	class DummyHash : public std::unary_function<KeyType, std::size_t>
	{
	public:
		forceinline size_t operator() (KeyType keyValue) const
		{
			return keyValue;
		}
	};

	//-------------------------------------------------------------------------------------------------
	/// type_classifier
	//-------------------------------------------------------------------------------------------------
	enum class TypeClass : uint32_t
	{
		SignedInteger = 0u,
		Float,
		Other
	};

	template <typename input_t, typename = void>
	class type_classifier
	{
	public:
		static constexpr TypeClass	this_class = TypeClass::Other;
	};

	template <typename input_t>
	class type_classifier<input_t, typename std::enable_if<std::is_integral<input_t>::value && std::is_signed<input_t>::value>::type>
	{
	public:
		static constexpr TypeClass	this_class = TypeClass::SignedInteger;
	};

	template <typename input_t>
	class type_classifier<input_t, typename std::enable_if<std::is_same<input_t, int24_t>::value>::type>
	{
	public:
		static constexpr TypeClass	this_class = TypeClass::SignedInteger;
	};

	template <typename input_t>
	class type_classifier<input_t, typename std::enable_if<std::is_floating_point<input_t>::value>::type>
	{
	public:
		static constexpr TypeClass	this_class = TypeClass::Float;
	};

	//-------------------------------------------------------------------------------------------------
	/// samples_converter
	//-------------------------------------------------------------------------------------------------
	template <typename input_t, typename output_t, TypeClass input_class = type_classifier<input_t>::this_class, TypeClass output_class = type_classifier<output_t>::this_class>
	class samples_converter
	{
	public:
		forceinline static void convert(const void* inputBuffer, output_t* outputBuffer, uint32_t samplesCount)
		{
			static_assert(std::is_same<input_t, output_t>::value, "Invalid input or output type.");

			memcpy(outputBuffer, inputBuffer, samplesCount * sizeof(input_t));
		}
	};

	//-------------------------------------------------------------------------------------------------
	///	int16_t to int32_t
	//-------------------------------------------------------------------------------------------------
	template <>
	class samples_converter<int16_t, int32_t, TypeClass::SignedInteger, TypeClass::SignedInteger>
	{
	public:
		forceinline static void convert(const void* inputBuffer, int32_t* outputBuffer, uint32_t samplesCount)
		{
			const int16_t* inputBufferStart = reinterpret_cast<const int16_t*>(inputBuffer);
			const int16_t* inputBufferEnd = inputBufferStart + samplesCount;

			for (auto* inputIter = inputBufferStart; inputIter != inputBufferEnd; ++inputIter, ++outputBuffer)
			{
				*outputBuffer = (static_cast<int32_t>(*inputIter) << 16) | (~(*inputIter >> 15) & 0x0000FFFF);
			}
		}
	};

	//-------------------------------------------------------------------------------------------------
	///	int16_t to int24_t
	//-------------------------------------------------------------------------------------------------
	template <>
	class samples_converter<int16_t, int24_t, TypeClass::SignedInteger, TypeClass::SignedInteger>
	{
	public:
		forceinline static void convert(const void* inputBuffer, int24_t* outputBuffer, uint32_t samplesCount)
		{
			assert(false);
		}
	};

	//-------------------------------------------------------------------------------------------------
	///	int24_t to int16_t
	//-------------------------------------------------------------------------------------------------
	template <>
	class samples_converter<int24_t, int16_t, TypeClass::SignedInteger, TypeClass::SignedInteger>
	{
	public:
		forceinline static void convert(const void* inputBuffer, int16_t* outputBuffer, uint32_t samplesCount)
		{
			assert(false);
		}
	};

	//-------------------------------------------------------------------------------------------------
	///	int24_t to int32_t
	//-------------------------------------------------------------------------------------------------
	template <>
	class samples_converter<int24_t, int32_t, TypeClass::SignedInteger, TypeClass::SignedInteger>
	{
	public:
		forceinline static void convert(const void* inputBuffer, int32_t* outputBuffer, uint32_t samplesCount)
		{
			const int24_t* inputBufferStart = reinterpret_cast<const int24_t*>(inputBuffer);
			const int24_t* inputBufferEnd = inputBufferStart + samplesCount;

			for (auto* inputIter = inputBufferStart; inputIter != inputBufferEnd; ++inputIter, ++outputBuffer)
			{
				*outputBuffer = inputIter->scaleUpIntoInt32();
			}
		}
	};

	//-------------------------------------------------------------------------------------------------
	///	int32_t to int16_t
	//-------------------------------------------------------------------------------------------------
	template <>
	class samples_converter<int32_t, int16_t, TypeClass::SignedInteger, TypeClass::SignedInteger>
	{
	public:
		forceinline static void convert(const void* inputBuffer, int16_t* outputBuffer, uint32_t samplesCount)
		{
			assert(false);
		}
	};

	//-------------------------------------------------------------------------------------------------
	///	int32_t to int24_t
	//-------------------------------------------------------------------------------------------------
	template <>
	class samples_converter<int32_t, int24_t, TypeClass::SignedInteger, TypeClass::SignedInteger>
	{
	public:
		forceinline static void convert(const void* inputBuffer, int24_t* outputBuffer, uint32_t samplesCount)
		{
			assert(false);
		}
	};

	//-------------------------------------------------------------------------------------------------
	///	int[x]_t to float/double
	//-------------------------------------------------------------------------------------------------
	template <typename input_t, typename output_t>
	class samples_converter<input_t, output_t, TypeClass::SignedInteger, TypeClass::Float>
	{
	private:
		static constexpr output_t	One = static_cast<output_t>(1);
		static constexpr output_t	MaxInput = static_cast<output_t>(std::numeric_limits<input_t>::max());
		static constexpr output_t	Multiplier = One / (MaxInput + One);

	public:
		forceinline static void convert(const void* inputBuffer, output_t* outputBuffer, uint32_t samplesCount)
		{
			const input_t* inputBufferStart = reinterpret_cast<const input_t*>(inputBuffer);
			const input_t* inputBufferEnd = inputBufferStart + samplesCount;

			for (auto* inputIter = inputBufferStart; inputIter != inputBufferEnd; ++inputIter, ++outputBuffer)
			{
				*outputBuffer = static_cast<output_t>(*inputIter) * Multiplier;
			}
		}
	};

	//-------------------------------------------------------------------------------------------------
	///	float/double to int[x]_t
	//-------------------------------------------------------------------------------------------------
	template <typename input_t, typename output_t>
	class samples_converter<input_t, output_t, TypeClass::Float, TypeClass::SignedInteger>
	{
	private:
		static constexpr input_t	One = static_cast<input_t>(1);
		static constexpr input_t	MaxOutput = static_cast<input_t>(std::numeric_limits<output_t>::max());
		static constexpr input_t	Multiplier = One / (MaxOutput + One);

	public:
		forceinline static void convert(const void* inputBuffer, output_t* outputBuffer, uint32_t samplesCount)
		{
			const input_t* inputBufferStart = reinterpret_cast<const input_t*>(inputBuffer);
			const input_t* inputBufferEnd = inputBufferStart + samplesCount;

			for (auto* inputIter = inputBufferStart; inputIter != inputBufferEnd; ++inputIter, ++outputBuffer)
			{
				// g++ don't recognize MaxOutput at link time
#ifdef _WIN32
				*outputBuffer = static_cast<output_t>(std::min(*inputIter * Multiplier, MaxOutput));
#else
				*outputBuffer = static_cast<output_t>(std::min(*inputIter * Multiplier, static_cast<input_t>(std::numeric_limits<output_t>::max())));
#endif
			}
		}
	};

	//-------------------------------------------------------------------------------------------------
	///	float to double
	//-------------------------------------------------------------------------------------------------
	template <>
	class samples_converter<float, double, TypeClass::Float, TypeClass::Float>
	{
	public:
		forceinline static void convert(const void* inputBuffer, double* outputBuffer, uint32_t samplesCount)
		{
			const float* inputBufferStart = reinterpret_cast<const float*>(inputBuffer);
			const float* inputBufferEnd = inputBufferStart + samplesCount;

			for (auto* inputIter = inputBufferStart; inputIter != inputBufferEnd; ++inputIter, ++outputBuffer)
			{
				*outputBuffer = static_cast<double>(*inputIter);
			}
		}
	};

	//-------------------------------------------------------------------------------------------------
	///	double to float
	//-------------------------------------------------------------------------------------------------
	template <>
	class samples_converter<double, float, TypeClass::Float, TypeClass::Float>
	{
	public:
		forceinline static void convert(const void* inputBuffer, float* outputBuffer, uint32_t samplesCount)
		{
			const double* inputBufferStart = reinterpret_cast<const double*>(inputBuffer);
			const double* inputBufferEnd = inputBufferStart + samplesCount;

			for (auto* inputIter = inputBufferStart; inputIter != inputBufferEnd; ++inputIter, ++outputBuffer)
			{
				*outputBuffer = static_cast<float>(*inputIter);
			}
		}
	};

	//-------------------------------------------------------------------------------------------------
	/// Fast itoa implementation.
	/// Last update - Apr 13 2015
	/// Source - https://github.com/miloyip/rapidjson/blob/master/include/rapidjson/internal/itoa.h
	//-------------------------------------------------------------------------------------------------
	inline const char* GetDigitsLut() {
		static const char cDigitsLut[200] = {
			'0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9',
			'1', '0', '1', '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8', '1', '9',
			'2', '0', '2', '1', '2', '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
			'3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3', '7', '3', '8', '3', '9',
			'4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5', '4', '6', '4', '7', '4', '8', '4', '9',
			'5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
			'6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8', '6', '9',
			'7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9',
			'8', '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9',
			'9', '0', '9', '1', '9', '2', '9', '3', '9', '4', '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'
		};
		return cDigitsLut;
	}

	inline char* u32toa(uint32_t value, char* buffer) {
		const char* cDigitsLut = GetDigitsLut();

		if (value < 10000) {
			const uint32_t d1 = (value / 100) << 1;
			const uint32_t d2 = (value % 100) << 1;

			if (value >= 1000)
				*buffer++ = cDigitsLut[d1];
			if (value >= 100)
				*buffer++ = cDigitsLut[d1 + 1];
			if (value >= 10)
				*buffer++ = cDigitsLut[d2];
			*buffer++ = cDigitsLut[d2 + 1];
		}
		else if (value < 100000000) {
			// value = bbbbcccc
			const uint32_t b = value / 10000;
			const uint32_t c = value % 10000;

			const uint32_t d1 = (b / 100) << 1;
			const uint32_t d2 = (b % 100) << 1;

			const uint32_t d3 = (c / 100) << 1;
			const uint32_t d4 = (c % 100) << 1;

			if (value >= 10000000)
				*buffer++ = cDigitsLut[d1];
			if (value >= 1000000)
				*buffer++ = cDigitsLut[d1 + 1];
			if (value >= 100000)
				*buffer++ = cDigitsLut[d2];
			*buffer++ = cDigitsLut[d2 + 1];

			*buffer++ = cDigitsLut[d3];
			*buffer++ = cDigitsLut[d3 + 1];
			*buffer++ = cDigitsLut[d4];
			*buffer++ = cDigitsLut[d4 + 1];
		}
		else {
			// value = aabbbbcccc in decimal

			const uint32_t a = value / 100000000; // 1 to 42
			value %= 100000000;

			if (a >= 10) {
				const unsigned i = a << 1;
				*buffer++ = cDigitsLut[i];
				*buffer++ = cDigitsLut[i + 1];
			}
			else
				*buffer++ = static_cast<char>('0' + static_cast<char>(a));

			const uint32_t b = value / 10000; // 0 to 9999
			const uint32_t c = value % 10000; // 0 to 9999

			const uint32_t d1 = (b / 100) << 1;
			const uint32_t d2 = (b % 100) << 1;

			const uint32_t d3 = (c / 100) << 1;
			const uint32_t d4 = (c % 100) << 1;

			*buffer++ = cDigitsLut[d1];
			*buffer++ = cDigitsLut[d1 + 1];
			*buffer++ = cDigitsLut[d2];
			*buffer++ = cDigitsLut[d2 + 1];
			*buffer++ = cDigitsLut[d3];
			*buffer++ = cDigitsLut[d3 + 1];
			*buffer++ = cDigitsLut[d4];
			*buffer++ = cDigitsLut[d4 + 1];
		}
		return buffer;
	}

	inline char* i32toa(int32_t value, char* buffer) {
		uint32_t u = static_cast<uint32_t>(value);
		if (value < 0) {
			*buffer++ = '-';
			u = ~u + 1;
		}

		return u32toa(u, buffer);
	}

	inline char* u64toa(uint64_t value, char* buffer) {
		const char* cDigitsLut = GetDigitsLut();
		const uint64_t  kTen8 = 100000000;
		const uint64_t  kTen9 = kTen8 * 10;
		const uint64_t kTen10 = kTen8 * 100;
		const uint64_t kTen11 = kTen8 * 1000;
		const uint64_t kTen12 = kTen8 * 10000;
		const uint64_t kTen13 = kTen8 * 100000;
		const uint64_t kTen14 = kTen8 * 1000000;
		const uint64_t kTen15 = kTen8 * 10000000;
		const uint64_t kTen16 = kTen8 * kTen8;

		if (value < kTen8) {
			uint32_t v = static_cast<uint32_t>(value);
			if (v < 10000) {
				const uint32_t d1 = (v / 100) << 1;
				const uint32_t d2 = (v % 100) << 1;

				if (v >= 1000)
					*buffer++ = cDigitsLut[d1];
				if (v >= 100)
					*buffer++ = cDigitsLut[d1 + 1];
				if (v >= 10)
					*buffer++ = cDigitsLut[d2];
				*buffer++ = cDigitsLut[d2 + 1];
			}
			else {
				// value = bbbbcccc
				const uint32_t b = v / 10000;
				const uint32_t c = v % 10000;

				const uint32_t d1 = (b / 100) << 1;
				const uint32_t d2 = (b % 100) << 1;

				const uint32_t d3 = (c / 100) << 1;
				const uint32_t d4 = (c % 100) << 1;

				if (value >= 10000000)
					*buffer++ = cDigitsLut[d1];
				if (value >= 1000000)
					*buffer++ = cDigitsLut[d1 + 1];
				if (value >= 100000)
					*buffer++ = cDigitsLut[d2];
				*buffer++ = cDigitsLut[d2 + 1];

				*buffer++ = cDigitsLut[d3];
				*buffer++ = cDigitsLut[d3 + 1];
				*buffer++ = cDigitsLut[d4];
				*buffer++ = cDigitsLut[d4 + 1];
			}
		}
		else if (value < kTen16) {
			const uint32_t v0 = static_cast<uint32_t>(value / kTen8);
			const uint32_t v1 = static_cast<uint32_t>(value % kTen8);

			const uint32_t b0 = v0 / 10000;
			const uint32_t c0 = v0 % 10000;

			const uint32_t d1 = (b0 / 100) << 1;
			const uint32_t d2 = (b0 % 100) << 1;

			const uint32_t d3 = (c0 / 100) << 1;
			const uint32_t d4 = (c0 % 100) << 1;

			const uint32_t b1 = v1 / 10000;
			const uint32_t c1 = v1 % 10000;

			const uint32_t d5 = (b1 / 100) << 1;
			const uint32_t d6 = (b1 % 100) << 1;

			const uint32_t d7 = (c1 / 100) << 1;
			const uint32_t d8 = (c1 % 100) << 1;

			if (value >= kTen15)
				*buffer++ = cDigitsLut[d1];
			if (value >= kTen14)
				*buffer++ = cDigitsLut[d1 + 1];
			if (value >= kTen13)
				*buffer++ = cDigitsLut[d2];
			if (value >= kTen12)
				*buffer++ = cDigitsLut[d2 + 1];
			if (value >= kTen11)
				*buffer++ = cDigitsLut[d3];
			if (value >= kTen10)
				*buffer++ = cDigitsLut[d3 + 1];
			if (value >= kTen9)
				*buffer++ = cDigitsLut[d4];
			if (value >= kTen8)
				*buffer++ = cDigitsLut[d4 + 1];

			*buffer++ = cDigitsLut[d5];
			*buffer++ = cDigitsLut[d5 + 1];
			*buffer++ = cDigitsLut[d6];
			*buffer++ = cDigitsLut[d6 + 1];
			*buffer++ = cDigitsLut[d7];
			*buffer++ = cDigitsLut[d7 + 1];
			*buffer++ = cDigitsLut[d8];
			*buffer++ = cDigitsLut[d8 + 1];
		}
		else {
			const uint32_t a = static_cast<uint32_t>(value / kTen16); // 1 to 1844
			value %= kTen16;

			if (a < 10)
				*buffer++ = static_cast<char>('0' + static_cast<char>(a));
			else if (a < 100) {
				const uint32_t i = a << 1;
				*buffer++ = cDigitsLut[i];
				*buffer++ = cDigitsLut[i + 1];
			}
			else if (a < 1000) {
				*buffer++ = static_cast<char>('0' + static_cast<char>(a / 100));

				const uint32_t i = (a % 100) << 1;
				*buffer++ = cDigitsLut[i];
				*buffer++ = cDigitsLut[i + 1];
			}
			else {
				const uint32_t i = (a / 100) << 1;
				const uint32_t j = (a % 100) << 1;
				*buffer++ = cDigitsLut[i];
				*buffer++ = cDigitsLut[i + 1];
				*buffer++ = cDigitsLut[j];
				*buffer++ = cDigitsLut[j + 1];
			}

			const uint32_t v0 = static_cast<uint32_t>(value / kTen8);
			const uint32_t v1 = static_cast<uint32_t>(value % kTen8);

			const uint32_t b0 = v0 / 10000;
			const uint32_t c0 = v0 % 10000;

			const uint32_t d1 = (b0 / 100) << 1;
			const uint32_t d2 = (b0 % 100) << 1;

			const uint32_t d3 = (c0 / 100) << 1;
			const uint32_t d4 = (c0 % 100) << 1;

			const uint32_t b1 = v1 / 10000;
			const uint32_t c1 = v1 % 10000;

			const uint32_t d5 = (b1 / 100) << 1;
			const uint32_t d6 = (b1 % 100) << 1;

			const uint32_t d7 = (c1 / 100) << 1;
			const uint32_t d8 = (c1 % 100) << 1;

			*buffer++ = cDigitsLut[d1];
			*buffer++ = cDigitsLut[d1 + 1];
			*buffer++ = cDigitsLut[d2];
			*buffer++ = cDigitsLut[d2 + 1];
			*buffer++ = cDigitsLut[d3];
			*buffer++ = cDigitsLut[d3 + 1];
			*buffer++ = cDigitsLut[d4];
			*buffer++ = cDigitsLut[d4 + 1];
			*buffer++ = cDigitsLut[d5];
			*buffer++ = cDigitsLut[d5 + 1];
			*buffer++ = cDigitsLut[d6];
			*buffer++ = cDigitsLut[d6 + 1];
			*buffer++ = cDigitsLut[d7];
			*buffer++ = cDigitsLut[d7 + 1];
			*buffer++ = cDigitsLut[d8];
			*buffer++ = cDigitsLut[d8 + 1];
		}

		return buffer;
	}

	inline char* i64toa(int64_t value, char* buffer) {
		uint64_t u = static_cast<uint64_t>(value);
		if (value < 0) {
			*buffer++ = '-';
			u = ~u + 1;
		}

		return u64toa(u, buffer);
	}
}
