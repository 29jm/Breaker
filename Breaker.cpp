#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "Breaker.hpp"
#include "Map.hpp"
#include "constants.hpp"

Breaker::Breaker()
	: paddle(sf::Vector2f(PADDLE_SIZE_X, PADDLE_SIZE_Y)),
	  ball(BALL_RADIUS, BALL_POINT_COUNT), ball_direction(BALL_SPEED, -BALL_SPEED),
	  actual_map(0), map_number(0), stuck(true), is_finished(false)
{
	std::ifstream map_list("map.list");
	std::string map_name;
	
	if (!map_list)
	{
		std::cout << "Breaker: failed to load map.list" << std::endl;
		is_finished = true;
	}
	
	while (!map_list.eof()) {
		if (map_list >> map_name) {
			Map m(map_name);
			maps.push_back(m);
			map_number++;
		}
	}
	
	std::cout << "Breaker: " << map_number << " maps loaded" << std::endl;
	
	paddle.setFillColor(sf::Color::White);
	paddle.setPosition(SIZE_X*NUM_X/2-PADDLE_SIZE_X/2, SIZE_Y*NUM_Y-PADDLE_SIZE_Y);
	
	ball.setFillColor(sf::Color::Magenta);
	ball.setOutlineColor(sf::Color::Black);
	ball.setOutlineThickness(1);
	ball.setPosition(SIZE_X*NUM_X/2-BALL_SIDE/2, SIZE_Y*NUM_Y-(PADDLE_SIZE_Y+BALL_SIDE));
}

void Breaker::handleEvents(const sf::Event& event)
{	
	switch (event.type) {
	case sf::Event::Closed:
		is_finished = true;
		break;
	case sf::Event::KeyPressed:
		sf::Keyboard::Key key = event.key.code;
		switch (key) {
		case sf::Keyboard::Escape:
			is_finished = true;
			break;
		case sf::Keyboard::N:
			if (actual_map+1 < map_number) {
				actual_map++;
				std::cout << "Breaker::handleKeyboard: switching to map number "
					<< actual_map+1 << std::endl;
			}
			break;
		case sf::Keyboard::P:
			if (actual_map > 0) {
				actual_map--;
				std::cout << "Breaker::handleKeyboard: switching to map number "
					<< actual_map+1 << std::endl;
			}
			break;
		case sf::Keyboard::Space:
			stuck = !stuck;
			break;
		}
		break;
	}
}

void Breaker::update() {
	float delta_t = getDeltaTime();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		paddle.move(-PADDLE_SPEED*delta_t, 0);
	} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		paddle.move(PADDLE_SPEED*delta_t, 0);
	}

	if (paddle.getPosition().x < 0) {
		paddle.setPosition(0, paddle.getPosition().y);
	}
	
	if (paddle.getPosition().x > SIZE_X*NUM_X-PADDLE_SIZE_X) {
		paddle.setPosition(SIZE_X*NUM_X-PADDLE_SIZE_X, paddle.getPosition().y);
	}
	
	if (stuck) {
		ball.setPosition(paddle.getPosition().x+PADDLE_SIZE_X/2-BALL_SIDE/2,
			paddle.getPosition().y-BALL_SIDE);
	} else {
		handleCollision();

		float x_sq = ball_direction.x*ball_direction.x;
		float y_sq = ball_direction.y*ball_direction.y;
		float norm = std::sqrt(x_sq+y_sq);
		ball_direction = (ball_direction/norm) * BALL_SPEED;
		ball.move(ball_direction*delta_t);
	}
}

void Breaker::render(sf::RenderWindow& window)
{
    maps[actual_map].render(window);
	window.draw(paddle);
	window.draw(ball);
}

bool Breaker::isFinished()
{
	return is_finished;
}

/** PRIVATE **/

float Breaker::getDeltaTime()
{
	return dt.restart().asSeconds();
}

void Breaker::handleCollision()
{
	int ball_x = ball.getPosition().x;
	int ball_y = ball.getPosition().y;
	
	if (ball_x <= 0) {
		ball_direction = sf::Vector2f(std::abs(ball_direction.x), ball_direction.y);
	}

	if (ball_x >= SIZE_X*NUM_X-BALL_SIDE) {
		ball_direction = sf::Vector2f(-std::abs(ball_direction.x), ball_direction.y);
	}

	if (ball_y <= 0) {
		ball_direction = sf::Vector2f(ball_direction.x, std::abs(ball_direction.y));
	}

	if (ball_y+BALL_SIDE/2. >= SIZE_Y*NUM_Y) {
		stuck = true;
	}
	
	if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
		float p_x = paddle.getPosition().x + PADDLE_SIZE_X;
		float delta_x = (p_x - (ball_x+BALL_SIDE/2.))/PADDLE_SIZE_X;
		float angle = delta_x * M_PI;

		ball_direction.x = std::cos(angle);
		ball_direction.y = -std::sin(angle);
	}

	sf::FloatRect bounds = ball.getGlobalBounds();
	std::vector<Brick>& map = maps[actual_map].bricks;

	for (int i = 0; i < map.size(); i++) {
		sf::Vector2i pos = map[i].getPosition();
		sf::FloatRect brick(pos.x, pos.y, SIZE_X, SIZE_Y);

		if (!bounds.intersects(brick)) {
			continue;
		}

		if (bounds.left+BALL_SIDE/2. < brick.left) { // O[ ]
			ball_direction.x = -abs(ball_direction.x);
		} else if (bounds.left+BALL_SIDE/2. > brick.left+SIZE_X) { // [ ]O
			ball_direction.x = abs(ball_direction.x);
		} else if (bounds.top+BALL_SIDE/2. > brick.top+SIZE_Y) { // [ ]
			ball_direction.y = abs(ball_direction.y);            //  O
		} else if (bounds.top+BALL_SIDE/2. < brick.top) { //  O
			ball_direction.y = -abs(ball_direction.y);    // [ ]
		}

		map.erase(map.begin()+i);
		break;
	}
}
