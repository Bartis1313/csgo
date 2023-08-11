#pragma once

#include "system/win.hpp"

#include <SDK/ClientClass.hpp>
#include <utilities/tools/wrappers.hpp>
#include <SDK/interfaces/ifc.hpp>

#include <utilities/console/console.hpp>

#include <array>
#include <optional>

namespace memory
{
	enum class Dereference : size_t
	{
		ONCE = 1,
		TWICE,
		THREE
	};

	class AddressBase
	{
	protected:
		uintptr_t m_addr{ };

	public:
		AddressBase() = default;
		constexpr AddressBase(uintptr_t addr)
			: m_addr{ addr }
		{}

		// raw place in memory as offset
		constexpr auto getAddr() const { return m_addr; }
	};

	template<typename T>
	class Address : public AddressBase
	{
	public:
		constexpr Address() = default;
		// pass by offset
		constexpr Address(uintptr_t addr) :
			AddressBase{ addr }
		{}
		// pass by pointer
		constexpr Address(void* addr) :
			AddressBase{ reinterpret_cast<uintptr_t>(addr) }
		{}
		// pass by another addr
		template<typename U>
		constexpr Address(const Address<U>& other)
			: AddressBase{ other.getAddr() }
		{}

		// cast to anything
		template<typename U>
		constexpr auto cast() const { return Address<U>{ m_addr }; }
		constexpr auto add(uintptr_t extraOffset) const { return Address{ m_addr + extraOffset }; }
		constexpr auto sub(uintptr_t extraOffset) const { return Address{ m_addr - extraOffset }; }
		// dereference x times. Possible args are: 1, 2, 3.
		// There will for sure won't be a case for 4 level dereference. 3rd is very rare.
		constexpr auto deRef(Dereference times = Dereference::ONCE) const
		{
			auto addr = m_addr;
			for (size_t i = 0U; i < E2T(times); ++i)
				addr = *reinterpret_cast<uintptr_t*>(addr);

			return Address<T>{ addr };
		}
		// get as rel32
		constexpr auto rel(uintptr_t relOffset = 0x1, uintptr_t absOffset = 0x0) const
		{
			const auto jump = m_addr + relOffset;
			const auto target = *reinterpret_cast<decltype(jump)*>(jump);
			return Address<T>{ jump + absOffset + 0x4 + target };
		}
		// will work for classes types too
		constexpr T operator()() const
		{
			if constexpr (std::is_class_v<T>)
				return *reinterpret_cast<T*>(m_addr);
			else
				return (T)(m_addr);
		}

		constexpr T operator->() const
		{
			if constexpr (std::is_class_v<T>)
				return *reinterpret_cast<T*>(m_addr);
			else
				return (T)(m_addr);
		}
	};

	template<size_t N>
	Address<uintptr_t> scan(const std::string_view mod, const std::array<std::optional<uint8_t>, N>& sig);
	template<li::detail::offset_hash_pair hash>
	Address<uintptr_t> byExport(const std::string_view module);
	// non template version, used to wrap nt stuff
	Address<uintptr_t> byExport(const std::string_view module, const std::string_view exportName);
	template<typename U>
	Address<uintptr_t> byVFunc(const Interface<U>& ifc, size_t index);
	// static pointer
	Address<uintptr_t> findFromGame(ClassID id);
	// anything that changes every round
	Address<uintptr_t> findFromGameLoop(ClassID id);
	// for more objects with same id
	std::optional<std::vector<Address<uintptr_t>>> findMultipleFromGameLoop(ClassID id);
}

#include "signature.hpp"
#include <SDK/helpers/vfunc.hpp>

template <size_t N>
memory::Address<uintptr_t> memory::scan(const std::string_view mod, const std::array<std::optional<uint8_t>, N>& sig)
{
	const auto _module = memory::modules::getModule(mod);
	const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(_module);
	const auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uint8_t*>(_module) + dosHeader->e_lfanew);
	const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;

	auto check = [sig](uint8_t* data)
	{
		for (const auto& _byte : sig)
		{
			if (_byte && _byte.value() != *data)
				return false;
			data++;
		}
		return true;
	};

	for (size_t i = reinterpret_cast<size_t>(_module); i < static_cast<size_t>(sizeOfImage + reinterpret_cast<size_t>(_module)); ++i)
	{
		if (check(reinterpret_cast<uint8_t*>(i)))
		{
			return Address<uintptr_t>{ i };
		}
	}

	HACK_THROW(std::format("Couldn't find signature, module {}, signature: {}", mod, signature::to_string(sig)));
	return Address<uintptr_t>{ 0U };
}

template<li::detail::offset_hash_pair hash>
memory::Address<uintptr_t> memory::byExport(const std::string_view _module)
{
	const auto addr = static_cast<uintptr_t>(::li::detail::lazy_function<hash, uintptr_t>().in(modules::getModule(_module)));

	return Address<uintptr_t>{ addr };
}

inline memory::Address<uintptr_t> memory::byExport(const std::string_view _module, const std::string_view exportName)
{
	// windows::getModuleHandle caches modules, although we better don't use modules:: namespace here
	const auto addr = reinterpret_cast<uintptr_t>(windows::getExportAddress(windows::getModuleHandle(_module), exportName));

	return Address<uintptr_t>{ addr };
}

template<typename TT>
memory::Address<uintptr_t> memory::byVFunc(const Interface<TT>& ifc, size_t index)
{
	const auto addr = reinterpret_cast<uintptr_t>(vfunc::getVFunc(reinterpret_cast<void*>(ifc.base), index));

	return Address<uintptr_t>{ addr };
}