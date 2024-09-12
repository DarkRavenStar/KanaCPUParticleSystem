#pragma once

//Not a bad approximation for now
constexpr float PI = 3.14f;
//constexpr float TWO_PI = 6.28318527f;


//Toggle between radians and degrees
constexpr bool c_ConvertToRadians = true;
constexpr float c_MaxAngle = c_ConvertToRadians ? 2 * PI : 360.0f;
