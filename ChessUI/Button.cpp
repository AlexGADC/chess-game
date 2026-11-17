#include "Button.h"

#include "UIDrawing.h"

namespace
{
	constexpr float BORDER_THICKNESS = 2.f;
	constexpr float BEVEL_THICKNESS = 3.f;
	constexpr unsigned int LABEL_FONT_SIZE = 28;

	const sf::Color BORDER_COLOR(0, 0, 0);
	const sf::Color NORMAL_FILL_COLOR(111, 111, 111);
	const sf::Color HOVER_FILL_COLOR(100, 110, 170);
	const sf::Color BEVEL_LIGHT_COLOR(255, 255, 255, 70);
	const sf::Color BEVEL_DARK_COLOR(0, 0, 0, 70);
	const sf::Color NORMAL_TEXT_COLOR(255, 255, 255);
	const sf::Color HOVER_TEXT_COLOR(255, 255, 160);
}

Button::Button(const sf::Font& font, std::string label, sf::Vector2f position, sf::Vector2f size)
	: m_font(font)
	, m_label(std::move(label))
	, m_position(position)
	, m_size(size)
{
}

void Button::UpdateHover(sf::Vector2i mousePosition)
{
	m_isHovered = Contains(mousePosition);
}

bool Button::Contains(sf::Vector2i pixel) const
{
	return sf::FloatRect(m_position, m_size).contains(sf::Vector2f(pixel));
}

void Button::Draw(sf::RenderTarget& target) const
{
	DrawRectangle(target, m_position, m_size, BORDER_COLOR);

	const sf::Vector2f innerPosition = m_position + sf::Vector2f(BORDER_THICKNESS, BORDER_THICKNESS);
	const sf::Vector2f innerSize = m_size - sf::Vector2f(2.f * BORDER_THICKNESS, 2.f * BORDER_THICKNESS);
	DrawRectangle(target, innerPosition, innerSize, m_isHovered ? HOVER_FILL_COLOR : NORMAL_FILL_COLOR);

	const sf::Vector2f bevelSize(innerSize.x, BEVEL_THICKNESS);
	DrawRectangle(target, innerPosition, bevelSize, BEVEL_LIGHT_COLOR);
	DrawRectangle(target, innerPosition + sf::Vector2f(0.f, innerSize.y - BEVEL_THICKNESS), bevelSize, BEVEL_DARK_COLOR);

	DrawShadowedText(
		target, m_font, m_label, LABEL_FONT_SIZE,
		m_position + m_size / 2.f,
		m_isHovered ? HOVER_TEXT_COLOR : NORMAL_TEXT_COLOR);
}