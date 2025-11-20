#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#include "Action.hpp"

// Convertir un ActionType en texte pour debug
std::string ToString(ActionType type)
{
    switch (type)
    {
    case ActionType::Move:      return "Move";
    case ActionType::Attack:    return "Attack";
    case ActionType::GetItem:   return "GetItem";
    case ActionType::UseItem:   return "UseItem";
    case ActionType::Interact:  return "Interact";
    case ActionType::Wait:      return "Wait";
    default:                    return "Unknown";
    }
}

// Parse une commande texte et crée une Action correspondante.
// Retourne true si la commande est valide, false sinon.
bool ParseCommand(const std::string& line, Action& outAction)
{
    std::istringstream iss(line);
    std::string cmd;
    if (!(iss >> cmd))
        return false;

    if (cmd == "walk" || cmd == "move")
    {
        float x = 0.f, y = 0.f;
        if (!(iss >> x >> y))
            return false;

        outAction = MakeMoveAction({x, y});
        return true;
    }
    else if (cmd == "attack")
    {
        int enemyId = -1;
        if (!(iss >> enemyId))
            return false;

        outAction = MakeAttackAction(enemyId);
        return true;
    }
    else if (cmd == "wait")
    {
        float seconds = 0.f;
        if (!(iss >> seconds))
            return false;

        outAction = MakeWaitAction(seconds);
        return true;
    }

    return false; // commande inconnue
}

// Pour l'instant : juste debug console.
// Plus tard tu pourras ajouter un player, ennemis, etc.
void ExecuteAction(const Action& action)
{
    std::cout << "[>>] Execution de l'action: "
              << action.description
              << " (type=" << ToString(action.type) << ")\n";
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({800u, 600u}), "File d'actions - Commandes en jeu");
    window.setFramerateLimit(60);

    ActionQueue actionQueue;

    // Chargement de la police (API SFML 3 : openFromFile)
    sf::Font font;
    try
    {
        font.openFromFile("arial.ttf");
    }
    catch (const sf::Exception& e)
    {
        std::cout << "[WARN] Impossible d'ouvrir la police: " << e.what() << "\n";
    }

    // sf::Text n'a PLUS de constructeur par défaut en SFML 3,
    // on doit lui donner une font directement
    sf::Text inputText(font, "", 20); // string vide, taille 20
    inputText.setFillColor(sf::Color::White);
    inputText.setPosition(sf::Vector2f{10.f, 570.f}); // SFML 3: Vector2f

    std::string commandBuffer;

    std::cout << "=== Commandes en jeu ===\n";
    std::cout << "Tape dans la fenetre SFML (en bas):\n";
    std::cout << "  walk X Y   -> Deplacement vers (X, Y)\n";
    std::cout << "  attack N   -> Attaque ennemi #N\n";
    std::cout << "  wait T     -> Attente de T secondes\n";
    std::cout << "ENTER pour valider la commande, LeftControl pour executer la prochaine action.\n\n";

    while (window.isOpen())
    {
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            // Fermeture
            if (event->is<sf::Event::Closed>())
            {
                window.close();
                continue;
            }

            // Texte tapé (caractères normaux)
            if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
            {
                char32_t unicode = textEntered->unicode;

                // On ignore les caractères de contrôle
                if (unicode >= 32 && unicode < 127)
                {
                    commandBuffer.push_back(static_cast<char>(unicode));
                }
            }

            // Touches spéciales (Backspace, Enter, Space)
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                sf::Keyboard::Key key = keyPressed->code;

                // Backspace = effacer un caractère
                if (key == sf::Keyboard::Key::Backspace)
                {
                    if (!commandBuffer.empty())
                        commandBuffer.pop_back();
                }
                // Enter = valider la commande
                else if (key == sf::Keyboard::Key::Enter)
                {
                    if (!commandBuffer.empty())
                    {
                        Action a;
                        if (ParseCommand(commandBuffer, a))
                        {
                            actionQueue.enqueue(a);
                            std::cout << "[+ ] Ajout depuis commande: "
                                      << a.description
                                      << " (type=" << ToString(a.type) << ")\n";
                        }
                        else
                        {
                            std::cout << "[!!] Commande invalide: '"
                                      << commandBuffer
                                      << "'\n";
                        }

                        commandBuffer.clear();
                    }
                }
                // LeftControl = executer la prochaine action de la file
                else if (key == sf::Keyboard::Key::LControl)
                {
                    if (!actionQueue.isEmpty())
                    {
                        Action& current = actionQueue.front();
                        ExecuteAction(current);
                        actionQueue.pop();
                    }
                    else
                    {
                        std::cout << "[!!] File vide, aucune action a executer.\n";
                    }
                }
            }
        }

        // Mise à jour du texte affiché
        inputText.setString("> " + commandBuffer);

        window.clear(sf::Color::Black);
        window.draw(inputText);
        window.display();
    }

    return 0;
}
