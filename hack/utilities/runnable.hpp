#pragma once

#include <functional>
#include <type_traits>

template<typename Func>
concept functionAble = requires
{
	typename std::function<std::decay_t<Func>>;
};

// this is from boost, because I failed to ANYHOW count std::function args by tuple size
template <typename T>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<std::function<R(Args...)>>
{
	enum { arity = sizeof...(Args) };
	using result_type = R;
	using parameter_pack = std::tuple<Args...>;
};

template<functionAble F>
struct vectorRunnable : public std::vector<F>
{
	constexpr vectorRunnable() :
		std::vector<F>()
	{}

	template<typename... Args_t>
	void run(Args_t&&... args)
	{
		static_assert(sizeof...(Args_t) == std::tuple_size_v<typename function_traits<F>::parameter_pack>,
			"Number of arguments does not match stored function");

		for (auto& element : *this)
		{
			static_assert(std::is_invocable_v<F, Args_t...>,
				"Arguments do not match stored function");

			element(std::forward<Args_t>(args)...);
		}
	}
};