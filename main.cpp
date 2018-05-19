#include <SFML/Graphics.hpp>
#include "Breaker.hpp"
#include "Brick.hpp"
#include "constants.hpp"

int main(int argc, char* argv[])
{
	Breaker breaker;
	sf::Vector2u window_size(breaker.window_size);

	sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y),
		"Breaker");
	sf::View view(window.getDefaultView());
	
	while (!breaker.isFinished()) {
		sf::Event event;
		
		while (window.pollEvent(event)) {
			breaker.handleEvents(event);
		}

		if (breaker.window_size != window_size) {
			window_size = breaker.window_size;
			view.setSize(window_size.x, window_size.y);
			view.setCenter(window_size.x/2., window_size.y/2.);

			window.setView(view);
			window.setSize(window_size);
		}

		breaker.update();
		
		window.clear(sf::Color::Black);
		breaker.render(window);
		window.display();

		sf::sleep(sf::milliseconds(3));
	}
	
	return 0;
}

