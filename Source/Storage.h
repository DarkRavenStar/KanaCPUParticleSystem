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
	template <typename T>
	struct TypeNameHelper;

	//Teena - example of codegen-ing error messages for typelist
	template <>
	struct TypeNameHelper<std::string>
	{
		// A function to explicitly trigger a compile-time error
		template <bool throwError = false>
		static constexpr void ThrowErrorIsMissing()
		{
			static_assert(!throwError, "std::string not found in the type list!");
		}
	};

	template <typename T, typename CastType, typename PureType, typename CastTypeListAlias, typename PureTypeListAlias>
	consteval void CheckValidAccess(CastTypeListAlias, PureTypeListAlias)
	{
		constexpr bool v_IsNotConstType = !std::is_const_v<CastType>;
		constexpr bool v_ContainsCastType = TypeListHelper::Contains<CastType>(CastTypeListAlias{});
		constexpr bool v_ContainsPureType = TypeListHelper::Contains<PureType>(PureTypeListAlias{});
		constexpr bool v_AllowNonConstAccess = v_IsNotConstType ? v_ContainsCastType : true;

		//Also make it trace-able to offending call-stack
		//Cannot output type name at the moment
		//Can solve using codegen and template specialization to do static assert
		//if the scope of this project expands
		static_assert(v_ContainsPureType, "Type not found in the type list!");

		//If the type is const, then only allow const access
		if constexpr (v_ContainsPureType)
		{
			static_assert(v_AllowNonConstAccess, "Non-const access is not allowed for this type");
		}
	}

	template <typename PureType, typename ...AvailableTypes>
	consteval void CheckContainsInTypeList(TypeList<AvailableTypes...> availableTypes)
	{
		constexpr bool v_ContainsPureType = TypeListHelper::Contains<PureType>(availableTypes);

		//Teena - example of codegen-ing error messages for typelist
		//if constexpr (!v_ContainsPureType) TypeNameHelper<PureType>::ThrowError();

		//Also make it trace-able to offending call-stack
		//Cannot output type name at the moment
		//Can solve using codegen and template specialization to do static assert
		//if the scope of this project expands
		static_assert(v_ContainsPureType, "Type not found in the type list!");

		// Ensures evaluation regardless of usage - not needed as consteval fixed this
		//[[maybe_unused]] constexpr bool enforceEvaluation = v_ContainsPureType;
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
	consteval decltype(auto) GetViewTypeIndices(TypeList<ViewTypes...>, TypeList<AvailableTypes...> availableTypes)
	{
		return std::array<std::size_t, sizeof...(ViewTypes)>{ TypeListHelper::GetIndex<ViewTypes>(availableTypes)... };
	}

	// Function to get compile-time indices for a subset of types
	template <typename... ViewTypes, typename... AvailableTypes>
	consteval decltype(auto) GetViewTypeIndicesIndexSequence(TypeList<ViewTypes...>, TypeList<AvailableTypes...> availableTypes)
	{
		return std::index_sequence<TypeListHelper::GetIndex<ViewTypes>(availableTypes)...>{};
	}


	template <typename... ViewTypes, typename ... AvailableTypes>
	consteval void CheckViewTypesVsAvailableTypesAccess(TypeList<ViewTypes...>, TypeList<AvailableTypes...> availableTypes)
	{
		(ViewHelper::CheckContainsInTypeList<ViewTypes>(availableTypes), ...);
	}

	template <typename T, typename ...CastTypeListAlias, typename ...PureTypeListAlias>
	consteval void CheckValidAccess(TypeList<CastTypeListAlias...> castTypes, TypeList<PureTypeListAlias...> pureTypes)
	{
		using PureType = std::remove_cvref_t<T>;
		using CastType = std::remove_reference_t<T>;

		ViewHelper::CheckValidAccess<T, CastType, PureType>(castTypes, pureTypes);
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

	StorageData()
	{
		InitializeStorage(PureTypeListAlias{});
	}

	decltype(auto) AddNewElement()
	{
		constexpr size_t typesSize = TypeListHelper::GetCount(PureTypeListAlias{});
		return CreateNewElementImpl(PureTypeListAlias{}, std::make_index_sequence<typesSize>());
	}

	void RemoveElement(std::size_t index)
	{
		constexpr size_t typesSize = TypeListHelper::GetCount(PureTypeListAlias{});
		DeleteElementImpl(PureTypeListAlias{}, std::make_index_sequence<typesSize>(), index);
	}

	// Teena - we can do the operation in one function for remove and add
	// but that causes debugging to suffer further than using 2 function
	/*
	void RemoveElementTemplatedOneFunction(std::size_t index)
	{
		constexpr size_t typesSize = TypeListHelper::GetCount(PureTypeListAlias{});

		std::apply
		(
			[index](auto&... vectors)
			{
				// Use a fold expression to erase the element at the specified index.
				((vectors.erase(vectors.begin() + index)), ...);
			},
			[&]<typename... StorageTypes, std::size_t... Indices>(TypeList<StorageTypes...>, std::index_sequence<Indices...>, std::size_t index)
			{
				return std::tuple<std::vector<StorageTypes>&...> 
				{
					std::any_cast<std::vector<StorageTypes>&>(std::get<Indices>(m_Storage))...
				};
			}(PureTypeListAlias{}, std::make_index_sequence<typesSize>{}, index)
		);
	}
	*/

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

	decltype(auto) GetStorageReferences()
	{
		constexpr size_t typesSize = TypeListHelper::GetCount(PureTypeListAlias{});
		return GetStorageReferencesImpl(PureTypeListAlias{}, std::make_index_sequence<typesSize>());
	}

private:
	template <typename... StorageTypes>
	void InitializeStorage(TypeList<StorageTypes...>)
	{
		m_Storage = { std::vector<StorageTypes>()... };
	}

	template<typename... StorageTypes, std::size_t... Indices>
	decltype(auto) GetStorageReferencesImpl(TypeList<StorageTypes...>, std::index_sequence<Indices...>)
	{
		return std::tuple<std::vector<StorageTypes>&...> {std::any_cast<std::vector<StorageTypes>&>(m_Storage[Indices])...};
	}

	// Helper function to create a new element in each vector
	template <typename... StorageTypes, std::size_t... Indices>
	std::tuple<StorageTypes&...> CreateNewElementImpl(TypeList<StorageTypes...>, std::index_sequence<Indices...>)
	{
		std::tuple<std::vector<StorageTypes>&...> elem = { std::any_cast<std::vector<StorageTypes>&>(m_Storage[Indices])...};

		return { std::get<Indices>(elem).emplace_back()... };
	}
	
	// Helper function to delete element
	template <typename... StorageTypes, std::size_t... Indices>
	void DeleteElementImpl(TypeList<StorageTypes...>, std::index_sequence<Indices...>, std::size_t index)
	{
		std::tuple<std::vector<StorageTypes>&...> elem = { std::any_cast<std::vector<StorageTypes>&>(m_Storage[Indices])...};

		((std::get<Indices>(elem).erase(std::get<Indices>(elem).begin() + index)), ...);
	}

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

		ViewHelper::CheckViewTypesVsAvailableTypesAccess(PureTypeListAlias{}, AvailableTypeListAlias{});

		auto viewTypeIndices = ViewHelper::GetViewTypeIndicesIndexSequence(PureTypeListAlias{}, AvailableTypeListAlias{});
		
		CreateSpanFromStorage(storageData, CastTypeListAlias{}, viewTypeIndices);
		//CreateSpanFromStorage(storageData, PureTypeListAlias{}, viewTypeIndices);
    }

	template <typename T>
    std::span<T> GetSpan() const
	{
		using PureType = std::remove_cvref_t<T>;
		using CastType = std::remove_reference_t<T>;
		//using AnyCastType = std::span<CastType>;
		using AnyCastType = std::span<PureType>;
		using ReturnCastType = std::span<CastType>;
		
		//std::cout << "GetSpan - PureType" << typeid(PureType).name() << "\n";
		//std::cout << "GetSpan - CastType" << typeid(CastType).name() << "\n";
		//std::cout << "GetSpan - AnyCastType" << typeid(AnyCastType).name() << "\n";

		ViewHelper::CheckValidAccess<T>(CastTypeListAlias{}, PureTypeListAlias{});
		
		constexpr std::size_t index = TypeListHelper::GetIndex<PureType>(PureTypeListAlias{});
		ReturnCastType temp = std::any_cast<ReturnCastType>(m_StorageSpan[index]);
		//ReturnCastType temp = std::any_cast<AnyCastType>(m_StorageSpan[index]);
		return temp;
    }

	decltype(auto) GetAllSpan() const
	{
		return GetMultiSpan(CastTypeListAlias{});
	}

	// Overload for direct variadic template arguments
	template<typename... ViewTypes>
	decltype(auto) GetMultiSpan() const
	{
		return GetMultiSpan(TypeList<ViewTypes...>{});
	}

    // Overload for TypeList
    template <typename... ViewTypes>
    decltype(auto) GetMultiSpan(TypeList<ViewTypes...>) const
    {
		// Can use TypeList directly also
	    // Cumbersome part is that the type need
        // to hovered over to properly declare
        // structure bindings. Maybe can be fixed
		
		//((std::cout << "GetMultiSpan" << typeid(ViewTypes).name() << "\n"), ...);
        return MultiSpanHelper::GetZipped(GetSpan<ViewTypes>()...);
    }

private:
	
	// Helper function to create a span from the storage and indices dynamically
	template <typename... StorageTypes, typename... ViewTypes, std::size_t... Indices>
	void CreateSpanFromStorage(StorageData<StorageTypes...>& storageData, TypeList<ViewTypes...> viewTypes, std::index_sequence<Indices...>)
	{
		//((std::cout << typeid(ViewTypes).name() << "\n"), ...);

		auto storage = storageData.GetStorageReferences();
		m_StorageSpan = { std::span<ViewTypes>(std::get<Indices>(storage))... };
	}

	std::array<std::any, sizeof...(Types)> m_StorageSpan;
};


template<typename... Types>
struct SystemData
{
	using PureTypeListAlias = TypeList<std::remove_cvref_t<Types>...>;

	SystemData()
	{
		InitializeStorage(PureTypeListAlias{});
	}

	// Implicit conversion to SystemDataViewAlias
	template<typename... ViewTypes>
	operator SystemDataView<ViewTypes...>() const
	{
		using CastTypeListAlias = SystemDataView<ViewTypes...>::CastTypeListAlias;
		return ToSystemDataView(CastTypeListAlias{});
	}

	template<typename ... ViewTypes>
	SystemDataView<ViewTypes...> ToSystemDataView(TypeList<ViewTypes...>)
	{
		return SystemDataView<ViewTypes...>(*this);
	}

	std::array<std::any, sizeof...(Types)> GetStorage()
	{
		return m_Storage;
	}

	decltype(auto) GetStorageReferences()
	{
		constexpr size_t typesSize = TypeListHelper::GetCount(PureTypeListAlias{});
		return GetStorageReferencesImpl(PureTypeListAlias{}, std::make_index_sequence<typesSize>());
	}

private:
	template <typename... StorageTypes>
	void InitializeStorage(TypeList<StorageTypes...>)
	{
		m_Storage = { StorageTypes{}... };
	}

	template<typename... StorageTypes, std::size_t... Indices>
	decltype(auto) GetStorageReferencesImpl(TypeList<StorageTypes...>, std::index_sequence<Indices...>)
	{
		return std::tuple<StorageTypes&...> {std::any_cast<StorageTypes&>(m_Storage[Indices])...};
	}

	std::array<std::any, sizeof...(Types)> m_Storage;
};

template<typename... Types>
struct SystemDataView
{
	using CastTypeListAlias = TypeList<std::remove_reference_t<Types>...>;
	using PureTypeListAlias = TypeList<std::remove_cvref_t<Types>...>;

	template<typename... SystemDataTypes>
	SystemDataView(SystemData<SystemDataTypes...>& systemData)
	{
		using AvailableTypeListAlias = StorageData<SystemDataTypes...>::PureTypeListAlias;

		ViewHelper::CheckViewTypesVsAvailableTypesAccess(PureTypeListAlias{}, AvailableTypeListAlias{});

		constexpr auto viewTypeIndices = ViewHelper::GetViewTypeIndicesIndexSequence(PureTypeListAlias{}, AvailableTypeListAlias{});

		CreateSystemDataView(systemData, viewTypeIndices);
	}

	template <typename T>
	decltype(auto) GetSystemData() const
	{
		using PureType = std::remove_cvref_t<T>;
		using CastType = std::remove_reference_t<T>;
		using AnyCastType = std::reference_wrapper<PureType>;
		using ReturnCastType = std::reference_wrapper<CastType>;
		
		//std::cout << "GetSystemData - PureType: " << typeid(PureType).name() << "\n";
		//std::cout << "GetSystemData - CastType1: " << typeid(CastType).name() << "\n";
		//std::cout << "GetSystemData - AnyCastType: " << typeid(AnyCastType).name() << "\n";
		//std::cout << "GetSystemData - ReturnCastType: " << typeid(ReturnCastType).name() << "\n";

		ViewHelper::CheckValidAccess<T>(CastTypeListAlias{}, PureTypeListAlias{});

		constexpr std::size_t index = TypeListHelper::GetIndex<PureType>(PureTypeListAlias{});
		ReturnCastType temp = std::any_cast<AnyCastType>(m_SystemDataView[index]);
		return temp.get();
	}

private:

	// Helper function to create a span from the storage and indices dynamically
	template <typename... SystemDataTypes, std::size_t... Indices>
	void CreateSystemDataView(SystemData<SystemDataTypes...>& systemData, std::index_sequence<Indices...>)
	{
		auto storage = systemData.GetStorageReferences();

		//((std::cout << typeid(ViewTypes).name() << "\n"), ...);
		//((std::cout << "CreateSystemDataView: " << typeid(std::ref(std::get<Indices>(storage))).name() << "\n"), ...);

		m_SystemDataView = { std::ref(std::get<Indices>(storage))... };
	}

	std::array<std::any, sizeof...(Types)> m_SystemDataView;
};
