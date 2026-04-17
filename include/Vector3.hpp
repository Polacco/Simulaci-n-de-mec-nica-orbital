#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <cmath>

struct Vector3 {
    double x, y, z;

    // Constructores
    Vector3(double _x = 0, double _y = 0, double _z = 0) : x(_x), y(_y), z(_z) {}

    // Suma de vectores
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    // Resta de vectores
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    // Multiplicacion por escalar
    Vector3 operator*(double scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    // Magnitud del vector
    double magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Normalizacion
    Vector3 normalized() const {
        double mag = magnitude();
        if (mag == 0) return Vector3(0, 0, 0);
        return *this * (1.0 / mag);
    }
};

#endif