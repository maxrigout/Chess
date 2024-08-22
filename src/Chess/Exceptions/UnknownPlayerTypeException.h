#include <exception>
#include <string>

class UnknownPlayerTypeException : public std::exception
{
public:
	explicit UnknownPlayerTypeException(const char* playerType) noexcept
	{
		m_errorMessage = "unknown player type: ";
		m_errorMessage += playerType;
	}

	explicit UnknownPlayerTypeException(const std::string& playerType) noexcept
	: UnknownPlayerTypeException(playerType.c_str())
	{}

	virtual ~UnknownPlayerTypeException() noexcept = default;

	virtual const char* what() const noexcept
	{
		return m_errorMessage.c_str();
	}

private:
	std::string m_errorMessage;
};