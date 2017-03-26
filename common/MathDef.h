#pragma once

const float PI_FULL = 3.1415926f;
const float PI_HALF = PI_FULL * 0.5f;
const float PI_QUARTER = PI_FULL / 4.0f;
const float PI_2X = PI_FULL * 2.0f;

template<typename T>
const T& clamp(const T& v, const T& low, const T& high)
{
    if (v < low) return low;
    if (v > high) return high;
    return v;
}
