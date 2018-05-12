#include <iostream>
#include "Brick.hpp"
#include "constants.hpp"

using namespace sf;

Brick::Brick(char brick_color, Vector2i pos, int brick_type)
	: x(pos.x), y(pos.y), type(brick_type)
{
	sprite.setSize(Vector2f(SIZE_X, SIZE_Y));

	switch (brick_color) {
	case 'R':
		color = Color(255, 0, 0);
		break;
	case 'G':
		color = Color(0, 255, 0);
		break;
	case 'B':
		color = Color(0, 0, 255);
	}

	sprite.setOutlineColor(Color::Black);
	sprite.setOutlineThickness(BORDER_SIZE);

	sprite.setFillColor(color);
	sprite.setPosition(x * SIZE_X, y * SIZE_Y);
}

void Brick::draw(RenderWindow& window)
{
	window.draw(sprite);
}

Vector2i Brick::getPosition() const {
	return Vector2i(x*SIZE_X, y*SIZE_Y);
}

FloatRect Brick::getRect() const {
	return FloatRect(x*SIZE_X, y*SIZE_Y, SIZE_X, SIZE_Y);
}
