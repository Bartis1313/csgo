#pragma once


// 2D Vector
class Vector2D {
public:
	// Members
	float x, y;

	// Construction/destruction:
	Vector2D(void);
	Vector2D(float X, float Y);
	Vector2D(float* clr);

	Vector2D(const Vector2D& vOther) {
		x = vOther.x; y = vOther.y;
	}

	// Initialization
	void Init(float ix = 0.0f, float iy = 0.0f);
	// TODO (Ilya): Should there be an init that takes a single float for consistency?

	// Got any nasty NAN's?
	bool IsValid() const;
	void Invalidate();

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
	bool operator==(const Vector2D& v) const;
	bool operator!=(const Vector2D& v) const;

	// arithmetic operations
	Vector2D& operator+=(const Vector2D& v) {
		x += v.x; y += v.y;
		return *this;
	}

	Vector2D& operator-=(const Vector2D& v) {
		x -= v.x; y -= v.y;
		return *this;
	}

	Vector2D& operator*=(float fl) {
		x *= fl;
		y *= fl;
		return *this;
	}

	Vector2D& operator*=(const Vector2D& v) {
		x *= v.x;
		y *= v.y;
		return *this;
	}

	Vector2D& operator/=(const Vector2D& v) {
		x /= v.x;
		y /= v.y;
		return *this;
	}

	// this ought to be an opcode.
	Vector2D& operator+=(float fl) {
		x += fl;
		y += fl;
		return *this;
	}

	// this ought to be an opcode.
	Vector2D& operator/=(float fl) {
		x /= fl;
		y /= fl;
		return *this;
	}
	Vector2D& operator-=(float fl) {
		x -= fl;
		y -= fl;
		return *this;
	}

	// negate the vector components
	void	Negate();

	// Get the vector's magnitude.
	float	Length() const;

	// Get the vector's magnitude squared.
	float LengthSqr(void) const {
		return (x * x + y * y);
	}

	// return true if this vector is (0,0,0) within tolerance
	bool IsZero(float tolerance = 0.01f) const {
		return (x > -tolerance && x < tolerance&&
			y > -tolerance && y < tolerance);
	}

	float	NormalizeInPlace();
	Vector2D	Normalized() const;
	bool	IsLengthGreaterThan(float val) const;
	bool	IsLengthLessThan(float val) const;

	// check if a vector is within the box defined by two other vectors
	bool WithinAABox(Vector2D const& boxmin, Vector2D const& boxmax);

	// Get the distance from this vector to the other one.
	float	DistTo(const Vector2D& vOther) const;

	// Get the distance from this vector to the other one squared.
	// NJS: note, VC wasn't inlining it correctly in several deeply nested inlines due to being an 'out of line' .  
	// may be able to tidy this up after switching to VC7
	float DistToSqr(const Vector2D& vOther) const {
		Vector2D delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;

		return delta.LengthSqr();
	}

	// Copy
	void	CopyToArray(float* rgfl) const;

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual vector equation (because it's done per-component
	// rather than per-vector).
	void	MulAdd(const Vector2D& a, const Vector2D& b, float scalar);

	// Dot product.
	float	Dot(const Vector2D& vOther) const;

	// assignment
	Vector2D& operator=(const Vector2D& vOther);

	// 2d
	float	Length2D(void) const;
	float	Length2DSqr(void) const;

	/// get the component of this vector parallel to some other given vector
	Vector2D  ProjectOnto(const Vector2D& onto);

	// copy constructors
	//	Vector2D(const Vector2D &vOther);

	// arithmetic operations
	Vector2D	operator-(void) const;

	Vector2D	operator+(const Vector2D& v) const;
	Vector2D	operator-(const Vector2D& v) const;
	Vector2D	operator*(const Vector2D& v) const;
	Vector2D	operator/(const Vector2D& v) const;
	Vector2D	operator*(float fl) const;
	Vector2D	operator/(float fl) const;

	// Cross product between two vectors.
	Vector2D	Cross(const Vector2D& vOther) const;

	// Returns a vector with the min or max in X, Y, and Z.
	Vector2D	Min(const Vector2D& vOther) const;
	Vector2D	Max(const Vector2D& vOther) const;
};
