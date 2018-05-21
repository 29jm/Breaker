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
    bool load();

    sf::Vector2u getSize();

    std::vector<Brick> bricks;
    std::string name;

private:
	void loadPalette();

	std::vector<Brick> padding_bricks;
	sf::Vector2u size;

	static std::vector<uint32_t> palette;
};

#endif	/* MAP_HPP */
