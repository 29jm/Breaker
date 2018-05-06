#ifndef MAP_HPP
#define	MAP_HPP

#include <vector>
#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include "Brick.hpp"

class Map
{
public:
    Map();
    Map(const std::string& map_name);

    void render(sf::RenderWindow& window);
    bool load(const std::string& map_name);
    
    const sf::Vector2u getSize();
    explicit operator bool() const;
    
    std::vector<Brick> bricks;
    std::string name;
};

#endif	/* MAP_HPP */

