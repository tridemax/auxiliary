#pragma once

#include "IStream.h"
#include "Miscellaneous.h"


namespace Auxiliary
{
	template <bool OwnVector = true>
	class VectorStream : public IMemoryStream, public boost::noncopyable
	{
	private:
		typedef std::vector<byte> ContainerType;
		typedef std::conditional_t<OwnVector, ContainerType, std::add_lvalue_reference_t<ContainerType> > VectorHolderType;
		typedef std::decay_t<VectorHolderType> VectorType;

	private:
		VectorHolderType			m_memoryBuffer;
		StreamPos					m_currentPosition;

	public:
		template <bool LocalOwnVector = OwnVector, typename = std::enable_if_t<!LocalOwnVector> >
		inline VectorStream(VectorHolderType memoryBuffer) : m_memoryBuffer(memoryBuffer), m_currentPosition(0u)
		{
		}

		template <bool LocalOwnVector = OwnVector, typename = std::enable_if_t<LocalOwnVector> >
		inline VectorStream(StreamPos capacity = 0u) : m_currentPosition(0u)
		{
			m_memoryBuffer.reserve(capacity);
		}

		virtual ~VectorStream()
		{
		}

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

	public:
		inline IStream::StreamPos FastWrite(const void* inputBuffer, StreamPos bytesToWrite)
		{
			const StreamPos newSize = m_currentPosition + bytesToWrite;

			if (m_memoryBuffer.size() < newSize)
			{
				m_memoryBuffer.resize(static_cast<typename VectorType::size_type>(newSize));
			}

			memcpy(m_memoryBuffer.data() + m_currentPosition, inputBuffer, bytesToWrite);

			m_currentPosition = newSize;

			return bytesToWrite;
		}

		inline void Swap(VectorType& anotherVector)
		{
			m_memoryBuffer.swap(anotherVector);
		}

		inline byte* Data()
		{
			return m_memoryBuffer.data();
		}
	};
}
