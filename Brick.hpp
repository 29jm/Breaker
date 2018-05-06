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
    void setType(int type);
    void draw(sf::RenderWindow& window);
    
private:
    sf::RectangleShape sprite;
    sf::Color color;
    
    int type;
    int x;
    int y;
};

#endif	/* BRICK_HPP */

