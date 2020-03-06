#include "ChronoHelpers.hpp"

std::ostream& operator << (std::ostream& os, const std::chrono::milliseconds ms)
{
	os << ms.count() << "ms";
	return os;
}

