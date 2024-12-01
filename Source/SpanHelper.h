#pragma once

#include <cstddef>
#include <iterator>    
#include <iostream>
#include <vector>
#include <array>
#include <tuple>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <span>

/*
template <typename T>
decltype(auto) ReverseRange(T& container)
{
	return std::make_pair(container.rbegin(), container.rend());
}

template <typename T>
decltype(auto) Range(T& container)
{
	return std::make_pair(container.begin(), container.end());
}
*/

namespace SpanHelper
{
	// Helper trait to check if a type is contiguous
	template<typename T>
	struct IsContiguousContainer : std::false_type {};

	// Specialization for std::array
	template<typename T, std::size_t N>
	struct IsContiguousContainer<std::array<T, N>> : std::true_type {};

	// Specialization for std::vector
	template<typename T, typename Allocator>
	struct IsContiguousContainer<std::vector<T, Allocator>> : std::true_type {};

	// Helper trait to check if a type is span
	template<typename T>
	struct IsSpan : std::false_type {};

	template<typename T>
	struct IsSpan<std::span<T>> : std::true_type {};

	//Convert other container type to Span first
	template<typename Container>
	decltype(auto) MakeSpan(Container& c)
	{
		if constexpr (IsSpan<Container>::value)
		{
			return c;
		}
		else if constexpr (IsContiguousContainer<Container>::value)
		{
			return std::span(c);
		}
		else
		{
			return std::begin(c);
		}
	}

}
