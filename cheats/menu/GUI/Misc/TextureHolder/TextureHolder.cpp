#include "TextureHolder.hpp"
#include "../../../../../utilities/renderer/renderer.hpp"
#include "../../../../../utilities/utilities.hpp"
#include "../../../../../dependencies/lodepng.h"
#include "../../../../globals.hpp"
#include <format>

TextureHolder::TextureHolder(const unsigned short resourceID)
{
	HRSRC hResInfo = LF(FindResourceA)(globals::instance, MAKEINTRESOURCEA(resourceID), XOR("PNG"));
	HGLOBAL hResData = LF(LoadResource)(globals::instance, hResInfo);
	unsigned char* hResPtr = reinterpret_cast<unsigned char*>(LF(LockResource)(hResData));
	size_t size = LF(SizeofResource)(globals::instance, hResInfo);
	std::vector<byte> vecImage = {};

	m_width = utilities::inByteOrder(*(reinterpret_cast<size_t*>(reinterpret_cast<size_t>(hResData) + 16)));
	m_height = utilities::inByteOrder(*(reinterpret_cast<size_t*>(reinterpret_cast<size_t>(hResData) + 20)));

	if (auto error = lodepng::decode(vecImage, m_width, m_height, hResPtr, size))
		throw std::runtime_error(std::format(XOR("lodepng::decode error {}"), error));

	auto data = std::make_unique<byte[]>(vecImage.size());

	std::copy(vecImage.cbegin(), vecImage.cend(), data.get());

	render.initNewTexture(m_textureID, data.get(), m_width, m_height);

	LOG(LOG_INFO, std::format(XOR("loaded texture ID - {} without error"), m_textureID));
};

void TextureHolder::draw(const int x, const int y)
{
	render.drawFromTexture(m_textureID, x, y, m_width, m_height, Colors::White);
};