#pragma once

#include <vector>

#include "Core/HTTP/APIClient.h"

class GeminiAgent
{
public:
	GeminiAgent(const std::string& apiKey);

	~GeminiAgent() = default;

	std::string Send(const std::string& msg);

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