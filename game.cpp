#include "game.hpp"

Game::Game():
    mWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT),
            TITLE_OF_MAIN_WINDOW, sf::Style::Close)
{

}

void Game::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while (mWindow.isOpen())
    {
        auto elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        while (timeSinceLastUpdate > mFrameTime) {
            timeSinceLastUpdate -= mFrameTime;
            inputPhase();
            updatePhase(mFrameTime);
        }
        renderPhase();
    }
}

void Game::inputPhase()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
            mWindow.close();
    }
}

void Game::updatePhase(sf::Time frameTime)
{

}

void Game::renderPhase()
{
    mWindow.clear();
    mWindow.display();
}
