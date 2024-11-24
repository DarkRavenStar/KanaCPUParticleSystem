#pragma once

template <typename T>
struct ValueWrapper
{
	T value = {};

	// Constructor to initialize the value
	ValueWrapper(T val) : value(val) {}

	// Conversion operator to return a reference to the value
	// Can be used in cases like structure bindings to prevent
	// too many indirection
	operator T& () { return value; }
	operator const T& () const { return value; }

	// Copy assignment operator
	ValueWrapper& operator=(const ValueWrapper& other)
	{
		if (this != &other)
		{
			value = other.value;
		}
		return *this;
	}

	// Move assignment operator
	ValueWrapper& operator=(ValueWrapper&& other) noexcept
	{
		if (this != &other)
		{
			value = std::move(other.value);
		}
		return *this;
	}

	// Assignment operator from the underlying type
	ValueWrapper& operator=(const T& val)
	{
		value = val;
		return *this;
	}

	// Move assignment from the underlying type
	ValueWrapper& operator=(T&& val) noexcept
	{
		value = std::move(val);
		return *this;
	}
};
