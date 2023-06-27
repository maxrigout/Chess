#pragma once

#include <string>

#if defined __linux__ || __APPLE__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (~0)

float htonf(float f);
double htond(double d);

#elif defined _WIN32 || _WIN64

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#define close closesocket
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

#endif

#include <stdint.h>

enum class MessageStatus
{
	CREATED,
	RECEIVING,
	COMPLETE,
	INCOMPLETE,
};

struct NetworkHeader
{
	MessageStatus status = MessageStatus::CREATED;
};

template <typename T>
struct NetworkMessage
{
	NetworkHeader header;
	T payload;
};

class Socket
{
public:
	Socket()
	{
		m_hints.ai_family = AF_UNSPEC;
		m_hints.ai_socktype = SOCK_STREAM;
		m_hints.ai_flags = AI_PASSIVE;
	}

	bool Connect(const std::string& host, uint16_t port)
	{
		return false;
	}

	bool Disconnect()
	{
		close(m_socket);
		return true;
	}

	bool Bind(const std::string& host, uint16_t port)
	{
		addrinfo hints = { 0 }, * res;		
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		close(m_socket);
		
		getaddrinfo(NULL, "3490", &m_hints, &res);

		for (addrinfo* ptr = res; ptr != nullptr; ptr = ptr->ai_next)
		{
			m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			int error = bind(m_socket, ptr->ai_addr, ptr->ai_addrlen);
			if (!error)
			{
				m_isBound = true;
				break;
			}
			close(m_socket);
		}
		freeaddrinfo(res);
		return m_isBound;
	}

	bool Listen();

	template <typename T>
	bool Send(T payload)
	{
		
	}

	template <typename T>
	NetworkMessage<T> Receive() const
	{
		NetworkMessage<T> message;
		uint8_t buffer[sizeof(T)] = { 0 };
		size_t totalBytesReceived = 0;
		while (totalBytesReceived < sizeof(T))
		{
			message.header.status = MessageStatus::RECEIVING;
			int bytes = recv(m_socket, (char*)buffer + totalBytesReceived, sizeof(T) - totalBytesReceived, 0);
			if (bytes == 0)
			{
				message.header.status = MessageStatus::INCOMPLETE;
				// only for TCP sockets
				// client disconnected...
				break;
			}
			totalBytesReceived += bytes;
		}
		return message;
	}

	// sending
	template <typename T>
	Socket& operator<<(const T& value)
	{
		uint8_t* data = (uint8_t*)&value;
		size_t bytesSent = 0;
		while (bytesSent < sizeof(T))
		{
			int sent = send(m_socket, (char*)data + bytesSent, sizeof(T) - bytesSent, 0);
			if (sent == 0)
			{
				// disconnected...
				m_isConnected = false;
			}
		}
		return *this;
	}

	template <>
	Socket& operator<<(const std::string& value)
	{
		size_t sz = value.length();
		*this << sz;
		for (int i = 0; i < sz; ++i)
		{
			*this << value[i];
		}
		return *this;
	}

	Socket& operator<<(const char* str)
	{
		size_t sz = strlen(str);
		*this << sz;
		for (int i = 0; i < sz; ++i)
		{
			*this << str[i];
		}
		return *this;
	}

	// receiving
	template <typename T>
	Socket& operator>>(T& value)
	{
		uint8_t buffer[sizeof(T)] = { 0 };
		size_t totalBytesReceived = 0;
		while (totalBytesReceived < sizeof(T))
		{
			int bytesReceived = recv(m_socket, (char*)buffer + totalBytesReceived, sizeof(T) - totalBytesReceived, 0);
			if (bytesReceived == 0)
			{
				// only for TCP sockets
				// client disconnected...
				m_isConnected = false;
				break;
			}
			totalBytesReceived += bytesReceived;
		}
		value = (T)(*buffer);
		return *this;
	}

	template <>
	Socket& operator>>(std::string& value)
	{
		size_t sz = 0;
		*this >> sz;
		value.reserve(sz);
		for (int i = 0; i < sz; ++i)
		{
			char c;
			*this >> c;
			value[i] = c;
		}
		return *this;
	}

private:
	int m_socket = -1;
	addrinfo m_hints;
	bool m_isBound = false;
	bool m_isConnected = false;
};