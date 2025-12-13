#include "Character.h"


void Character::Initialize(float size, float positionX, float positionY, sf::Color color, float maxHealth, float damage)
{
	shape.setSize({ size, size });
	shape.setOrigin({ size / 2.0f, size / 2.0f });
	shape.setPosition({ positionX, positionY });
	shape.setFillColor(color);
	MaxHealth = maxHealth;
	HP = MaxHealth;
	this->damage = damage;
}

sf::Vector2f Character::GetPosition()
{
	return shape.getPosition();
}

void Character::InitializeHealthDisplay(sf::Font& font, int fontSize, float positionX, float positionY, sf::Color color)
{
	healthDisplayText.setFont(font);
	healthDisplayText.setCharacterSize(fontSize);
	healthDisplayText.setPosition({ positionX, positionY});
	healthDisplayText.setFillColor(color);

	positionDisplayText.setFont(font);
	positionDisplayText.setCharacterSize(14);
	positionDisplayText.setPosition({ positionX, (positionY + 15.0f) });
	positionDisplayText.setFillColor(sf::Color::White);
	positionDisplayText.setOutlineColor(sf::Color::Black);
	positionDisplayText.setOutlineThickness(1.0f);

	UpdateHealthDisplay();
}

void Character::UpdateHealthDisplay()
{
	ssHP.str("");
	ssHP.clear();
	ssHP << std::fixed << std::setprecision(0) << HP; // float to int
	healthDisplayText.setString("HP: " + ssHP.str());
	positionDisplayText.setString("( " + std::to_string(static_cast<int>(shape.getPosition().x)) + ", " + std::to_string(static_cast<int>(shape.getPosition().y)) + " )");
}

void Character::InitializeDialogueDisplay(sf::String dialogue, sf::Font& font, int fontSize, float positionX, float positionY, sf::Color color)
{
	dialogueDisplayText.setFont(font);
	dialogueDisplayText.setCharacterSize(fontSize);
	dialogueDisplayText.setPosition({ positionX, positionY });
	dialogueDisplayText.setFillColor(color);
	dialogueDisplayText.setString(dialogue);
}

void Character::takeDamage(float damageTaken)
{
	HP -= damageTaken;
	if (HP <= 0) HP = 0, OnDeath();
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

void Character::OnDeath()
{
	if (isDead) return;
	isTalking = false;
	isDead = true;
}