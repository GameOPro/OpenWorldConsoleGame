#pragma once

#include <iostream>

struct Vector2{
    float x;
    float y;

    Vector2() {x = 0.f; y = 0.f;};
    Vector2(float X, float Y) {x = X; y = Y;}
};

typedef struct Vector2I{
    int x;
    int y;
    
    Vector2I() { x = -__INT_MAX__; y = -__INT_MAX__;}
    Vector2I(int X, int Y) {x = X; y = Y;}

    void write(std::ostream& f){
        f.write((char*)&x, sizeof(x));
        f.write((char*)&y, sizeof(y));
    }
    void read(std::istream& f){
        f.read((char*)&x, sizeof(x));
        f.read((char*)&y, sizeof(y));
    }
} Vector2I;

inline bool operator==(const Vector2I& lhs, const Vector2I& rhs)
{
    if(lhs.x == rhs.x && lhs.y == rhs.y){
        return true;
    }
    return false;
};
inline bool operator!=(const Vector2I& lhs, const Vector2I& rhs){
    if(lhs.x != rhs.x || lhs.y != rhs.y){
        return true;
    }
    return false;
};
inline std::ostream& operator<<(std::ostream& os, const Vector2I& pos){
    os << "(" << pos.x << ", " << pos.y << ")";
    return os;
};

struct Vector3{
    float x;
    float y;
    float z;

    Vector3(float X, float Y, float Z) {x = X; y = Y; z = Z;}
};

struct Vector3I{
    int x;
    int y;
    int z;

    Vector3I(int X, int Y, int Z) {x = X; y = Y; z = Z;}
};