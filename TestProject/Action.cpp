#include "Action.h"

// Ajoute a la fin
void ActionQueue::add(const Action& action) {
    m_queue.push_back(action);
}

// Vire le premier
void ActionQueue::pop() {
    if (!m_queue.empty()) m_queue.pop_front();
}

bool ActionQueue::isEmpty() const {
    return m_queue.empty();
}

// Check c'est quoi l'action en cours
Action& ActionQueue::current() {
    return m_queue.front();
}

// Getter pour l'interface graphique
const std::deque<Action>& ActionQueue::getAll() const {
    return m_queue;
}

// --- Mes fonctions ---

Action CreateMove(float x, float y) {
    Action a;
    a.type = ActionType::Move;
    a.targetPos = { x, y };
    a.description = "WALK -> (" + std::to_string((int)x) + ", " + std::to_string((int)y) + ")";
    return a;
}

Action CreateWait(float sec) {
    Action a;
    a.type = ActionType::Wait;
    a.duration = sec;
    a.description = "WAIT (" + std::to_string((int)sec) + "s)";
    return a;
}