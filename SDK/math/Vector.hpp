#pragma once
#define M_PI	3.14159265358979323846f
#define NOTHING Vector(0, 0, 0)
#include <numbers>
#include <cmath>
#include <algorithm>
constexpr auto DEG2RAD(float degrees) noexcept { return degrees * (std::numbers::pi_v<float> / 180.0f); }
constexpr auto RAD2DEG(float radians) noexcept { return radians * (180.0f / std::numbers::pi_v<float>); }

class matrix3x4_t;

class Vector
{
public:

	float x, y, z;

	// Construction/destruction:
	Vector(void);
	Vector(float X, float Y, float Z);
	Vector(float* clr);

	void Rotate2D(const float& f);

	// Initialization
	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f);
	// TODO (Ilya): Should there be an init that takes a single float for consistency?

	// Got any nasty NAN's?
	bool IsValid() const;
	void Invalidate();

	constexpr void clamp()
	{
		x = std::clamp(x, -89.0f, 89.0f);
		y = std::clamp(std::remainder(y, 360.0f), -180.0f, 180.0f);
		z = std::clamp(z, -50.0f, 50.0f);
	}

	// array access...
	float operator[](int i) const;
	float& operator[](int i);

	// Base address...
	float* Base();
	float const* Base() const;

	// Initialization methods
	void Random(float minVal, float maxVal);
	void Zero(); ///< zero out a vector

				 // equality
	bool operator==(const Vector& v) const;
	bool operator!=(const Vector& v) const;

	// arithmetic operations



	Vector& operator+=(const Vector& v) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	Vector& operator-=(const Vector& v) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	Vector& operator*=(float fl) {
		x *= fl;
		y *= fl;
		z *= fl;
		return *this;
	}

	Vector& operator*=(const Vector& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	Vector& operator/=(const Vector& v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	// this ought to be an opcode.
	Vector& operator+=(float fl) {
		x += fl;
		y += fl;
		z += fl;
		return *this;
	}

	// this ought to be an opcode.
	Vector& operator/=(float fl) {
		x /= fl;
		y /= fl;
		z /= fl;
		return *this;
	}
	Vector& operator-=(float fl) {
		x -= fl;
		y -= fl;
		z -= fl;
		return *this;
	}

	bool operator!() const
	{
		return IsZero();
	}

	// negate the vector components
	void	Negate();

	void Set(float X = 0.0f, float Y = 0.0f, float Z = 0.0f)
	{
		x = X; y = Y; z = Z;
	}

	float Length2d(void) const
	{
		return sqrtf(x * x + y * y);
	}

	float Length2dSqr(void) const
	{
		return (x * x + y * y);
	}

	/*float DistTo(const Vector& v) const
	{
		return (*this - v).Length();
	}*/

	// Get the vector's magnitude.
	float	Length() const;

	// Get the vector's magnitude squared.
	float LengthSqr(void) const {
		return (x * x + y * y + z * z);
	}

	// return true if this vector is (0,0,0) within tolerance
	bool IsZero(float tolerance = 0.01f) const {
		return (x > -tolerance && x < tolerance&&
			y > -tolerance && y < tolerance&&
			z > -tolerance && z < tolerance);
	}

	float	NormalizeInPlace();
	Vector	Normalized() const;

	bool	IsLengthGreaterThan(float val) const {
		return LengthSqr() > val * val;
	}

	bool	IsLengthLessThan(float val) const {
		return LengthSqr() < val * val;
	}

	// check if a vector is within the box defined by two other vectors
	bool WithinAABox(Vector const& boxmin, Vector const& boxmax);

	// Get the distance from this vector to the other one.
	float	DistTo(const Vector& vOther) const;

	// Get the distance from this vector to the other one squared.
	// NJS: note, VC wasn't inlining it correctly in several deeply nested inlines due to being an 'out of line' .  
	// may be able to tidy this up after switching to VC7
	float DistToSqr(const Vector& vOther) const {
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.LengthSqr();
	}

	Vector& normalize()
	{
		x = std::isfinite(x) ? std::remainder(x, 360.0f) : 0.0f;
		y = std::isfinite(y) ? std::remainder(y, 360.0f) : 0.0f;
		z = 0.0f;
		return *this;
	}

	// Copy
	void	CopyToArray(float* rgfl) const;

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual vector equation (because it's done per-component
	// rather than per-vector).
	void	MulAdd(const Vector& a, const Vector& b, float scalar);

	// Dot product.
	float Dot(const Vector& vOther) const
	{
		return (x * vOther.x + y * vOther.y + z * vOther.z);
	}

	// assignment
	Vector& operator=(const Vector& vOther);

	// 2d
	float	Length2D(void) const {
		return sqrt(x * x + y * y);
	}
	float	Length2DSqr(void) const {
		return (x * x + y * y);
	}

	/// get the component of this vector parallel to some other given vector
	Vector  ProjectOnto(const Vector& onto);

	// copy constructors
	//	Vector(const Vector &vOther);

	// arithmetic operations
	Vector	operator-(void) const;

	Vector	operator+(const Vector& v) const;
	Vector	operator-(const Vector& v) const;
	Vector	operator*(const Vector& v) const;
	Vector	operator/(const Vector& v) const;
	Vector	operator*(float fl) const;
	Vector	operator/(float fl) const;

	// Cross product between two vectors.
	Vector	Cross(const Vector& vOther) const;

	Vector Direction()
	{
		return Vector(cos(y * M_PI / 180.0f) * cos(x * M_PI / 180.0f), sin(y * M_PI / 180.0f) * cos(x * M_PI / 180.0f), sin(-x * M_PI / 180.0f)).Normalized();
	}

	Vector Forward()
	{
		return Direction();
	}

	Vector Up()
	{
		return Vector(cos(y * M_PI / 180.0f) * cos(x * M_PI / 180.0f), sin(y * M_PI / 180.0f) * cos(x * M_PI / 180.0f), sin(-(x - 90.0f) * M_PI / 180.0f)).Normalized();
	}

	Vector Right()
	{
		return Vector(cos((y + 90.f) * M_PI / 180.0f) * cos(x * M_PI / 180.0f), sin((y + 90.f) * M_PI / 180.0f) * cos(x * M_PI / 180.0f), sin(-x * M_PI / 180.0f)).Normalized();
	}


	// Returns a vector with the min or max in X, Y, and Z.
	Vector	Min(const Vector& vOther) const;
	Vector	Max(const Vector& vOther) const;
};




class TableVector
{
public:
	float x, y, z;

	operator Vector& () { return *((Vector*)(this)); }
	operator const Vector& () const { return *((const Vector*)(this)); }

	// array access...
	inline float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	inline float operator[](int i) const
	{
		return ((float*)this)[i];
	}
};

void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out);
void VectorCopy(const Vector& src, Vector& dst);
float VectorLength(const Vector& v);
void VectorLerp(const Vector& src1, const Vector& src2, float t, Vector& dest);
void VectorCrossProduct(const Vector& a, const Vector& b, Vector& result);
float NormalizeVector(Vector& v);

class __declspec(align(16)) VectorAligned : public Vector {
public:
	inline VectorAligned(void) {};
	inline VectorAligned(float X, float Y, float Z) {
		Init(X, Y, Z);
	}

public:
	explicit VectorAligned(const Vector& vOther) {
		Init(vOther.x, vOther.y, vOther.z);
	}

	VectorAligned& operator=(const Vector& vOther) {
		Init(vOther.x, vOther.y, vOther.z);
		return *this;
	}

	VectorAligned& operator=(const VectorAligned& vOther) {
		this->Base()[0] = vOther.Base()[0];
		this->Base()[1] = vOther.Base()[1];
		this->Base()[2] = vOther.Base()[2];
		return *this;
	}

	float w;
};