#ifndef BRICK_HPP
#define	BRICK_HPP

#include <string>
#include <SFML/Graphics.hpp>

class Brick
{
public:
    Brick();
    Brick(char color, sf::Vector2i pos, int brick_type);

    void setColor(sf::Color color);
    void draw(sf::RenderWindow& window);

	sf::Vector2i getPosition() const;

    int type;
    int x;
    int y;

    sf::RectangleShape sprite;
    sf::Color color;
};

#endif	/* BRICK_HPP */

