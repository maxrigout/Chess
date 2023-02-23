#include <chrono>

class TestTimer
{
public:
	TestTimer();
	~TestTimer();
	void Start();
	void Stop();
	int GetEllapsedTime() const;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_end;
};