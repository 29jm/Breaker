#include <fstream>
#include <iostream>
#include <sstream>

#include "Map.hpp"
#include "constants.hpp"

using namespace sf;
using std::max;

static std::vector<uint32_t> palette{
	0xFF1415FF, // R
	0x00DB00FF, // G
	0x4CB7FFFF  // B
};

Map::Map()
{
	
}

Map::Map(const std::string& map_name)
	: name(map_name)
{
	std::cout << "Map: Loading map: " << map_name << std::endl;

	if (!load()) {
		std::cout << "Map: Failed to load map: " << map_name << std::endl;
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
	int y = 0;
	int type = 0;
	unsigned int color_index = 0;

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
			stream >> color_index;

			if (fp.eof()) {
				return true;
			} else if (fp.fail()) {
				return false;
			}

			Color color;
			if (color_index < palette.size()) {
				color = Color(palette[color_index]);
			} else {
				color = Color::Red; // best color
			}

			Brick b(Color(color), Vector2f(x, y), Brick::Type(type));

			if (type == 0) {
				padding_bricks.push_back(b);
			} else {
				bricks.push_back(b);
			}

			x += SIZE_X;
		}

		y += SIZE_Y;
	}

	return true;
}

Vector2u Map::getSize()
{
	float x = 0, y = 0;

	if (size == Vector2u()) {
		for (const Brick& b : bricks) {
			x = std::max(x, b.x);
			y = std::max(y, b.y);
		}

		for (const Brick& b : padding_bricks) {
			x = std::max(x, b.x);
			y = std::max(y, b.y);
		}

		// The coordinates are the top-left corner of each brick
		x += SIZE_X;
		y += SIZE_Y;

		size = Vector2u(int(x), int(y) + VOID_SIZE);
	}

	return size;
}

