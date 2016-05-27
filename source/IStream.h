#pragma once


namespace Auxiliary
{
	//-------------------------------------------------------------------------------------------------
	/// IStream
	//-------------------------------------------------------------------------------------------------
	class IStream
	{
	public:
		enum class SeekOrigin : uint32_t
		{
			Begin = 0u,
			Current,
			End
		};

		typedef uint64_t StreamPos;
		typedef int64_t StreamSeek;

	public:
		virtual ~IStream() {}

		virtual bool Reserve(StreamPos requiredCapacity) = 0;
		virtual StreamPos Read(byte* outputBuffer, StreamPos bytesToRead) = 0;
		virtual StreamPos Write(const void* inputBuffer, StreamPos bytesToWrite) = 0;
		virtual StreamPos Seek(SeekOrigin seekOrigin, StreamSeek bytesToSeek) = 0;
		virtual StreamPos SetLength(StreamPos requiredLength) = 0;
		virtual StreamPos Tell() const = 0;
		virtual StreamPos Length() const = 0;
	};

	//-------------------------------------------------------------------------------------------------
	/// IMemoryStream
	//-------------------------------------------------------------------------------------------------
	class IMemoryStream : public IStream
	{
	public:
		virtual ~IMemoryStream() {}

		virtual const byte* EntireData() const = 0;
	};
}
