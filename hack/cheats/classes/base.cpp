#include "base.hpp"

#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>

void BaseHack::initAll()
{
	for (auto el : m_hacks)
		el->init();

	console::debug("Inited {} functions!", m_hacks.size());
}

void BaseHack::shutdownAll()
{
	for (auto el : m_hacks)
		el->shutdown();
}

void BaseHack::resetAll()
{
	for (auto el : m_hacks)
		el->reset();
}

void BaseHack::loadAll()
{
	for (auto el : m_hacks)
		el->load();
}

void BaseHack::restartRoundAll()
{
	for (auto el : m_hacks)
		el->roundRestart();
}