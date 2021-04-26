#pragma once
#include <iostream>

// A header with functions for debug printing

template <typename T1>
void debug(T1 x1)
{
	std::cout << x1 << std::endl;
}

template <typename T1, typename T2>
void debug(T1 x1, T2 x2)
{
	std::cout << x1 << " " << x2 << std::endl;
}

template <typename T1, typename T2, typename T3>
void debug(T1 x1, T2 x2, T3 x3)
{
	std::cout << x1 << " " << x2 << " " << x3 << std::endl;
}

template <typename T1, typename T2, typename T3, typename T4>
void debug(T1 x1, T2 x2, T3 x3, T4 x4)
{
	std::cout << x1 << " " << x2 << " " << x3 << " " << x4 << std::endl;
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
void debug(T1 x1, T2 x2, T3 x3, T4 x4, T5 x5)
{
	std::cout << x1 << " " << x2 << " " << x3 << " " << x4 << " " << x5 << std::endl;
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
	typename T6>
	void debug(T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6)
{
	std::cout << x1 << " " << x2 << " " << x3 << " " << x4 << " " << x5 <<
		" " << x6 << std::endl;
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
	typename T6, typename T7>
	void debug(T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7)
{
	std::cout << x1 << " " << x2 << " " << x3 << " " << x4 << " " << x5 <<
		" " << x6 << " " << x7 << std::endl;
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
	typename T6, typename T7, typename T8>
	void debug(T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8)
{
	std::cout << x1 << " " << x2 << " " << x3 << " " << x4 << " " << x5 <<
		" " << x6 << " " << x7 << " " << x8 << std::endl;
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
	typename T6, typename T7, typename T8, typename T9>
	void debug(T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9)
{
	std::cout << x1 << " " << x2 << " " << x3 << " " << x4 << " " << x5 <<
		" " << x6 << " " << x7 << " " << x8 << " " << x9 << std::endl;
}

template <typename T1, typename T2, typename T3, typename T4, typename T5,
	typename T6, typename T7, typename T8, typename T9, typename T10>
	void debug(T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10)
{
	std::cout << x1 << " " << x2 << " " << x3 << " " << x4 << " " << x5 <<
		" " << x6 << " " << x7 << " " << x8 << " " << x9 << " " << x10 << std::endl;
}