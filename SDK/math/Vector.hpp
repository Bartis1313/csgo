#pragma once

#include <array>
#include <type_traits>
#include <ranges>
#include <cmath>
#include <algorithm>

#include <utilities/tools/wrappers.hpp>
#include <dependencies/ImGui/imgui.h>

enum class Coord { X, Y, Z };

#define ASSERT_VEC3 static_assert(m_numArgs == 3, "Use for only v3");

template<typename T>
class Vector
{
private:
	template<typename... Args_t>
	struct RealVec
	{
	protected:
		constexpr static size_t m_numArgs = sizeof...(Args_t);
		std::array<T, m_numArgs> m_arr{};
	public:
		constexpr RealVec()
		{
			for (auto& el : m_arr)
				el = static_cast<T>(0);
		}

		explicit constexpr RealVec(const Args_t&... args)
			: m_arr{ args... }
		{}

		explicit constexpr RealVec(const std::array<T, m_numArgs>& arr)
			: m_arr{ arr }
		{}

		explicit constexpr RealVec(const T* arr)
		{
			for (auto i : std::views::iota(0U, m_numArgs))
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
			return m_numArgs;
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

		constexpr void clamp()
		{
			ASSERT_VEC3;

			m_arr[0] = std::clamp(m_arr[0], (T)-89, (T)89);
			m_arr[1] = std::clamp(std::remainder(m_arr[1], (T)360), (T)-180, (T)180);
			m_arr[2] = std::clamp(m_arr[2], (T)-50, (T)50);
		}

		[[nodiscard]] T length() const
		{
			return std::sqrt(this->dotFields());
		}

		[[nodiscard]] constexpr T lengthSqrt() const
		{
			return this->dotFields();
		}

		// make it possible like for 4d? how to get correct type though?
		template<typename TT = T>
		[[nodiscard]] constexpr std::enable_if_t<m_numArgs == 3, RealVec<TT, TT>> toVec2D() const
		{
			return RealVec<TT, TT>{ getPack<m_numArgs - 1U>() };
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
			return this->m_arr[E2T(coord)];
		}

		constexpr auto& operator[](Coord coord)
		{
			return this->m_arr[E2T(coord)];
		}

		constexpr auto& operator+=(const RealVec& vec)
		{
			for (auto i : std::views::iota(0U, m_numArgs))
				m_arr[i] += vec[i];

			return *this;
		}

		constexpr auto& operator+=(T val)
		{
			for (auto& el : m_arr)
				el += val;

			return *this;
		}

		constexpr auto& operator-=(const RealVec& vec)
		{
			for (auto i : std::views::iota(0U, m_numArgs))
				m_arr[i] -= vec[i];

			return *this;
		}

		constexpr auto& operator-=(T val)
		{
			for (auto& el : m_arr)
				el -= val;

			return *this;
		}

		constexpr auto& operator*=(const RealVec& vec)
		{
			for (auto i : std::views::iota(0U, m_numArgs))
				m_arr[i] *= vec[i];

			return *this;
		}

		constexpr auto& operator*=(T val)
		{
			for (auto& el : m_arr)
				el *= val;

			return *this;
		}

		constexpr auto& operator/=(const RealVec& vec)
		{
			for (auto i : std::views::iota(0U, m_numArgs))
				m_arr[i] /= vec[i];

			return *this;
		}

		constexpr auto& operator/=(T val)
		{
			for (auto i : std::views::iota(0U, m_numArgs))
				m_arr[i] /= val;

			return *this;
		}

		constexpr auto operator+(const RealVec& vec) const
		{
			RealVec res = {};
			for (auto i : std::views::iota(0U, m_numArgs))
				res[i] = this->m_arr[i] + vec[i];

			return res;
		}

		constexpr auto operator-(const RealVec& vec) const
		{
			RealVec res = {};
			for (auto i : std::views::iota(0U, m_numArgs))
				res[i] = this->m_arr[i] - vec[i];

			return res;
		}

		constexpr auto operator*(const RealVec& vec) const
		{
			RealVec res = {};
			for (auto i : std::views::iota(0U, m_numArgs))
				res[i] = this->m_arr[i] * vec[i];

			return res;
		}

		constexpr auto operator/(const RealVec& vec) const
		{
			RealVec res = {};
			for (auto i : std::views::iota(0U, m_numArgs))
				res[i] = this->m_arr[i] / vec[i];

			return res;
		}

		constexpr auto operator*(T val) const
		{
			RealVec res = {};
			for (auto i : std::views::iota(0U, m_numArgs))
				res[i] = this->m_arr[i] * val;

			return res;
		}

		constexpr auto operator/(T val) const
		{
			RealVec res = {};
			for (auto i : std::views::iota(0U, m_numArgs))
				res[i] = this->m_arr[i] / val;

			return res;
		}

		constexpr auto& normalize()
		{
			ASSERT_VEC3;

			m_arr[0] = std::isfinite(m_arr[0]) ? (T)std::remainder(m_arr[0], (T)360) : (T)0;
			m_arr[1] = std::isfinite(m_arr[1]) ? (T)std::remainder(m_arr[1], (T)360) : (T)0;
			m_arr[2] = (T)0;

			return *this;
		}

		[[nodiscard]] constexpr auto normalized() const
		{
			ASSERT_VEC3;

			RealVec vec = this->get();
			vec.normalize();

			return vec;
		}

		[[nodiscard]] constexpr T dot(const RealVec& vec) const
		{
			T res = {};
			for (auto i : std::views::iota(0U, m_numArgs))
				res += m_arr[i] * vec[i];

			return res;
		}

		[[nodiscard]] T distTo(const RealVec& vec) const
		{
			return RealVec{ *this - vec }.length();
		}

		[[nodiscard]] T distToMeters(const RealVec& vec) const
		{
			return distTo(vec) * (T)0.0254f;
		}

		[[nodiscard]] constexpr ImVec2 toImVec() const
		{
			return ImVec2{ m_arr[0], m_arr[1] };
		}
	private:
		[[nodiscard]] constexpr T dotFields(size_t times = m_numArgs) const
		{
			T temp = static_cast<T>(0);
			for (auto i : std::views::iota(0U, times))
				temp += m_arr[i] * m_arr[i];

			return temp;
		}

		template<size_t SIZE = m_numArgs, typename TT = T>
		[[nodiscard]] constexpr std::array<TT, SIZE> getPack() const
		{
			std::array<TT, SIZE> arr = {};
			for (auto i : std::views::iota(0U, SIZE))
				arr[i] = (TT)m_arr[i];

			return arr;
		}
	};
public:
	using v2 = RealVec<T, T>;
	using v3 = RealVec<T, T, T>;

	static_assert(std::is_same_v<T, float> || std::is_same_v<T, int>, "Possible types are only: float, int");
};

using Vec2 = Vector<float>::v2;
using Vec3 = Vector<float>::v3;

#undef ASSERT_VEC3