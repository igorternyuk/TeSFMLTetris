#include "game.hpp"
#include <ctime>
#include <iostream>

Game::Game():
    mWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT),
            TITLE_OF_MAIN_WINDOW, sf::Style::Close)
{
    createAssets();
    initField();
    srand(time(nullptr));
    spawnNewPiece();
    mWindow.setFramerateLimit(FPS);
    centralizeWindow();
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
        if(event.type == sf::Event::KeyReleased)
        {
            if(event.key.code == sf::Keyboard::N)
            {
                spawnNewPiece();
            }
        }
    }
}

void Game::updatePhase(sf::Time frameTime)
{


}

void Game::renderPhase()
{
    mWindow.clear(mColors[EMPTY]);
    drawField();
    drawCurrentPiece();
    mWindow.display();
}

void Game::drawField()
{
    for(int y = 0; y < FIELD_HEIGHT; ++y)
    {
        for(int x = 0; x < FIELD_WIDTH; ++x)
        {
            sf::RectangleShape block(sf::Vector2f(BLOCK_SIZE,BLOCK_SIZE));
            block.setPosition(x * BLOCK_SIZE, y * BLOCK_SIZE);
            block.setFillColor(mColors[mField[y * FIELD_WIDTH + x]]);
            bool isBound = (x == 0 || x == FIELD_WIDTH - 1) || (y == FIELD_HEIGHT - 1);
            if(!isBound)
            {
                block.setOutlineColor(sf::Color::Green);
                block.setOutlineThickness(1);
            }
            mWindow.draw(block);
        }
    }

}

void Game::drawCurrentPiece()
{
    for(int py = 0; py < BLOCKS_IN_PIECE; ++py)
    {
        for(int px = 0; px < BLOCKS_IN_PIECE; ++px)
        {
            int pi = rotate(px, py, mCurrentPiece.rotation);
            if(mShapes[mCurrentPiece.type][pi] == 'X')
            {
                int fx = mCurrentPiece.x + px;
                int fy = mCurrentPiece.y + py;
                sf::RectangleShape piece(sf::Vector2f(BLOCK_SIZE,BLOCK_SIZE));

                piece.setPosition(fx * BLOCK_SIZE, fy * BLOCK_SIZE);
                piece.setFillColor(mColors[mCurrentPiece.type]);
                piece.setOutlineColor(sf::Color(70,70,70));
                piece.setOutlineThickness(1);
                mWindow.draw(piece);
            }
        }
    }
}

bool Game::fit(const Game::Tetramino &tetramino, int destX, int destY, int rotation)
{
    for(int py = 0; py < BLOCKS_IN_PIECE; ++py)
    {
        for(int px = 0; px < BLOCKS_IN_PIECE; ++px)
        {
            int pi = rotate(px, py, rotation);
            int fieldX = destX + px;
            int fieldY = destY + py;
            int fieldIndex = fieldY * FIELD_WIDTH + fieldX;
            if(isValidCoordinates(fieldX, fieldY))
            {
                if(mField[fieldIndex] != EMPTY && mShapes[tetramino.type][pi] == 'X')
                {
                    return false;
                }
            }
        }
    }
    return true;
}

int Game::rotate(int blockX, int blockY, int angle) const
{
    switch (angle % 4) {
        default: return blockY * 4 + blockX;      // 0 degrees;
        case 1: return 12 + blockY - 4 * blockX; // 90 degrees;
        case 2: return 15 - blockY * 4 - blockX; // 180 degrees;
        case 3: return 3 - blockY + 4 * blockX;  // 270 degrees;
    }
}

void Game::initField()
{
    for(int y = 0; y < FIELD_HEIGHT; ++y)
    {
        for(int x = 0; x < FIELD_WIDTH; ++x)
        {
            int index = y * FIELD_WIDTH + x;
            bool isBound = (x == 0 || x == FIELD_WIDTH - 1) || (y == FIELD_HEIGHT - 1);
            mField[index] = isBound ? BOUNDARY_BLOCK : EMPTY;
        }
    }
}

void Game::spawnNewPiece()
{
    mCurrentPiece.x = FIELD_WIDTH / 2 - 2;
    mCurrentPiece.y = 0;
    mCurrentPiece.rotation = 0;
    mCurrentPiece.type = rand() % NUM_SHAPES + 1;
}

void Game::createAssets()
{
    mColors[EMPTY] = sf::Color(0x807962ff);
    mColors[BOUNDARY_BLOCK] = sf::Color(0xb7c2c6ff);
    // I-shape
    mShapes[SHAPE_I].append("..X.");
    mShapes[SHAPE_I].append("..X.");
    mShapes[SHAPE_I].append("..X.");
    mShapes[SHAPE_I].append("..X.");
    mColors[SHAPE_I] = sf::Color::Red;

    // L-shape
    mShapes[SHAPE_L].append(".X..");
    mShapes[SHAPE_L].append(".X..");
    mShapes[SHAPE_L].append(".XX.");
    mShapes[SHAPE_L].append("....");
    mColors[SHAPE_L] = sf::Color(0x81ae04ff);

    // J-shape
    mShapes[SHAPE_J].append("..X.");
    mShapes[SHAPE_J].append("..X.");
    mShapes[SHAPE_J].append(".XX.");
    mShapes[SHAPE_J].append("....");
    mColors[SHAPE_J] = sf::Color(0xf6546aff);

    // O-shape
    mShapes[SHAPE_O].append("....");
    mShapes[SHAPE_O].append(".XX.");
    mShapes[SHAPE_O].append(".XX.");
    mShapes[SHAPE_O].append("....");
    mColors[SHAPE_O] = sf::Color(0x4a6ca8ff);

    // S-shape
    mShapes[SHAPE_S].append("..X.");
    mShapes[SHAPE_S].append(".XX.");
    mShapes[SHAPE_S].append(".X..");
    mShapes[SHAPE_S].append("....");
    mColors[SHAPE_S] = sf::Color(0x420690ff);

    // Z-shape
    mShapes[SHAPE_Z].append(".X.");
    mShapes[SHAPE_Z].append(".XX.");
    mShapes[SHAPE_Z].append("..X.");
    mShapes[SHAPE_Z].append("....");
    mColors[SHAPE_Z] = sf::Color(0xffb90fff);

    // T-shape
    mShapes[SHAPE_T].append("..X.");
    mShapes[SHAPE_T].append(".XX.");
    mShapes[SHAPE_T].append("..X.");
    mShapes[SHAPE_T].append("....");
    mColors[SHAPE_T] = sf::Color(0xff0040ff);
}

bool Game::isValidCoordinates(int x, int y) const
{
    return x >= 0 && x < FIELD_WIDTH && y >= 0 && y < FIELD_HEIGHT;
}

void Game::centralizeWindow()
{
    const int screenWidth = sf::VideoMode::getDesktopMode().width;
    const int screenHeight = sf::VideoMode::getDesktopMode().height;
    mWindow.setPosition(sf::Vector2i((screenWidth - SCREEN_WIDTH) / 2,
                                     (screenHeight - SCREEN_HEIGHT) / 2));
}
