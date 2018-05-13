#pragma once

#include <vector>
#include "Map.hpp"
#include "Brick.hpp"

class Breaker
{
public:
	Breaker();

	void handleEvents(const sf::Event& event);
	void update();
	void render(sf::RenderWindow& window);

	bool isFinished();

	sf::Vector2u window_size;

private:
	float getDeltaTime();

	void handleCollision();
	Brick ballBricksCollision();

	void handleBrickDestruction(const Brick& b);
	void applyBonus(Brick::Type type);

	float getBounceAngle(float dx);
	float getBounceSpeed(float dx);

	void changeMap(unsigned int map);

	std::vector<Map> maps;
	sf::RectangleShape paddle;
	sf::CircleShape ball;
	sf::Clock dt;
	sf::Vector2f ball_direction;
	int actual_map; // the _actual_ map
	bool is_finished;

	bool stuck;
	unsigned int lives;
	float paddle_speed;
	float base_ball_speed;
	float ball_speed;
	std::vector<Brick> bonuses;
};
