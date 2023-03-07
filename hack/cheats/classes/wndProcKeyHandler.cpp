#include "wndProcKeyHandler.hpp"

void WndProcKeyHandler::updateAllKeys()
{
	for (auto el : m_allHandlers)
		el->updateKeys();
}