#include "mbed.h"
#include "CPolledTimer.h"

CPolledTimer::CPolledTimer(int32_t waitTime) {

	if(waitTime >= 0)
	{
		m_waitTime = waitTime;
	}

	else{
		m_waitTime = 5;
	}

	m_targetTime =  m_waitTime + millisSinceStart();

}

int64_t CPolledTimer::millisSinceStart() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
			HighResClock::now().time_since_epoch()).count();
}

bool CPolledTimer::timeReached() {

	if(millisSinceStart() >= m_targetTime)
	{
		return true;

	}

	else
	{
		return false;

	}
}

void CPolledTimer::updateWaitTime(int32_t waitTime)
{
	m_waitTime = waitTime;
	m_targetTime =  m_waitTime + millisSinceStart();
}



