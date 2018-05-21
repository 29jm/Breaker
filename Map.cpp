#include <fstream>
#include <iostream>
#include <sstream>

#include "Map.hpp"
#include "constants.hpp"
#include "default_files.hpp"

using namespace sf;
using std::max;
using std::ofstream;
using std::ifstream;

std::vector<uint32_t> Map::palette;

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
	ifstream map_file(name);
	std::string line;
	int y = 0;
	int type = 0;
	unsigned int color_index = 0;

	bricks.clear();
	padding_bricks.clear();
	size = Vector2u();

	if (!map_file) {
		return false;
	}

	if (palette.size() == 0) {
		loadPalette();
	}

	while (!map_file.eof()) {
		std::getline(map_file, line);

		std::istringstream stream(line);
		int x = 0;

		while (!stream.eof()) {
			stream >> type;
			stream >> color_index;

			if (map_file.eof()) {
				return true;
			} else if (map_file.fail()) {
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

		// The coordinates are the position of the top-left corner of each brick
		x += SIZE_X;
		y += SIZE_Y;

		size = Vector2u(int(x), int(y) + VOID_SIZE);
	}

	return size;
}

void Map::loadPalette() {
	ifstream palette_file("palette.list");

	if (!palette_file) {
		ofstream npalette("palette.list", ofstream::out | ofstream::trunc);
		npalette << default_palette_str;
		npalette.close();
		palette_file.open("palette.list");
	}

	std::string line;
	uint32_t color;

	while (true) {
		std::getline(palette_file, line);

		if (line.empty()) {
			break;
		}

		line = line.substr(0, line.find(" "));
		line += "FF";
		std::stringstream stream(line);
		stream >> std::hex >> color;
		palette.push_back(color);
	}
}
