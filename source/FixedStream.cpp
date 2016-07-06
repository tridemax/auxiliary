#include "platform.h"
#include "FixedStream.h"


namespace aux
{
	//-------------------------------------------------------------------------------------------------
	FixedStream::FixedStream(byte* bufferStart, StreamPos bufferLength) : m_bufferStart(bufferStart), m_bufferLength(bufferLength), m_currentPosition(0u)
	{
	}

	//-------------------------------------------------------------------------------------------------
	FixedStream::~FixedStream()
	{
	}

	//-------------------------------------------------------------------------------------------------
	IStream::StreamPos FixedStream::Read(byte* outputBuffer, StreamPos bytesToRead)
	{
		bytesToRead = std::min(m_currentPosition + bytesToRead, m_bufferLength) - m_currentPosition;

		memcpy(outputBuffer, m_bufferStart + m_currentPosition, bytesToRead);

		m_currentPosition += bytesToRead;

		return bytesToRead;
	}

	//-------------------------------------------------------------------------------------------------
	IStream::StreamPos FixedStream::Write(const void* inputBuffer, StreamPos bytesToWrite)
	{
		bytesToWrite = std::min(m_currentPosition + bytesToWrite, m_bufferLength) - m_currentPosition;

		memcpy(m_bufferStart + m_currentPosition, inputBuffer, bytesToWrite);

		m_currentPosition = bytesToWrite;

		return bytesToWrite;
	}

	//-------------------------------------------------------------------------------------------------
	IStream::StreamPos FixedStream::Seek(SeekOrigin seekOrigin, StreamSeek bytesToSeek)
	{
		const auto clampStreamPos = [&](StreamSeek newPos) -> StreamPos
		{
			if (newPos > static_cast<StreamSeek>(m_bufferLength))
			{
				return m_bufferLength;
			}
			if (newPos < 0)
			{
				return 0u;
			}
			return static_cast<StreamPos>(newPos);
		};

		switch (seekOrigin)
		{
		case SeekOrigin::Begin:
			m_currentPosition = clampStreamPos(bytesToSeek);
			break;

		case SeekOrigin::Current:
			m_currentPosition = clampStreamPos(static_cast<StreamSeek>(m_currentPosition) + bytesToSeek);
			break;

		case SeekOrigin::End:
			m_currentPosition = clampStreamPos(static_cast<StreamSeek>(m_bufferLength) + bytesToSeek);
			break;
		}

		return m_currentPosition;
	}

	//-------------------------------------------------------------------------------------------------
	IStream::StreamPos FixedStream::SetLength(StreamPos requiredLength)
	{
		assert(false);
		return m_bufferLength;
	}

	//-------------------------------------------------------------------------------------------------
	IStream::StreamPos FixedStream::Tell() const
	{
		return m_currentPosition;
	}

	//-------------------------------------------------------------------------------------------------
	IStream::StreamPos FixedStream::Length() const
	{
		return m_bufferLength;
	}

	//-------------------------------------------------------------------------------------------------
	bool FixedStream::Reserve(StreamPos requiredCapacity)
	{
		return m_bufferLength <= requiredCapacity;
	}

	//-------------------------------------------------------------------------------------------------
	const byte* FixedStream::EntireData() const
	{
		return m_bufferStart;
	}
}
