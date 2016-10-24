#ifndef COLOR_H
#define COLOR_H

#include <cstdlib>

class Color
{
public:
	float r, g, b, a;
    
    Color(): r(0), g(0), b(0), a(0){}
    Color(float _r, float _g, float _b, float _a = 1.0f);
    explicit Color(uint32_t rgba);
    
    void set(float _r, float _g, float _b, float _a = 1.0f);
    void fromRGBA(uint32_t cr);
    void fromARGB(uint32_t cr);
    void fromRGB(uint32_t cr);
};

inline Color::Color(float _r, float _g, float _b, float _a)
: r(_r), g(_g), b(_b), a(_a)
{}

inline void Color::set(float _r, float _g, float _b, float _a)
{
    r = _r; g = _g; b = _b; a = _a;
}

inline Color::Color(uint32_t rgba)
{
    fromRGBA(rgba);
}

inline void Color::fromRGBA(uint32_t cr)
{
    float f = 1.0f / 255.0f;
    r = ((cr >> 24) & 0xff) * f;
    g = ((cr >> 16) & 0xff) * f;
    b = ((cr >>  8) & 0xff) * f;
    a = ((cr >>  0) & 0xff) * f;
}

inline void Color::fromARGB(uint32_t cr)
{
    float f = 1.0f / 255.0f;
    a = ((cr >> 24) & 0xff) * f;
    r = ((cr >> 16) & 0xff) * f;
    g = ((cr >>  8) & 0xff) * f;
    b = ((cr >>  0) & 0xff) * f;
}

inline void Color::fromRGB(uint32_t cr)
{
    float f = 1.0f / 255.0f;
    a = 1.0f;
    r = ((cr >> 16) & 0xff) * f;
    g = ((cr >>  8) & 0xff) * f;
    b = ((cr >>  0) & 0xff) * f;
}

#endif //COLOR_H
