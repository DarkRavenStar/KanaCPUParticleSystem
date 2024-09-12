#pragma once

struct ImVec4;

// RGBA = XYZW
using Color = ImVec4;

namespace ColorHelper
{
	//Alpha Color
	constexpr static Color Alpha1 = {1.0f, 1.0f, 0.5f, 1.0f};
	constexpr static Color Alpha2 = {0.0f, 1.0f, 0.5f, 0.5f};


	//Color Declaration
	constexpr static Color White =	{1.0f, 1.0f, 1.0f, 1.0f};
	constexpr static Color Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	constexpr static Color Black15 = { 0.15f, 0.15f, 0.15f, 1.15f };
	constexpr static Color Black25 = { 0.25f, 0.25f, 0.25f, 1.15f };
	constexpr static Color Silver = 	{0.75f, 0.75f, 0.75f, 1.0f};
	constexpr static Color Gray = 	{0.5f, 0.5f, 0.5f, 1.0f};
	
	//Primary Color
	constexpr static Color Orange = 	{1.0f, 0.65f, 0.0f, 1.0f};
	constexpr static Color Blue = 	{0.0f, 0.0f, 1.0f, 1.0f};
	constexpr static Color Green = 	{0.0f, 1.0f, 0.0f, 1.0f};
	constexpr static Color Red = 	{1.0f, 0.0f, 0.0f, 1.0f};
	constexpr static Color Yellow = 	{1.0f, 1.0f, 0.0f, 1.0f};
	constexpr static Color Purple = 	{0.5f, 0.0f, 0.5f, 1.0f};
	constexpr static Color LightPurple = {1.0f, 0.5f, 1.0f, 1.0f};
	
	//Tertiary Color
	constexpr static Color Maroon =	{1.0f, 1.0f, 1.0f, 1.0f};
	constexpr static Color Olive = 	{0.5f, 0.5f, 0.0f, 1.0f};
	constexpr static Color Lime = 	{0.2f, 1.0f, 0.0f, 1.0f};
	constexpr static Color Aqua = 	{0.0f, 1.0f, 1.0f, 1.0f};
	constexpr static Color Teal = 	{0.0f, 0.5f, 0.5f, 1.0f};
	constexpr static Color Navy = 	{0.0f, 0.0f, 0.5f, 1.0f};
	constexpr static Color Fuchsia = { 1.0f, 0.0f, 1.0f, 1.0f };

	static inline Color LerpRGBA(Color a, Color b, float t)
	{
		return ImLerp(a, b, t);
	}
}


