#ifndef VEC2
#define VEC2
#include <cmath>

// 2 demensional math vector class
class Vec2
{
    double x;
    double y;

    public:
    Vec2() = default;
    Vec2(double x, double y) : x(x), y(y) {}
    Vec2(const Vec2& rhs) : x(rhs.x), y(rhs.y) {}
    Vec2& operator=(const Vec2& rhs) {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }
    ~Vec2() = default;

    Vec2 operator+(const Vec2& rhs) {
        return Vec2(x + rhs.x, y + rhs.y);
    }

    Vec2& operator+=(const Vec2& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vec2 operator*(double mult) {
        return Vec2(x * mult, y * mult);
    }

    Vec2  operator/(double div) {
        return Vec2(x / div, y / div);
    }
    
    inline void zero() {
        x = 0;
        y = 0;
    }

    inline double mag() {
        return std::sqrtl(std::powl(x, 2) + std::powl(y, 2));
    }
};
#endif
