#pragma once
#include <SFML/Graphics.hpp>
#include <iomanip> // Add this include at the top of the file
#include <sstream>


class Character {
public:

    float MaxHealth = 100.0f;
    float HP = 100.0f;
    float damage = 5.0f;

    sf::Font font;
    sf::Text playerHealthDisplay = sf::Text(font, "", 16);
    std::ostringstream ssHP;



    void takeDamage(float damageTaken);

    void heal(float healAmount);

    void Attack(Character& target);

    void InitializeHealthDisplay(sf::Font& font, int fontSize, float positionX, float positionY, sf::Color color);
    void UpdateHealthDisplay();
};