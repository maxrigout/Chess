#pragma once

#include <string>
#include <format>

#include <curl/curl.h>

enum class METHOD
{
	GET,
	POST,
	PUT,
	DELETE,
	OPTIONS,

};

class APIClient;

class Headers
{
public:
	Headers() = default;
	~Headers()
	{
		curl_slist_free_all(headers);
	}

	Headers& add(const std::string& key, const std::string& value)
	{
		std::string headerValue = key + ": " + value;
		headers = curl_slist_append(headers, headerValue.c_str());
		return *this;
	}

private:
	curl_slist* headers = nullptr;

	friend class APIClient;
};

struct Response
{
	static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
	{
		((Response*)userp)->body.append((char*)contents, size * nmemb);
		return size * nmemb;
	}

	long code = -1;
	std::string body;
};

class APIClient
{
public:
	APIClient(const std::string& baseUrl)
	{
		if (baseUrl[baseUrl.size() - 1] != '/')
		{
			m_baseUrl = baseUrl + "/";
		}
		else
		{
			m_baseUrl = baseUrl;
		}
	}

	virtual ~APIClient() = default;

	Response makeAPICall(METHOD method, const std::string& path, const Headers& headers, const std::string& body = "")
	{
		std::string url = m_baseUrl + path;
		CURL *curl;
		Response response;

		curl = curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, toChar(method));
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers.headers);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Response::WriteCallback);
    	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		// https://curl.se/libcurl/c/CURLOPT_READFUNCTION.html

		if (methodCanHaveBody(method))
		{
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
		}

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			curl_easy_cleanup(curl);
			return Response{};
		}

		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.code);

		curl_easy_cleanup(curl);

		return response;
	}

private:
	const char* toChar(METHOD method)
	{
		switch (method)
		{
			case METHOD::GET: return "GET";
			case METHOD::POST: return "POST";
			case METHOD::DELETE: return "DELETE";
			case METHOD::OPTIONS: return "OPTIONS";
			case METHOD::PUT: return "PUT";
		}
	}

	bool methodCanHaveBody(METHOD method)
	{
		return method == METHOD::POST || method == METHOD::PUT;
	}

	std::string m_baseUrl;
};