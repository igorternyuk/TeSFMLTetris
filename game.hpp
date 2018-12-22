#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>

class Game
{
public:
    explicit Game();
    void run();

private:
    enum
    {
        SCREEN_WIDTH = 640,
        SCREEN_HEIGHT = 480,
        FPS = 30
    };
    const std::string TITLE_OF_MAIN_WINDOW { "Tetris" };
    const sf::Time mFrameTime { sf::seconds(1.0f /FPS) };
    sf::RenderWindow mWindow;

    void inputPhase();
    void updatePhase(sf::Time frameTime);
    void renderPhase();
    void centralizeWindow();
};

#endif // GAME_HPP
