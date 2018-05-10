#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "Breaker.hpp"
#include "Map.hpp"
#include "helpers.hpp"
#include "constants.hpp"

using namespace sf;
using std::abs;

Breaker::Breaker()
	: paddle(Vector2f(PADDLE_SIZE_X, PADDLE_SIZE_Y)),
	  ball(BALL_RADIUS, BALL_POINT_COUNT),
	  ball_direction(0, 0),
	  stuck(true),
	  is_finished(false),
	  paddle_speed(PADDLE_SPEED),
	  base_ball_speed(BALL_SPEED),
	  ball_speed(BALL_SPEED)
{
	std::ifstream map_list("map.list");
	std::string map_name;

	if (!map_list)
	{
		std::cout << "Breaker: failed to load map.list" << std::endl;
		is_finished = true;
	}

	while (!map_list.eof()) {
		map_list >> map_name;

		if (map_list.fail()) {
			break;
		}

		Map m(map_name);
		maps.push_back(m);
	}

	std::cout << "Breaker: " << maps.size() << " maps loaded" << std::endl;

	changeMap(0);

	paddle.setFillColor(Color::White);
	paddle.setPosition(window_size.x/2.-PADDLE_SIZE_X/2.,
		window_size.y-PADDLE_SIZE_Y*1.2);

	ball.setFillColor(Color::Magenta);
	ball.setOutlineColor(Color::Black);
	ball.setOutlineThickness(1);
}

void Breaker::handleEvents(const Event& event)
{
	Keyboard::Key key;

	switch (event.type) {
	case Event::Closed:
		is_finished = true;
		break;
	case Event::KeyPressed:
		key = event.key.code;
		switch (key) {
		case Keyboard::Escape:
			is_finished = true;
			break;
		case Keyboard::N:
			changeMap(actual_map + 1);
			break;
		case Keyboard::P:
			changeMap(actual_map - 1);
			break;
		case Keyboard::Space:
			stuck = !stuck;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void Breaker::update() {
	float delta_t = getDeltaTime();

	if (Keyboard::isKeyPressed(Keyboard::Left)) {
		paddle.move(-PADDLE_SPEED*delta_t, 0);
	} else if (Keyboard::isKeyPressed(Keyboard::Right)) {
		paddle.move(PADDLE_SPEED*delta_t, 0);
	}

	if (stuck) {
		ball.setPosition(paddle.getPosition().x+PADDLE_SIZE_X/2-BALL_SIDE/2,
			paddle.getPosition().y-BALL_SIDE);
	} else {
		float speed = norm<>(ball_direction);
		ball_direction = (ball_direction / speed) * ball_speed;
		ball.move(ball_direction * delta_t);
	}

	handleCollision();
}

void Breaker::render(RenderWindow& window)
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
	Vector2u map_size = maps[actual_map].getSize();

	/* ball/walls collisions */
	if (ball_x <= 0) {
		ball_direction = Vector2f(abs(ball_direction.x), ball_direction.y);
	}

	if (ball_x >= map_size.x-BALL_SIDE) {
		ball_direction = Vector2f(-abs(ball_direction.x), ball_direction.y);
	}

	if (ball_y <= 0) {
		ball_direction = Vector2f(ball_direction.x, abs(ball_direction.y));
	}

	if (ball_y+BALL_SIDE/2. >= map_size.y) {
		stuck = true;
	}

	/* paddle/walls collisions*/
	if (paddle.getPosition().x < 0) {
		paddle.setPosition(0, paddle.getPosition().y);
	}
	
	if (paddle.getPosition().x > map_size.x-PADDLE_SIZE_X) {
		paddle.setPosition(map_size.x-PADDLE_SIZE_X, paddle.getPosition().y);
	}

	/* paddle/ball collisions*/
	if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
		float p_x = paddle.getPosition().x + PADDLE_SIZE_X/2.;
		float delta_x = (p_x - (ball_x+BALL_SIDE/2.)) / (PADDLE_SIZE_X/2.);
		delta_x = clamp(delta_x, -1, 1);

		float angle = getBounceAngle(delta_x);
		ball_direction.x = std::cos(angle);
		ball_direction.y = -std::sin(angle);

		ball_speed = getBounceSpeed(delta_x);
	}

	/* ball/bricks collisions */
	FloatRect bounds = ball.getGlobalBounds();
	std::vector<Brick>& map = maps[actual_map].bricks;

	// A better version:
	// get all (theoretically) colliding bricks in a list
	// then do proper-ish circle/AABB collision tests
	for (int i = 0; i < map.size(); i++) {
		Vector2i position = map[i].getPosition();
		FloatRect brick(position.x, position.y, SIZE_X, SIZE_Y);

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

// 0 < dx < 1, with 0 the beginning of the paddle
float Breaker::getBounceAngle(float dx) {
	return map(dx, -1, 1, M_PI/12, M_PI - M_PI/12);
}

float Breaker::getBounceSpeed(float dx) {
	if (dx < 0) {
		return base_ball_speed*(1 + map(dx, -1, 0, .5, 0));
	}

	return base_ball_speed*(1 + map(dx, 0, 1, 0, .5));
}

void Breaker::changeMap(int map) {
	if (map < 0 || map >= maps.size()) {
		return;
	}

	std::cout << "Breaker: switching to map number " << map+1 << "\n";

	actual_map = map;
	maps[actual_map].load();
	window_size = maps[actual_map].getSize();
	paddle.setPosition(window_size.x/2.-PADDLE_SIZE_X/2.,
		window_size.y-PADDLE_SIZE_Y*1.2);
	stuck = true;
}
