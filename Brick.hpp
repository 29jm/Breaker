#ifndef BRICK_HPP
#define	BRICK_HPP

#include <string>
#include <SFML/Graphics.hpp>

class Brick
{
public:
	Brick();
	Brick(char color, sf::Vector2i pos, int brick_type);

	void loadTextures();
	void draw(sf::RenderWindow& window);

	sf::Vector2i getPosition() const;
	sf::FloatRect getRect() const;

	enum Type {
		BLANK,
		NORMAL,
		EXPAND,
		ONE_UP,
		NEW_BALL
	};

	int type;
	int x;
	int y;

	sf::Sprite sprite;
	sf::RectangleShape background;
	sf::Color color;

	static std::vector<sf::Texture> textures;
};

#endif	/* BRICK_HPP */

