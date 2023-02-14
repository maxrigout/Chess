#include <iostream>
#include <string>

// inline void Debug(const std::string& callingFunc, int lineNumber)
// {
// 	std::cout << __func__ << std::endl;
// 	std::cout << __LINE__ << std::endl;
// 	std::cout << __FILE__ << std::endl;
// }

// inline std::string GetTime() {
// 	return "";
// }

#define LOG_DEBUG(msg) do {			\
	std::cout << "[" << "" << "] - '" << __func__ << "'::" << "'" << __FILE__ << "'::" << __LINE__ << "\n\t" << msg << "\n=====" << std::endl; \
} while(false)