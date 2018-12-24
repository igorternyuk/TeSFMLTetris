#include "game.hpp"
#include <ctime>
#include <iostream>

Game::Game():
    mWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT),
            TITLE_OF_MAIN_WINDOW, sf::Style::Close)
{
    createAssets();
    if(!mFont.loadFromFile("res/fonts/BRLNSR.TTF"))
    {
        std::cout << "Failed to load font" << std::endl;
    }
    srand(time(nullptr));
    mWindow.setFramerateLimit(FPS);
    centralizeWindow();

    startNewGame();

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
                startNewGame();
            }
            else if(event.key.code == sf::Keyboard::P)
            {
                togglePause();
            }
            else if(event.key.code == sf::Keyboard::Up)
            {
                rotateCurrentPiece(Direction::LEFT);
            }
            else if(event.key.code == sf::Keyboard::Down)
            {
                rotateCurrentPiece(Direction::RIGHT);
            }
            else if(event.key.code == sf::Keyboard::Space)
            {
                dropCurrentPieceDown();
                ++mPiecesDropped;
            }
        }
    }
}

void Game::updatePhase(sf::Time frameTime)
{
    if(mGameState != State::PLAY)
        return;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        moveCurrentPiece(Direction::LEFT);
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        moveCurrentPiece(Direction::RIGHT);
    }

    if(mPiecesSpawned >= NUM_SPAWNED_PIECES_TO_NEXT_LEVEL * mLevel)
    {
        ++mLevel;
    }

    mTetraminoTimer += frameTime.asSeconds();
    const float currDelay = 1.0f - 0.5f * (mLevel - 1);

    if(mTetraminoTimer >= currDelay)
    {
        if(!stepDownCurrentPiece())
        {
            lockCurrentPiece();
            checkLines();
            spawnNewPiece();
            if(!fit(mCurrentPiece, mCurrentPiece.x, mCurrentPiece.y, mCurrentPiece.rotation))
            {
                mGameState = State::GAME_OVER;
            }
        }
        mTetraminoTimer = 0;
    }
}

void Game::moveCurrentPiece(Game::Direction dir)
{
    if(dir == Direction::LEFT)
    {
        if(fit(mCurrentPiece, mCurrentPiece.x - 1, mCurrentPiece.y, mCurrentPiece.rotation))
        {
            --mCurrentPiece.x;
        }
    }
    else if(dir == Direction::RIGHT)
    {
        if(fit(mCurrentPiece, mCurrentPiece.x + 1, mCurrentPiece.y, mCurrentPiece.rotation))
        {
            ++mCurrentPiece.x;
        }
    }
}

void Game::rotateCurrentPiece(Game::Direction dir)
{
    if(dir == Direction::LEFT)
    {
        if(fit(mCurrentPiece, mCurrentPiece.x, mCurrentPiece.y, mCurrentPiece.rotation + 3))
        {
            mCurrentPiece.rotation += 3;
        }
    }
    else if(dir == Direction::RIGHT)
    {
        if(fit(mCurrentPiece, mCurrentPiece.x, mCurrentPiece.y, mCurrentPiece.rotation + 1))
        {
            ++mCurrentPiece.rotation;
        }
    }
}

bool Game::stepDownCurrentPiece()
{
    if(fit(mCurrentPiece, mCurrentPiece.x, mCurrentPiece.y + 1, mCurrentPiece.rotation))
    {
        ++mCurrentPiece.y;
        return true;
    }
    return false;
}

void Game::lockCurrentPiece()
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
                mField[fy * FIELD_WIDTH + fx] = LOCKED_BLOCK;
            }
        }
    }
}

void Game::dropCurrentPieceDown()
{
    while(fit(mCurrentPiece, mCurrentPiece.x, mCurrentPiece.y + 1, mCurrentPiece.rotation))
    {
        ++mCurrentPiece.y;
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

void Game::checkLines()
{
    std::vector<int> filledLines;
    for(int py = 0; py < BLOCKS_IN_PIECE; ++py)
    {
        int fy = mCurrentPiece.y + py;
        if(fy >= FIELD_HEIGHT - 1) break;
        bool lineFilled = true;
        for(int fx = 1; fx < FIELD_WIDTH - 1; ++fx)
        {
            lineFilled &= (mField[fy * FIELD_WIDTH + fx] == LOCKED_BLOCK);
        }
        std::cout << std::endl;
        if(lineFilled)
        {
            filledLines.push_back(fy);
        }
    }

    if(!filledLines.empty())
    {
        mLinesRemoved += filledLines.size();
        mScore += SCORES[filledLines.size()];

        for(auto &line: filledLines)
        {
            for(int y = line; y > 0; --y)
            {
                for(int x = 1; x < FIELD_WIDTH - 1; ++x)
                {
                    mField[y * FIELD_WIDTH + x] = mField[(y - 1) * FIELD_WIDTH + x];
                    mField[(y - 1) * FIELD_WIDTH + x] = EMPTY;
                }
            }
        }
    }
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
    mCurrentPiece.type = mNextPiece.type;
    do {
        mNextPiece.type = rand() % NUM_SHAPES + 1;
    } while(mNextPiece.type == mCurrentPiece.type);
    ++mPiecesSpawned;
}

void Game::startNewGame()
{
    mLevel = 1;
    mScore = 0;
    mLinesRemoved = 0;
    mPiecesDropped = 0;
    mPiecesSpawned = 0;
    mNextPiece.x = FIELD_WIDTH + 1;
    mNextPiece.y = 2;
    mNextPiece.rotation = 0;
    mNextPiece.type = rand() % NUM_SHAPES + 1;
    initField();
    spawnNewPiece();
    mGameState = State::PLAY;
}

void Game::togglePause()
{
    if(mGameState == State::PLAY)
    {
        mGameState = State::PAUSE;
    }
    else if(mGameState == State::PAUSE)
    {
        mGameState = State::PLAY;
    }
}

void Game::renderPhase()
{
    mWindow.clear(mColors[EMPTY]);
    drawField();
    drawTetramino(mCurrentPiece);
    drawGameInfo();
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

void Game::drawTetramino(const Game::Tetramino &tetramino)
{
    for(int py = 0; py < BLOCKS_IN_PIECE; ++py)
    {
        for(int px = 0; px < BLOCKS_IN_PIECE; ++px)
        {
            int pi = rotate(px, py, tetramino.rotation);
            if(mShapes[tetramino.type][pi] == 'X')
            {
                int fx = tetramino.x + px;
                int fy = tetramino.y + py;
                sf::RectangleShape piece(sf::Vector2f(BLOCK_SIZE,BLOCK_SIZE));
                piece.setPosition(fx * BLOCK_SIZE, fy * BLOCK_SIZE);
                piece.setFillColor(mColors[tetramino.type]);
                piece.setOutlineColor(sf::Color(70,70,70));
                piece.setOutlineThickness(1);
                mWindow.draw(piece);
            }
        }
    }
}

void Game::drawGameInfo()
{
    sf::Text nextPieceLabel("NEXT", mFont, 24);
    nextPieceLabel.setFillColor(sf::Color(127, 0, 0));
    nextPieceLabel.setPosition((FIELD_WIDTH + 2) * BLOCK_SIZE, BLOCK_SIZE / 2);
    mWindow.draw(nextPieceLabel);
    drawTetramino(mNextPiece);

    sf::Text levelLabel("LEVEL:\n" + std::to_string(mLevel), mFont, 24);
    levelLabel.setFillColor(sf::Color(0, 127, 0));
    levelLabel.setPosition((FIELD_WIDTH + 0.3) * BLOCK_SIZE, 6 * BLOCK_SIZE);
    mWindow.draw(levelLabel);

    sf::Text linesRemovedLabel("LINES REMOVED:\n" + std::to_string(mLinesRemoved), mFont, 24);
    linesRemovedLabel.setFillColor(sf::Color(0, 0, 127));
    linesRemovedLabel.setPosition((FIELD_WIDTH + 0.3) * BLOCK_SIZE, 8 * BLOCK_SIZE);
    mWindow.draw(linesRemovedLabel);

    sf::Text droppedPiecesLabel("PIECES DROPPED:\n" + std::to_string(mPiecesDropped), mFont, 24);
    droppedPiecesLabel.setFillColor(sf::Color(127, 0, 0));
    droppedPiecesLabel.setPosition((FIELD_WIDTH + 0.3) * BLOCK_SIZE, 10 * BLOCK_SIZE);
    mWindow.draw(droppedPiecesLabel);

    sf::Text spawnedPiecesLabel("PIECES SPAWNED:\n" + std::to_string(mPiecesSpawned), mFont, 24);
    spawnedPiecesLabel.setFillColor(sf::Color(0x501d73ff));
    spawnedPiecesLabel.setPosition((FIELD_WIDTH + 0.3) * BLOCK_SIZE, 12 * BLOCK_SIZE);
    mWindow.draw(spawnedPiecesLabel);

    sf::Text scoreLabel("SCORE:\n" + std::to_string(mScore), mFont, 24);
    scoreLabel.setFillColor(sf::Color(0, 230, 0));
    scoreLabel.setPosition((FIELD_WIDTH + 0.3) * BLOCK_SIZE, 14 * BLOCK_SIZE);
    mWindow.draw(scoreLabel);

    if(mGameState != State::PLAY)
    {
        sf::Text statusLabel;
        statusLabel.setFont(mFont);
        statusLabel.setCharacterSize(24);
        std::string msg = mGameState == State::PAUSE ? "PAUSED" : "GAME OVER";
        if(mGameState == State::PAUSE)
        {
            std::string msg = "PAUSED";
            statusLabel.setFillColor(sf::Color(0, 127, 0));
        }
        else
        {
            std::string msg = "GAME OVER";
            statusLabel.setFillColor(sf::Color(230, 0, 0));
        }
        statusLabel.setString(msg);
        statusLabel.setPosition((FIELD_WIDTH + 0.3) * BLOCK_SIZE, 16 * BLOCK_SIZE);
        mWindow.draw(statusLabel);
    }
}

void Game::createAssets()
{
    mColors[EMPTY] = sf::Color(0x807962ff);
    mColors[BOUNDARY_BLOCK] = sf::Color(0xb7c2c6ff);
    mColors[LOCKED_BLOCK] = sf::Color::White;

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
