#pragma once

#include "IStream.h"
#include "Miscellaneous.h"


namespace Auxiliary
{
	//-------------------------------------------------------------------------------------------------
	/// AbstractValue
	//-------------------------------------------------------------------------------------------------
	enum class AbstractValue : uint32_t
	{
		Unknown = 0u,
		Enum,
		SerializableStruct
	};

	//-------------------------------------------------------------------------------------------------
	/// JsonPrinter
	//-------------------------------------------------------------------------------------------------
	template <class Storage, bool UserReadableOutput = true>
	class JsonPrinter : public boost::noncopyable
	{
	private:
		Storage&					m_storage;
		const uint32_t				m_writeBufferLength;
		char**						m_writeBuffer;
		uint32_t*					m_writeBufferFullness;
		uint32_t					m_levelShift;

	public:
		//---------------------------------------------------------------------------------------------
		inline JsonPrinter(Storage& storage) : m_storage(storage), m_writeBufferLength(storage.GetWriteBufferLength()), m_levelShift(0u)
		{
			m_storage.GetWriteBuffer(m_writeBuffer, m_writeBufferFullness);
		}

		//---------------------------------------------------------------------------------------------
		inline ~JsonPrinter()
		{
		}

		//---------------------------------------------------------------------------------------------
		template <typename ValueType>
		bool RootValue(ValueType& value)
		{
			const bool result = Value(nullptr, value);

			RemoveTrailingComma();

			return result;
		}

		//---------------------------------------------------------------------------------------------
		template <typename ValueType>
		bool Value(const char* key, ValueType& value)
		{
			return CustomSerializer<ValueType,
				std::is_enum<ValueType>::value ? AbstractValue::Enum :
				std::is_base_of<ISerializableStruct, ValueType>::value ? AbstractValue::SerializableStruct :
				AbstractValue::Unknown
				>::Serialize(*this, key, value);
		}

		//---------------------------------------------------------------------------------------------
		template <typename ValueType, size_t ArrayLength>
		bool Value(const char* key, std::array<ValueType, ArrayLength>& value)
		{
			BeginArray(key);

			for (size_t itemIndex = 0u; itemIndex != ArrayLength; ++itemIndex)
			{
				Value(nullptr, value[itemIndex]);
			}

			EndArray();
			PrintChar(',');

			return true;
		}

		//---------------------------------------------------------------------------------------------
		template <typename ValueType>
		bool Value(const char* key, std::vector<ValueType>& value)
		{
			BeginArray(key);

			for (auto& arrayItem : value)
			{
				Value(nullptr, arrayItem);
			}

			EndArray();
			PrintChar(',');

			return true;
		}

		//---------------------------------------------------------------------------------------------
		template <typename ValueType>
		bool Value(const char* key, std::list<ValueType>& value)
		{
			BeginArray(key);

			for (auto& arrayItem : value)
			{
				Value(nullptr, arrayItem);
			}

			EndArray();
			PrintChar(',');

			return true;
		}

		//---------------------------------------------------------------------------------------------
		template <>
		bool Value<byte>(const char* key, byte& value)
		{
			// Print shift
			PrintNewLine<UserReadableOutput>();
			PrintLevelShift<UserReadableOutput>();

			// Print key
			PrintKey(key);

			// Print value
			char* convStart = AllocateConvBuffer();
			char* convEnd = u32toa(value, convStart);
			CommitConvBuffer(static_cast<uint32_t>(convEnd - convStart));

			PrintChar(',');

			return true;
		}

		//---------------------------------------------------------------------------------------------
		template <>
		bool Value<uint16_t>(const char* key, uint16_t& value)
		{
			// Print shift
			PrintNewLine<UserReadableOutput>();
			PrintLevelShift<UserReadableOutput>();

			// Print key
			PrintKey(key);

			// Print value
			char* convStart = AllocateConvBuffer();
			char* convEnd = u32toa(value, convStart);
			CommitConvBuffer(static_cast<uint32_t>(convEnd - convStart));

			PrintChar(',');

			return true;
		}

		//---------------------------------------------------------------------------------------------
		template <>
		bool Value<uint32_t>(const char* key, uint32_t& value)
		{
			// Print shift
			PrintNewLine<UserReadableOutput>();
			PrintLevelShift<UserReadableOutput>();

			// Print key
			PrintKey(key);

			// Print value
			char* convStart = AllocateConvBuffer();
			char* convEnd = u32toa(value, convStart);
			CommitConvBuffer(static_cast<uint32_t>(convEnd - convStart));

			PrintChar(',');

			return true;
		}

		//---------------------------------------------------------------------------------------------
		template <>
		bool Value<uint64_t>(const char* key, uint64_t& value)
		{
			// Print shift
			PrintNewLine<UserReadableOutput>();
			PrintLevelShift<UserReadableOutput>();

			// Print key
			PrintKey(key);

			// Print value
			PrintChar('\"');

			char* convStart = AllocateConvBuffer();
			char* convEnd = u64toa(value, convStart);
			CommitConvBuffer(static_cast<uint32_t>(convEnd - convStart));

			PrintDoubleChar('\"', ',');

			return true;
		}

		//---------------------------------------------------------------------------------------------
		template <>
		bool Value<bool>(const char* key, bool& value)
		{
			// Print shift
			PrintNewLine<UserReadableOutput>();
			PrintLevelShift<UserReadableOutput>();

			// Print key
			PrintKey(key);

			// Print value
			if (value)
			{
				PrintFiverChar('t', 'r', 'u', 'e', ',');
			}
			else
			{
				PrintSiceChar('f', 'a', 'l', 's', 'e', ',');
			}

			return true;
		}

		//---------------------------------------------------------------------------------------------
		template <>
		bool Value<std::string>(const char* key, std::string& value)
		{
			// Print shift
			PrintNewLine<UserReadableOutput>();
			PrintLevelShift<UserReadableOutput>();

			// Print key
			PrintKey(key);

			// Print value
			PrintChar('\"');
			PrintText(value.c_str(), static_cast<uint32_t>(value.size()));
			PrintDoubleChar('\"', ',');

			return true;
		}

		//---------------------------------------------------------------------------------------------
		template <>
		bool Value<std::wstring>(const char* key, std::wstring& value)
		{
			// Print shift
			PrintNewLine<UserReadableOutput>();
			PrintLevelShift<UserReadableOutput>();

			// Print key
			PrintKey(key);

			// Print value
			PrintChar('\"');
			PrintText(value.c_str(), static_cast<uint32_t>(value.size()));
			PrintDoubleChar('\"', ',');

			return true;
		}

	private:
		//-------------------------------------------------------------------------------------------------
		void PrintEscapedChar(char escapedChar)
		{
			switch (escapedChar)
			{
			case '\b':				// Backspace(ascii code 08)
				PrintDoubleChar('\\', 'b');
				break;

			case '\f':				// Form feed(ascii code 0C)
				PrintDoubleChar('\\', 'f');
				break;

			case '\n':				// New line
				PrintDoubleChar('\\', 'n');
				break;

			case '\r':				// Carriage return
				PrintDoubleChar('\\', 'r');
				break;

			case '\t':				// Tab
				PrintDoubleChar('\\', 't');
				break;

			case '\"':				// Double quote
				PrintDoubleChar('\\', '\"');
				break;

			case '\\':				// Backslash caracter
				PrintDoubleChar('\\', '\\');
				break;

			default:
				PrintChar(escapedChar);
			}
		}

		//-------------------------------------------------------------------------------------------------
		void PrintTextWithoutEscapes(const char* text, uint32_t textLength)
		{
			if (textLength == 0u)
			{
				return;
			}

			// Enlarge writing buffer if needed
			if (textLength > m_writeBufferLength)
			{
				m_storage.AllocateChunk(textLength);
			}
			else if (*m_writeBufferFullness + textLength > m_writeBufferLength)
			{
				m_storage.AllocateChunk();
			}

			// Write text to buffer
			memcpy_s(*m_writeBuffer + *m_writeBufferFullness, textLength, text, textLength);

			*m_writeBufferFullness += textLength;

		}

		//-------------------------------------------------------------------------------------------------
		void PrintText(const char* text, uint32_t textLength)
		{
			for (uint32_t charIndex = 0u; charIndex != textLength; ++charIndex)
			{
				PrintEscapedChar(text[charIndex]);
			}
		}

		//-------------------------------------------------------------------------------------------------
		void PrintTextWithoutEscapes(const wchar_t* text, uint32_t textLength)
		{
			assert(false);
		}

		//-------------------------------------------------------------------------------------------------
		void PrintText(const wchar_t* text, uint32_t textLength)
		{
			for (uint32_t charIndex = 0u; charIndex != textLength; ++charIndex)
			{
				if (text[charIndex] <= 0x7F)
				{
					const char firstChar = static_cast<char>(text[charIndex] & 0xFF);

					PrintEscapedChar(firstChar);
				}
				else if (text[charIndex] <= 0x7FF)
				{
					const char firstChar = static_cast<char>(0xC0 | ((text[charIndex] >> 6) & 0xFF));
					const char secondChar = static_cast<char>(0x80 | ((text[charIndex] & 0x3F)));

					PrintDoubleChar(firstChar, secondChar);
				}
				else if (text[charIndex] <= 0xFFFF)
				{
					const char firstChar = static_cast<char>(0xE0 | ((text[charIndex] >> 12) & 0xFF));
					const char secondChar = static_cast<char>(0x80 | ((text[charIndex] >> 6) & 0x3F));
					const char thirdChar = static_cast<char>(0x80 | (text[charIndex] & 0x3F));

					PrintTripleChar(firstChar, secondChar, thirdChar);
				}
				else
				{
					assert(text[charIndex] <= 0x10FFFF);
#pragma warning (disable : 4333)
					const char firstChar = static_cast<char>(0xF0 | ((text[charIndex] >> 18) & 0xFF));
					const char secondChar = static_cast<char>(0x80 | ((text[charIndex] >> 12) & 0x3F));
					const char thirdChar = static_cast<char>(0x80 | ((text[charIndex] >> 6) & 0x3F));
					const char fourthChar = static_cast<char>(0x80 | (text[charIndex] & 0x3F));
#pragma warning (default : 4333)
					PrintQuadrupleChar(firstChar, secondChar, thirdChar, fourthChar);
				}
			}
		}

		//---------------------------------------------------------------------------------------------
		template <bool DoOutput>
		__forceinline void PrintNewLine()
		{
			PrintDoubleChar('\r', '\n');
		}

		template <>
		__forceinline void PrintNewLine<false>()
		{
		}

		//---------------------------------------------------------------------------------------------
		template <bool DoOutput>
		void PrintLevelShift()
		{
			if (m_levelShift == 0u)
			{
				return;
			}

			// Enlarge writing buffer if needed
			if (m_levelShift > m_writeBufferLength)
			{
				m_storage.AllocateChunk(m_levelShift);
			}
			else if (*m_writeBufferFullness + m_levelShift > m_writeBufferLength)
			{
				m_storage.AllocateChunk();
			}

			// Write text to buffer
			char* bufferStart = *m_writeBuffer + *m_writeBufferFullness;

			for (uint32_t charIndex = 0u; charIndex != m_levelShift; ++charIndex)
			{
				bufferStart[charIndex] = '\t';
			}

			*m_writeBufferFullness += m_levelShift;
		}

		template <>
		void PrintLevelShift<false>()
		{
		}

		//---------------------------------------------------------------------------------------------
		__forceinline void PrintChar(char firstChar)
		{
			// Enlarge writing buffer if needed
			if (*m_writeBufferFullness + 1u > m_writeBufferLength)
			{
				m_storage.AllocateChunk();
			}

			// Write text to buffer
			char* bufferStart = *m_writeBuffer + *m_writeBufferFullness;

			bufferStart[0u] = firstChar;

			*m_writeBufferFullness += 1u;
		}

		//---------------------------------------------------------------------------------------------
		__forceinline void PrintDoubleChar(char firstChar, char secondChar)
		{
			// Enlarge writing buffer if needed
			if (*m_writeBufferFullness + 2u > m_writeBufferLength)
			{
				m_storage.AllocateChunk();
			}

			// Write text to buffer
			char* bufferStart = *m_writeBuffer + *m_writeBufferFullness;

			bufferStart[0u] = firstChar;
			bufferStart[1u] = secondChar;

			*m_writeBufferFullness += 2u;
		}

		//---------------------------------------------------------------------------------------------
		__forceinline void PrintTripleChar(char firstChar, char secondChar, char thirdChar)
		{
			// Enlarge writing buffer if needed
			if (*m_writeBufferFullness + 3u > m_writeBufferLength)
			{
				m_storage.AllocateChunk();
			}

			// Write text to buffer
			char* bufferStart = *m_writeBuffer + *m_writeBufferFullness;

			bufferStart[0u] = firstChar;
			bufferStart[1u] = secondChar;
			bufferStart[2u] = thirdChar;

			*m_writeBufferFullness += 3u;
		}

		//---------------------------------------------------------------------------------------------
		__forceinline void PrintQuadrupleChar(char firstChar, char secondChar, char thirdChar, char fourthChar)
		{
			// Enlarge writing buffer if needed
			if (*m_writeBufferFullness + 4u > m_writeBufferLength)
			{
				m_storage.AllocateChunk();
			}

			// Write text to buffer
			char* bufferStart = *m_writeBuffer + *m_writeBufferFullness;

			bufferStart[0u] = firstChar;
			bufferStart[1u] = secondChar;
			bufferStart[2u] = thirdChar;
			bufferStart[3u] = fourthChar;

			*m_writeBufferFullness += 4u;
		}

		//---------------------------------------------------------------------------------------------
		__forceinline void PrintFiverChar(char firstChar, char secondChar, char thirdChar, char fourthChar, char fifthChar)
		{
			// Enlarge writing buffer if needed
			if (*m_writeBufferFullness + 5u > m_writeBufferLength)
			{
				m_storage.AllocateChunk();
			}

			// Write text to buffer
			char* bufferStart = *m_writeBuffer + *m_writeBufferFullness;

			bufferStart[0u] = firstChar;
			bufferStart[1u] = secondChar;
			bufferStart[2u] = thirdChar;
			bufferStart[3u] = fourthChar;
			bufferStart[4u] = fifthChar;

			*m_writeBufferFullness += 5u;
		}

		//---------------------------------------------------------------------------------------------
		__forceinline void PrintSiceChar(char firstChar, char secondChar, char thirdChar, char fourthChar, char fifthChar, char sixthChar)
		{
			// Enlarge writing buffer if needed
			if (*m_writeBufferFullness + 6u > m_writeBufferLength)
			{
				m_storage.AllocateChunk();
			}

			// Write text to buffer
			char* bufferStart = *m_writeBuffer + *m_writeBufferFullness;

			bufferStart[0u] = firstChar;
			bufferStart[1u] = secondChar;
			bufferStart[2u] = thirdChar;
			bufferStart[3u] = fourthChar;
			bufferStart[4u] = fifthChar;
			bufferStart[5u] = sixthChar;

			*m_writeBufferFullness += 6u;
		}

		//---------------------------------------------------------------------------------------------
		__forceinline void PrintOwnLineKey(const char* key)
		{
			if (key)
			{
				PrintLevelShift<UserReadableOutput>();
				PrintChar('\"');
				PrintTextWithoutEscapes(key, static_cast<uint32_t>(strlen(key)));
				PrintDoubleChar('\"', ':');
				PrintNewLine<UserReadableOutput>();
			}
		}

		//---------------------------------------------------------------------------------------------
		__forceinline void PrintKey(const char* key)
		{
			if (key)
			{
				PrintChar('\"');
				PrintTextWithoutEscapes(key, static_cast<uint32_t>(strlen(key)));
				PrintTripleChar('\"', ':', ' ');
			}
		}

		//---------------------------------------------------------------------------------------------
		__forceinline void BeginMap(const char* key)
		{
			PrintNewLine<UserReadableOutput>();

			// Print key
			PrintOwnLineKey(key);

			// Print header
			PrintLevelShift<UserReadableOutput>();
			PrintChar('{');

			++m_levelShift;
		}

		//---------------------------------------------------------------------------------------------
		__forceinline void EndMap()
		{
			// Remove trailing comma
			RemoveTrailingComma();

			// Decrement level shift
			--m_levelShift;

			// Print footer
			PrintNewLine<UserReadableOutput>();
			PrintLevelShift<UserReadableOutput>();
			PrintChar('}');
		}

		//---------------------------------------------------------------------------------------------
		__forceinline void BeginArray(const char* key)
		{
			PrintNewLine<UserReadableOutput>();

			// Print key
			PrintOwnLineKey(key);

			// Print header
			PrintLevelShift<UserReadableOutput>();
			PrintChar('[');

			++m_levelShift;
		}

		//---------------------------------------------------------------------------------------------
		__forceinline void EndArray()
		{
			// Remove trailing comma
			RemoveTrailingComma();

			// Decrement level shift
			--m_levelShift;

			// Print footer
			PrintNewLine<UserReadableOutput>();
			PrintLevelShift<UserReadableOutput>();
			PrintChar(']');
		}

		//---------------------------------------------------------------------------------------------
		__forceinline void RemoveTrailingComma()
		{
			assert(*m_writeBufferFullness != 0u);

			if ((*m_writeBuffer)[*m_writeBufferFullness - 1u] == ',')
			{
				*m_writeBufferFullness -= 1u;
			}
		}

		//---------------------------------------------------------------------------------------------
		__forceinline char* AllocateConvBuffer()
		{
			if (*m_writeBufferFullness + 32u > m_writeBufferLength)
			{
				m_storage.AllocateChunk();
			}

			return *m_writeBuffer + *m_writeBufferFullness;
		}

		//---------------------------------------------------------------------------------------------
		__forceinline void CommitConvBuffer(uint32_t convSize)
		{
			*m_writeBufferFullness += convSize;
		}

	private:
		//---------------------------------------------------------------------------------------------
		template <typename ValueType, AbstractValue UnsupportedValue>
		class CustomSerializer
		{
		public:
			__forceinline static bool Serialize(JsonPrinter<Storage, UserReadableOutput>& serializer, const char* key, ValueType& value)
			{
				static_assert(false, "Unsupported value type.");
			}
		};

		//---------------------------------------------------------------------------------------------
		template <typename ValueType>
		class CustomSerializer<typename ValueType, AbstractValue::Enum>
		{
		public:
			__forceinline static bool Serialize(JsonPrinter<Storage, UserReadableOutput>& serializer, const char* key, ValueType& value)
			{
				serializer.Value(key, reinterpret_cast<std::underlying_type<ValueType>::type&>(value));
				return true;
			}
		};

		//---------------------------------------------------------------------------------------------
		template <typename ValueType>
		class CustomSerializer<typename ValueType, AbstractValue::SerializableStruct>
		{
		public:
			__forceinline static bool Serialize(JsonPrinter<Storage, UserReadableOutput>& serializer, const char* key, ValueType& value)
			{
				serializer.BeginMap(key);

				value.Serialize(&serializer);

				serializer.EndMap();
				serializer.PrintChar(',');

				return true;
			}
		};
	};
}
