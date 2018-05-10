#include <iostream>
#include "Brick.hpp"
#include "constants.hpp"

Brick::Brick(char brick_color, sf::Vector2i pos, int brick_type)
	: x(pos.x), y(pos.y), type(brick_type)
{
	sprite.setSize(sf::Vector2f(SIZE_X, SIZE_Y));

	switch (brick_color) {
	case 'R':
		color = sf::Color(255, 0, 0);
		break;
	case 'G':
		color = sf::Color(0, 255, 0);
		break;
	case 'B':
		color = sf::Color(0, 0, 255);
	}

	sprite.setOutlineColor(sf::Color::Black);
	sprite.setOutlineThickness(BORDER_SIZE);

	sprite.setFillColor(color);
	sprite.setPosition(x * SIZE_X, y * SIZE_Y);
}

void Brick::draw(sf::RenderWindow& window)
{
	window.draw(sprite);
}

sf::Vector2i Brick::getPosition() const {
	return sf::Vector2i(x*SIZE_X, y*SIZE_Y);
}
