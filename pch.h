#pragma once

#include <Windows.h>
#include <ShlObj.h>

#include <filesystem>
#include <format>

#include <deps/lazy_importer.hpp>
#include <deps/xorstr.hpp>
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
#include <deps/ImGui/imgui.h>
#include <deps/ImGui/imgui_internal.h>
#include <deps/json.hpp>
#include <deps/freetype/include/freetype/internal/ftobjs.h>