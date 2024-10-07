#pragma once

#include <vector>

#include "../LLMAgent.h"
#include "Core/HTTP/APIClient.h"

class GeminiAgent : public LLMAgent
{
public:
	GeminiAgent(const std::string& apiKey);

	virtual ~GeminiAgent() = default;

	void Reset() override;

	std::string Send(const std::string& msg) override;

	virtual void dump(std::ostream& os) const override;

private:
	enum class ROLE
	{
		USER,
		MODEL,
	};

	struct Content
	{
		ROLE role;
		std::string parts;
	};

	std::string makePayload(const std::string& msg);

	const char* roleToString(ROLE role)
	{
		switch (role)
		{
		case ROLE::USER: return "user";
		case ROLE::MODEL: return "model";
		}
	}

	Headers m_headers;
	APIClient m_client;
	std::vector<Content> m_history;
};