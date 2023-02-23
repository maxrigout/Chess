#include "TestTimer.h"

TestTimer::TestTimer()
{

}

TestTimer::~TestTimer()
{

}

void TestTimer::Start()
{
	m_begin = std::chrono::high_resolution_clock::now();
}

void TestTimer::Stop()
{
	m_end = std::chrono::high_resolution_clock::now();
}

int TestTimer::GetEllapsedTime() const
{
	return std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_begin).count();
}