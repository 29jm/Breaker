#ifndef BREAKER_HPP
#define	BREAKER_HPP

#include <vector>
#include "Map.hpp"

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
	void ballBricksCollision();

	float getBounceAngle(float dx);
	float getBounceSpeed(float dx);

	void changeMap(int map);

	std::vector<Map> maps;
	sf::RectangleShape paddle;
	sf::CircleShape ball;
	sf::Clock dt;
	sf::Vector2f ball_direction;
	int actual_map; // the _actual_ map
	int map_number;
	bool stuck;
	bool is_finished;
	float paddle_speed;
	float base_ball_speed;
	float ball_speed;
};

#endif	/* BREAKER_HPP */

