#include "decalCreate.hpp"

#include <SDK/material.hpp>

hooks::R_DecalCreate::value hooks::R_DecalCreate::hook(int a1, int a2, int a3, int a4, int a5, char a6, char a7)
{
	original(a1, a2, a3, a4, a5, a6, a7);
}