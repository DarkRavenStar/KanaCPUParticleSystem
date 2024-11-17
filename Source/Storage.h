#include <iostream>
#include <windows.h>
#include <vector>
#include <cassert>
#include <typeindex>
#include <any>
#include <span>
#include <array>
#include "TypeList.h"
#include "MultiSpanHelper.h"

namespace TypeIDHelper
{
	//Helper function to get a string of available types
	std::string GetAvailableTypes(const std::vector<std::type_index>& typeIds)
	{
		std::string result;
		for (const auto& typeId : typeIds)
		{
			if (!result.empty())
			{
				result += ", ";
			}

			result += typeid(typeId).name();
		}

		return result;
	}
}

namespace ViewHelper
{
	template <typename T, typename CastType, typename PureType, typename CastTypeListAlias, typename PureTypeListAlias>
	void CheckValidAccess(CastTypeListAlias, PureTypeListAlias)
	{
		constexpr bool v_IsNotConstType = !std::is_const_v<CastType>;
		constexpr bool v_ContainsCastType = TypeListHelper::Contains<CastType>(CastTypeListAlias{});
		constexpr bool v_ContainsPureType = TypeListHelper::Contains<PureType>(PureTypeListAlias{});
		constexpr bool v_AllowNonConstAccess = v_IsNotConstType ? v_ContainsCastType : true;

		//Also make it trace-able to offending call-stack
		//Cannot output type name at the moment
		static_assert(v_ContainsPureType, "Type not found in the type list!");

		//If the type is const, then only allow const access
		if constexpr (v_ContainsPureType)
		{
			static_assert(v_AllowNonConstAccess, "Non-const access is not allowed for this type");
		}
	}

	template <typename PureType, typename ...AvailableTypes>
	void CheckContainsInTypeList(TypeList<AvailableTypes...> availableTypes)
	{
		constexpr bool v_ContainsPureType = TypeListHelper::Contains<PureType>(availableTypes);

		//Also make it trace-able to offending call-stack
		//Cannot output type name at the moment
		static_assert(v_ContainsPureType, "Type not found in the type list!");
	}

	template <typename T>
	std::size_t GetTypeIndex(const std::vector<std::type_index>& typeIds)
	{
		auto typeId = std::type_index(typeid(T));
		auto it = std::find(typeIds.begin(), typeIds.end(), typeId);

		//If run-time assert is preferred with more details
		if (it == typeIds.end())
		{
			std::string errorMessage = "Type not found. Expected type: " + std::string(typeid(T).name()) +
				", Available types: " + TypeIDHelper::GetAvailableTypes(typeIds);

			assert(false && errorMessage.c_str());
		}

		return std::distance(typeIds.begin(), it);
	}
	
	// Function to get compile-time indices for a subset of types
	template <typename... ViewTypes, typename... AvailableTypes>
	constexpr auto GetViewTypeIndices(TypeList<ViewTypes...>, TypeList<AvailableTypes...> availableTypes)
	{
		return std::array<std::size_t, sizeof...(ViewTypes)>{ TypeListHelper::GetIndex<ViewTypes>(availableTypes)... };
	}

	// Function to get compile-time indices for a subset of types
	template <typename... ViewTypes, typename... AvailableTypes>
	constexpr auto GetViewTypeIndicesIndexSequence(TypeList<ViewTypes...>, TypeList<AvailableTypes...> availableTypes)
	{
		return std::index_sequence<TypeListHelper::GetIndex<ViewTypes>(availableTypes)...>{};
	}
}

//Forward Declaration
//////////////////////////////////////

// Can use tuple-like structure or unordered map
// and variadic template recursion
// but std::array/vector + index array/compile-time index
// use case is valid as well as interesting design choice
// to experiment with, as a faster alternative and
// since it a void*/std::any to data array
// So half compile-time, half run-time structure
// with both validation?

// In general, none of the these data structure below
// will have add/append new type functionality
// Either make the type list append-able or switch to
// runtime based type index checking or both
// So for now, current implementation is fine

// Usage for type list
// Can be used to auto-schedule system
// Can be used to enforce compile time checks

//Unless we are going with chuck or storage or page
//for in-place creation. Keep it simple for now
template<typename... Types>
struct StorageData;

template<typename... Types>
struct StorageView;

template<typename... Types>
struct SystemDataView;

// This is the common occurring pattern
// in the data structure below
// Instead of rectifying wrongful input
// its better to assert and inform of wrongful
// input and let the user fix it
// Need to revisit if needed
// 
// using PureType = std::remove_cvref_t<T>;
// using CastType = std::remove_reference_t<T>;
// using AnyCastType = std::span<CastType>;
// using RefTypeListAlias = TypeList<std::remove_reference_t<Types>&...>;
// using PureTypeListAlias = TypeList<std::remove_cvref_t<Types>...>;


//////////////////////////////////////

namespace
{

	template<typename T>
	struct type_to_string;

	template<>
	struct type_to_string<int> { constexpr static auto value = "integer"; };

	template <char... T>
	constexpr bool string_value = false;

	template<char... c>
	void compile_time_display()
	{
		static_assert(string_value<c...>);
	}

	// Helper to convert integers to a compile-time string
	template<int N>
	constexpr char ToString()
	{
		if constexpr (N == 0) return '0';
		else if constexpr (N == 1) return '1';
		else if constexpr (N == 2) return '2';
		else if constexpr (N == 3) return '3';
		else if constexpr (N == 4) return '4';
		else if constexpr (N == 5) return '5';
		else if constexpr (N == 6) return '6';
		else if constexpr (N == 7) return '7';
		else if constexpr (N == 8) return '8';
		else if constexpr (N == 9) return '9';
		else return '0'; // Handle unsupported cases
	}

	// Recursively assert for each integer in std::integer_sequence
	template<int N>
	constexpr void PrintIntegerAtCompileTime()
	{
		//constexpr bool print = N >= 0;
		//constexpr const char* message = ToString<N>();
		compile_time_display<ToString<N>()>();
		//static_assert(false, message); // Forces compile-time output
	}

	template<std::size_t... Indices>
	constexpr void PrintIntegerSequenceAtCompileTime(std::index_sequence<Indices...>)
	{
		(PrintIntegerAtCompileTime<Indices>(), ...); // Print each integer in sequence
	}

// 
// 	int main() {
// 		constexpr std::string_view sv = type_to_string<int>::value;
// 		constexpr size_t n = sv.size();
// 		constexpr auto indices = std::make_index_sequence<n>();
// 
// 		[&] <std::size_t... I>
// 			(std::index_sequence<I...>)
// 		{
// 			compile_time_display<sv.at(I)...>();
// 		}(indices);
// 	}
}

template<typename... Types>
struct StorageData
{
	//using CastTypeListAlias = TypeList<std::remove_reference_t<Types>...>;
	using RefTypeListAlias = TypeList<std::remove_reference_t<Types>&...>;
	using PureTypeListAlias = TypeList<std::remove_cvref_t<Types>...>;

	StorageData()
	{
		InitializeStorage(PureTypeListAlias{});
	}

	auto AddNewElement()
	{
		constexpr size_t typesSize = TypeListHelper::GetCount(PureTypeListAlias{});
		return CreateNewElementImpl(PureTypeListAlias{}, std::make_index_sequence<typesSize>());
	}

	void RemoveElement(std::size_t index)
	{
		for (auto& anyVec : m_Storage)
		{
			std::any_cast<std::vector<void>*>(&anyVec)->erase(index);
		}
	}

	// Implicit conversion to StorageViewAlias
	template<typename... ViewTypes>
	operator StorageView<ViewTypes...>() const
	{
		using CastTypeListAlias = StorageView<ViewTypes...>::CastTypeListAlias;
		return ToStorageView(CastTypeListAlias{});
	}

	template<typename ... ViewTypes>
	StorageView<ViewTypes...> ToStorageView(TypeList<ViewTypes...>)
	{
		return StorageView<ViewTypes...>(*this);
	}

	std::array<std::any, sizeof...(Types)> GetStorage()
	{
		return m_Storage;
	}

	auto GetStorageReferences()
	{
		constexpr size_t typesSize = TypeListHelper::GetCount(PureTypeListAlias{});
		return GetStorageReferencesImpl(PureTypeListAlias{}, std::make_index_sequence<typesSize>());
	}


private:

	/*
	// Helper function to create a new element in each vector
	template <typename T, typename... Rest>
	std::tuple<T&, Rest&...> CreateNewElementImpl(std::size_t index)
	{
		auto& vec = std::any_cast<std::vector<T>&>(m_Storage[index]);

		vec.emplace_back(); // Add a default-constructed element

		if constexpr (sizeof...(Rest) > 0)
		{
			return std::tuple_cat(std::tie(vec.back()), CreateNewElementImpl<Rest...>(index + 1));
		}
		else
		{
			return std::tie(vec.back());
		}
	}
	*/
	
	template <typename... StorageTypes>
	void InitializeStorage(TypeList<StorageTypes...>)
	{
		m_Storage = { std::vector<StorageTypes>()... };
	}


	template<typename... StorageTypes, std::size_t... Indices>
	auto GetStorageReferencesImpl(TypeList<StorageTypes...>, std::index_sequence<Indices...>)
	{
		static_assert(true, "GetStorageReferencesImplIndexTesting");
		(PrintIntegerAtCompileTime<Indices>(), ...); // Print each integer in sequence
		return std::tuple<std::vector<StorageTypes>&...> {std::any_cast<std::vector<StorageTypes>&>(m_Storage[Indices])...};
	}

	// Helper function to create a new element in each vector
	template <typename... StorageTypes, std::size_t... Indices>
	std::tuple<StorageTypes&...> CreateNewElementImpl(TypeList<StorageTypes...>, std::index_sequence<Indices...>)
	{
		static_assert(true, "CreateNewElementImplIndexTesting");
		(PrintIntegerAtCompileTime<Indices>(), ...); // Print each integer in sequence

		std::tuple<std::vector<StorageTypes>&...> elem = { std::any_cast<std::vector<StorageTypes>&>(std::get<Indices>(m_Storage))...};

		return { std::get<Indices>(elem).emplace_back()... };

		
		/*
		std::tuple<std::vector<ViewTypes>...> typedStorage = { std::any_cast<std::vector<ViewTypes>>(std::get<Indices>(storageData.GetStorage()))... };

		m_StorageSpan = { std::span(std::get<Indices>(typedStorage))... };

		vec.emplace_back(); // Add a default-constructed element

		if constexpr (sizeof...(Rest) > 0)
		{
			return std::tuple_cat(std::tie(vec.back()), CreateNewElementImpl<Rest...>(index + 1));
		}
		else
		{
			return std::tie(vec.back());
		}
		*/
	}


	//std::array<std::any, sizeof...(Types)> m_Storage{ std::vector<std::remove_cvref_t<Types>>() ... };
	std::array<std::any, sizeof...(Types)> m_Storage;
};

template<typename... Types>
struct StorageView
{
	using CastTypeListAlias = TypeList<std::remove_reference_t<Types>...>;
    using PureTypeListAlias = TypeList<std::remove_cvref_t<Types>...>;
	
	template<typename... StorageTypes>
    StorageView(StorageData<StorageTypes...>& storageData)
	{
		using AvailableTypeListAlias = StorageData<StorageTypes...>::PureTypeListAlias;

		CheckViewTypesVsAvailableTypesAccess(PureTypeListAlias{}, AvailableTypeListAlias{});
		
		constexpr auto viewTypeIndices = ViewHelper::GetViewTypeIndicesIndexSequence(PureTypeListAlias{}, AvailableTypeListAlias{});;

		CreateSpanFromStorage(storageData, PureTypeListAlias{}, viewTypeIndices);

    }

	template <typename T>
    std::span<T> GetSpan() const
	{
		using PureType = std::remove_cvref_t<T>;
		using CastType = std::remove_reference_t<T>;
		using AnyCastType = std::span<CastType>;
		
		CheckValidAccess<T>(CastTypeListAlias{}, PureTypeListAlias{});
		
		constexpr std::size_t index = TypeListHelper::GetIndex<PureType>(PureTypeListAlias{});
        return std::any_cast<AnyCastType>(m_StorageSpan[index]);
    }

	auto GetAllSpan() const
	{
		return GetMultiSpan(CastTypeListAlias{});
	}

	// Overload for direct variadic template arguments
	template<typename... ViewTypes>
	auto GetMultiSpan() const
	{
		return GetMultiSpan(TypeList<ViewTypes...>{});
	}

    // Overload for TypeList
    template <typename... ViewTypes>
    auto GetMultiSpan(TypeList<ViewTypes...>) const
    {
		// Can use TypeList directly also
	    // Cumbersome part is that the type need
        // to hovered over to properly declare
        // structure bindings. Maybe can be fixed
        return MultiSpanHelper::GetZipped(GetSpan<ViewTypes>()...);
    }

private:
	
	// Helper function to create a span from the storage and indices dynamically
	template <typename... StorageTypes, typename... ViewTypes, std::size_t... Indices>
	void CreateSpanFromStorage
	(
		StorageData<StorageTypes...>& storageData,
		TypeList<ViewTypes...> viewTypes,
		std::index_sequence<Indices...>
	)
	{
		auto storage = storageData.GetStorageReferences();
		
		static_assert(false, "CreateSpanFromStorageIndexTesting");
		(PrintIntegerAtCompileTime<Indices>(), ...); // Print each integer in sequence

		m_StorageSpan = { std::span<ViewTypes>(std::get<Indices>(storage))... };
	}

	template <typename... ViewTypes, typename ... AvailableTypes>
	void CheckViewTypesVsAvailableTypesAccess(TypeList<ViewTypes...>, TypeList<AvailableTypes...> availableTypes)
	{
		(ViewHelper::CheckContainsInTypeList<ViewTypes>(availableTypes), ...);
	}


	template <typename T, typename ...CastTypeListAlias, typename ...PureTypeListAlias>
	void CheckValidAccess(TypeList<CastTypeListAlias...> castTypes, TypeList<PureTypeListAlias...> pureTypes) const
	{
		using PureType = std::remove_cvref_t<T>;
		using CastType = std::remove_reference_t<T>;
		
		ViewHelper::CheckValidAccess<T, CastType, PureType>(castTypes, pureTypes);
	}
	
	std::array<std::any, sizeof...(Types)> m_StorageSpan;
};

template<typename... Types>
struct SystemDataView
{
    //Only store reference type for SystemData 
    using TypeListAlias = TypeList<Types&...>;
    using PureTypeListAlias = TypeList<std::remove_cvref_t<Types>...>;
	
	SystemDataView(Types&... args):
		//m_TypeIds{ std::type_index(typeid(std::remove_cvref_t<Types>))... },
		m_SystemDataArray{ args... }
	{

    }

	/*
	// Constructor taking a Storage and creating span views
	StorageView(const StorageData<Types...>& storage)
	{
		storage.ToStorageView().

		views = std::tuple<std::span<const Types>...>(
			std::span<const Types>(std::get<std::vector<Types>>())...
			);
	}

	// Implicit conversion operator (Storage to StorageView)
	template<typename... T>
	StorageView(const Storage<T...>& storage)
	{
		*this = StorageView<Types...>(storage);  // Use the constructor for conversion
	}
	*/


	/*
	template <typename T>
	void AddSystemData(T& systemData)
	{
		using PureType = std::remove_cvref_t<T>;
		constexpr bool v_ContainsPureType = TypeListHelper::Contains<PureType>(PureTypeListAlias{});

		static_assert(!v_ContainsPureType, "Type already exist, only one unique type per storage view!");

		m_TypeIds.emplace_back(std::type_index(typeid(std::remove_cvref_t<T>)));
		m_SystemDataArray.emplace_back(systemData);
	}
	*/

	template <typename T>
    T& GetSystemData()
	{
		using CastType = T&;
		using PureType = std::remove_cvref_t<T>;

		ViewHelper::CheckValidAccess<T, CastType, PureType>(TypeListAlias{}, PureTypeListAlias{});

		constexpr std::size_t index = TypeListHelper::GetIndex<PureType>(PureTypeListAlias{});

		//std::size_t index = ViewHelper::GetTypeIndex<PureType>(m_TypeIds);
		return std::any_cast<CastType>(m_SystemDataArray[index]);
    }

private:
	//std::vector<std::type_index> m_TypeIds;
	std::vector<std::any> m_SystemDataArray;
};
