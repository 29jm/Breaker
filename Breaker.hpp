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
    float getDeltaTime();
    void handleCollision();
    
    std::vector<Map> maps;
    sf::RectangleShape paddle;
    sf::CircleShape ball;
    sf::Clock dt;
    sf::Vector2f ball_direction;
    int actual_map; // the _actual_ map
    int map_number;
    bool stuck;
    bool is_finished;
};

#endif	/* BREAKER_HPP */

