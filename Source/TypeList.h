#include <iostream>
#include <windows.h>
#include <vector>
#include <cassert>
#include <typeindex>
#include <any>

template <typename T>
struct Type { using type = T; };

template<typename... Types>
struct TypeList {};

namespace TypeListHelper
{
	// Forward-Declaration
	/////////////////////////////////////////////////////
	template <typename T, typename... TTypes>
	constexpr bool Contains(TypeList<TTypes...>  typeList);
	
	template <typename T, typename ... TTypes>
	constexpr bool Contains(Type<T>, TypeList<TTypes...>);

	template <typename T, typename... TTypes>
	constexpr std::size_t GetIndex(TypeList<TTypes...>);
	/////////////////////////////////////////////////////


	// Implementation - should be in .inl file
	// but since it header-level compilation
	// prefer to kept in one place
	/////////////////////////////////////////////////////
	template <typename T, typename... AvailableTypes>
    constexpr bool Contains(TypeList<AvailableTypes...>  availableTypeList)
    {
        return Contains(Type<T>{}, availableTypeList);
    }

    template <typename T, typename ... AvailableTypes>
    constexpr bool Contains(Type<T>, TypeList<AvailableTypes...>)
    {
		// Technically this can be replaced by std::disjunction
		// but this is valid as well for now
        return (false || ... || std::is_same_v<T, AvailableTypes>);
    }
	
	template <typename T, typename... AvailableTypes>
	constexpr std::size_t GetIndex(TypeList<AvailableTypes...>)
	{
		std::size_t index = -1;
		bool found = false;

		((std::is_same_v<T, AvailableTypes> && !found ? found = true : index++), ...);

		return index;
	}

	template <typename... Types>
	constexpr std::size_t GetCount(TypeList<Types...>)
	{
		return sizeof...(Types);
	}
	/////////////////////////////////////////////////////
}