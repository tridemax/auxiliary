#pragma once


namespace Aux
{
	//-------------------------------------------------------------------------------------------------
	/// Clock
	//-------------------------------------------------------------------------------------------------
	class Clock
	{
	private:
		const clockid_t				m_clockId;
		struct timespec				m_timeSpec;

	public:
		//---------------------------------------------------------------------------------------------
		inline Clock(clockid_t clockId = CLOCK_MONOTONIC) : m_clockId(clockId)
		{
			clock_gettime(m_clockId, &m_timeSpec);
		}

		//---------------------------------------------------------------------------------------------
		inline void Update()
		{
			clock_gettime(m_clockId, &m_timeSpec);
		}

		//---------------------------------------------------------------------------------------------
		inline int64_t DeltaSeconds() const
		{
			struct timespec currentSpec;
			clock_gettime(m_clockId, &currentSpec);

			const int64_t deltaSeconds = currentSpec.tv_sec - m_timeSpec.tv_sec;

			return deltaSeconds;
		}

		//---------------------------------------------------------------------------------------------
		inline int64_t DeltaMilliseconds() const
		{
			struct timespec currentSpec;
			clock_gettime(m_clockId, &currentSpec);

			const int64_t deltaSeconds = currentSpec.tv_sec - m_timeSpec.tv_sec;
			const int64_t deltaNanoseconds = currentSpec.tv_nsec - m_timeSpec.tv_nsec;

			return deltaSeconds * 1000 + deltaNanoseconds / 1000000;
		}

		//---------------------------------------------------------------------------------------------
		inline int64_t DeltaMicroseconds() const
		{
			struct timespec currentSpec;
			clock_gettime(m_clockId, &currentSpec);

			const int64_t deltaSeconds = currentSpec.tv_sec - m_timeSpec.tv_sec;
			const int64_t deltaNanoseconds = currentSpec.tv_nsec - m_timeSpec.tv_nsec;

			return deltaSeconds * 1000000 + deltaNanoseconds / 1000;
		}

		//---------------------------------------------------------------------------------------------
		inline int64_t Seconds() const
		{
			return m_timeSpec.tv_sec;
		}

		//---------------------------------------------------------------------------------------------
		inline int64_t Milliseconds() const
		{
			return m_timeSpec.tv_sec * 1000 + m_timeSpec.tv_nsec / 1000000;
		}

		//---------------------------------------------------------------------------------------------
		inline int64_t Microseconds() const
		{
			return m_timeSpec.tv_sec * 1000000 + m_timeSpec.tv_nsec / 1000;
		}
	};
}
