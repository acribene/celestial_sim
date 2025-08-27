// My own "vector struct"

struct Vec2
{
    double x;
    double y;

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

    void zero() {
        x = 0;
        y = 0;
    }
};
