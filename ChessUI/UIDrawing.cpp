#include "UIDrawing.h"

#include <algorithm>

namespace
{
	const sf::Color SHADOW_COLOR(40, 40, 40);
	constexpr float MIN_SHADOW_OFFSET = 2.f;
	constexpr float SHADOW_OFFSET_DIVISOR = 12.f;
}

void DrawRectangle(sf::RenderTarget& target, sf::Vector2f position, sf::Vector2f size, sf::Color color)
{
	sf::RectangleShape rectangle(size);
	rectangle.setPosition(position);
	rectangle.setFillColor(color);
	target.draw(rectangle);
}

void DrawShadowedText(
	sf::RenderTarget& target,
	const sf::Font& font,
	const std::string& content,
	unsigned int characterSize,
	sf::Vector2f center,
	sf::Color fillColor)
{
	sf::Text text(font, content, characterSize);
	const sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.position + bounds.size / 2.f);

	const float shadowOffset = std::max(MIN_SHADOW_OFFSET, characterSize / SHADOW_OFFSET_DIVISOR);
	text.setPosition(center + sf::Vector2f(shadowOffset, shadowOffset));
	text.setFillColor(SHADOW_COLOR);
	target.draw(text);

	text.setPosition(center);
	text.setFillColor(fillColor);
	target.draw(text);
}