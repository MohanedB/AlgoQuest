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
	else if (cmd == "attack")
	{
		outAction = CreateAttack();
		return true;
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
	Character player;
	player.Initialize(30.f, 100.f, 300.f, sf::Color::Cyan, 100.0f, 10.0f);
	float speed = 200.f;

	// 2. SPAWN ENEMIES

	// Ennemy 1
	std::random_device rdE1;
	std::mt19937 genE1(rdE1());
	std::uniform_real_distribution<float> distE1(100.0f, 500.0f);
	float RandomXE1 = distE1(genE1);
	float RandomYE1 = distE1(genE1);
	Character enemy1;
	enemy1.Initialize(30.f, RandomXE1, RandomYE1, sf::Color::Red, 25.0f, 5.0f);

	// Ennemy 2
	std::random_device rdE2;
	std::mt19937 genE2(rdE2());
	std::uniform_real_distribution<float> distE2(100.0f, 500.0f);
	float RandomXE2 = distE2(genE2);
	float RandomYE2 = distE2(genE2);
	Character enemy2;
	enemy2.Initialize(30.f, RandomXE2, RandomYE2, sf::Color::Red, 35.0f, 7.0f);
	
	// Ennemy 3
	std::random_device rdE3;
	std::mt19937 genE3(rdE3());
	std::uniform_real_distribution<float> distE3(100.0f, 500.0f);
	float RandomXE3 = distE3(genE3);
	float RandomYE3 = distE3(genE3);
	Character enemy3;
	enemy3.Initialize(50.f, RandomXE3, RandomYE3, sf::Color::Red, 50.0f, 15.0f);
	

	int numberOfEnemies = 3;
	Character* enemies[3] = { &enemy1, &enemy2, &enemy3 };

	// 3. SPAWN NPC
	Character npc;
	std::random_device rdNPC;
	std::mt19937 genNPC(rdNPC());
	std::uniform_real_distribution<float> distNPC(100.0f, 500.0f);
	float RandomXNPC = distNPC(genNPC);
	float RandomYNPC = distNPC(genNPC);
	npc.Initialize(30.f, RandomXNPC, RandomYNPC, sf::Color::Blue, 999.0f, 0.0f);

	Character* talkables[4] = { &npc, &enemy1, &enemy2, &enemy3 };

	// 4. SETUP ITEMS AND IVENTORY
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

	// 5. SETUP INTERFACE

	// Zone de texte
	sf::Text inputText(font, "> ", 20);
	inputText.setPosition({ 10.f, 560.f });

	// Cheat sheet
	sf::Text helpText(font, "", 16);
	helpText.setPosition({ 10.f, 450.f });
	helpText.setFillColor(sf::Color(150, 150, 150));
	helpText.setString("COMMANDES:\n - walk [x] [y]\n - wait [secondes]\n - use [itemId]\n - attack (if near enemy)\n - talk (if near npc/enemy)");

	// Player and Enemies Health Display
	player.InitializeHealthDisplay(font, 16, 10.f, 100.f, sf::Color(0, 255, 0));
	enemy1.InitializeHealthDisplay(font, 16, enemy1.GetPosition().x - 25.f, enemy1.GetPosition().y + 15.f, sf::Color(255, 0, 0));
	enemy2.InitializeHealthDisplay(font, 16, enemy2.GetPosition().x - 25.f, enemy2.GetPosition().y + 15.f, sf::Color(255, 0, 0));
	enemy3.InitializeHealthDisplay(font, 16, enemy3.GetPosition().x - 25.f, enemy3.GetPosition().y + 25.f, sf::Color(255, 0, 0));

	//NPC and Enemies Dialogue Display
	enemy1.InitializeDialogueDisplay("I'm gonna beat you up", font, 16, enemy1.GetPosition().x - 70.f, enemy1.GetPosition().y - 35.f, sf::Color(255, 255, 255));
	enemy2.InitializeDialogueDisplay("Fight me coward!", font, 16, enemy2.GetPosition().x - 55.f, enemy2.GetPosition().y - 35.f, sf::Color(255, 255, 255));
	enemy3.InitializeDialogueDisplay("You don't stand a chance punk", font, 16, enemy3.GetPosition().x - 90.f, enemy3.GetPosition().y - 45.f, sf::Color(255, 255, 255));
	npc.InitializeDialogueDisplay("Hello traveler!", font, 16, npc.GetPosition().x - 50.f, npc.GetPosition().y - 35.f, sf::Color(255, 255, 255));

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

	//TODO REMOVE AFTER GET IS DONE
	inv.add(potion);
	inv.add(fireball);
	inv.add(damageBuff);

	for (const auto& a : inv.getAllItems()) {
		inventoryText += std::to_string(idx++) + ". " + " itemID: " + std::to_string(a.itemId) + " Name: " + a.itemDesc + "\n";
	}

	// 6. MAIN LOOP
	ActionQueue myQueue;
	Action currentAction;
	bool isBusy = false; // State: est-ce qu'on fait une action ou pas
	std::string userBuffer;
	sf::Clock clock;

	while (window.isOpen()) {
		float deltaTime = clock.restart().asSeconds();

		// 6.1 - INPUTS
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

		// 6.2 - MOTEUR (UPDATE)

		// Si libre, on pop la prochaine action
		if (!isBusy && !myQueue.isEmpty()) {
			currentAction = myQueue.current();
			myQueue.pop();
			isBusy = true;
		}

		// Action Executions
		if (isBusy) {
			// Move action
			if (currentAction.type == ActionType::Move) {
				// Logique de deplacement vectoriel
				sf::Vector2f dir = currentAction.targetPos - player.GetPosition();
				float dist = getLength(dir);

				if (dist < 5.f) {
					player.GetShape().setPosition(currentAction.targetPos); // Arrive
					isBusy = false;
				}
				else {
					player.GetShape().move((dir / dist) * speed * deltaTime);
				}
			}

			// Wait action
			else if (currentAction.type == ActionType::Wait) {
				currentAction.duration -= deltaTime;
				if (currentAction.duration <= 0) isBusy = false;
			}

			//Get action
			else if (currentAction.type == ActionType::Get)
			{
				//TODO: Check for collision with an item
				//inv.add(itemHit);
				//Destroy item ref

				//Destroy temporaire, si une meilleure manière est trouvée, ceci va être supprimé
				potionRef.setPosition({ 9999.0f, 9999.0f });
			}

			//Use action
			else if (currentAction.type == ActionType::Use)
			{
				//TODO: Ajouter switch case qui check si item utilisé est heal, damage ou buff
				player.heal(potion.itemPower);
				//Use Damage Item
				//Use Damage Buff
			}

			//Attack action
			else if (currentAction.type == ActionType::Attack)
			{
				static Character* targetEnemy = nullptr;
				static bool inCombat = false;
				if (!inCombat)
				{
					// choose first enemy in range
					for (size_t i = 0; i < numberOfEnemies; i++)
					{
						if (enemies[i]->IsDead() || enemies[i] == nullptr)
							continue;

						float distToEnemy = sf::Vector2f(player.GetPosition() - enemies[i]->GetPosition()).length();
						if (distToEnemy <= 50.0f)
						{
							targetEnemy = enemies[i];
							inCombat = true;
							std::cout << "Combat Starts!\n";
							break;
						}
					}
				}

				if (targetEnemy == nullptr)  // no enemy was chosen, cancel attack
				{
					std::cout << "No enemy in range to attack.\n";
					inCombat = false;
					isBusy = false;
				}

				if (inCombat)
				{
					static int currentAttacker = 0; // 0 = player, 1 = enemy
					static float attackCooldown = 1.0f; // 1 sec between attacks
					switch (currentAttacker)
					{
					case 0:
						// wait 1sec
						attackCooldown -= deltaTime;
						if (attackCooldown <= 0)
						{
							attackCooldown = 1.0f;
							currentAttacker = 1;

							// attack
							player.Attack(*targetEnemy);
							targetEnemy->UpdateHealthDisplay();
							std::cout << "Attacking enemy! (Enemy HP remaining: " << targetEnemy->HP << ")\n";

							// check if dead
							if (targetEnemy->IsDead())
							{
								inCombat = false;
								std::cout << "Enemy Defeated!!!";
								// reset for next combat
								currentAttacker = 0;
								targetEnemy = nullptr;

								// TODO Fred : spawn item

								/**
								*****************
								*****************
								*****************
								*****************
								*****************
								*****************
								**/

								isBusy = false;
								break;
							}
						}
						break;
					case 1:
						// wait 1sec
						attackCooldown -= deltaTime;
						if (attackCooldown <= 0)
						{
							attackCooldown = 1.0f;
							currentAttacker = 0;

							// Get attacked
							targetEnemy->Attack(player);

							// Update HP HUD
							player.UpdateHealthDisplay();
							std::cout << "Enemy is attacking! (Player HP remaining: " << player.HP << ")\n";

							// check if dead
							if (player.IsDead())
							{
								inCombat = false;
								std::cout << "GAME OVER";
								// reset for next combat
								currentAttacker = 0;
								targetEnemy = nullptr;

								// TODO GAME OVER

								isBusy = false;
								break;
							}
						}
						break;
					default:
						break;
					}
				}
			}

			//Talk action
			else if (currentAction.type == ActionType::Talk)
			{

			}
		}

		// 6.3 - UI DRAWING
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
		window.draw(potionRef);
		window.draw(fireballRef);
		window.draw(damageBuffRef);

		window.draw(npc.GetShape());
		if (!enemy1.IsDead())
		{
			window.draw(enemy1.GetShape());
			window.draw(enemy1.healthDisplayText);
		}
		if (!enemy2.IsDead())
		{
			window.draw(enemy2.GetShape());
			window.draw(enemy2.healthDisplayText);
		}
		if (!enemy3.IsDead())
		{
			window.draw(enemy3.GetShape());
			window.draw(enemy3.healthDisplayText);
		}
		if (!player.IsDead())
		{
			window.draw(player.GetShape());
			window.draw(player.healthDisplayText);
		}
		window.draw(npc.dialogueDisplayText);
		window.draw(enemy1.dialogueDisplayText);
		window.draw(enemy2.dialogueDisplayText);
		window.draw(enemy3.dialogueDisplayText);

		window.draw(inputText);
		window.draw(helpText);
		window.draw(queueDisplay);
		window.draw(inventoryShow);
		window.display();
	}
	return 0;
}