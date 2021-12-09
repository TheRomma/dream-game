#pragma once

#include <iostream>
#include <cmath>
#include <cstring>
#include <algorithm>

//A three dimentional vector.
struct Vec3{
	Vec3();
	Vec3(float x, float y, float z);
	Vec3(const Vec3& other);
	~Vec3();

	//Operators
	Vec3 operator+(const Vec3& u);
	Vec3 operator-(const Vec3& u);
	Vec3 operator*(float s);
	Vec3 operator/(float s);
	bool operator==(const Vec3& u);

	//Static functions
	static float dot(const Vec3& u, const Vec3& v);
	static Vec3 cross(const Vec3& u, const Vec3& v);
	static Vec3 tripleCross(const Vec3& u, const Vec3& v, const Vec3& w);
	static Vec3 normalize(const Vec3& u);
	static Vec3 interpolate(Vec3& u, Vec3& v, float t);
	static Vec3 max(const Vec3& u, const Vec3& v);
	static Vec3 min(const Vec3& u, const Vec3& v);

	//Internal functions.
	void normalize();
	void makeMax(Vec3& u);
	void makeMin(Vec3& u);

	//Getters
	float* ptr();
	float length();
	float manhattan();

	//Debug
	void print();

	float x, y, z;
};

struct Mat4{
	Mat4();
	Mat4(float* arr);
	Mat4(const Mat4& other);
	~Mat4();

	//Operators
	Mat4 operator*(const Mat4& M);
	Vec3 operator*(const Vec3& u);
	Mat4 operator*(const float& b);

	//Static create functions
	static Mat4 identity();
	static Mat4 translation(float x, float y, float z);
	static Mat4 translation(Vec3 u);
	static Mat4 scale(float w, float h, float d);
	static Mat4 scale(Vec3 u);
	static Mat4 orthographic(float left, float right, float bottom, float top, float near, float far);
	static Mat4 perspective(float fov, float aspect, float near, float far);
	static Mat4 lookAt(Vec3 position, Vec3 target, Vec3 up);

	//Static functions
	static Mat4 transpose(const Mat4& M);
	
	//Internal functions
	void transform(const Mat4& M);
	Vec3 transform(const Vec3& u, float W, float& w);
	void transpose();

	//Getters
	float* ptr();
	float determinant();
	Mat4 cofactor();
	Mat4 inverse();

	//Debug
	void print();

	float m[4][4];
};

struct Quat{
	Quat();
	Quat(float w, float x, float y, float z);
	Quat(float angle, Vec3 axis);
	Quat(const Quat& other);
	~Quat();

	//Static functions
	static Quat slerp(const Quat& a, const Quat& b, float t);
	static Quat normalize(Quat q);

	//Internal functions
	void normalize();
	Mat4 toMatrix();

	//Getters
	float* ptr();
	float length();

	//Debug
	void print();

	float w, x, y, z;
};
