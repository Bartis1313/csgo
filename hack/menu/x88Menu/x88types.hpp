#pragma once

#pragma warning(disable: 26495)

#include <variant>
#include <string>
#include <vector>
#include <utility>
#include <type_traits>

// pair in pair
class X88Types
{
private:
	// ptr to get actual config vars updated
	using Types = std::variant<bool*, int*, float*>;
	using X88Pair = std::pair<std::string, Types>;

	struct Limits
	{
		Limits() = default;
		constexpr Limits(const std::pair<float, float>& limits)
			: m_floatLimits{ limits }
		{}
		constexpr Limits(int limits)
			: m_intLimtis{ limits }
		{}

		std::pair<float, float> m_floatLimits;
		int m_intLimtis;
	};
public:
	template<typename T>
	constexpr void push(const std::string& name, const T& var)
	{
		m_vars.emplace_back(std::make_pair(X88Pair{ name, var }, Limits{}));
	}
	template<typename T>
	constexpr void push(const std::string& name, const T& var, const std::pair<float, float>& limits)
	{
		m_vars.emplace_back(std::make_pair(X88Pair{ name, var }, Limits{ limits }));
		return;
	}
	template<typename T>
	constexpr void push(const std::string& name, const T& var, int limits)
	{
		m_vars.emplace_back(std::make_pair(X88Pair{ name, var }, Limits{ limits }));
	}
	constexpr std::vector<std::pair<X88Pair, Limits>> getVars() const { return m_vars; }
private:
	std::vector<std::pair<X88Pair, Limits>> m_vars;
};