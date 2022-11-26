#pragma once

#include <chrono>
#include <type_traits>

namespace chrono {

using namespace std::chrono;

template <class T>
concept Clock_Type = is_clock_v<T>;

template <class T>
concept Duration_Type = std::convertible_to<T, chrono::seconds>;

};  // namespace chrono
