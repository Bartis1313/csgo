#pragma once

#include <Windows.h>
#include <ShlObj.h>

#include <filesystem>
#include <format>

#include <dependencies/lazy_importer.hpp>
#include <dependencies/xorstr.hpp>
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
#include <dependencies/ImGui/imgui.h>
#include <dependencies/ImGui/imgui_internal.h>
#include <dependencies/json.hpp>
#include <dependencies/freetype/include/freetype/internal/ftobjs.h>