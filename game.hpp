#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <map>

class Game
{
public:
    explicit Game();
    void run();

private:
    enum
    {
        FIELD_WIDTH = 12,
        FIELD_HEIGHT = 20,
        BLOCK_SIZE = 30,
        SCREEN_WIDTH = FIELD_WIDTH * BLOCK_SIZE + 200,
        SCREEN_HEIGHT = FIELD_HEIGHT * BLOCK_SIZE,
        FPS = 5,
        EMPTY = 0,
        SHAPE_I = 1,
        SHAPE_L = 2,
        SHAPE_J = 3,
        SHAPE_O = 4,
        SHAPE_S = 5,
        SHAPE_Z = 6,
        SHAPE_T = 7,
        LOCKED_BLOCK = 8,
        BOUNDARY_BLOCK = 9,
        BLOCKS_IN_PIECE = 4,
        NUM_SHAPES = 7,
        NUM_SPAWNED_PIECES_TO_NEXT_LEVEL = 20
    };

    const int SCORES[BLOCKS_IN_PIECE] = { 100, 300, 500, 1600 };

    enum class State
    {
        PLAY,
        PAUSE,
        GAME_OVER
    };

    const std::string TITLE_OF_MAIN_WINDOW { "Tetris" };
    const sf::Time mFrameTime { sf::seconds(1.0f /FPS) };
    sf::RenderWindow mWindow;
    std::map<int, std::string> mShapes;
    std::map<int, sf::Color> mColors;
    int mField[FIELD_HEIGHT * FIELD_WIDTH];

    struct Tetramino
    {
        int type;
        int x, y;
        int rotation;
    };

    enum class Direction { LEFT, RIGHT };

    Tetramino mCurrentPiece, mNextPiece;
    State mGameState = State::PLAY;
    float mTetraminoTimer = 0;
    int mLevel = 1;
    int mLinesRemoved = 0;
    int mPiecesSpawned = 0;
    int mPiecesDropped = 0;
    int mScore = 0;
    sf::Font mFont;
    void inputPhase();
    void updatePhase(sf::Time frameTime);    
    void initField();
    void spawnNewPiece();
    void startNewGame();
    void togglePause();
    void moveCurrentPiece(Direction dir);
    void rotateCurrentPiece(Direction dir);
    bool stepDownCurrentPiece();
    void lockCurrentPiece();
    void dropCurrentPieceDown();
    bool isValidCoordinates(int x, int y) const;
    int rotate(int blockX, int blockY, int angle) const;
    bool fit(const Tetramino& tetramino, int destX, int destY, int rotation);
    void checkLines();
    void renderPhase();
    void drawField();
    void drawCurrentPiece();
    void drawTetramino(const Tetramino& tetramino);
    void drawGameInfo();
    void centralizeWindow();
    void createAssets();
};

#endif // GAME_HPP
