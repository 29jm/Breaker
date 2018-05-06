#ifndef BREAKER_HPP
#define	BREAKER_HPP

#include <vector>
#include "Map.hpp"

class Event;
class RenderWindow;
class Keyboard;

class Breaker
{
public:
    Breaker();
    
    void handleEvents(const sf::Event& event);
    void update();
    void render(sf::RenderWindow& window);
    
    bool isFinished();
    
private:
    void handleKeyboard(const sf::Keyboard::Key& key);
    float getDeltaTime();
    void handleCollision();
    
    std::vector<Map> maps;
    sf::RectangleShape paddle;
    sf::CircleShape ball;
    sf::Clock dt;
    sf::Vector2f ball_direction;
    float accel;
    float delta_t;
    int actual_map;
    int map_number;
    bool stuck;
    bool moved;
    bool is_finished;
};

#endif	/* BREAKER_HPP */

