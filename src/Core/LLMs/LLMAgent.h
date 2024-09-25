#pragma once

#include <string>

class LLMAgent
{
public:
	virtual ~LLMAgent() = default;

	virtual void Reset() = 0;
	virtual std::string Send(const std::string& msg) = 0;
};