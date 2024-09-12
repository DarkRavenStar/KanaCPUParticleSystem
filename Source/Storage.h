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


template<typename... Types>
struct StorageData
{
	//using CastTypeListAlias = TypeList<std::remove_reference_t<Types>...>;
	using RefTypeListAlias = TypeList<std::remove_reference_t<Types>&...>;
	using PureTypeListAlias = TypeList<std::remove_cvref_t<Types>...>;

	auto AddNewElement()
	{
		return AddNewElementTypeList(PureTypeListAlias{});
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

// 
// 	template<typename ... FindTypes>
// 	auto ToStorageView(TypeList<FindTypes...>)
// 	{
// 
// 		//return StorageView<Types...>(m_Storage[0]...);
// 		//return StorageView<FindTypes...>(std::any_cast<std::vector<int>>(m_Storage[0]), std::any_cast<std::vector<float>>(m_Storage[1]));
// 
// 		return StorageView<FindTypes...>(std::any_cast<std::vector<FindTypes>&>(m_Storage[TypeListHelper::GetIndex<FindTypes>(PureTypeListAlias{})]) ...);
// 		//return StorageView<int, float>{std::any_cast<std::vector<int>&>(m_Storage[0]), std::any_cast<std::vector<float>&>(m_Storage[1])};
// 	}

	/*
	// Conversion function to transform StorageData to StorageView
	template <typename... FindTypes>
	StorageView<FindTypes...> ToStorageView(TypeList<FindTypes...>)
	{
		//TypeListHelper::GetIndex(Types{}, PureTypeListAlias{})
		//return StorageView<Types...>(m_Storage[0]...);
		int index = 0;
		std::any_cast<std::vector<T>&>(m_Storage[index]);
		return StorageView<FindTypes...>(std::any_cast<std::vector<FindTypes>>(m_Storage[index++])...);
	}
	*/

	std::array<std::any, sizeof...(Types)> GetStorage()
	{
		return m_Storage;
	}

private:

	template<typename ... FindTypes>
	auto AddNewElementTypeList(TypeList<FindTypes...>)
	{
		return CreateNewElementImpl<FindTypes...>(0);
	}


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


	std::array<std::any, sizeof...(Types)> m_Storage{ std::vector<std::remove_cvref_t<Types>>() ... };
};


template<typename... Types>
struct StorageView
{
	using CastTypeListAlias = TypeList<std::remove_reference_t<Types>...>;
    using PureTypeListAlias = TypeList<std::remove_cvref_t<Types>...>;
	
	/*
	// Helper function to create a tuple from the storage and indices dynamically
	template <typename... RequestedTypes, std::size_t... Indices>
	auto CreateSpanFromStorage(const std::array<std::size_t, sizeof...(RequestedTypes)>& indices, std::index_sequence<Indices...>)
	{
		return std::make_tuple(std::any_cast<RequestedTypes>(storage[indices[Indices]])...);
	}
	*/
	
	

	template<typename... StorageTypes>
    StorageView(const StorageData<StorageTypes...>& storageData)
	{
		using AvailableTypeListAlias = StorageData<StorageTypes...>::PureTypeListAlias;

		//CheckViewTypesVsAvailableTypesAccess(PureTypeListAlias{}, AvailableTypeListAlias{});
		
		//constexpr auto indices = ViewHelper::GetViewTypeIndices(viewTypes, availableTypes);
		//constexpr auto viewTypeCount = std::make_index_sequence<(indices.size())>{};
		constexpr auto viewTypeCount = ViewHelper::GetViewTypeIndicesIndexSequence(PureTypeListAlias{}, AvailableTypeListAlias{});;

		//std::index_sequence<>

		CreateSpanFromStorage(viewTypeCount, storageData, PureTypeListAlias{});

		//constexpr auto indices = ViewHelper::GetViewTypeIndices(PureTypeListAlias{}, AvailableTypeListAlias{});
		//

		//return std::any_cast<AnyCastType>(m_StorageSpan[index]);

		//int index = 0;
		//constexpr auto viewTypeIndices = GetSelectedStorageIndices(PureTypeListAlias{}, AvailableTypeListAlias{});

		//m_StorageSpan. = { std::get<ViewTypeIndex>(storageData.m_Storage), ... };

    }



	/*
	//template <typename... ViewTypes, std::size_t... Indices>
	template <typename... ViewTypes, typename... AvailableTypes>
	constexpr std::array<std::size_t, sizeof...(ViewTypes)> GetSelectedStorageIndices(TypeList<ViewTypes...>, TypeList<AvailableTypes...>)
	{
		return { TypeListHelper::GetIndex<ViewTypes>(AvailableStorageTypes{}), ... };
		//return std::index_sequence_for < TypeListHelper::GetIndex<ViewTypes>(TypeList<AvailableTypes...>{}), ... > ();
	}
	*/

// 	
//     StorageView(Types&... args) :
// 		m_StorageSpan{ std::span<std::remove_cvref_t<Types>>(args)... }
// 	{
//     }

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
	template <std::size_t... Indices, typename... StorageTypes, typename... ViewTypes>
	void CreateSpanFromStorage
	(
		std::index_sequence<Indices...>,
		const StorageData<StorageTypes...>& storageData, 
		TypeList<ViewTypes...> viewTypes
	)
	{
		
		m_StorageSpan = { std::span<std::remove_cvref_t<ViewTypes>>(std::get<Indices>(storageData,GetStorage()))... };
	}


	template <typename... ViewTypes, typename ... AvailableTypes>
	void CheckViewTypesVsAvailableTypesAccess(TypeList<ViewTypes...>, TypeList<AvailableTypes...> availableTypes)
	{
		(CheckContainsInTypeList<ViewTypes>(availableTypes), ...);
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
