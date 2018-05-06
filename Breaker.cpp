#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "Breaker.hpp"
#include "Map.hpp"
#include "constants.hpp"

Breaker::Breaker()
	: paddle(sf::Vector2f(PADDLE_SIZE_X, PADDLE_SIZE_Y)),
	  ball(BALL_RADIUS, BALL_POINT_COUNT), ball_direction(BALL_SPEED, -BALL_SPEED),
	  accel(0), actual_map(0), map_number(0), stuck(true), is_finished(false)
{
	std::ifstream map_list("map.list");
	std::string map_name;
	
	if (!map_list)
	{
		std::cout << "Breaker::Breaker: failed to load map.list" << std::endl;
		is_finished = true;
	}
	
	while (!map_list.eof()) {
		if (map_list >> map_name) {
			Map m(map_name);
			maps.push_back();
			map_number++;
		}
	}
	
	std::cout << "Breaker::Breaker: " << map_number << " maps loaded" << std::endl;
	
	paddle.setFillColor(sf::Color::White);
	paddle.setPosition(SIZE_X*NUM_X/2-PADDLE_SIZE_X/2, SIZE_Y*NUM_Y-PADDLE_SIZE_Y);
	
	ball.setFillColor(sf::Color::Magenta);
	ball.setOutlineColor(sf::Color::Black);
	ball.setOutlineThickness(1);
	ball.setPosition(SIZE_X*NUM_X/2-BALL_SIDE/2, SIZE_Y*NUM_Y-(PADDLE_SIZE_Y+BALL_SIDE));
}

void Breaker::handleEvents(const sf::Event& event)
{	
	moved = false;
	
	switch (event.type)
	{
	case sf::Event::Closed:
		is_finished = true;
		break;
	case sf::Event::KeyPressed:
		handleKeyboard(event.key.code);
		break;
	}
}

void Breaker::update()
{
	delta_t = getDeltaTime();
	
	if (!moved && accel != 0) {
		accel += (accel > 0 ? -DECEL : DECEL);
	}
	
	if (accel > MAX_ACCEL) {
		accel = MAX_ACCEL;
	}
	
	if (accel < -MAX_ACCEL) {
		accel = -MAX_ACCEL;
	}
	
	paddle.move(delta_t*accel, 0);
	
	if (paddle.getPosition().x < 0) {
		paddle.setPosition(0, paddle.getPosition().y);
		accel = 0;
	}
	
	if (paddle.getPosition().x > SIZE_X*NUM_X-PADDLE_SIZE_X) {
		paddle.setPosition(SIZE_X*NUM_X-PADDLE_SIZE_X, paddle.getPosition().y);
		accel = 0;
	}
	
	if (stuck) {
		ball.setPosition(paddle.getPosition().x+PADDLE_SIZE_X/2-BALL_SIDE/2,
						 paddle.getPosition().y-BALL_SIDE);
	}
	else
	{
		ball.move(ball_direction*delta_t);
		handleCollision();
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

void Breaker::handleKeyboard(const sf::Keyboard::Key& key)
{	
	switch (key)
	{
	case sf::Keyboard::Escape:
		is_finished = true;
		break;
		
	case sf::Keyboard::N:
		if (actual_map+1 < map_number) {
			actual_map++;
			std::cout	<< "Breaker::handleKeyboard: switching to map number "
						<< actual_map+1 << std::endl;
		}
		break;
		
	case sf::Keyboard::P:
		if (actual_map > 0) {
			actual_map--;
			std::cout	<< "Breaker::handleKeyboard: switching to map number "
						<< actual_map+1 << std::endl;
		}
		break;
	
	case sf::Keyboard::Left:
		accel -= ACCEL;
		moved = true;
		break;
		
	case sf::Keyboard::Right:
		accel += ACCEL;
		moved = true;
		break;
	
	case sf::Keyboard::Space:
		stuck = !stuck;
		break;
	}
}

float Breaker::getDeltaTime()
{
	return dt.restart().asSeconds();
}

void Breaker::handleCollision()
{
	int ball_x = ball.getPosition().x;
	int ball_y = ball.getPosition().y;
	
	if (ball_x <= 0) {
		ball_direction = sf::Vector2f(BALL_SPEED, ball_direction.y);
	}
	if (ball_x >= SIZE_X*NUM_X-BALL_SIDE) {
		ball_direction = sf::Vector2f(-BALL_SPEED, ball_direction.y);
	}
	if (ball_y <= 0) {
		ball_direction = sf::Vector2f(ball_direction.x, BALL_SPEED);
	}
	
	if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds()))
	{
		ball_direction = sf::Vector2f(ball_direction.x, -BALL_SPEED);
	}	
}