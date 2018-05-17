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
	  is_finished(false),
	  stuck(true),
	  lives(0),
	  paddle_speed(PADDLE_SPEED),
	  base_ball_speed(BALL_SPEED)
{
	std::ifstream map_list("map.list");
	std::string map_name;

	if (!map_list)
	{
		// TODO create the file and a default map if absent
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
			if (stuck) {
				stuck = false;
				balls[0].direction.y = base_ball_speed;
			}
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

	// paddle
	if (Keyboard::isKeyPressed(Keyboard::Left)) {
		paddle.move(-paddle_speed*delta_t, 0);
	} else if (Keyboard::isKeyPressed(Keyboard::Right)) {
		paddle.move(paddle_speed*delta_t, 0);
	}

	// ball
	if (stuck) {
		balls[0].sprite.setPosition(paddle.getPosition().x+paddle.getSize().x/2-BALL_SIDE/2,
			paddle.getPosition().y-BALL_SIDE);
	} else {
		for (Ball& ball : balls) {
			float speed = norm(ball.direction);
			ball.direction = (ball.direction/speed) * ball.speed;
			ball.sprite.move(ball.direction * delta_t);
		}
	}

	// bonuses
	for (Brick& b : bonuses) {
		b.setPosition(b.x, b.y + BONUS_SPEED*delta_t);
	}

	// collisions
	handleCollision();
}

void Breaker::render(RenderWindow& window)
{
	maps[actual_map].render(window);

	for (Brick& b : bonuses) {
		b.draw(window);
	}

	window.draw(paddle);

	for (Ball& ball : balls) {
		window.draw(ball.sprite);
	}
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
	Vector2u map_size = maps[actual_map].getSize();

	for (auto it = balls.begin(); it != balls.end();) {
		Ball& b = *it;
		Vector2f pos = b.sprite.getPosition();

		/* ball/walls collisions */
		if (pos.x <= 0) {
			b.direction = Vector2f(abs(b.direction.x), b.direction.y);
		}

		if (pos.x >= map_size.x-BALL_SIDE) {
			b.direction = Vector2f(-abs(b.direction.x), b.direction.y);
		}

		if (pos.y <= 0) {
			b.direction = Vector2f(b.direction.x, abs(b.direction.y));
		}

		if (pos.y+BALL_SIDE/2. >= map_size.y) {
			it = balls.erase(it);

			if (balls.size() == 0) {
				if (lives == 0) {
					changeMap(actual_map);
				} else {
					stuck = true;
					addBall();
					lives -= 1;
					std::cout << "Lost a life, total " << lives << "\n";
				}

				return;
			}

			// avoids useless computations and moving `it` twice
			continue;
		}

		/* ball/paddle collisions*/
		if (b.sprite.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
			float width = paddle.getSize().x;
			float p_x = paddle.getPosition().x + width/2.;
			float delta_x = (p_x - (pos.x+BALL_SIDE/2.)) / (width/2.);
			delta_x = clamp(delta_x, -1, 1);

			float angle = getBounceAngle(delta_x);
			b.direction.x = std::cos(angle);
			b.direction.y = -std::sin(angle);

			b.speed = getBounceSpeed(delta_x);
		}

		/* ball/bricks collisions */
		Brick collided = ballBricksCollision(b);

		if (collided.type != Brick::BLANK) {
			handleBrickDestruction(collided);
		}

		++it;
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
	for (auto it = bonuses.begin(); it != bonuses.end();) {
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

	/* exit measures*/
	if (maps[actual_map].bricks.empty()) {
		std::cout << "You won. Well played.\n";
		changeMap(actual_map);
	}
}

/* The algorithm is based on
 * https://gamedev.stackexchange.com/a/120897
 * */
Brick Breaker::ballBricksCollision(Ball& ball) {
	FloatRect bounds = ball.sprite.getGlobalBounds();
	std::vector<Brick>& bricks = maps[actual_map].bricks;
	const float r = bounds.width/2;

	for (unsigned int i = 0; i < bricks.size(); i++) {
		FloatRect b(bricks[i].getRect());
		FloatRect h_rect(b.left-r, b.top, b.width+2*r, b.height);
		FloatRect v_rect(b.left, b.top-r, b.width, b.height+2*r);
		Vector2f ball_center(bounds.left+bounds.width/2, bounds.top+bounds.height/2);

		if (h_rect.contains(ball_center)) {
			ball.direction.x = -ball.direction.x; goto out;
		}

		if (v_rect.contains(ball_center)) {
			ball.direction.y = -ball.direction.y; goto out;
		}

		if (distance(ball_center, Vector2f(b.left, b.top)) < r) { // top left
			if (b.left - ball_center.x < b.top - ball_center.y) {
				ball.direction.y = -ball.direction.y; goto out;
			} else {
				ball.direction.x = -ball.direction.x; goto out;
			}
		}

		if (distance(ball_center, Vector2f(b.left+b.width, b.top)) < r) { // top right
			if (ball_center.x - (b.left+b.width) < b.top - ball_center.y) {
				ball.direction.y = -ball.direction.y; goto out;
			} else {
				ball.direction.x = -ball.direction.x; goto out;
			}
		}

		if (distance(ball_center, Vector2f(b.left+b.width, b.top+b.height)) < r) { // bottom right
			if (ball_center.x - (b.left+b.width) < ball_center.y - (b.top+b.height)) {
				ball.direction.y = -ball.direction.y; goto out;
			} else {
				ball.direction.x = -ball.direction.x; goto out;
			}
		}

		if (distance(ball_center, Vector2f(b.left, b.top+b.height)) < r) { // bottom left
			if (b.left - ball_center.x < ball_center.y - (b.top+b.height)) {
				ball.direction.y = -ball.direction.y; goto out;
			} else {
				ball.direction.x = -ball.direction.x; goto out;
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

	if (type == Brick::NEW_BALL) {
		addBall();

		Ball& b = balls.back();
		b.direction = balls[0].direction;
		b.direction.x = -b.direction.x;
		b.sprite.setPosition(balls[0].sprite.getPosition());
	}
}

void Breaker::addBall() {
	Ball b;

	CircleShape sprite(BALL_RADIUS, BALL_POINT_COUNT);
	sprite.setFillColor(Color(150, 150, 150));

	b.sprite = sprite;
	b.speed = base_ball_speed;

	balls.push_back(b);
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
	stuck = true;
	lives = 0;
	maps[actual_map].load();
	window_size = maps[actual_map].getSize();

	paddle.setFillColor(Color::White);
	paddle.setSize(Vector2f(PADDLE_SIZE_X, PADDLE_SIZE_Y));
	paddle.setPosition(window_size.x/2.-paddle.getSize().x/2.,
		window_size.y-PADDLE_SIZE_Y*1.2);
	paddle_speed = PADDLE_SPEED;

	base_ball_speed = BALL_SPEED;
	balls.clear();
	addBall();

	bonuses.clear();
}
