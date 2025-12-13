#pragma once
#include <SFML/Graphics.hpp>
#include <iomanip>
#include <sstream>


class Character {
protected:
    bool isDead = false;
    virtual void OnDeath();

    sf::RectangleShape shape;

	// Health Display
    sf::Font font;
    std::ostringstream ssHP;
public:

	void Initialize(float size, float positionX, float positionY, sf::Color color, float maxHealth, float damage);
    sf::Vector2f GetPosition();
	sf::RectangleShape& GetShape() { return shape; }

    float MaxHealth = 100.0f;
    float HP = 100.0f;
    float damage = 5.0f;
	bool& IsDead() { return isDead; }

	bool isTalking = false;

    void takeDamage(float damageTaken);
    void heal(float healAmount);

    void Attack(Character& target);

    sf::Text healthDisplayText = sf::Text(font, "", 16);
    sf::Text positionDisplayText = sf::Text(font, "", 16);
    void InitializeHealthDisplay(sf::Font& font, int fontSize, float positionX, float positionY, sf::Color color);
    void UpdateHealthDisplay();

    sf::Text dialogueDisplayText = sf::Text(font, "...", 16);
    void InitializeDialogueDisplay(sf::String dialogue, sf::Font& font, int fontSize, float positionX, float positionY, sf::Color color);

};