#pragma once

enum NoBaseInit
{
	NO_BASE_INIT
};

template <typename T, size_t L>
struct array_wrapper{
	T data[L];
};