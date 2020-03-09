#include "ChronoHelpers.hpp"

namespace std {
namespace chrono {

std::ostream& operator<<(std::ostream& os, const std::chrono::nanoseconds duration)
{
    os << duration.count() << "ns";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::chrono::microseconds duration)
{
    os << duration.count() << "us";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::chrono::milliseconds duration)
{
    os << duration.count() << "ms";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::chrono::seconds duration)
{
    os << duration.count() << "s";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::chrono::minutes duration)
{
    os << duration.count() << "min";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::chrono::hours duration)
{
    os << duration.count() << "h";
    return os;
}

} // namespace chrono
} // namespace std
