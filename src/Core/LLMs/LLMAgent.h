#pragma once

#include <string>
#include <ostream>

class LLMAgent
{
public:
	virtual ~LLMAgent() = default;

	virtual void Reset() = 0;
	virtual std::string Send(const std::string& msg) = 0;

	virtual void dump(std::ostream& os) const = 0;
};