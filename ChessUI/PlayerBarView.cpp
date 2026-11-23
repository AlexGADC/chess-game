#include "PlayerBarView.h"

#include "UIDrawing.h"

namespace
{
	constexpr int SECONDS_PER_MINUTE = 60;
	constexpr int FIRST_TWO_DIGIT_SECOND = 10;

	constexpr unsigned int NAME_FONT_SIZE = 26;
	constexpr unsigned int CLOCK_FONT_SIZE = 28;
	constexpr float HORIZONTAL_PADDING = 20.f;
	constexpr float CLOCK_VERTICAL_MARGIN = 6.f;
	constexpr float CLOCK_WIDTH = 120.f;

	const sf::Color BAR_COLOR(30, 30, 30);
	const sf::Color NAME_COLOR(255, 255, 255);
	const sf::Color ACTIVE_CLOCK_COLOR(100, 150, 80);
	const sf::Color INACTIVE_CLOCK_COLOR(70, 70, 70);
	const sf::Color ACTIVE_CLOCK_TEXT_COLOR(255, 255, 255);
	const sf::Color INACTIVE_CLOCK_TEXT_COLOR(170, 170, 170);

	// 185 seconds -> "3:05"
	std::string FormatClockTime(int totalSeconds)
	{
		const int minutes = totalSeconds / SECONDS_PER_MINUTE;
		const int seconds = totalSeconds % SECONDS_PER_MINUTE;
		const std::string secondsPadding = seconds < FIRST_TWO_DIGIT_SECOND ? "0" : "";

		return std::to_string(minutes) + ":" + secondsPadding + std::to_string(seconds);
	}
}

PlayerBarView::PlayerBarView(const sf::Font& font, std::string playerName, sf::Vector2f position, sf::Vector2f size)
	: m_font(font)
	, m_playerName(std::move(playerName))
	, m_position(position)
	, m_size(size)
{
}

void PlayerBarView::Draw(sf::RenderTarget& target, int remainingSeconds, bool isActive) const
{
	DrawRectangle(target, m_position, m_size, BAR_COLOR);
	DrawPlayerName(target);
	DrawClock(target, remainingSeconds, isActive);
}

void PlayerBarView::DrawPlayerName(sf::RenderTarget& target) const
{
	sf::Text text(m_font, m_playerName, NAME_FONT_SIZE);
	text.setFillColor(NAME_COLOR);

	// Left-aligned, vertically centered in the bar.
	const sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin({ bounds.position.x, bounds.position.y + bounds.size.y / 2.f });
	text.setPosition(m_position + sf::Vector2f(HORIZONTAL_PADDING, m_size.y / 2.f));
	target.draw(text);
}

void PlayerBarView::DrawClock(sf::RenderTarget& target, int remainingSeconds, bool isActive) const
{
	const sf::Vector2f clockSize(CLOCK_WIDTH, m_size.y - 2.f * CLOCK_VERTICAL_MARGIN);
	const sf::Vector2f clockPosition =
		m_position + sf::Vector2f(m_size.x - HORIZONTAL_PADDING - CLOCK_WIDTH, CLOCK_VERTICAL_MARGIN);

	DrawRectangle(target, clockPosition, clockSize, isActive ? ACTIVE_CLOCK_COLOR : INACTIVE_CLOCK_COLOR);
	DrawShadowedText(
		target, m_font, FormatClockTime(remainingSeconds), CLOCK_FONT_SIZE,
		clockPosition + clockSize / 2.f,
		isActive ? ACTIVE_CLOCK_TEXT_COLOR : INACTIVE_CLOCK_TEXT_COLOR);
}
