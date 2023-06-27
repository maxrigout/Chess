#include "Socket.h"

#if defined __linux__ || __APPLE__
float htonf(float f) {
    uint32_t i;
    std::memcpy(&i, &f, sizeof(float));
    i = htonl(i);
    std::memcpy(&f, &i, sizeof(float));
    return f;
}

double htond(double d) {
    uint64_t i;
    std::memcpy(&i, &d, sizeof(double));
    i = htonll(i);
    std::memcpy(&d, &i, sizeof(double));
    return d;
}
#endif