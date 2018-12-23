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
    // I L J O S Z T
    enum
    {
        FIELD_WIDTH = 12,
        FIELD_HEIGHT = 20,
        BLOCK_SIZE = 30,
        SCREEN_WIDTH = FIELD_WIDTH * BLOCK_SIZE,
        SCREEN_HEIGHT = FIELD_HEIGHT * BLOCK_SIZE,
        FPS = 30,
        EMPTY = 0,
        SHAPE_I = 1,
        SHAPE_L = 2,
        SHAPE_J = 3,
        SHAPE_O = 4,
        SHAPE_S = 5,
        SHAPE_Z = 6,
        SHAPE_T = 7,
        BOUNDARY_BLOCK = 9,
        BLOCKS_IN_PIECE = 4,
        NUM_SHAPES = 7
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

    Tetramino mCurrentPiece;
    bool mGameOver;

    void inputPhase();
    void updatePhase(sf::Time frameTime);
    void renderPhase();
    void drawField();
    void drawCurrentPiece();
    void centralizeWindow();
    void createAssets();
    void initField();
    void spawnNewPiece();
    bool isValidCoordinates(int x, int y) const;
    int rotate(int blockX, int blockY, int angle) const;

    bool fit(const Tetramino& tetramino, int destX, int destY, int rotation);
};

#endif // GAME_HPP
