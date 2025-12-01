#include "Character.h"

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