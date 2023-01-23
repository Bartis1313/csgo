#pragma once

#include <ranges>
#include <string_view>
#include <vector>
#include <optional>

// can't make it constexpr with xor'ed strings
// only way is to use const char (&str)[N] as template literal which wont work
template<typename T>
struct SigConvert
{
	static auto get(const std::string_view sig)
	{
		std::vector<std::optional<T>> actualPattern; // idk how to get view split size shorty to optimize known vec size
		for (const auto& el : std::views::split(sig, delimer))
		{
			const std::string_view part{ el.begin(), el.end() };
			if (const auto itr = std::ranges::find(masks, part); itr != masks.end())
				actualPattern.emplace_back(std::nullopt);
			else
				actualPattern.emplace_back(convertHex(part));
		}

		return actualPattern;
	}
private:
	static T convertHex(std::string_view str)
	{
		int val;
		auto fallback = std::from_chars(str.data(), str.data() + str.size(), val, base);
		assert(fallback.ec == std::errc{});
		return static_cast<T>(val);
	}
	constexpr static std::array<std::string_view, 2U> masks = { "?", "??" };
	constexpr static char delimer = ' ';
	constexpr static short base = 0x10;
};