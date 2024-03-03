typedef struct
{
    float r, g, b, a;
} Colour;

struct Vector3
{
    float x, y, z;

    Vector3 operator-(const Vector3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    Vector3 operator*(const Vector3& other) const {
        return {x * other.x, y * other.y, z * other.z};
    }

    Vector3 operator+(const Vector3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    Vector3 normalized() const {
    float mag = magnitude();
    if (mag != 0.0f) {
        return {x / mag, y / mag, z / mag};
    } else {
        return {0.0f, 0.0f, 0.0f};
    }
}

    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    float magnitude() const {
        return sqrt(x * x + y * y + z * z);
    }

    void normalize() {
        float mag = magnitude();
        if (mag != 0.0f) {
            x /= mag;
            y /= mag;
            z /= mag;
        }
    }
};

typedef struct
{
    Vector3 position;
} Point;

typedef struct
{
    Point origin;
    Vector3 direction;
} Ray;

typedef struct
{
    Vector3 direction;
} Normal;

typedef struct {
    Vector3 center;
    float radius;   
} Sphere;

enum ShapeType {
    SPHERE,
    CUBE,
    //Lol no
};

typedef struct {
    //If i cared enough i'd have roughness etc here
    Colour colour;
    ShapeType shape;
    union {
        Sphere sphere;
    } geometry;
} Object;

typedef struct
{
    Vector3 position;
    Colour colour;
    float brightness;
} Light;

typedef struct
{
    Colour colour;
} Pixel;
