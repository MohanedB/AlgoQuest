#pragma once
#include <string>
#include <deque>
#include <SFML/System/Vector2.hpp>
#include "Inventory.h"

// J'ai mis les types pour les boys, mais je code juste Move et Wait
enum class ActionType { Move, Wait, Attack, Get, Use, Interact };

struct Action {
    ActionType type;
    std::string description;    // Texte pour l'UI
    sf::Vector2f targetPos;     // Sert juste pour le Move
    float duration;             // Sert pour le Wait
    // TODO: Ajouter int targetId pour l'attaque (voir avec coequipier)
};

class ActionQueue {
public:
    void add(const Action& action);
    void pop();
    bool isEmpty() const;
    Action& current();

    // J'ai besoin de ca pour afficher la liste dans le main
    const std::deque<Action>& getAll() const;

private:
    std::deque<Action> m_queue; // deque c mieux que queue pour l'affichage
};

// Mes helpers a moi
Action CreateMove(float x, float y);
Action CreateWait(float sec);

//Actions pour Items
Action CreateGet(const Item& item);
Action CreateUse(int itemId);