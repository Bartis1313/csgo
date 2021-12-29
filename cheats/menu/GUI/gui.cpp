#include "gui.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../dependencies/lodepng.h"
#include "../../../resource.h"
#include "../../globals.hpp"
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

	m_textureID = interfaces::surface->createNewTextureID(true);
	if (m_textureID)
		interfaces::surface->setTextureRGBA(m_textureID, data.get(), m_width, m_height);
	else
		throw std::runtime_error(std::format(XOR("setTextureRGBA failed to create new texture, ID was: {}"), resourceID));
};

void TextureHolder::draw(const int x, const int y)
{
	if (!interfaces::surface->isTextureValid(m_textureID))
		return;

	interfaces::surface->drawSetColor(Colors::White);
	interfaces::surface->setTextureId(m_textureID);
	interfaces::surface->drawTexturedRect(x, y, x + m_width, y + m_height);
};


bool GUI::Element::isMouseInRange(const int x, const int y, const int w, const int h)
{
	return globals::mouseX > x && globals::mouseY > y && globals::mouseX < w + x && globals::mouseY < h + y;
}

void test::init()
{
	fatchad = TextureHolder(IMAGE_PNG1);
	LOG(LOG_INFO, XOR("all images done"));
}

void test::run()
{
	fatchad.draw(400, 500);
}