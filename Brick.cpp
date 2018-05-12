#include <iostream>
#include "Brick.hpp"
#include "constants.hpp"

using namespace sf;

std::vector<Texture> Brick::textures;

Brick::Brick()
	: type(BLANK)
{

}

Brick::Brick(char brick_color, Vector2i pos, int brick_type)
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
	background.setPosition(x*SIZE_X, y*SIZE_Y);

	sprite.setTexture(textures[type]);
	sprite.setPosition(x * SIZE_X, y * SIZE_Y);
}

void Brick::loadTextures() {
	Texture blank, normal, expand, one_up, new_ball;

	// Resizing+loading would avoid a copy
	if (!normal.loadFromFile("textures/normal.png"))
		std::cout << "Brick: error loading textures\n";
	expand.loadFromFile("textures/expand.png");
	one_up.loadFromFile("textures/one_up.png");
	new_ball.loadFromFile("textures/new_ball.png");

	textures.push_back(blank);
	textures.push_back(normal);
	textures.push_back(expand);
	textures.push_back(one_up);
	textures.push_back(new_ball);
}

void Brick::draw(RenderWindow& window)
{
	window.draw(background);
	window.draw(sprite);
}

Vector2i Brick::getPosition() const {
	return Vector2i(x*SIZE_X, y*SIZE_Y);
}

FloatRect Brick::getRect() const {
	return FloatRect(x*SIZE_X, y*SIZE_Y, SIZE_X, SIZE_Y);
}
