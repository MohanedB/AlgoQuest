#pragma once

#include <string>
#include <queue>
#include <SFML/System/Vector2.hpp>

enum class ActionType
{
    Move,       // déplacement
    Attack,     // attaque
    GetItem,    // ramasser un objet
    UseItem,    // utiliser un objet
    Interact,   // interaction
    Wait        // attente
};

struct Action
{
    ActionType   type{};                 // type d’action
    std::string  description;            // texte explicatif

    // Paramètres possibles selon le type
    sf::Vector2f targetPosition{ 0.f, 0.f }; // pour Move / Interact
    int          targetEnemyId{ -1 };        // pour Attack
    std::string  itemId;                   // pour GetItem / UseItem
    float        waitSeconds{ 0.f };         // pour Wait
};

class ActionQueue
{
public:
    void enqueue(const Action& action);
    bool isEmpty() const;
    Action& front();
    const Action& front() const;
    void pop();
    std::size_t size() const;

private:
    std::queue<Action> m_actions;
};

// Fabriques d’actions (helpers)
Action MakeMoveAction(const sf::Vector2f& destination);
Action MakeAttackAction(int enemyId);
Action MakeGetItemAction(const std::string& itemId);
Action MakeUseItemAction(const std::string& itemId);
Action MakeInteractAction(const sf::Vector2f& position, const std::string& label);
Action MakeWaitAction(float seconds);
