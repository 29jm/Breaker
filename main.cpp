#include <SFML/Graphics.hpp>
#include "Breaker.hpp"
#include "Brick.hpp"
#include "constants.hpp"

int main(int argc, char* argv[])
{
	sf::RenderWindow window(sf::VideoMode(SIZE_X*NUM_X, SIZE_Y*NUM_Y),
						    "Breaker by p0ney");
	Breaker breaker;
	
	while (!breaker.isFinished()) 
	{
		sf::Event event;
		
		while (window.pollEvent(event))
		{
			breaker.handleEvents(event);
		}

		breaker.update();
		
		window.clear(sf::Color::Black);
		breaker.render(window);
		window.display();
	}
	
	return 0;
}

