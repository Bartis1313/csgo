#pragma once

#include <cstddef>

#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#define PAD(SIZE) \
private: \
	std::byte MACRO_CONCAT(_pad, __COUNTER__)[SIZE]; \
public: