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

	template<typename T = void, typename... Args_t>
	T run(Args_t&&... args)
	{
		static_assert(sizeof...(Args_t) == std::tuple_size_v<typename function_traits<F>::parameter_pack>,
			"Number of arguments does not match stored function");

        if constexpr (std::is_same_v<T, bool>)
        {
            T ret{ false };
            for (auto& element : *this)
            {
                ret |= element(std::forward<Args_t>(args)...);
            }
            return ret;
        }
        else if constexpr (std::is_void_v<T>)
        {
            for (auto& element : *this)
            {
                element(std::forward<Args_t>(args)...);
            }
        }
        else // not handled, do this if needed
        {
            T ret{};
            for (auto& element : *this)
            {
                element(std::forward<Args_t>(args)...);
            }
            return ret;
        }
	}
};