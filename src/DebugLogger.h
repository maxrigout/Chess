#include <iostream>
#include <string>

#define LOG_DEBUG(msg) do {			\
	std::cout << "[" << "" << "] - '" << __func__ << "'::" << "'" << __FILE__ << "'::" << __LINE__ << "\n\t" << msg << "\n=====" << std::endl; \
} while(false)