#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <cmath>
#include <optional>
#include "Action.hpp"

// Je parse juste mes commandes, les autres feront les leurs
bool ParseCommand(const std::string& line, Action& outAction) {
    std::stringstream ss(line);
    std::string cmd;
    ss >> cmd;

    if (cmd == "walk" || cmd == "move") {
        float x, y;
        if (ss >> x >> y) {
            outAction = CreateMove(x, y);
            return true;
        }
    }
    else if (cmd == "wait") {
        float t;
        if (ss >> t) {
            outAction = CreateWait(t);
            return true;
        }
    }
    // TODO: Ajouter les else if pour attack, get, use ici (voir coequipiers)

    return false;
}

// Calcul de distance basic
float getLength(sf::Vector2f v) { return std::sqrt(v.x * v.x + v.y * v.y); }

int main() {
    // Setup fenetre
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Moteur Action Queue");
    window.setFramerateLimit(60);

    // 1. SETUP PLAYER
    sf::RectangleShape player(sf::Vector2f(30.f, 30.f));
    player.setOrigin({ 15.f, 15.f });
    player.setPosition({ 100.f, 300.f });
    player.setFillColor(sf::Color::Cyan);
    float speed = 200.f;

    sf::Font font;
    if (!font.openFromFile("arial.ttf")) return -1;

    // --- INTERFACE ---

    // Zone de texte
    sf::Text inputText(font, "> ", 20);
    inputText.setPosition({ 10.f, 560.f });

    // Cheat sheet
    sf::Text helpText(font, "", 16);
    helpText.setPosition({ 10.f, 480.f });
    helpText.setFillColor(sf::Color(150, 150, 150));
    helpText.setString("COMMANDES:\n - walk [x] [y]\n - wait [secondes]");

    // File d'attente (HUD)
    sf::Text queueDisplay(font, "FILE:", 18);
    queueDisplay.setPosition({ 550.f, 20.f });
    queueDisplay.setFillColor(sf::Color::Yellow);

    // Logique
    ActionQueue myQueue;
    Action currentAction;
    bool isBusy = false; // State: est-ce qu'on bouge ou pas
    std::string userBuffer;
    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // 2. INPUTS
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            // Typing system
            if (const auto* textEvt = event->getIf<sf::Event::TextEntered>()) {
                if (textEvt->unicode >= 32 && textEvt->unicode < 128) {
                    userBuffer += static_cast<char>(textEvt->unicode);
                }
            }
            // Touches
            if (const auto* keyEvt = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvt->code == sf::Keyboard::Key::Backspace && !userBuffer.empty()) {
                    userBuffer.pop_back();
                }
                else if (keyEvt->code == sf::Keyboard::Key::Enter) {
                    Action act;
                    if (ParseCommand(userBuffer, act)) {
                        myQueue.add(act);
                        std::cout << "Added: " << act.description << "\n";
                    }
                    userBuffer.clear();
                }
            }
        }

        // 3. MOTEUR (UPDATE)

        // Si libre, on pop la prochaine action
        if (!isBusy && !myQueue.isEmpty()) {
            currentAction = myQueue.current();
            myQueue.pop();
            isBusy = true;
        }

        // Execution
        if (isBusy) {
            if (currentAction.type == ActionType::Move) {
                // Logique de deplacement vectoriel
                sf::Vector2f dir = currentAction.targetPos - player.getPosition();
                float dist = getLength(dir);

                if (dist < 5.f) {
                    player.setPosition(currentAction.targetPos); // Arrive
                    isBusy = false;
                }
                else {
                    player.move((dir / dist) * speed * dt);
                }
            }
            else if (currentAction.type == ActionType::Wait) {
                currentAction.duration -= dt;
                if (currentAction.duration <= 0) isBusy = false;
            }
            // TODO: Ajouter les else if pour Attack/Use ici
        }

        // 4.  UI
        inputText.setString("> " + userBuffer);

        std::string qStr = "--- QUEUE ---\n";
        int idx = 1;
        for (const auto& a : myQueue.getAll()) {
            qStr += std::to_string(idx++) + ". " + a.description + "\n";
        }
        if (isBusy) qStr += "\n[RUNNING]...\n";

        queueDisplay.setString(qStr);

        window.clear(sf::Color::Black);
        window.draw(player);
        window.draw(inputText);
        window.draw(helpText);
        window.draw(queueDisplay);
        window.display();
    }
    return 0;
}