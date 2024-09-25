#include "GeminiAgent.h"

#include <nlohmann/json.hpp>

#include "Core/Logger.h"

GeminiAgent::GeminiAgent(const std::string& apiKey)
	: m_client("https://generativelanguage.googleapis.com/")
{
	m_headers.add("x-goog-api-key", apiKey);
	m_headers.add("Content-Type", "application/json");
}

void GeminiAgent::Reset()
{
	m_history.clear();
}

std::string GeminiAgent::Send(const std::string& msg)
{
	m_history.push_back({ ROLE::USER, msg });

	std::string payload = makePayload(msg);

	auto res = m_client.makeAPICall(METHOD::POST, "v1/models/gemini-pro:generateContent", m_headers, payload);

	if (res.code != 200)
	{
		LOG_ERROR("error while communicating with Gemini: ", res.body);
		return "";
	}

	nlohmann::json j = nlohmann::json::parse(res.body);
	m_history.push_back({ ROLE::MODEL, j["candidates"][0]["content"]["parts"][0]["text"].get<std::string>() });
	return m_history.back().parts;
}

std::string GeminiAgent::makePayload(const std::string& msg)
{
	nlohmann::json j;
	j["contents"] = nlohmann::json::array();

	for (const auto& history : m_history)
	{
		nlohmann::json h;

		h = {
			{ "role", roleToString(history.role) },
			{ "parts", {
				{ "text", history.parts }
			} }
		};

		j["contents"].push_back(h);
	}

	return j.dump();
}