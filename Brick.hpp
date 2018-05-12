#ifndef BRICK_HPP
#define	BRICK_HPP

#include <string>
#include <SFML/Graphics.hpp>

class Brick
{
public:
	enum Type {
		BLANK,
		NORMAL,
		EXPAND,
		ONE_UP,
		NEW_BALL
	};

	Brick();
	Brick(char color, sf::Vector2f pos, Type brick_type);

	void loadTextures();
	void draw(sf::RenderWindow& window);

	void setPosition(float xp, float yp);
	sf::FloatRect getRect() const;

	Type type;
	float x;
	float y;

	sf::Sprite sprite;
	sf::RectangleShape background;
	sf::Color color;

	static std::vector<sf::Texture> textures;
};

#endif	/* BRICK_HPP */

