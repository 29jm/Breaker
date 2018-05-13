#include <iostream>
#include "Brick.hpp"
#include "constants.hpp"

using namespace sf;

std::vector<Texture> Brick::textures;

Brick::Brick()
	: type(BLANK)
{

}

Brick::Brick(char brick_color, Vector2f pos, Type brick_type)
	: type(brick_type), x(pos.x), y(pos.y)
{
	if (textures.empty()) {
		loadTextures();
	}

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

	background.setSize(Vector2f(SIZE_X, SIZE_Y));
	background.setFillColor(color);
	background.setPosition(x, y);

	sprite.setTexture(textures[type]);
	sprite.setPosition(x, y);
}

void Brick::loadTextures() {
	textures.resize(5);

	textures[1].loadFromFile("textures/normal.png");
	textures[2].loadFromFile("textures/expand.png");
	textures[3].loadFromFile("textures/one_up.png");
	textures[4].loadFromFile("textures/new_ball.png");
}

void Brick::draw(RenderWindow& window)
{
	window.draw(background);
	window.draw(sprite);
}

void Brick::setPosition(float xp, float yp) {
	x = xp;
	y = yp;

	sprite.setPosition(x, y);
	background.setPosition(x, y);
}

FloatRect Brick::getRect() const {
	return FloatRect(x, y, SIZE_X, SIZE_Y);
}
