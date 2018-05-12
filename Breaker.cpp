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
	  is_finished(false),
	  stuck(true),
	  lives(0),
	  paddle_speed(PADDLE_SPEED),
	  base_ball_speed(BALL_SPEED),
	  ball_speed(BALL_SPEED)
{
	std::ifstream map_list("map.list");
	std::string map_name;

	if (!map_list)
	{
		std::cerr << "Breaker: failed to load map.list" << std::endl;
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
	paddle.setPosition(window_size.x/2.-paddle.getSize().x/2.,
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
		ball.setPosition(paddle.getPosition().x+paddle.getSize().x/2-BALL_SIDE/2,
			paddle.getPosition().y-BALL_SIDE);
	} else {
		float speed = norm<>(ball_direction);
		ball_direction = (ball_direction / speed) * ball_speed;
		ball.move(ball_direction * delta_t);
	}

	for (Brick& b : bonuses) {
		b.setPosition(b.x, b.y + BONUS_SPEED*delta_t);
	}

	handleCollision();
}

void Breaker::render(RenderWindow& window)
{
	maps[actual_map].render(window);

	for (Brick& b : bonuses) {
		b.draw(window);
	}

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
	unsigned int ball_x = ball.getPosition().x;
	unsigned int ball_y = ball.getPosition().y;
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
		if (lives == 0) {
			changeMap(actual_map);
			return;
		} else {
			stuck = true;
		}

		lives -= 1;
		std::cout << "Lost a life, total " << lives << "\n";
	}

	/* paddle/walls collisions*/
	float paddle_size = paddle.getSize().x;
	if (paddle.getPosition().x < 0) {
		paddle.setPosition(0, paddle.getPosition().y);
	}
	
	if (paddle.getPosition().x > map_size.x-paddle_size) {
		paddle.setPosition(map_size.x-paddle_size, paddle.getPosition().y);
	}

	/* bonuses/bottom/paddle collisions */
	auto it = bonuses.begin();
	while (it != bonuses.end()) {
		Brick& b = *it;

		if (b.y > map_size.y) {
			it = bonuses.erase(it);
		} else  if (b.getRect().intersects(paddle.getGlobalBounds())) {
			applyBonus(b.type);
			it = bonuses.erase(it);
		} else {
			++it;
		}
	}

	/* paddle/ball collisions*/
	if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
		float p_x = paddle.getPosition().x + paddle_size/2.;
		float delta_x = (p_x - (ball_x+BALL_SIDE/2.)) / (paddle_size/2.);
		delta_x = clamp(delta_x, -1, 1);

		float angle = getBounceAngle(delta_x);
		ball_direction.x = std::cos(angle);
		ball_direction.y = -std::sin(angle);

		ball_speed = getBounceSpeed(delta_x);
	}

	/* ball/bricks collisions */
	Brick collided = ballBricksCollision();

	if (collided.type != Brick::BLANK) {
		handleBrickDestruction(collided);
	}

	/* exit measures*/
	if (maps[actual_map].bricks.empty()) {
		std::cout << "You won. Well played.\n";
		changeMap(actual_map);
	}
}

/* The algorithm is based on
 * https://gamedev.stackexchange.com/a/120897
 * */
Brick Breaker::ballBricksCollision() {
	FloatRect bounds = ball.getGlobalBounds();
	std::vector<Brick>& bricks = maps[actual_map].bricks;
	const float r = bounds.width/2;

	for (unsigned int i = 0; i < bricks.size(); i++) {
		FloatRect b(bricks[i].getRect());
		FloatRect h_rect(b.left-r, b.top, b.width+2*r, b.height);
		FloatRect v_rect(b.left, b.top-r, b.width, b.height+2*r);
		Vector2f ball_center(bounds.left+bounds.width/2, bounds.top+bounds.height/2);

		if (h_rect.contains(ball_center)) {
			ball_direction.x = -ball_direction.x; goto out;
		}

		if (v_rect.contains(ball_center)) {
			ball_direction.y = -ball_direction.y; goto out;
		}

		if (distance(ball_center, Vector2f(b.left, b.top)) < r) { // top left
			if (b.left - ball_center.x < b.top - ball_center.y) {
				ball_direction.y = -ball_direction.y; goto out;
			} else {
				ball_direction.x = -ball_direction.x; goto out;
			}
		}

		if (distance(ball_center, Vector2f(b.left+b.width, b.top)) < r) { // top right
			if (ball_center.x - (b.left+b.width) < b.top - ball_center.y) {
				ball_direction.y = -ball_direction.y; goto out;
			} else {
				ball_direction.x = -ball_direction.x; goto out;
			}
		}

		if (distance(ball_center, Vector2f(b.left+b.width, b.top+b.height)) < r) { // bottom right
			if (ball_center.x - (b.left+b.width) < ball_center.y - (b.top+b.height)) {
				ball_direction.y = -ball_direction.y; goto out;
			} else {
				ball_direction.x = -ball_direction.x; goto out;
			}
		}

		if (distance(ball_center, Vector2f(b.left, b.top+b.height)) < r) { // bottom left
			if (b.left - ball_center.x < ball_center.y - (b.top+b.height)) {
				ball_direction.y = -ball_direction.y; goto out;
			} else {
				ball_direction.x = -ball_direction.x; goto out;
			}
		}

		// no collision was found
		continue;

		// a collision occured
		out:
		Brick erased_brick(bricks[i]);
		bricks.erase(bricks.begin()+i);
		return erased_brick;
	}

	return Brick();
}

void Breaker::handleBrickDestruction(const Brick& b) {
	if (b.type == Brick::NORMAL) {
		return;
	}

	bonuses.push_back(b);
}

void Breaker::applyBonus(Brick::Type type) {
	if (type == Brick::EXPAND) {
		Vector2f size = paddle.getSize();
		paddle.setSize(Vector2f(size.x+PADDLE_EXPAND, size.y));
	}

	if (type == Brick::ONE_UP) {
		lives += 1;
		std::cout << "Gained a life, total " << lives << "\n";
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

void Breaker::changeMap(unsigned int map) {
	if (map >= maps.size()) {
		return;
	}

	std::cout << "Breaker: switching to map number " << map+1 << "\n";

	actual_map = map;
	maps[actual_map].load();
	window_size = maps[actual_map].getSize();
	paddle.setSize(Vector2f(PADDLE_SIZE_X, PADDLE_SIZE_Y));
	paddle.setPosition(window_size.x/2.-paddle.getSize().x/2.,
		window_size.y-PADDLE_SIZE_Y*1.2);
	stuck = true;
	lives = 0;
}
