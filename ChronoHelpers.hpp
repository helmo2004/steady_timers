#pragma once

#include <chrono>
#include <ostream>

std::ostream& operator << (std::ostream& os, const std::chrono::milliseconds ms);

