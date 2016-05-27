#include "platform.h"
#include "VectorStream.h"


namespace Auxiliary
{
	//-------------------------------------------------------------------------------------------------
	template <bool OwnVector>
	IStream::StreamPos VectorStream<OwnVector>::Read(byte* outputBuffer, StreamPos bytesToRead)
	{
		bytesToRead = std::min(m_currentPosition + bytesToRead, static_cast<StreamPos>(m_memoryBuffer.size())) - m_currentPosition;

		memcpy(outputBuffer, m_memoryBuffer.data() + m_currentPosition, bytesToRead);

		m_currentPosition += bytesToRead;

		return bytesToRead;
	}

	//-------------------------------------------------------------------------------------------------
	template <bool OwnVector>
	IStream::StreamPos VectorStream<OwnVector>::Write(const void* inputBuffer, StreamPos bytesToWrite)
	{
		return FastWrite(inputBuffer, bytesToWrite);
	}

	//-------------------------------------------------------------------------------------------------
	template <bool OwnVector>
	IStream::StreamPos VectorStream<OwnVector>::Seek(SeekOrigin seekOrigin, StreamSeek bytesToSeek)
	{
		const auto ClampStreamPos = [&](StreamSeek newPos)->StreamPos
		{
			if (newPos > static_cast<StreamSeek>(m_memoryBuffer.size()))
			{
				return m_memoryBuffer.size();
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
			m_currentPosition = ClampStreamPos(bytesToSeek);
			break;

		case SeekOrigin::Current:
			m_currentPosition = ClampStreamPos(static_cast<StreamSeek>(m_currentPosition) + bytesToSeek);
			break;

		case SeekOrigin::End:
			m_currentPosition = ClampStreamPos(static_cast<StreamSeek>(m_memoryBuffer.size()) + bytesToSeek);
			break;
		}

		return m_currentPosition;
	}

	//-------------------------------------------------------------------------------------------------
	template <bool OwnVector>
	IStream::StreamPos VectorStream<OwnVector>::SetLength(StreamPos requiredLength)
	{
		if (m_memoryBuffer.size() != requiredLength)
		{
			m_memoryBuffer.resize(static_cast<typename VectorType::size_type>(requiredLength));
		}

		if (m_currentPosition > m_memoryBuffer.size())
		{
			m_currentPosition = m_memoryBuffer.size();
		}

		return m_memoryBuffer.size();
	}

	//-------------------------------------------------------------------------------------------------
	template <bool OwnVector>
	IStream::StreamPos VectorStream<OwnVector>::Tell() const
	{
		return m_currentPosition;
	}

	//-------------------------------------------------------------------------------------------------
	template <bool OwnVector>
	IStream::StreamPos VectorStream<OwnVector>::Length() const
	{
		return m_memoryBuffer.size();
	}

	//-------------------------------------------------------------------------------------------------
	template <bool OwnVector>
	bool VectorStream<OwnVector>::Reserve(StreamPos requiredCapacity)
	{
		m_memoryBuffer.reserve(requiredCapacity);
		return true;
	}

	//-------------------------------------------------------------------------------------------------
	template <bool OwnVector>
	const byte* VectorStream<OwnVector>::EntireData() const
	{
		return m_memoryBuffer.data();
	}

	//-------------------------------------------------------------------------------------------------
	template class VectorStream<true>;
	template class VectorStream<false>;
}
