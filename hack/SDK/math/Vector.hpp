#pragma once

#include <array>
#include <type_traits>
#include <ranges>
#include <cmath>
#include <algorithm>
#include <compare>

#include <utilities/tools/wrappers.hpp>

enum class Coord { X, Y, Z };

template<typename T, size_t SIZE>
class Vector
{
protected:
	std::array<T, SIZE> m_arr{(T)0};
public:
	constexpr Vector()
	{
		for (auto& el : m_arr)
			el = static_cast<T>(0);
	}

	template<typename... Args_t>
	explicit constexpr Vector(const Args_t&... args)
		: m_arr{ args... }
	{
		static_assert(sizeof...(Args_t) == SIZE, "packed args mismatch");
	}

	explicit constexpr Vector(const std::array<T, SIZE>& arr)
		: m_arr{ arr }
	{}

	explicit constexpr Vector(const T* arr)
	{
		for (auto i : std::views::iota(0U, SIZE))
			m_arr[i] = arr[i];
	}

	constexpr static T MAX_ARG = static_cast<T>(0x7FFF);

	[[nodiscard]] constexpr auto get() const
	{
		return *this;
	}

	[[nodiscard]] constexpr auto begin()
	{
		return m_arr.begin();
	}

	[[nodiscard]] const constexpr auto begin() const
	{
		return m_arr.cbegin();
	}

	[[nodiscard]] constexpr auto end()
	{
		return m_arr.end();
	}

	[[nodiscard]] const constexpr auto end() const
	{
		return m_arr.cend();
	}

	[[nodiscard]] constexpr size_t size() const
	{
		return SIZE;
	}

	[[nodiscard]] constexpr bool isZero() const
	{
		bool res = std::all_of(m_arr.cbegin(), m_arr.cend(),
			[zero = static_cast<T>(0)](T el)
		{
			return el == zero;
		});

		return res;
	}

	[[nodiscard]] constexpr bool isValid() const
	{
		bool res = std::all_of(m_arr.cbegin(), m_arr.cend(),
			[](T el)
			{
				return std::isfinite(el);
			});

		return res;
	}

	constexpr auto& clamp()
	{
		m_arr[0] = std::clamp(m_arr[0], (T)-89, (T)89);
		m_arr[1] = std::clamp(std::remainder(m_arr[1], (T)360), (T)-180, (T)180);
		if constexpr (SIZE > 3)
			m_arr[2] = std::clamp(m_arr[2], (T)-50, (T)50);

		return *this;
	}

	[[nodiscard]] T length() const
	{
		return std::sqrt(this->dotFields());
	}

	[[nodiscard]] constexpr T lengthSqrt() const
	{
		return this->dotFields();
	}

	[[nodiscard]] constexpr auto toVecPrev() const
	{
		return Vector<T, SIZE - 1U>{ getPack<SIZE - 1U>() };
	}

	constexpr const auto operator[](size_t i) const
	{
		return this->m_arr[i];
	}

	constexpr auto& operator[](size_t idx)
	{
		return this->m_arr[idx];
	}

	constexpr const auto operator[](Coord coord) const
	{
		return this->m_arr[static_cast<size_t>(coord)];
	}

	constexpr auto& operator[](Coord coord)
	{
		return this->m_arr[static_cast<size_t>(coord)];
	}

	constexpr auto& operator+=(const Vector& vec)
	{
		for (auto i : std::views::iota(0U, SIZE))
			m_arr[i] += vec[i];

		return *this;
	}

	constexpr auto& operator+=(T val)
	{
		for (auto& el : m_arr)
			el += val;

		return *this;
	}

	constexpr auto& operator-=(const Vector& vec)
	{
		for (auto i : std::views::iota(0U, SIZE))
			m_arr[i] -= vec[i];

		return *this;
	}

	constexpr auto& operator-=(T val)
	{
		for (auto& el : m_arr)
			el -= val;

		return *this;
	}

	constexpr auto& operator*=(const Vector& vec)
	{
		for (auto i : std::views::iota(0U, SIZE))
			m_arr[i] *= vec[i];

		return *this;
	}

	constexpr auto& operator*=(T val)
	{
		for (auto& el : m_arr)
			el *= val;

		return *this;
	}

	constexpr auto& operator/=(const Vector& vec)
	{
		for (auto i : std::views::iota(0U, SIZE))
			m_arr[i] /= vec[i];

		return *this;
	}

	constexpr auto& operator/=(T val)
	{
		for (auto i : std::views::iota(0U, SIZE))
			m_arr[i] /= val;

		return *this;
	}

	constexpr auto operator+(const Vector& vec) const
	{
		Vector res = {};
		for (auto i : std::views::iota(0U, SIZE))
			res[i] = this->m_arr[i] + vec[i];

		return res;
	}

	constexpr auto operator-(const Vector& vec) const
	{
		Vector res = {};
		for (auto i : std::views::iota(0U, SIZE))
			res[i] = this->m_arr[i] - vec[i];

		return res;
	}

	constexpr auto operator*(const Vector& vec) const
	{
		Vector res = {};
		for (auto i : std::views::iota(0U, SIZE))
			res[i] = this->m_arr[i] * vec[i];

		return res;
	}

	constexpr auto operator/(const Vector& vec) const
	{
		Vector res = {};
		for (auto i : std::views::iota(0U, SIZE))
			res[i] = this->m_arr[i] / vec[i];

		return res;
	}

	constexpr auto operator*(T val) const
	{
		Vector res = {};
		for (auto i : std::views::iota(0U, SIZE))
			res[i] = this->m_arr[i] * val;

		return res;
	}

	constexpr auto operator/(T val) const
	{
		Vector res = {};
		for (auto i : std::views::iota(0U, SIZE))
			res[i] = this->m_arr[i] / val;

		return res;
	}

	constexpr auto operator+(T val) const
	{
		Vector res = {};
		for (auto i : std::views::iota(0U, SIZE))
			res[i] = this->m_arr[i] + val;

		return res;
	}

	constexpr auto operator-(T val) const
	{
		Vector res = {};
		for (auto i : std::views::iota(0U, SIZE))
			res[i] = this->m_arr[i] - val;

		return res;
	}

	constexpr auto operator<=>(const Vector&) const = default;

	[[nodiscard]] constexpr auto toArray() const
	{
		return m_arr;
	}

	constexpr auto& normalize()
	{
		m_arr[0] = std::isfinite(m_arr[0]) ? (T)std::remainder(m_arr[0], (T)360) : (T)0;
		m_arr[1] = std::isfinite(m_arr[1]) ? (T)std::remainder(m_arr[1], (T)360) : (T)0;
		if constexpr(SIZE > 2)
			m_arr[2] = (T)0;

		return* this;
	}

	[[nodiscard]] auto normalizeInPlace() const
	{
		const auto n = length();
		if (n > 1e-4f)
		{
			Vector res = {};
			for (auto i : std::views::iota(0U, SIZE))
				res[i] = this->m_arr[i] / n;

			return res;
		}

		return Vector{};
	}

	[[nodiscard]] constexpr auto normalized() const
	{
		Vector vec = this->get();
		vec.normalize();

		return vec;
	}

	[[nodiscard]] constexpr T dot(const Vector& vec) const
	{
		T res = {};
		for (auto i : std::views::iota(0U, SIZE))
			res += m_arr[i] * vec[i];

		return res;
	}

	[[nodiscard]] T distTo(const Vector& vec) const
	{
		return Vector{ *this - vec }.length();
	}

	[[nodiscard]] T distToMeters(const Vector& vec) const
	{
		return distTo(vec) * (T)0.0254f;
	}

	[[nodiscard]] constexpr auto lerp(const Vector& end, float t) const
	{
		return Vector{ *this + (end - *this) * t };
	}
private:
	[[nodiscard]] constexpr T dotFields(size_t times = SIZE) const
	{
		T temp = static_cast<T>(0);
		for (auto i : std::views::iota(0U, times))
			temp += m_arr[i] * m_arr[i];

		return temp;
	}

	template<size_t SIZ = SIZE, typename TT = T>
	[[nodiscard]] constexpr std::array<TT, SIZ> getPack() const
	{
		std::array<TT, SIZ> arr = {};
		for (auto i : std::views::iota(0U, SIZ))
			arr[i] = (TT)m_arr[i];

		return arr;
	}
public:

	static_assert(std::is_same_v<T, float> || std::is_same_v<T, int>, "Possible types are only: float, int");
	static_assert(SIZE >= 2 && SIZE <=4, "Sizes mismatch");
};

using Vec2 = Vector<float, 2U>;
using Vec3 = Vector<float, 3U>;
using Vec4 = Vector<float, 4U>;

#undef ASSERT_VEC3