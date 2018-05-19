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
	struct Ball {
		sf::CircleShape sprite;
		sf::Vector2f direction;
		float speed;
	};

	float getDeltaTime();

	void handleCollision();
	int ballBricksCollision(Ball& ball);

	void handleBrickDestruction(int index);
	void applyBonus(Brick::Type type);
	void addBall();

	float getBounceAngle(float dx);
	float getBounceSpeed(float dx);

	bool hasWon() const;
	void changeMap(unsigned int map);

	void loadMaps();
	void createDefaultMap();

	std::vector<Map> maps;
	sf::RectangleShape paddle;
	std::vector<Ball> balls;
	sf::Clock dt;
	int actual_map; // the _actual_ map
	bool is_finished;

	bool stuck;
	unsigned int lives;
	float paddle_speed;
	float base_ball_speed;
	std::vector<Brick> bonuses;
};
