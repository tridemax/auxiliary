#pragma once

#include "IStream.h"


namespace Auxiliary
{
	class FixedStream : public IMemoryStream
	{
	private:
		byte*						m_bufferStart;
		StreamPos					m_bufferLength;
		StreamPos					m_currentPosition;

	public:
		FixedStream(byte* bufferStart, StreamPos bufferLength);
		virtual ~FixedStream();

		// IStream
		virtual StreamPos Read(byte* outputBuffer, StreamPos bytesToRead) override final;
		virtual StreamPos Write(const void* inputBuffer, StreamPos bytesToWrite) override final;
		virtual StreamPos Seek(SeekOrigin seekOrigin, StreamSeek bytesToSeek) override final;
		virtual StreamPos SetLength(StreamPos requiredLength) override final;
		virtual StreamPos Tell() const override final;
		virtual StreamPos Length() const override final;

		// IMemoryStream
		virtual bool Reserve(StreamPos requiredCapacity) override final;
		virtual const byte* EntireData() const override final;
	};
}
