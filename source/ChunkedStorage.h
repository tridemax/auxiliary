#pragma once

#include "IStream.h"


namespace aux
{
	//-------------------------------------------------------------------------------------------------
	/// ChunkedStorage
	//-------------------------------------------------------------------------------------------------
	template <typename DataType>
	class ChunkedStorage : public boost::noncopyable
	{
	private:
		class Chunk
		{
		public:
			DataType*					m_chunkData;
			uint32_t					m_chunkFullness;			///< in DataType items; efficient length is usually smaller than total chunk length

		public:
			inline Chunk(DataType* chunkData, uint32_t chunkFullness) : m_chunkData(chunkData), m_chunkFullness(chunkFullness) {}
		};

		typedef std::vector<Chunk> ChunkVector;

	private:
		ChunkVector					m_chunkVector;
		const uint32_t				m_defaultChunkLength;			///< in DataType items
		uint32_t					m_totalDataLength;				///< in DataType items
		DataType*					m_currentChunkData;
		uint32_t					m_currentChunkFullness;			///< in DataType items

	public:
		//---------------------------------------------------------------------------------------------
		inline ChunkedStorage(uint32_t defaultChunkLength = 4096u) :
			m_defaultChunkLength(defaultChunkLength),
			m_totalDataLength(0u)
		{
			m_currentChunkData = new DataType[m_defaultChunkLength];
			m_currentChunkFullness = 0u;
		}

		//---------------------------------------------------------------------------------------------
		inline ~ChunkedStorage()
		{
			for (auto& chunk : m_chunkVector)
			{
				delete[] chunk.m_chunkData;
			}

			delete[] m_currentChunkData;
		}

		//---------------------------------------------------------------------------------------------
		inline void AllocateChunk(uint32_t chunkLength)
		{
			if (m_currentChunkFullness == 0u)
			{
				delete[] m_currentChunkData;
			}
			else
			{
				m_chunkVector.emplace_back(Chunk(m_currentChunkData, m_currentChunkFullness));
				m_totalDataLength += m_currentChunkFullness;
			}

			m_currentChunkData = new DataType[chunkLength];
			m_currentChunkFullness = 0u;
		}

		//---------------------------------------------------------------------------------------------
		inline void AllocateChunk()
		{
			AllocateChunk(m_defaultChunkLength);
		}

		//---------------------------------------------------------------------------------------------
		inline void Merge(IMemoryStream& outputStream) const
		{
			// Setup output stream
			outputStream.Reserve((m_totalDataLength + m_currentChunkFullness) * sizeof(DataType));

			// Write all chunks
			for (auto& chunk : m_chunkVector)
			{
				outputStream.Write(chunk.m_chunkData, chunk.m_chunkFullness * sizeof(DataType));
			}

			// Write the current chunk
			outputStream.Write(m_currentChunkData, m_currentChunkFullness * sizeof(DataType));
		}

		//---------------------------------------------------------------------------------------------
		inline void GetWriteBuffer(DataType**& bufferData, uint32_t*& bufferFullness)
		{
			bufferData = &m_currentChunkData;
			bufferFullness = &m_currentChunkFullness;
		}

		//---------------------------------------------------------------------------------------------
		inline uint32_t GetWriteBufferLength() const
		{
			return m_defaultChunkLength;
		}
	};
}
