#include "MenuScreen.h"

#include "UIDrawing.h"

namespace
{
	constexpr unsigned int TITLE_FONT_SIZE = 40;
	constexpr float TITLE_HEIGHT_RATIO = 0.2f;
	constexpr float FIRST_BUTTON_HEIGHT_RATIO = 0.38f;
	constexpr float BUTTON_SPACING = 80.f;
	const sf::Vector2f BUTTON_SIZE(400.f, 60.f);
	const sf::Color TITLE_COLOR(255, 255, 255);
}

MenuScreen::MenuScreen(
	const sf::Font& font,
	std::string title,
	const std::vector<MenuEntry>& entries,
	sf::Vector2u layoutSize)
	: m_font(font)
	, m_title(std::move(title))
	, m_titleCenter(layoutSize.x / 2.f, layoutSize.y * TITLE_HEIGHT_RATIO)
{
	const float buttonLeft = (layoutSize.x - BUTTON_SIZE.x) / 2.f;
	float buttonTop = layoutSize.y * FIRST_BUTTON_HEIGHT_RATIO;

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
	DrawShadowedText(target, m_font, m_title, TITLE_FONT_SIZE, m_titleCenter, TITLE_COLOR);

	for (const MenuItem& item : m_items)
	{
		item.button.Draw(target);
	}
}
