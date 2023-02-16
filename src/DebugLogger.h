#include <iostream>
#include <string>

#define LOG_DEBUG(msg) do { \
	std::cout << "[" << "" << "] - '" << __FILE__ << "'::" << "'" << __func__ << "'::" << __LINE__ << "\n\t" << msg << "\n=====" << std::endl; \
} while(false)