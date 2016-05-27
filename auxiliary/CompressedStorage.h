#pragma once

#include "IStream.h"


namespace Auxiliary
{
	//-------------------------------------------------------------------------------------------------
	/// CompressedStorage
	//-------------------------------------------------------------------------------------------------
	template <typename DataType>
	class CompressedStorage : public boost::noncopyable
	{
	private:
		std::vector<byte*>			m_chunkVector;					///< chunks array of compressed data
		const uint32_t				m_compressedChunkLength;		///< length of all compressed data chunks (in bytes)
		byte*						m_currentChunkData;				///< the currently filling chunk of compressed data
		uint32_t					m_currentChunkFullness;			///< fullness of the currently filling chunk (in bytes)
		DataType*					m_compressionBuffer;			///< buffer that should be filled by external logic
		uint32_t					m_compressionBufferLength;		///< maximal length of compression buffer (in DataType items)
		uint32_t					m_compressionBufferFullness;	///< fullness of compression buffer (in DataType items)
		z_stream					m_deflatingStream;

	public:
		//---------------------------------------------------------------------------------------------
		inline CompressedStorage(uint32_t compressedChunkLength = 4096u, uint32_t compressionBufferLength = 4096u) :
			m_compressedChunkLength(compressedChunkLength),
			m_compressionBufferLength(compressionBufferLength)
		{
			m_currentChunkData = new byte[compressedChunkLength];
			m_currentChunkFullness = 0u;

			// Allocate compression buffer
			m_compressionBuffer = reinterpret_cast<DataType*>(TF_MALLOC(compressionBufferLength * sizeof(DataType)));
			m_compressionBufferFullness = 0u;

			// Setup deflating stream
			memset(&m_deflatingStream, 0, sizeof(m_deflatingStream));
			deflateInit(&m_deflatingStream, Z_DEFAULT_COMPRESSION);
		}

		//---------------------------------------------------------------------------------------------
		inline ~CompressedStorage()
		{
			// Release compressed chunks
			for (auto& chunk : m_chunkVector)
			{
				delete[] chunk;
			}

			delete[] m_currentChunkData;

			// Release compression buffer
			TF_FREE(m_compressionBuffer);

			// Release deflating stream
			deflateEnd(&m_deflatingStream);
		}

		//---------------------------------------------------------------------------------------------
		inline void AllocateChunk(uint32_t chunkLength)
		{
			// Flush compression buffer
			DeflateCompressionBuffer(false);

			// Resize compression buffer
			if (m_compressionBufferLength < chunkLength)
			{
				TF_FREE(m_compressionBuffer);
				m_compressionBuffer = reinterpret_cast<DataType*>(TF_MALLOC(chunkLength * sizeof(DataType)));
				m_compressionBufferLength = chunkLength;
			}
		}

		//---------------------------------------------------------------------------------------------
		inline void AllocateChunk()
		{
			AllocateChunk(m_compressionBufferLength);
		}

		//---------------------------------------------------------------------------------------------
		inline void Merge(IMemoryStream& outputStream)
		{
			// Flush compression buffer
			DeflateCompressionBuffer(true);

			// Setup output stream
			outputStream.Reserve(m_chunkVector.size() * m_compressedChunkLength + m_currentChunkFullness);

			// Write all chunks
			for (auto& chunk : m_chunkVector)
			{
				outputStream.Write(chunk, m_compressedChunkLength);
			}

			// Write the current chunk
			outputStream.Write(m_currentChunkData, m_currentChunkFullness);
		}

		//---------------------------------------------------------------------------------------------
		inline void GetWriteBuffer(DataType**& bufferData, uint32_t*& bufferFullness)
		{
			bufferData = &m_compressionBuffer;
			bufferFullness = &m_compressionBufferFullness;
		}

		//---------------------------------------------------------------------------------------------
		inline uint32_t GetWriteBufferLength() const
		{
			return m_compressionBufferLength;
		}

	private:
		//---------------------------------------------------------------------------------------------
		void DeflateCompressionBuffer(bool flushDeflatingStream)
		{
			// Setup data to deflate
			m_deflatingStream.next_in = reinterpret_cast<byte*>(m_compressionBuffer);
			m_deflatingStream.avail_in = m_compressionBufferFullness * sizeof(DataType);

			// Deflating loop
			do
			{
				do
				{
					const uint32_t outputBufferLength = m_compressedChunkLength - m_currentChunkFullness;

					// Setup output buffer
					m_deflatingStream.next_out = m_currentChunkData + m_currentChunkFullness;
					m_deflatingStream.avail_out = outputBufferLength;

					// Deflate
					deflate(&m_deflatingStream, flushDeflatingStream ? Z_FINISH : Z_NO_FLUSH);

					// Process deflated data
					const uint32_t deflatedDataLength = outputBufferLength - m_deflatingStream.avail_out;

					if (deflatedDataLength != 0u)
					{
						m_currentChunkFullness += deflatedDataLength;
						assert(m_currentChunkFullness <= m_compressedChunkLength);

						// Allocate new chunk
						if (m_currentChunkFullness == m_compressedChunkLength)
						{
							m_chunkVector.emplace_back(m_currentChunkData);

							m_currentChunkData = new byte[m_compressedChunkLength];
							m_currentChunkFullness = 0u;
						}
					}
				}
				while (m_deflatingStream.avail_out == 0u);
			}
			while (m_deflatingStream.avail_in != 0u);

			// Reset compression buffer
			m_compressionBufferFullness = 0u;
		}
	};
}
