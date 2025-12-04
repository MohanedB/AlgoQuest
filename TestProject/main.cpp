#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <cmath>
#include <optional>
#include <random>
#include "Action.h"
#include "Inventory.h"
#include "Character.h"

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

    Character playerChar;
	playerChar.HP = playerChar.MaxHealth;
	playerChar.damage = 10.0f;



    // Spawn Ennemies
    
    // Ennemy 1
	sf::RectangleShape enemy1(sf::Vector2f(30.f, 30.f));
	enemy1.setOrigin({ 15.f, 15.f });
    std::random_device rdE1;
    std::mt19937 genE1(rdE1());
    std::uniform_real_distribution<float> distE1(0.0f, 500.0f);
    float RandomXE1 = distE1(genE1);
    float RandomYE1 = distE1(genE1);
    enemy1.setPosition({ RandomXE1, RandomYE1 });
	enemy1.setFillColor(sf::Color::Red);
    Character enemyChar1;
    enemyChar1.MaxHealth = 25.0f;
    enemyChar1.HP = enemyChar1.MaxHealth;
    enemyChar1.damage = 5.0f;

    // Ennemy 2
    sf::RectangleShape enemy2(sf::Vector2f(30.f, 30.f));
    enemy2.setOrigin({ 15.f, 15.f });
    std::random_device rdE2;
    std::mt19937 genE2(rdE2());
    std::uniform_real_distribution<float> distE2(0.0f, 500.0f);
    float RandomXE2 = distE1(genE2);
    float RandomYE2 = distE1(genE2);
    enemy2.setPosition({ RandomXE2, RandomYE2 });
    enemy2.setFillColor(sf::Color::Red);
    Character enemyChar2;
    enemyChar2.MaxHealth = 35.0f;
    enemyChar2.HP = enemyChar1.MaxHealth;
    enemyChar2.damage = 5.0f;

    // Ennemy 3
    sf::RectangleShape enemy3(sf::Vector2f(50.f, 50.f));
    enemy3.setOrigin({ 25.f, 25.f });
    std::random_device rdE3;
    std::mt19937 genE3(rdE3());
    std::uniform_real_distribution<float> distE3(0.0f, 500.0f);
    float RandomXE3 = distE3(genE3);
    float RandomYE3 = distE3(genE3);
    enemy3.setPosition({ RandomXE3, RandomYE3 });
    enemy3.setFillColor(sf::Color::Red);
    Character enemyChar3;
    enemyChar3.MaxHealth = 50.0f;
    enemyChar3.HP = enemyChar1.MaxHealth;
    enemyChar3.damage = 7.0f;



    //SETUP ITEMS AND IVENTORY
    CharInventory inv;
    Item potion;
	potion.type = ItemType::Health;
    potion.itemId = 583;
    potion.itemDesc = "Health Potion";
    potion.itemPower = 10.0f;

    sf::CircleShape potionRef(20.0f);
	potionRef.setFillColor(sf::Color::Green);
	potionRef.setOrigin({ 15.f, 15.f });
    std::random_device rdH;
    std::mt19937 gen(rdH());
    std::uniform_real_distribution<float> dist(0.0f, 500.0f);
    float RandomXH = dist(gen);
    float RandomYH = dist(gen);
    potionRef.setPosition({ RandomXH , RandomYH });

    Item fireball;
    fireball.type = ItemType::Damage;
    fireball.itemId = 194;
    fireball.itemDesc = "Fireball Spell";
    fireball.itemPower = 15.0f;

    sf::CircleShape fireballRef(20.0f);
    fireballRef.setFillColor(sf::Color(255, 128, 0, 255));
    fireballRef.setOrigin({ 15.f, 15.f });
    std::random_device rdD;
    std::mt19937 genD(rdD());
    float RandomXD = dist(genD);
    float RandomYD = dist(genD);
	fireballRef.setPosition({ RandomXD , RandomYD });

    Item damageBuff;
    damageBuff.type = ItemType::Buff;
    damageBuff.itemId = 277;
    damageBuff.itemDesc = "Damage Buff";
    damageBuff.itemPower = 5.0f;

    sf::CircleShape damageBuffRef(20.0f);
    damageBuffRef.setFillColor(sf::Color::Yellow);
    damageBuffRef.setOrigin({ 15.f, 15.f });
    std::random_device rdB;
    std::mt19937 genB(rdB());
    float RandomXB = dist(gen);
    float RandomYB = dist(gen);
    damageBuffRef.setPosition({ RandomXB , RandomYB });

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
    helpText.setString("COMMANDES:\n - walk [x] [y]\n - wait [secondes]\n - use [itemId]");

    // PlayerHealth
	sf::Text playerHealthDisplay(font, "", 16);
	playerHealthDisplay.setPosition({ 10.f, 100.f });
	playerHealthDisplay.setFillColor(sf::Color(0, 255, 0));
    std::ostringstream ssHP;
    ssHP << std::fixed << std::setprecision(0) << playerChar.HP; // float to int
    playerHealthDisplay.setString("HP: " + ssHP.str());

    // File d'attente (HUD)
    sf::Text queueDisplay(font, "FILE:", 18);
    queueDisplay.setPosition({ 550.f, 20.f });
    queueDisplay.setFillColor(sf::Color::Yellow);

    // Inventaire
    std::string inventoryText = "Inventaire: \n";
	sf::Text inventoryShow(font, "", 16);
    inventoryShow.setPosition({ 15.f, 15.f });
    inventoryShow.setFillColor(sf::Color(150, 150, 150));
    inventoryShow.setString(inventoryText);
    int idx = 1;

    //REMOVE AFTER GET IS DONE
    inv.add(potion);
	inv.add(fireball);
	inv.add(damageBuff);

    for (const auto& a : inv.getAllItems()) {
        inventoryText += std::to_string(idx++) + ". " + " itemID: " + std::to_string(a.itemId) + " Name: " + a.itemDesc + "\n";
    }

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
            else if (currentAction.type == ActionType::Get)
            {
                //TODO: Check for collision with an item
                //inv.add(itemHit);
                //Destroy item ref

                //Destroy temporaire, si une meilleure manière est trouvée, ceci va être supprimé
                potionRef.setPosition({9999.0f, 9999.0f});
            }
            else if (currentAction.type == ActionType::Use) 
            {
                //TODO: Ajouter switch case qui check si item utilisé est heal, damage ou buff
				playerChar.heal(potion.itemPower);
                //Use Damage Item
                //Use Damage Buff
            }
            // TODO: Ajouter les else if pour Attack ici
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
		inventoryShow.setString(inventoryText);

        window.clear(sf::Color::Black);
        window.draw(player);
		window.draw(enemy1);
        window.draw(enemy2);
		window.draw(enemy3);
		window.draw(potionRef);
        window.draw(fireballRef);
        window.draw(damageBuffRef);
        window.draw(inputText);
        window.draw(helpText);
		window.draw(playerHealthDisplay);
        window.draw(queueDisplay);
		window.draw(inventoryShow);
        window.display();
    }
    return 0;
}