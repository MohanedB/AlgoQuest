#include "Character.h"
#include <sstream>
#include <iomanip> // Add this include at the top of the file


void Character::takeDamage(float damageTaken)
{
	HP -= damageTaken;
	if (HP <= 0) HP = 0;
}

void Character::heal(float healAmount)
{
	HP += healAmount;
	if (HP >= MaxHealth) HP = MaxHealth;
}

void Character::Attack(Character& target)
{
	target.takeDamage(damage);
}

void Character::InitializeHealthDisplay(sf::Font& font, int fontSize, float positionX, float positionY, sf::Color color)
{
	playerHealthDisplay.setFont(font);
	playerHealthDisplay.setCharacterSize(fontSize);

	playerHealthDisplay.setPosition({ positionX, positionY});
	playerHealthDisplay.setFillColor(color);


	ssHP.str(""); // clear the stream before use
	ssHP.clear();
	ssHP << std::fixed << std::setprecision(0) << HP; // float to int
	playerHealthDisplay.setString("HP: " + ssHP.str());
}

void Character::UpdateHealthDisplay()
{
	ssHP.str(""); // clear
	ssHP.clear();
	ssHP << std::fixed << std::setprecision(0) << HP; // float to int
	playerHealthDisplay.setString("HP: " + ssHP.str());
}
