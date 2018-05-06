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

	if (!load(name)) {
		std::cout << "Map: Failed to load map: " << map_name << std::endl;
	} else {
		std::cout << "Map: " << map_name << " loaded successfully"
			<< std::endl;
	}
}

void Map::render(sf::RenderWindow& window)
{
	for (Brick& b : bricks) {
		b.draw(window);
	}
}

bool Map::load(const std::string& map_name)
{
	std::ifstream fp(map_name);
	int x = 0;
    int y = 0;
	int type = 0;
	char color = '\0';
	
	if (!fp) {
		std::cout << "Map::load: Failed to open " << map_name << std::endl;
		return false;
	}

	for (int y = 0; y < NUM_Y && !fp.eof(); y++) {
		for (int x = 0; x < NUM_X && !fp.eof(); x++) {
			fp >> type;
			fp >> color;

			if (fp.fail()) {
				std::cerr << "Map::load: failed while reading " << map_name << "\n";
				fp.clear(fp.rdstate() | std::ios_base::eofbit); // end loops
				break;
			}

			bricks.push_back(Brick(color, sf::Vector2i(x, y), type));
		}
	}
    
    return true;
}

const sf::Vector2u Map::getSize()
{
	return sf::Vector2u(SIZE_X*NUM_X, SIZE_Y*NUM_Y);
}

