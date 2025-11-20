#include "Action.hpp"
#include <string>

// ==========================
// ActionQueue
// ==========================

void ActionQueue::enqueue(const Action& action)
{
    m_actions.push(action);
}

bool ActionQueue::isEmpty() const
{
    return m_actions.empty();
}

Action& ActionQueue::front()
{
    return m_actions.front();
}

const Action& ActionQueue::front() const
{
    return m_actions.front();
}

void ActionQueue::pop()
{
    if (!m_actions.empty())
        m_actions.pop();
}

std::size_t ActionQueue::size() const
{
    return m_actions.size();
}

// ==========================
// Helpers pour créer des actions
// ==========================

Action MakeMoveAction(const sf::Vector2f& destination)
{
    Action a;
    a.type = ActionType::Move;
    a.description = "Déplacement vers (" +
        std::to_string(static_cast<int>(destination.x)) + ", " +
        std::to_string(static_cast<int>(destination.y)) + ")";
    a.targetPosition = destination;
    return a;
}

Action MakeAttackAction(int enemyId)
{
    Action a;
    a.type = ActionType::Attack;
    a.description = "Attaque ennemi #" + std::to_string(enemyId);
    a.targetEnemyId = enemyId;
    return a;
}

Action MakeGetItemAction(const std::string& itemId)
{
    Action a;
    a.type = ActionType::GetItem;
    a.description = "Ramasse l’objet: " + itemId;
    a.itemId = itemId;
    return a;
}

Action MakeUseItemAction(const std::string& itemId)
{
    Action a;
    a.type = ActionType::UseItem;
    a.description = "Utilise l’objet: " + itemId;
    a.itemId = itemId;
    return a;
}

Action MakeInteractAction(const sf::Vector2f& position, const std::string& label)
{
    Action a;
    a.type = ActionType::Interact;
    a.description = "Interaction avec: " + label;
    a.targetPosition = position;
    return a;
}

Action MakeWaitAction(float seconds)
{
    Action a;
    a.type = ActionType::Wait;
    a.description = "Attente de " + std::to_string(seconds) + " secondes";
    a.waitSeconds = seconds;
    return a;
}
