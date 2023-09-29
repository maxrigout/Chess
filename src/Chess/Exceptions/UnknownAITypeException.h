#include <exception>
#include <string>

class UnknownAITypeException : public std::exception
{
public:
	explicit UnknownAITypeException(const char* aiType) noexcept
	{
		m_errorMessage = "unknown ai type: ";
		m_errorMessage += aiType;
	}

	explicit UnknownAITypeException(const std::string& aiType) noexcept
	: UnknownAITypeException(aiType.c_str())
	{}

	virtual ~UnknownAITypeException() noexcept = default;

	virtual const char* what() const noexcept
	{
		return m_errorMessage.c_str();
	}

private:
	std::string m_errorMessage;
};