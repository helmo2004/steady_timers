#pragma once

#include <chrono>
#include <ostream>

namespace std {
namespace chrono {

/** ostream-operator to pretty print nanoseconds */
std::ostream& operator<<(std::ostream& os, const std::chrono::nanoseconds duration);

/** ostream-operator to pretty print microseconds */
std::ostream& operator<<(std::ostream& os, const std::chrono::microseconds duration);

/** ostream-operator to pretty print milliseconds */
std::ostream& operator<<(std::ostream& os, const std::chrono::milliseconds duration);

/** ostream-operator to pretty print seconds */
std::ostream& operator<<(std::ostream& os, const std::chrono::seconds duration);

/** ostream-operator to pretty print minutes */
std::ostream& operator<<(std::ostream& os, const std::chrono::minutes duration);

/** ostream-operator to pretty print hours */
std::ostream& operator<<(std::ostream& os, const std::chrono::hours duration);

} // namespace chrono
} // namespace std
