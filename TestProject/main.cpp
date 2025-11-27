#include <SFML/Graphics.hpp>

int main()
{
    // 1. Créer la fenêtre
    sf::RenderWindow window(sf::VideoMode({ 200, 200 }), "SFML Works!");

    // 2. Créer un rond vert (le test classique)
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    // 3. Boucle principale
    while (window.isOpen())
    {
        // Gestion des événements (Version compatible SFML 3 comme ton code précédent)
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // 4. Affichage
        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}