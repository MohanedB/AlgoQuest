#include <SFML/Graphics.hpp>

int main()
{
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 600), "AlgoQuest");
    window.setFramerateLimit(60);

    // Simple shape to show something on screen
    sf::CircleShape circle(50.f);      // radius 50
    circle.setFillColor(sf::Color::Blue);
    circle.setPosition(800.f / 2.f - 50.f, 600.f / 2.f - 50.f); // roughly center

    // Main loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color(20, 20, 20)); // dark gray background
        window.draw(circle);
        window.display();
    }

    return 0;
}
