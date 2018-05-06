#include <fstream>
#include <iostream>

#include "Map.hpp"
#include "constants.hpp"

Map::Map()
{
	
}

Map::Map(const std::string& map_name)
	: name(map_name)
{
	std::cout << "Map: Loading map: " << map_name << std::endl;
	if (!this->load(name))
	{
		std::cout << "Map: Failed to load map: " << map_name << std::endl;
	}
	else
	{
		std::cout << "Map: " << map_name << " loaded successfully"
				  << std::endl;
	}
}

void Map::render(sf::RenderWindow& window)
{
	for (Brick& b : bricks)
	{
		b.draw(window);
	}
}

bool Map::load(const std::string& map_name)
{
	std::ifstream fp(map_name);
	int x = 0;
    int y = 0;
	
	if (!fp) {
		std::cout << "Map::load: Failed to open " << map_name << std::endl;
		return false;
	}

	while (y < NUM_Y)
	{
		x = 0;

		for (int i = 0; i < NUM_X; i++)
		{
			int type;
			char color;

			fp >> type;
			fp >> color;

			bricks.push_back(Brick(color, sf::Vector2i(x, y),
							 type));

			x += 1;
		}

		y += 1;
	}
    
    return true;
}

const sf::Vector2u Map::getSize()
{
	return sf::Vector2u(SIZE_X*NUM_X, SIZE_Y*NUM_Y);
}

