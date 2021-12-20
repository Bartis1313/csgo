#pragma once
#include "../dependencies/SimpleIni.h"

// seems like SimpleIni may fail with some compiler options, especially debug
// EDIT: this is not anymore, about 1000th line you gotta add empty null string, I edited it

namespace config
{
	void init();
	void save();
}