#include <fstream>
#include <iostream>
#include <sstream>

#include "Map.hpp"
#include "constants.hpp"

using namespace sf;

Map::Map()
{
	
}

Map::Map(const std::string& map_name)
	: name(map_name)
{
	std::cout << "Map: Loading map: " << map_name << std::endl;

	if (!load()) {
		std::cout << "Map: Failed to load map: " << map_name << std::endl;
	} else {
		std::cout << "Map: " << map_name << " loaded successfully"
			<< std::endl;
	}
}

void Map::render(RenderWindow& window)
{
	for (Brick& b : bricks) {
		b.draw(window);
	}
}

bool Map::load()
{
	std::ifstream fp(name);
	std::string line;
	int x = 0;
	int y = 0;
	int type = 0;
	char color = '\0';

	bricks.clear();
	padding_bricks.clear();
	size = Vector2u();

	if (!fp) {
		return false;
	}

	while (!fp.eof()) {
		std::getline(fp, line);

		std::istringstream stream(line);
		int x = 0;

		while (!stream.eof()) {
			stream >> type;
			stream >> color;

			if (fp.eof()) {
				return true;
			} else if (fp.fail()) {
				return false;
			}

			if (type == 0) {
				padding_bricks.push_back(Brick(color, Vector2i(x, y), type));
			} else {
				bricks.push_back(Brick(color, Vector2i(x, y), type));
			}

			x += 1;
		}

		y += 1;
	}

	return true;
}

Vector2u Map::getSize()
{
	int x = 0, y = 0;

	if (size == Vector2u()) {
		for (const Brick& b : bricks) {
			x = std::max(x, b.x);
			y = std::max(y, b.y);
		}

		for (const Brick& b : padding_bricks) {
			x = std::max(x, b.x);
			y = std::max(y, b.y);
		}

		size = Vector2u(SIZE_X*x, SIZE_Y*y + VOID_SIZE);
	}

	return size;
}

