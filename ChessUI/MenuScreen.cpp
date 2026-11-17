#include "MenuScreen.h"

#include "UIDrawing.h"

#include <cstdint>
#include <initializer_list>

namespace
{
	constexpr float TILE_SIZE = 32.f;
	constexpr unsigned int TITLE_FONT_SIZE = 40;
	constexpr float TITLE_HEIGHT_RATIO = 0.2f;
	constexpr float FIRST_BUTTON_HEIGHT_RATIO = 0.38f;
	constexpr float BUTTON_SPACING = 80.f;
	const sf::Vector2f BUTTON_SIZE(400.f, 60.f);
	const sf::Color TITLE_COLOR(255, 255, 255);

	void AppendTile(sf::VertexArray& vertices, sf::Vector2f position, sf::Color color)
	{
		const sf::Vector2f topLeft = position;
		const sf::Vector2f topRight = position + sf::Vector2f(TILE_SIZE, 0.f);
		const sf::Vector2f bottomLeft = position + sf::Vector2f(0.f, TILE_SIZE);
		const sf::Vector2f bottomRight = position + sf::Vector2f(TILE_SIZE, TILE_SIZE);

		for (const sf::Vector2f& corner : { topLeft, topRight, bottomLeft, topRight, bottomRight, bottomLeft })
		{
			vertices.append(sf::Vertex{ corner, color });
		}
	}
}

MenuScreen::MenuScreen(
	const sf::Font& font,
	std::string title,
	const std::vector<MenuEntry>& entries,
	sf::Vector2u windowSize)
	: m_font(font)
	, m_title(std::move(title))
	, m_titleCenter(windowSize.x / 2.f, windowSize.y* TITLE_HEIGHT_RATIO)
	, m_background(sf::PrimitiveType::Triangles)
{
	BuildBackground(windowSize);

	const float buttonLeft = (windowSize.x - BUTTON_SIZE.x) / 2.f;
	float buttonTop = windowSize.y * FIRST_BUTTON_HEIGHT_RATIO;

	m_items.reserve(entries.size());
	for (const MenuEntry& entry : entries)
	{
		m_items.push_back({ Button(m_font, entry.label, { buttonLeft, buttonTop }, BUTTON_SIZE), entry.action });
		buttonTop += BUTTON_SPACING;
	}
}

void MenuScreen::UpdateHover(sf::Vector2i mousePosition)
{
	for (MenuItem& item : m_items)
	{
		item.button.UpdateHover(mousePosition);
	}
}

EMenuAction MenuScreen::GetClickedAction(sf::Vector2i pixel) const
{
	for (const MenuItem& item : m_items)
	{
		if (item.button.Contains(pixel))
		{
			return item.action;
		}
	}
	return EMenuAction::None;
}

void MenuScreen::Draw(sf::RenderTarget& target) const
{
	target.draw(m_background);
	DrawShadowedText(target, m_font, m_title, TITLE_FONT_SIZE, m_titleCenter, TITLE_COLOR);

	for (const MenuItem& item : m_items)
	{
		item.button.Draw(target);
	}
}

void MenuScreen::BuildBackground(sf::Vector2u windowSize)
{
	const unsigned int columns = static_cast<unsigned int>(windowSize.x / TILE_SIZE) + 1;
	const unsigned int rows = static_cast<unsigned int>(windowSize.y / TILE_SIZE) + 1;

	for (unsigned int row = 0; row < rows; ++row)
	{
		for (unsigned int col = 0; col < columns; ++col)
		{
			// Deterministic pseudo-random brightness, so the pattern is the same on every run.
			const unsigned int noise = ((col * 73856093u) ^ (row * 19349663u)) % 24u;
			const auto channel = [noise](unsigned int base) { return static_cast<std::uint8_t>(base + noise); };

			AppendTile(m_background, { col * TILE_SIZE, row * TILE_SIZE }, sf::Color(channel(60), channel(42), channel(30)));
		}
	}
}