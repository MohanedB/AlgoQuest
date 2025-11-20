#include <SFML/Graphics.hpp>
#include <optional>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "AlgoQuest");
    window.setFramerateLimit(60);

    sf::CircleShape circle(50.f);         
    circle.setFillColor(sf::Color::Blue);
    circle.setPosition({ 800.f / 2.f - 50.f, 600.f / 2.f - 50.f });

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color(20, 20, 20)); 
        window.draw(circle);
        window.display();
    }

    return 0;
}
