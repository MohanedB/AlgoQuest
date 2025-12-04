#pragma once


class Character {
public:
	float MaxHealth = 100.0f;
	float HP;
	float damage;

	void takeDamage(float damageTaken);

	void heal(float healAmount);

	void Attack(Character& target);
};