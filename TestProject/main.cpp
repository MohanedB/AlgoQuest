#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <cmath>
#include <optional>
#include <random>
#include <iomanip> // Pour setprecision
#include "Action.h"
#include "Inventory.h"
#include "Character.h"

// Je parse juste mes commandes, les autres feront les leurs
bool ParseCommand(const std::string& line, Action& outAction) {
	std::stringstream ss(line);
	std::string cmd;
	ss >> cmd;

	// Note: Je mets des {} vides pour clean les warnings d'initialisation
	outAction = {};

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
	else if (cmd == "talk")
	{
		outAction = CreateTalk();
		return true;
	}
	else if (cmd == "use")
	{
		int id;
		if (ss >> id)
		{
			outAction = CreateUse(id);
			return true;
		}
	}
	else if (cmd == "get")
	{
		outAction = CreateGet();
		return true;
	}

	return false;
}

// Calcul de distance basic
float getLength(sf::Vector2f v) { return std::sqrt(v.x * v.x + v.y * v.y); }

int main() {
	bool isGameOver = false;

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
	enemy1.Initialize(30.f, RandomXE1, RandomYE1, sf::Color::Red, 25.0f, 7.0f);

	// Ennemy 2
	std::random_device rdE2;
	std::mt19937 genE2(rdE2());
	std::uniform_real_distribution<float> distE2(100.0f, 500.0f);
	float RandomXE2 = distE2(genE2);
	float RandomYE2 = distE2(genE2);
	Character enemy2;
	enemy2.Initialize(30.f, RandomXE2, RandomYE2, sf::Color::Red, 35.0f, 10.0f);

	// Ennemy 3
	std::random_device rdE3;
	std::mt19937 genE3(rdE3());
	std::uniform_real_distribution<float> distE3(100.0f, 500.0f);
	float RandomXE3 = distE3(genE3);
	float RandomYE3 = distE3(genE3);
	Character enemy3;
	enemy3.Initialize(50.f, RandomXE3, RandomYE3, sf::Color::Red, 50.0f, 15.0f);

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

	// Utilisation de {} pour initialiser à zéro et éviter les warnings 'uninitialized'
	Item potion{};
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

	Item damageBuff{}; // Hop, zero-init
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

	sf::Shape* items[2] = { &potionRef, &damageBuffRef };

	sf::Font font;
	// SFML 3: openFromFile obligatoire
	if (!font.openFromFile("arial.ttf")) return -1;

	// 5. SETUP INTERFACE

	// Zone de texte
	sf::Text inputText(font, "> ", 20);
	inputText.setPosition({ 10.f, 560.f });

	// Cheat sheet
	sf::Text helpText(font, "", 16);
	helpText.setPosition({ 10.f, 430.f });
	helpText.setFillColor(sf::Color(150, 150, 150));
	helpText.setString("COMMANDES:\n - walk [x] [y]\n - wait [secondes]\n - use [itemId]\n - get (if near item)\n - attack (if near enemy)\n - talk (if near npc/enemy)");

	// Game over text
	sf::Text gameOverText(font, "", 50);
	gameOverText.setPosition({ 100.f, 200.f });
	gameOverText.setFillColor(sf::Color::Red);
	gameOverText.setString("          GAME OVER\nClosing game in " + std::to_string(5) + " seconds");
	gameOverText.setOutlineColor(sf::Color::Magenta);
	gameOverText.setOutlineThickness(0.5f);

	// On recycle ce text object pour afficher les coords des items
	sf::Text itemPosText(font, "", 14);
	itemPosText.setFillColor(sf::Color::White);
	itemPosText.setOutlineColor(sf::Color::Black);
	itemPosText.setOutlineThickness(1.0f);

	// Player and Enemies Health Display
	// InitializeHealthDisplay setup la font pour tout le perso (HP + Position)
	player.InitializeHealthDisplay(font, 16, 10.f, 100.f, sf::Color(0, 255, 0));
	enemy1.InitializeHealthDisplay(font, 16, enemy1.GetPosition().x - 25.f, enemy1.GetPosition().y + 15.f, sf::Color(255, 0, 0));
	enemy2.InitializeHealthDisplay(font, 16, enemy2.GetPosition().x - 25.f, enemy2.GetPosition().y + 15.f, sf::Color(255, 0, 0));
	enemy3.InitializeHealthDisplay(font, 16, enemy3.GetPosition().x - 25.f, enemy3.GetPosition().y + 25.f, sf::Color(255, 0, 0));
	
	// Player Damage Display (sous l'inventaire)
	sf::Text damageDisplay(font, "", 16);
	damageDisplay.setPosition({ 10.f, 120.f }); // Position arbitraire a gauche
	damageDisplay.setFillColor(sf::Color::Magenta);

	// NPC and Enemies Dialogue Display
	enemy1.InitializeDialogueDisplay("I'm gonna beat you up", font, 16, enemy1.GetPosition().x - 70.f, enemy1.GetPosition().y - 35.f, sf::Color(255, 255, 255));
	enemy2.InitializeDialogueDisplay("Fight me coward!", font, 16, enemy2.GetPosition().x - 55.f, enemy2.GetPosition().y - 35.f, sf::Color(255, 255, 255));
	enemy3.InitializeDialogueDisplay("You don't stand a chance punk", font, 16, enemy3.GetPosition().x - 90.f, enemy3.GetPosition().y - 45.f, sf::Color(255, 255, 255));
	npc.InitializeDialogueDisplay("Hello traveler!", font, 16, npc.GetPosition().x - 50.f, npc.GetPosition().y - 35.f, sf::Color(255, 255, 255));
	
	// NEW: On init le display du NPC pour qu'il ait la font, meme si on s'en fout de ses HP
	npc.InitializeHealthDisplay(font, 14, npc.GetPosition().x - 25.f, npc.GetPosition().y + 15.f, sf::Color::White);

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

	for (const auto& a : inv.getAllItems()) {
		inventoryText += std::to_string(idx++) + ". " + " itemID: " + std::to_string(a.itemId) + " Name: " + a.itemDesc + " Item Power:" + std::to_string(a.itemPower) + "\n";
	}

	// 6. MAIN LOOP
	ActionQueue myQueue;
	Action currentAction{}; // init à zero
	bool isBusy = false; // State: est-ce qu'on fait une action ou pas
	std::string userBuffer;
	sf::Clock clock;

	while (window.isOpen()) {
		float deltaTime = clock.restart().asSeconds();

		if (!isGameOver)
		{
			// 6.1 - INPUTS (SFML 3 style)
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
						Action act{}; // init ici aussi
						if (ParseCommand(userBuffer, act)) {
							myQueue.add(act);
							std::cout << "Added: " << act.description << "\n";
						}
						userBuffer.clear();
					}
				}
			}

			// 6.2 - MOTEUR (UPDATE)

			// Update Damage HUD (en temps reel pour voir les buffs)
			damageDisplay.setString("Player Damage: " + std::to_string((int)player.damage));

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
					static sf::Shape* grabbedItem = nullptr;

					for (size_t i = 0; i < 2; i++)
					{
						if (items[i] == nullptr)
						{
							std::cout << "Empty List";
							continue;
						}

						// SFML 3: Vector2f a la fonction .length()
						float distToItem = sf::Vector2f(player.GetPosition() - items[i]->getPosition()).length();

						if (distToItem <= 50.0f)
						{
							std::cout << "Item found";
							grabbedItem = items[i];
							grabbedItem->getFillColor() == sf::Color::Green ? inv.add(potion) : inv.add(damageBuff);

							// On cache l'item loin
							grabbedItem->setPosition({ 9999.0f, 9999.0f });
						}
						else
						{
							std::cout << "Item not in range \n";
						}
					}

					std::string inventoryText = "Inventaire: \n";
					int idx = 1;

					for (const auto& item : inv.getAllItems())
					{
						inventoryText += std::to_string(idx++) + ". "
							+ "itemID: " + std::to_string(item.itemId)
							+ " Name: " + item.itemDesc
							+ " Item Power: " + std::to_string(item.itemPower)
							+ "\n";
					}

					inventoryShow.setString(inventoryText);
					isBusy = false;
				}

				//Use action
				else if (currentAction.type == ActionType::Use)
				{
					Item usedItem = inv.getItemById(currentAction.itemId);

					if (currentAction.itemId != -1 && usedItem.itemDesc != "Invalid")
					{
						switch (usedItem.type)
						{
						case ItemType::Health:
							player.heal(usedItem.itemPower);
							player.UpdateHealthDisplay();
							inv.removeItemById(usedItem.itemId);
							std::cout << "Used item: " << usedItem.itemDesc << "\n";
							break;

						case ItemType::Buff:
							player.damage += usedItem.itemPower;
							inv.removeItemById(usedItem.itemId);
							std::cout << "Used item: " << usedItem.itemDesc << "\n";
							break;

						default:
							break;
						}
					}

					else
					{
						std::cout << "You just used this item, no cheating!";
					}

					std::string inventoryText = "Inventaire: \n";
					int idx = 1;

					for (const auto& item : inv.getAllItems())
					{
						inventoryText += std::to_string(idx++) + ". "
							+ "itemID: " + std::to_string(item.itemId)
							+ " Name: " + item.itemDesc
							+ " Item Power: " + std::to_string(item.itemPower)
							+ "\n";
					}

					inventoryShow.setString(inventoryText);

					isBusy = false;
				}

				//Attack action
				else if (currentAction.type == ActionType::Attack)
				{
					static Character* targetEnemy = nullptr;
					static bool inCombat = false;
					if (!inCombat)
					{
						// choose first enemy in range
						for (size_t i = 0; i < 3; i++)
						{
							if (enemies[i]->IsDead() || enemies[i] == nullptr)
								continue;

							// SFML 3 .length() encore
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

									Item droppedItem{}; // Init
									std::random_device rdID;
									std::mt19937 genID(rdID());
									std::uniform_int_distribution<int> distID(1, 100);
									droppedItem.itemId = distID(genID);

									std::random_device rdType;
									std::mt19937 genType(rdType());
									std::uniform_int_distribution<int> distType(1, 2);
									int RandomItemType = distType(genType);

									switch (RandomItemType)
									{
									case(1):
										droppedItem.type = ItemType::Health;
										droppedItem.itemDesc = "Dropped Health Item";
										break;

									case(2):
										droppedItem.type = ItemType::Buff;
										droppedItem.itemDesc = "Dropped Buff";
										break;
									}

									droppedItem.itemPower = 10.0f;
									inv.add(droppedItem);
									std::string inventoryText = "Inventaire: \n";
									int idx = 1;

									for (const auto& item : inv.getAllItems())
									{
										inventoryText += std::to_string(idx++) + ". "
											+ "itemID: " + std::to_string(item.itemId)
											+ " Name: " + item.itemDesc
											+ " Item Power: " + std::to_string(item.itemPower)
											+ "\n";
									}

									inventoryShow.setString(inventoryText);
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
									// reset
									currentAttacker = 0;
									targetEnemy = nullptr;

									//Start game over screen
									isGameOver = true;

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
					static Character* talkTarget = nullptr;
					static bool inConversation = false;

					if (!inConversation)
					{
						for (size_t i = 0; i < 4; i++)
						{
							if (talkables[i]->IsDead() || talkables[i] == nullptr)
								continue;

							float distToTalkable = sf::Vector2f(player.GetPosition() - talkables[i]->GetPosition()).length();
							if (distToTalkable <= 50.0f)
							{
								talkTarget = talkables[i];
								inConversation = true;
								talkTarget->isTalking = true;
								std::cout << "They say: " << talkTarget->dialogueDisplayText.getString().toAnsiString() << "\n";
								break;
							}
						}
					}

					if (talkTarget == nullptr)
					{
						std::cout << "There's no one to talk to nearby.\n";
						isBusy = false;
					}

					if (inConversation)
					{
						static float talkDuration = 5.0f; // 5 seconds talking 
						talkDuration -= deltaTime;
						if (talkDuration <= 0)
						{
							talkTarget->isTalking = false;
							talkDuration = 2.0f;
							inConversation = false;
							talkTarget = nullptr;
							isBusy = false;
						}
					}
				}
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

		window.clear(sf::Color::Black);

		// On dessine les items si ils ne sont pas "picked up" (loin en x)
		for (sf::Shape* item : items)
		{
			if (item->getPosition().x < 2000.f)
			{
				window.draw(*item);

				// Prepare Coordinate Text
				std::stringstream ssPos;
				ssPos << "(" << (int)item->getPosition().x << ", " << (int)item->getPosition().y << ")";

				itemPosText.setString(ssPos.str());

				// FIX SFML 3: setPosition({x, y}) obligatoire maintenant
				itemPosText.setPosition({ item->getPosition().x - 20.f, item->getPosition().y + 20.f });

				window.draw(itemPosText);
			}
		}

		// DRAW NPC + Position
		window.draw(npc.GetShape());
		window.draw(npc.positionDisplayText);
		if (npc.isTalking) window.draw(npc.dialogueDisplayText);

		if (!enemy1.IsDead())
		{
			window.draw(enemy1.GetShape());
			window.draw(enemy1.healthDisplayText);
			window.draw(enemy1.positionDisplayText);
			if (enemy1.isTalking) window.draw(enemy1.dialogueDisplayText);
		}
		if (!enemy2.IsDead())
		{
			window.draw(enemy2.GetShape());
			window.draw(enemy2.healthDisplayText);
			window.draw(enemy2.positionDisplayText);
			if (enemy2.isTalking) window.draw(enemy2.dialogueDisplayText);
		}
		if (!enemy3.IsDead())
		{
			window.draw(enemy3.GetShape());
			window.draw(enemy3.healthDisplayText);
			window.draw(enemy3.positionDisplayText);
			if (enemy3.isTalking) window.draw(enemy3.dialogueDisplayText);
		}
		if (!player.IsDead())
		{
			window.draw(player.GetShape());
			window.draw(player.healthDisplayText);
		}

		window.draw(inputText);
		window.draw(helpText);
		window.draw(queueDisplay);
		window.draw(inventoryShow);
		window.draw(damageDisplay);

		if (isGameOver)
		{
			static float gameOverTimer = 5.0f; // 5 seconds timer before closing game
			gameOverTimer -= deltaTime;
			if (gameOverTimer <= 0)
			{
				window.close();
			}

			std::ostringstream ssGameOver;
			ssGameOver.str("");
			ssGameOver.clear();
			ssGameOver << std::fixed << std::setprecision(0) << gameOverTimer; 
			gameOverText.setString("          GAME OVER\nClosing game in " + ssGameOver.str() + " seconds");
			window.draw(gameOverText);
		}

		window.display();
	}
	return 0;
}