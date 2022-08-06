#include "base.hpp"

#include "../../utilities/console/console.hpp"

void BaseHack::initAll()
{
	for (auto el : m_hacks)
		el->init();

	console.log(TypeLogs::LOG_INFO, XOR("Inited {} functions!"), m_hacks.size());
}

void BaseHack::shutdownAll()
{
	for (auto el : m_hacks)
		el->shutdown();
}

void HackException::handleError(const HackException& err) const
{
	console.log(TypeLogs::LOG_ERR, XOR("Reason of crash {}"), err.what());
}