
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <map>
#include <cmath>
#include <random>

// --- Constants ---
const int SCREEN_WIDTH = 160;
const int SCREEN_HEIGHT = 40;
const std::string HIGHSCORE_FILE = "highscores.txt";
const int MAX_LEVEL = 200;
const int MAX_LIVES = 5;
const std::vector<std::string> DIFFICULTY_LEVELS = {"Easy", "Normal", "Hard", "Hell", "Nightmare", "Ultimate"};
const int POWERUP_CHANCE = 20;
const int EFFECT_CHANCE = 10;

// --- Additional Game Constants ---
const std::vector<std::string> THEMES = {"Classic", "Dark", "Neon", "Retro", "Future"};
const int MAX_MESSAGES = 10;
const int BONUS_INTERVAL = 30; // Seconds
const int CHALLENGE_INTERVAL = 60; // Seconds
const double GRAVITY_ACCELERATION = 0.5;

// --- Enums ---
enum class FruitType { APPLE, BANANA, ORANGE, GRAPE, WATERMELON, STRAWBERRY, SPECIAL };
enum class PowerupType { DOUBLE_POINTS, SLOW_MOTION, EXTRA_LIFE, MAGNET, SCORE_BOOST, FREEZE_TIME };
enum class GameEffectType { SPEED_BOOST, SHIELD, DOUBLE_SCORE, MAGNET, INVISIBILITY, COLOR_SHIFT };
enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER, HIGH_SCORES, SETTINGS, SHOP, INSTRUCTIONS };
enum class ParticleType { SPARKLE, EXPLOSION, TRAIL, SCORE_POPUP };
enum class ShopItemType { BASKET_SKIN, FRUIT_SKIN, POWER_UP, BACKGROUND };
enum class ChallengeType { SPEED_CHALLENGE, COMBO_CHALLENGE, ACCURACY_CHALLENGE, SURVIVAL_CHALLENGE, COLOR_CHALLENGE };

// --- Structures ---
struct Fruit {
    FruitType type;
    std::string symbol;
    int points;
    std::string name;
    double velocity_y;  // Add velocity in y direction
    Fruit(FruitType type, const std::string& symbol, int points, const std::string& name)
        : type(type), symbol(symbol), points(points), name(name), velocity_y(1.0) {}
};

struct Basket {
    int x;
    int width;
    FruitType type;
    std::string symbol;
    Basket(int x, int width, FruitType type, const std::string& symbol) : x(x), width(width), type(type), symbol(symbol) {}
};

struct Achievement {
    std::string name;
    std::string description;
    bool unlocked;
    int requirement;
};

struct GameStats {
    int totalFruitsCaught;
    int totalSpecialFruitsCaught;
    int highestCombo;
    int totalScore;
    int gamesPlayed;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime; // For tracking game end time
    int totalFruitsMissed;
    int totalPowerUpsCollected;
    int totalEffectsActivated;
};

struct PlayerProfile {
    std::string name;
    int totalGames;
    int totalScore;
    std::map<std::string, bool> achievements;
    int highestLevel;
    std::chrono::system_clock::time_point lastPlayed;
};

struct GameEffect {
    GameEffectType type;
    int duration;
    std::string symbol;
    bool active;
    std::chrono::system_clock::time_point startTime;
    int colorIndex;  // For Color Shift effect
    GameEffect(GameEffectType t, int d, const std::string& s) : type(t), duration(d), symbol(s), active(false), colorIndex(0) {}
};

struct Powerup {
    PowerupType type;
    int duration;
    std::string description;
    bool active;
};

struct Particle {
    int x, y;
    ParticleType type;
    std::string symbol;
    int lifetime;
    double velocity_x;
    double velocity_y;
    int color;
    std::chrono::system_clock::time_point creationTime;
};

struct ShopItem {
    ShopItemType type;
    std::string name;
    std::string description;
    int price;
    bool unlocked;
    std::string preview;
};

struct AchievementTier {
    std::string name;
    int requirement;
    bool claimed;
    std::string reward;
};

// Structure for game challenges
struct Challenge {
    ChallengeType type;
    std::string description;
    bool active;
    int target;
    int progress;
    std::chrono::system_clock::time_point startTime;

    Challenge(ChallengeType type) : type(type), active(false), progress(0) {
        switch (type) {
            case ChallengeType::SPEED_CHALLENGE:
                description = "Catch 50 fruits in under 60 seconds";
                target = 50;
                break;
            case ChallengeType::COMBO_CHALLENGE:
                description = "Achieve a combo of 30";
                target = 30;
                break;
            case ChallengeType::ACCURACY_CHALLENGE:
                description = "Catch 100 fruits without missing more than 5";
                target = 100;
                break;
            case ChallengeType::SURVIVAL_CHALLENGE:
                description = "Survive for 120 seconds";
                target = 120;
                break;
            case ChallengeType::COLOR_CHALLENGE:
                description = "Catch 20 red fruits in a row";
                target = 20;
                break;
            default:
                description = "Unknown Challenge";
                target = 0;
        }
    }
};

// --- Function Prototypes ---
int kbhit();
char getch();
void clearScreen();
std::string getCurrentTimestamp();

// --- Game Class ---
class Game {
private:
    // Game variables
    bool running;
    int score;
    int lives;
    int level;
    int gameSpeed;
    std::vector<Fruit> fruits;
    std::vector<Basket> baskets;
    Fruit* currentFruit;
    int fruitY;
    int fruitX;
    std::vector<int> highScores;
    int combo;
    int maxCombo;
    GameStats stats;
    std::vector<Achievement> achievements;
    std::vector<std::string> animations;
    std::string currentAnimation;
    int animationFrame;
    bool showTutorial;
    std::vector<std::pair<int, std::string>> recentScores;
    Powerup currentPowerup;
    bool hasPowerup;
    std::string playerName;
    PlayerProfile profile;
    int difficultyLevel;
    std::vector<GameEffect> activeEffects;
    std::vector<std::string> gameMessages;
    bool isPaused;
    int comboMultiplier;
    int consecutiveCatches;
    std::chrono::system_clock::time_point lastScoreTime;
    std::chrono::system_clock::time_point lastPowerupTime;
    int totalFruits;
    std::mt19937 randomEngine;
    GameState currentState;
    std::vector<Particle> particles;
    std::vector<ShopItem> shopItems;
    std::map<std::string, std::vector<AchievementTier>> tieredAchievements;
    int selectedTheme;
    bool musicEnabled;
    bool effectsEnabled;
    int coins;
    std::string currentBackground;
    std::vector<std::string> unlockedBackgrounds;
    std::vector<std::string> unlockedBasketSkins;
    double gravity;
    std::vector<std::pair<std::string, int>> floatingTexts;
    std::vector<std::pair<int, int>> sparkles;
    int screenShakeIntensity;
    bool rainbowMode;
    int dailyStreak;
    std::time_t lastPlayTime;
    int specialFruitSpawnTimer;
    std::vector<Challenge> challenges; // In-game challenges
    bool bonusModeActive;
    int bonusModeTimer;
    std::map<FruitType, int> fruitsCaughtByType;
    std::vector<int> scoreHistory;
    int longestStreak;
    int totalPlayTime;
    std::chrono::system_clock::time_point lastBonusTime;
    std::chrono::system_clock::time_point lastChallengeTime;
    bool freezeTime; // Added for the new powerup effect
    const std::vector<std::string> BORDER_STYLES = { "═║╔╗╚╝", "═║╔╗╚╝", "─│┌┐└┘", "━┃┏┓┗┛" };

    // Initialization Functions
    void initializeFruits();
    void initializeBaskets();
    void initializeAchievements();
    void initializeAnimations();
    void initializeEffects();
    void loadHighScores();
    void saveHighScore(int score);

    // Drawing Functions
    void drawGameBorder();
    void drawPowerupStatus();
    void drawCombo();
    void drawGame();
    void drawMenu();
    void drawInstructions();
    void drawHighScores();
    void drawGameOver();
    void drawGameStats();
    void drawProgressBar();
    void drawGameMessages();
    void drawEffects();
    void printCenteredText(const std::string& text, int y);
    void drawScoreBoard();
    void displayShop();

    // Game Logic Functions
    void spawnFruit();
    void updateGameSpeed();
    void checkAchievements();
    void updateGameLogic();
    void applyPowerup();
    void updateAnimation();
    void addGameMessage(const std::string& message);
    void updateEffects();
    void activateRandomEffect();
    void manageRecentScores();
    void resetGame();
    void updateFruitVelocity();

    // Additional functions for enhanced gameplay
    void handleLevelProgression();
    void activateBonusMode();
    void updateBonusMode();
    void triggerChallenge();
    void updateChallenges();
    void updateParticles();
    void addParticles(int x, int y, ParticleType type, int num, int color = -1);
    void applyScreenShake();
    void applyFreezeTime(); // Implementation for the new powerup effect
    void startNewGame();

    // Utility Functions
    std::string colorCode(int color);
    int generateRandomColor();
    void updateAchievementsProgress(const std::string& achievementName, int increment);
    void unlockAchievement(const std::string& achievementName);

public:
    Game();
    void run();
    ~Game() { delete currentFruit; }
};

// --- Non-member Functions ---
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    // Use a char pointer or strftime for compatibility
    std::tm* timeInfo = std::localtime(&currentTime);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
    ss << buffer;
    return ss.str();
}

// --- Game Class Implementation ---

Game::Game() : running(true), score(0), lives(MAX_LIVES), level(1), gameSpeed(150),
             currentFruit(nullptr), fruitY(0), fruitX(SCREEN_WIDTH / 2),
             combo(0), maxCombo(0), animationFrame(0), showTutorial(true),
             hasPowerup(false), playerName("Player"), difficultyLevel(0), isPaused(false),
             comboMultiplier(1), consecutiveCatches(0), totalFruits(0), randomEngine(std::random_device{}()),
             currentState(GameState::MENU), selectedTheme(0), musicEnabled(true), effectsEnabled(true),
             coins(0), currentBackground("Default"), gravity(GRAVITY_ACCELERATION),
             screenShakeIntensity(0), rainbowMode(false), dailyStreak(0), specialFruitSpawnTimer(0),
             bonusModeActive(false), bonusModeTimer(0), longestStreak(0), totalPlayTime(0), freezeTime(false) {
    // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));

    initializeFruits();
    initializeBaskets();
    initializeAchievements();
    initializeAnimations();
    initializeEffects();
    loadHighScores();
    stats = {0, 0, 0, 0, 0, std::chrono::system_clock::now(),std::chrono::system_clock::now(), 0, 0, 0};
    lastScoreTime = std::chrono::system_clock::now();
    lastPlayTime = std::time(nullptr);
    lastBonusTime = std::chrono::system_clock::now();
    lastChallengeTime = std::chrono::system_clock::now();

    // Initialize challenges
    for (int i = 0; i < 3; ++i) { // Select 3 random challenges
        std::uniform_int_distribution<> dist(0, static_cast<int>(ChallengeType::COLOR_CHALLENGE));
        ChallengeType type = static_cast<ChallengeType>(dist(randomEngine));
        challenges.emplace_back(type);
    }
}

void Game::startNewGame() {
    // Reset game statistics and states
    score = 0;
    lives = MAX_LIVES;
    level = 1;
    combo = 0;
    maxCombo = 0;
    consecutiveCatches = 0;
    comboMultiplier = 1;
    totalFruits = 0;
    gameSpeed = std::max(20, 250 - (level * 10) - (difficultyLevel * 25));
    fruitY = 0;
    fruitX = SCREEN_WIDTH / 2;
    stats.totalFruitsCaught = 0;
    stats.totalSpecialFruitsCaught = 0;
    stats.totalFruitsMissed = 0;
    stats.totalPowerUpsCollected = 0;
    stats.totalEffectsActivated = 0;
    stats.startTime = std::chrono::system_clock::now();
    stats.endTime = stats.startTime; // Reset end time as well
    isPaused = false;
    bonusModeActive = false;
    bonusModeTimer = 0;
    freezeTime = false;

    // Clear existing fruits and reset baskets
    if (currentFruit) {
        delete currentFruit;
        currentFruit = nullptr;
    }
    initializeBaskets();

    // Reset or reinitialize challenges
    challenges.clear();
    for (int i = 0; i < 3; ++i) {
        std::uniform_int_distribution<> dist(0, static_cast<int>(ChallengeType::COLOR_CHALLENGE));
        ChallengeType type = static_cast<ChallengeType>(dist(randomEngine));
        challenges.emplace_back(type);
    }

    // Reset active effects
    for (auto& effect : activeEffects) {
        effect.active = false;
        effect.duration = 0;
    }

    // Additional resets as needed...

    // Change game state to PLAYING
    currentState = GameState::PLAYING;
    running = true;
}

// Enum conversion functions
std::string powerupTypeToString(PowerupType type) {
    switch(type) {
        case PowerupType::DOUBLE_POINTS: return "Double Points";
        case PowerupType::SLOW_MOTION: return "Slow Motion";
        case PowerupType::EXTRA_LIFE: return "Extra Life";
        case PowerupType::MAGNET: return "Magnet";
        case PowerupType::SCORE_BOOST: return "Score Boost";
        case PowerupType::FREEZE_TIME: return "Freeze Time";
        default: return "Unknown";
    }
}

std::string gameEffectTypeToString(GameEffectType type) {
    switch(type) {
        case GameEffectType::SPEED_BOOST: return "Speed Boost";
        case GameEffectType::SHIELD: return "Shield";
        case GameEffectType::DOUBLE_SCORE: return "Double Score";
        case GameEffectType::MAGNET: return "Magnet";
        case GameEffectType::INVISIBILITY: return "Invisibility";
        case GameEffectType::COLOR_SHIFT: return "Color Shift";
        default: return "Unknown";
    }
}

std::string Game::colorCode(int color) {
    // Color codes for different platforms and terminals
    switch (color) {
        case 0: return "\033[30m"; // Black
        case 1: return "\033[31m"; // Red
        case 2: return "\033[32m"; // Green
        case 3: return "\033[33m"; // Yellow
        case 4: return "\033[34m"; // Blue
        case 5: return "\033[35m"; // Magenta
        case 6: return "\033[36m"; // Cyan
        case 7: return "\033[37m"; // White
        default: return "\033[0m";  // Reset
    }
}

int Game::generateRandomColor() {
    return rand() % 8;
}

void Game::initializeFruits() {
    fruits.clear();
    fruits.emplace_back(FruitType::APPLE, "🍎", 10, "Apple");
    fruits.emplace_back(FruitType::BANANA, "🍌", 15, "Banana");
    fruits.emplace_back(FruitType::ORANGE, "🍊", 12, "Orange");
    fruits.emplace_back(FruitType::GRAPE, "🍇", 8, "Grape");
    fruits.emplace_back(FruitType::WATERMELON, "🍉", 20, "Watermelon");
    fruits.emplace_back(FruitType::STRAWBERRY, "🍓", 18, "Strawberry");
    fruits.emplace_back(FruitType::SPECIAL, "🌟", 30, "Star");
}

void Game::initializeBaskets() {
    baskets.clear();
    int basketWidth = 3; // Initial width for all baskets
    int spacing = SCREEN_WIDTH / fruits.size();
    for (size_t i = 0; i < fruits.size(); ++i) {
        baskets.emplace_back(i * spacing + spacing / 2, basketWidth, static_cast<FruitType>(i), fruits[i].symbol);
    }
}

void Game::initializeAchievements() {
    achievements = {
        {"Rookie Collector", "Play your first game", false, 1},
        {"Basket Master", "Reach 10 combo", false, 10},
        {"Fruit Expert", "Catch 100 fruits", false, 100},
        {"Pro Player", "Reach Level 10", false, 10},
        {"Perfect Game", "Complete a game without missing", false, 1},
        {"Fruit Master", "Catch 500 fruits", false, 500},
        {"Combo King", "Reach 20 combo", false, 20},
        {"Level Challenger", "Reach Level 50", false, 50},
        {"Super Player", "Unlock all achievements", false, 8},
        {"Fruit Collector", "Collect all fruit types", false, 6},
        {"Power Master", "Collect 20 power-ups", false, 20}
    };
}

void Game::initializeAnimations() {
    animations = { "✨", "💫", "🌟", "💥", "🔥", "🌪️", "🌈", "⚡️", "🚀", "🍄", "🎉", "🎊" };
}

void Game::initializeEffects() {
    activeEffects.clear(); // Ensure the vector is empty before initializing
    activeEffects.emplace_back(GameEffectType::SPEED_BOOST, 0, "💨");
    activeEffects.emplace_back(GameEffectType::SHIELD, 0, "🛡️");
    activeEffects.emplace_back(GameEffectType::DOUBLE_SCORE, 0, "2️⃣X");
    activeEffects.emplace_back(GameEffectType::MAGNET, 0, "🧲");
    activeEffects.emplace_back(GameEffectType::INVISIBILITY, 0, "👻");
    activeEffects.emplace_back(GameEffectType::COLOR_SHIFT, 0, "🎨");
}

void Game::loadHighScores() {
    std::ifstream file(HIGHSCORE_FILE);
    if (file.is_open()) {
        int score;
        while (file >> score) {
            highScores.push_back(score);
        }
        file.close();
    }
}

void Game::saveHighScore(int score) {
    highScores.push_back(score);
    std::sort(highScores.begin(), highScores.end(), std::greater<int>());
    if (highScores.size() > 10) highScores.resize(10);

    std::ofstream file(HIGHSCORE_FILE);
    if (file.is_open()) {
        for (int s : highScores) {
            file << s << "\n";
        }
        file.close();
    }
}

void Game::drawGameBorder() {
    std::cout << colorCode(4); // Blue color for the border
    std::cout << "╔";
    for (int i = 0; i < SCREEN_WIDTH; ++i) std::cout << "═";
    std::cout << "╗" << "\n";
    std::cout << colorCode(7); // Reset color
}

void Game::drawCombo() {
    if (combo > 0) {
        std::string comboText = "Combo: " + std::to_string(combo);
        if (comboMultiplier > 1) {
            comboText += " (x" + std::to_string(comboMultiplier) + ")";
        }
        std::cout << colorCode(3) << comboText << colorCode(7) << "\n"; // Yellow color for combo
    }
}

void Game::drawGame() {
    clearScreen();
    applyScreenShake(); // Apply screen shake effect if active
    drawGameBorder();
    drawGameStats();
    std::cout << "\n";

    // Player information
    std::cout << colorCode(1) << "Player: " << playerName << " | Score: " << score << " | Lives: ";
    for (int i = 0; i < lives; ++i) std::cout << "❤️ ";
    std::cout << " | Level: " << level << " | Difficulty: " << DIFFICULTY_LEVELS[difficultyLevel] << colorCode(7) << "\n";

    drawCombo();
    drawEffects();
    drawProgressBar();
    drawGameMessages();
    drawPowerupStatus();
    std::cout << "\n";

    // Game area
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        std::cout << colorCode(4) << "║" << colorCode(7); // Blue border
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            // Check for particles
            bool particleDrawn = false;
            for (const auto& particle : particles) {
                if (particle.x == x && particle.y == y) {
                    std::cout << colorCode(particle.color) << particle.symbol << colorCode(7);
                    particleDrawn = true;
                    break;
                }
            }
            if (particleDrawn) continue;

            // Check for floating texts
            bool textDrawn = false;
            for (const auto& text : floatingTexts) {
                if (x >= static_cast<int>(text.first.length() / 2) && x < SCREEN_WIDTH - static_cast<int>(text.first.length() / 2) && y == 10) { // Adjust y as needed
                    std::cout << text.first;
                    textDrawn = true;
                    break;
                }
            }
            if (textDrawn) continue;

            // Draw game elements
            if (currentFruit && y == fruitY && x == fruitX) {
                std::cout << colorCode(1) << currentFruit->symbol << colorCode(7);
            } else if (y == SCREEN_HEIGHT - 1) {
                bool isBasket = false;
                for (const auto& basket : baskets) {
                    if (x >= basket.x - basket.width / 2 && x <= basket.x + basket.width / 2) {
                        std::cout << colorCode(2) << basket.symbol << colorCode(7);
                        isBasket = true;
                        break;
                    }
                }
                if (!isBasket) std::cout << ' ';
            } else {
                std::cout << ' ';
            }
        }
        std::cout << colorCode(4) << "║" << colorCode(7) << "\n"; // Blue border
    }
    
    std::cout << colorCode(4); // Blue color for the border
    std::cout << "╚";
    for (int i = 0; i < SCREEN_WIDTH; ++i) std::cout << "═";
    std::cout << "╝" << "\n";
    std::cout << colorCode(7); // Reset color

    std::cout << "\n" << colorCode(6) << "Controls: [A/D] Move Baskets, [P] Pause, [Q] Quit" << colorCode(7) << "\n";
    drawScoreBoard();
}

void Game::drawMenu() {
    clearScreen();
    printCenteredText("Fruit Basket Game", SCREEN_HEIGHT / 2 - 10);
    printCenteredText("1. Start Game", SCREEN_HEIGHT / 2 - 4);
    printCenteredText("2. Shop", SCREEN_HEIGHT / 2 - 2);
    printCenteredText("3. Instructions", SCREEN_HEIGHT / 2);
    printCenteredText("4. High Scores", SCREEN_HEIGHT / 2 + 2);
    printCenteredText("5. Settings", SCREEN_HEIGHT / 2 + 4);
    printCenteredText("6. Exit Game", SCREEN_HEIGHT / 2 + 6);
    std::cout << "\nSelect option: ";
}

void Game::displayShop() {
    clearScreen();
    printCenteredText("Welcome to the Shop!", 5);
    printCenteredText("Your Coins: " + std::to_string(coins), 7);

    // Display the shop items with increased spacing and borders
    for (size_t i = 0; i < shopItems.size(); ++i) {
        std::cout << std::setw(5) << "" << "╔═════════════════════════════════════╗" << std::endl;
        std::cout << std::setw(5) << "" << "║ Item " << i + 1 << ": " << std::left << std::setw(25) << shopItems[i].name << "║" << std::endl;
        std::cout << std::setw(5) << "" << "║ Description: " << std::left << std::setw(18) << shopItems[i].description << "║" << std::endl;
        std::cout << std::setw(5) << "" << "║ Price: " << std::left << std::setw(22) << shopItems[i].price << "║" << std::endl;
        std::cout << std::setw(5) << "" << "║ Status: " << (shopItems[i].unlocked ? "Unlocked" : "Locked") << std::setw(15) << " " << "║" << std::endl;
        std::cout << std::setw(5) << "" << "╚═════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
    }

    printCenteredText("Enter item number to buy, or 0 to return to menu:", SCREEN_HEIGHT - 3);

    // Shop logic
    int choice;
    std::cin >> choice;
    if (choice > 0 && choice <= shopItems.size()) {
        ShopItem& item = shopItems[choice - 1];
        if (!item.unlocked && coins >= item.price) {
            coins -= item.price;
            item.unlocked = true;
            // Implement item-specific logic here (e.g., unlocking new baskets)
            printCenteredText("You have purchased " + item.name + "!", SCREEN_HEIGHT - 1);
        } else if (item.unlocked) {
            printCenteredText("Item already unlocked!", SCREEN_HEIGHT - 1);
        } else {
            printCenteredText("Not enough coins!", SCREEN_HEIGHT - 1);
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void Game::drawInstructions() {
    clearScreen();
    printCenteredText("Instructions", 5);
    printCenteredText("Use A/D keys to move baskets", 7);
    printCenteredText("Catch falling fruits with the correct basket", 9);
    printCenteredText("Special fruits (🌟) give extra points", 11);
    printCenteredText("Avoid missing fruits to keep lives", 13);
    printCenteredText("Press P to pause the game", 15);
    printCenteredText("Press Q to quit the game", 17);
    printCenteredText("Earn points to level up and unlock new features", 19);
    printCenteredText("Collect power-ups to gain special abilities", 21);
    printCenteredText("Complete challenges to earn bonus rewards", 23);
    printCenteredText("Visit the shop to unlock new items and customize your game", 25);
    printCenteredText("Press any key to return to the main menu", SCREEN_HEIGHT - 3);
    getch();
}

void Game::drawHighScores() {
    clearScreen();
    printCenteredText("High Scores", 5);
    for (size_t i = 0; i < highScores.size(); ++i) {
        std::cout << std::setw(3) << i + 1 << ". " << highScores[i] << "\n";
    }
    printCenteredText("Press any key to return to the main menu", SCREEN_HEIGHT - 3);
    getch();
}

void Game::drawGameOver() {
    stats.endTime = std::chrono::system_clock::now();
    clearScreen();
    printCenteredText(colorCode(1) + "Game Over!" + colorCode(7), SCREEN_HEIGHT / 2 - 6);
    printCenteredText("Final Score: " + std::to_string(score), SCREEN_HEIGHT / 2 - 4);
    printCenteredText("Level Reached: " + std::to_string(level), SCREEN_HEIGHT / 2 - 2);
    printCenteredText("Highest Combo: " + std::to_string(maxCombo), SCREEN_HEIGHT / 2);
    printCenteredText("Fruits Caught: " + std::to_string(stats.totalFruitsCaught), SCREEN_HEIGHT / 2 + 2);
    printCenteredText("Special Fruits: " + std::to_string(stats.totalSpecialFruitsCaught), SCREEN_HEIGHT / 2 + 4);
    printCenteredText("Fruits Missed: " + std::to_string(stats.totalFruitsMissed), SCREEN_HEIGHT / 2 + 6);
    printCenteredText("Power-ups Collected: " + std::to_string(stats.totalPowerUpsCollected), SCREEN_HEIGHT / 2 + 8);
    printCenteredText("Effects Activated: " + std::to_string(stats.totalEffectsActivated), SCREEN_HEIGHT / 2 + 10);

    // Display unlocked achievements
    printCenteredText(colorCode(3) + "Unlocked Achievements:" + colorCode(7), SCREEN_HEIGHT / 2 + 12);
    for (const auto& achievement : achievements) {
        if (achievement.unlocked) {
            std::cout << "  ★ " << achievement.name << " - " << achievement.description << "\n";
        }
    }

    // Calculate and display the duration of the game
    if (stats.endTime > stats.startTime) {
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(stats.endTime - stats.startTime).count();
        printCenteredText("Game Duration: " + std::to_string(duration) + " seconds", SCREEN_HEIGHT / 2 + 14);
    }

    std::cout << "\n";
}

void Game::drawGameStats() {
    auto now = std::chrono::system_clock::now();
    auto gameDuration = std::chrono::duration_cast<std::chrono::seconds>(now - stats.startTime).count();
    int scorePerMinute = (gameDuration > 0) ? static_cast<int>(round(60.0 * score / gameDuration)) : score;

    std::cout << colorCode(4) << "╔═══════════════════ Game Stats ═══════════════════╗" << colorCode(7) << "\n";
    std::cout << colorCode(4) << "║ " << colorCode(7) << "Game Time: " << std::setw(5) << gameDuration << " seconds" << std::setw(17) << colorCode(4) << " ║" << colorCode(7) << "\n";
    std::cout << colorCode(4) << "║ " << colorCode(7) << "Score/Minute: " << std::setw(5) << scorePerMinute << std::setw(20) << colorCode(4) << " ║" << colorCode(7) << "\n";
    std::cout << colorCode(4) << "║ " << colorCode(7) << "Level: " << std::setw(2) << level << std::setw(28) << colorCode(4) << " ║" << colorCode(7) << "\n";
    std::cout << colorCode(4) << "╚═════════════════════════════════════════════════╝" << colorCode(7) << "\n";
}

void Game::drawProgressBar() {
    int width = SCREEN_WIDTH / 2;
    int progress = (score % 100) * width / 100;
    std::cout << colorCode(5) << "["; // Magenta color for progress bar
    for (int i = 0; i < width; ++i) {
        if (i < progress) std::cout << "=";
        else if (i == progress) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << score % 100 << "/100 to next level" << colorCode(7) << "\n";
}

void Game::drawGameMessages() {
    if (!gameMessages.empty()) {
        std::cout << colorCode(3) << "Latest Messages:" << colorCode(7) << "\n"; // Yellow for messages
        for (size_t i = 0; i < std::min(gameMessages.size(), static_cast<size_t>(3)); ++i) {
            std::cout << "  - " << gameMessages[i] << "\n";
        }
    }
}

void Game::drawEffects() {
    std::cout << colorCode(3) << "Active Effects: " << colorCode(7); // Yellow for effects
    bool hasEffects = false;
    for (const auto& effect : activeEffects) {
        if (effect.active) {
            std::cout << effect.symbol << " (" << effect.duration << "s) ";
            hasEffects = true;
        }
    }
    if (!hasEffects) std::cout << "None";
    std::cout << "\n";
}

void Game::drawPowerupStatus() {
    if (hasPowerup) {
        std::cout << colorCode(6) << currentPowerup.description << " (" 
                  << std::max(0, static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(lastPowerupTime + std::chrono::seconds(currentPowerup.duration) - std::chrono::system_clock::now()).count()))
                  << "s)" << colorCode(7) << "\n";
    }
}

void Game::printCenteredText(const std::string& text, int y) {
    int padding = (SCREEN_WIDTH - static_cast<int>(text.length())) / 2;
    for(int i = 0; i < y; i++)
        std::cout << std::endl;
    std::cout << std::setw(padding + text.length()) << text << "\n";
}

void Game::drawScoreBoard() {
    std::cout << colorCode(5) << "Recent Scores:" << colorCode(7) << "\n"; // Magenta for recent scores
    for (const auto& score : recentScores) {
        std::cout << "  " << score.second << " - " << score.first << " points\n";
    }
}

void Game::spawnFruit() {
    if (!currentFruit) {
        std::uniform_int_distribution<> distrib(0, fruits.size() - 1);
        int fruitIndex = distrib(randomEngine);
        currentFruit = new Fruit(fruits[fruitIndex]);
        currentFruit->velocity_y = 1.0 + (level - 1) * 0.1; // Increase velocity with level

        // Adjust special fruit spawn rate
        if (specialFruitSpawnTimer > 0) {
            specialFruitSpawnTimer--;
            if (currentFruit->type == FruitType::SPECIAL) {
                std::uniform_int_distribution<> specialDistrib(0, fruits.size() - 2); // excluding special fruit from re-roll
                fruitIndex = specialDistrib(randomEngine);
                delete currentFruit;
                currentFruit = new Fruit(fruits[fruitIndex]);
            }
        }
        if (currentFruit->type == FruitType::SPECIAL) {
            specialFruitSpawnTimer = 10; // Reset timer after spawning a special fruit
        }
        fruitY = 0;
        fruitX = distrib(randomEngine) % (SCREEN_WIDTH - 10) + 5;
        totalFruits++;
    }
}

void Game::updateGameSpeed() {
    // Adjust game speed based on difficulty, level, and a minimum speed
    int baseSpeed = 250;
    int levelImpact = level * 5;
    int difficultyImpact = difficultyLevel * 25;
    int minSpeed = 50;
    gameSpeed = std::max(minSpeed, baseSpeed - levelImpact - difficultyImpact);
}

void Game::checkAchievements() {
    std::map<FruitType, bool> fruitsCollected;
    for (auto& achievement : achievements) {
        if (!achievement.unlocked) {
            if (achievement.name == "Rookie Collector" && stats.gamesPlayed >= achievement.requirement) achievement.unlocked = true;
            else if (achievement.name == "Basket Master" && maxCombo >= achievement.requirement) achievement.unlocked = true;
            else if (achievement.name == "Fruit Expert" && stats.totalFruitsCaught >= achievement.requirement) achievement.unlocked = true;
            else if (achievement.name == "Pro Player" && level >= achievement.requirement) achievement.unlocked = true;
            else if (achievement.name == "Perfect Game" && stats.totalFruitsMissed == 0 && lives == MAX_LIVES) achievement.unlocked = true;
            else if (achievement.name == "Fruit Master" && stats.totalFruitsCaught >= achievement.requirement) achievement.unlocked = true;
            else if (achievement.name == "Combo King" && maxCombo >= achievement.requirement) achievement.unlocked = true;
            else if (achievement.name == "Level Challenger" && level >= achievement.requirement) achievement.unlocked = true;
            else if (achievement.name == "Super Player") {
                bool allUnlocked = true;
                for (const auto& a : achievements) {
                    if (a.name != "Super Player" && !a.unlocked) {
                        allUnlocked = false;
                        break;
                    }
                }
                if (allUnlocked) achievement.unlocked = true;
            } else if (achievement.name == "Fruit Collector") {
                for (size_t i = 0; i < fruits.size() - 1; ++i){
                    fruitsCollected[static_cast<FruitType>(i)] = false;
                }
                for (auto const& [type, num] : fruitsCaughtByType){
                    fruitsCollected[type] = true;
                }
                bool allCollected = true;
                for (size_t i = 0; i < fruits.size() - 1; ++i){
                    if (!fruitsCollected[static_cast<FruitType>(i)]){
                        allCollected = false;
                        break;
                    }
                }
                if (allCollected) achievement.unlocked = true;
            } else if (achievement.name == "Power Master" && stats.totalPowerUpsCollected >= achievement.requirement) achievement.unlocked = true;

            if (achievement.unlocked) {
                // Add an achievement unlocked message
                addGameMessage("Achievement Unlocked: " + achievement.name);
            }
        }
    }
}

void Game::updateGameLogic() {
    if (freezeTime) {
        // Skip the fruit falling logic
        return;
    }
    if (currentFruit) {
        // Apply gravity effect
        currentFruit->velocity_y += gravity;
        fruitY += static_cast<int>(currentFruit->velocity_y);
        // Check if the fruit has reached the bottom
        if (fruitY >= SCREEN_HEIGHT - 1) {
            bool caught = false;
            for (const auto& basket : baskets) {
                if (fruitX >= basket.x - basket.width / 2 && fruitX <= basket.x + basket.width / 2 && basket.type == currentFruit->type) {
                    caught = true;
                    int points = currentFruit->points;

                    for (const auto& effect : activeEffects) {
                        if (effect.active) {
                            if (effect.type == GameEffectType::DOUBLE_SCORE) points *= 2;
                            if (effect.type == GameEffectType::MAGNET) {
                                if (abs(fruitX - basket.x) < 5) points *= 2; // Double points if fruit is close to the correct basket
                            }
                        }
                    }

                    score += points * comboMultiplier;
                    combo++;
                    consecutiveCatches++;

                    comboMultiplier = (consecutiveCatches >= 10) ? 3 : ((consecutiveCatches >= 5) ? 2 : 1);
                    maxCombo = std::max(maxCombo, combo);
                    stats.totalFruitsCaught++;
                    if (currentFruit->type == FruitType::SPECIAL) stats.totalSpecialFruitsCaught++;

                    handleLevelProgression();
                    lastScoreTime = std::chrono::system_clock::now();

                    // Generate particles when a fruit is caught
                    addParticles(fruitX, fruitY, ParticleType::EXPLOSION, 5, 2); // Green particles for normal catch
                    if (currentFruit->type == FruitType::SPECIAL) {
                        addParticles(fruitX, fruitY, ParticleType::SPARKLE, 10, 3); // Yellow sparkles for special fruit
                    }

                    // Update challenges progress
                    for (auto& challenge : challenges) {
                        if (challenge.active) {
                            if (challenge.type == ChallengeType::SPEED_CHALLENGE) {
                                challenge.progress++;
                            } else if (challenge.type == ChallengeType::COMBO_CHALLENGE && combo > challenge.progress) {
                                challenge.progress = combo;
                            } else if (challenge.type == ChallengeType::ACCURACY_CHALLENGE) {
                                challenge.progress++;
                            } else if (challenge.type == ChallengeType::COLOR_CHALLENGE) {
                                if (currentFruit->symbol == "🍎") { // Check for red fruit symbol (adjust as needed)
                                    challenge.progress++;
                                } else {
                                    challenge.progress = 0; // Reset progress if not a red fruit
                                }
                            }
                        }
                    }
                    fruitsCaughtByType[currentFruit->type]++; // Increment the count for the type of fruit caught
                }
            }
            if (!caught) {
                lives--;
                combo = 0;
                consecutiveCatches = 0;
                comboMultiplier = 1;
                stats.totalFruitsMissed++;
                addGameMessage("Missed! Lost a life");

                // Update challenges progress
                for (auto& challenge : challenges) {
                    if (challenge.active && challenge.type == ChallengeType::ACCURACY_CHALLENGE) {
                        challenge.target--; // Reduce target for accuracy challenge when a fruit is missed
                    }
                }
                // Generate particles for a miss
                addParticles(fruitX, fruitY, ParticleType::EXPLOSION, 5, 1); // Red particles for a miss
            }
            delete currentFruit;
            currentFruit = nullptr;
            checkAchievements();
        }
    }

    // Trigger bonus mode and challenges at set intervals
    auto now = std::chrono::system_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastBonusTime).count() >= BONUS_INTERVAL) {
        activateBonusMode();
        lastBonusTime = now;
    }
    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastChallengeTime).count() >= CHALLENGE_INTERVAL) {
        triggerChallenge();
        lastChallengeTime = now;
    }

    // Update bonus mode and challenges
    updateBonusMode();
    updateChallenges();
    updateParticles();
    updateFruitVelocity();
    updateEffects();
    applyPowerup();
    activateRandomEffect();
    updateAnimation();
}

void Game::updateFruitVelocity() {
    if (currentFruit) {
        // Increase the velocity based on the level
        currentFruit->velocity_y = 1.0 + (level - 1) * 0.05;
        // Apply active effects to the fruit
        for (const auto& effect : activeEffects) {
            if (effect.active) {
                if (effect.type == GameEffectType::SPEED_BOOST) {
                    currentFruit->velocity_y *= 1.5; // Increase velocity by 50%
                } else if (effect.type == GameEffectType::MAGNET) {
                    // Find the nearest correct basket
                    int minDistance = SCREEN_WIDTH;
                    int targetX = fruitX;
                    for (const auto& basket : baskets) {
                        if (basket.type == currentFruit->type) {
                            int distance = std::abs(basket.x - fruitX);
                            if (distance < minDistance) {
                                minDistance = distance;
                                targetX = basket.x;
                            }
                        }
                    }
                    // Move fruit towards the target basket
                    if (fruitX < targetX) {
                        fruitX = std::min(fruitX + 1, targetX);
                    } else if (fruitX > targetX) {
                        fruitX = std::max(fruitX - 1, targetX);
                    }
                }
            }
        }
    }
}

void Game::applyPowerup() {
    if (hasPowerup) {
        auto now = std::chrono::system_clock::now();
        if (now >= lastPowerupTime + std::chrono::seconds(currentPowerup.duration)) {
            hasPowerup = false;
            if (currentPowerup.type == PowerupType::FREEZE_TIME) {
                freezeTime = false; // Deactivate Freeze Time
            }
            addGameMessage(powerupTypeToString(currentPowerup.type) + " effect ended");
            return;
        }

        switch (currentPowerup.type) {
            case PowerupType::DOUBLE_POINTS:
                // Already handled in updateGameLogic()
                break;
            case PowerupType::SLOW_MOTION:
                gameSpeed = 200; // Slow down the game
                break;
            case PowerupType::EXTRA_LIFE:
                lives++;
                hasPowerup = false; // Consume the powerup immediately
                break;
            case PowerupType::MAGNET:
                // Handled in updateFruitVelocity()
                break;
            case PowerupType::SCORE_BOOST:
                score += 50; // Add a flat score boost
                hasPowerup = false; // Consume the powerup immediately
                break;
            case PowerupType::FREEZE_TIME:
                freezeTime = true; // Activate Freeze Time
                break;
            default:
                break;
        }
    } else {
        std::uniform_int_distribution<> distrib(1, 100);
        if (distrib(randomEngine) <= POWERUP_CHANCE) {
            hasPowerup = true;
            lastPowerupTime = std::chrono::system_clock::now();
            std::uniform_int_distribution<> powerupDistrib(0, static_cast<int>(PowerupType::FREEZE_TIME));
            currentPowerup.type = static_cast<PowerupType>(powerupDistrib(randomEngine));
            currentPowerup.duration = 5;
            currentPowerup.description = "Power-up: " + powerupTypeToString(currentPowerup.type);
            stats.totalPowerUpsCollected++;
            addGameMessage(currentPowerup.description);
        }
    }
}

void Game::applyFreezeTime() {
    freezeTime = true;
}

void Game::handleLevelProgression() {
    // Existing logic for handling level progression
    if (score >= level * 100 && level < MAX_LEVEL) {
        level++;
        updateGameSpeed();
        addGameMessage("Level Up! Now at level " + std::to_string(level));

        // Increase basket width at higher levels, up to a maximum width
        if (level % 5 == 0 && level <= 50) {
            for (auto& basket : baskets) {
                int newBasketWidth = std::min(basket.width + 1, 10);
                basket.width = newBasketWidth;
            }
        }
        // Add coins as a level-up reward
        coins += level * 10;  // Example: 10 coins per level
        addGameMessage("You earned " + std::to_string(level * 10) + " coins!");
    }
}

void Game::updateAnimation() {
    animationFrame = (animationFrame + 1) % animations.size();
    currentAnimation = animations[animationFrame];
}

void Game::addGameMessage(const std::string& message) {
    gameMessages.insert(gameMessages.begin(), message);
    if (gameMessages.size() > 5) gameMessages.pop_back();
}

void Game::updateEffects() {
    auto now = std::chrono::system_clock::now();
    for (auto& effect : activeEffects) {
        if (effect.active) {
            if (effect.type == GameEffectType::COLOR_SHIFT) {
                effect.colorIndex = (effect.colorIndex + 1) % 8;
            }

            // Decrease the duration of the effect
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - effect.startTime);
            effect.duration = std::max(0, static_cast<int>(10 - elapsed.count()));

            if (effect.duration <= 0) {
                effect.active = false;
                effect.colorIndex = 0;
                addGameMessage(gameEffectTypeToString(effect.type) + " effect ended");
            }
        }
    }
}

void Game::activateRandomEffect() {
    if (!bonusModeActive) { // Prevent effect activation during bonus mode
        std::uniform_int_distribution<> distrib(1, 100);
        if (distrib(randomEngine) <= EFFECT_CHANCE) {
            std::uniform_int_distribution<> effectDistrib(0, activeEffects.size() - 1);
            int effectIndex = effectDistrib(randomEngine);
            if (!activeEffects[effectIndex].active) {
                activeEffects[effectIndex].active = true;
                activeEffects[effectIndex].duration = 10; // Adjust duration as needed
                activeEffects[effectIndex].startTime = std::chrono::system_clock::now(); // Set the start time
                activeEffects[effectIndex].colorIndex = generateRandomColor();
                stats.totalEffectsActivated++;
                addGameMessage("Activated " + gameEffectTypeToString(activeEffects[effectIndex].type) + " effect!");
                if (activeEffects[effectIndex].type == GameEffectType::COLOR_SHIFT) {
                    // Apply color shift
                }
                if (activeEffects[effectIndex].type == GameEffectType::SHIELD) {
                    // Add a shield logic here, for example, prevent losing a life on the next miss
                }
                if (activeEffects[effectIndex].type == GameEffectType::INVISIBILITY) {
                    // Make the baskets invisible
                }
                if (activeEffects[effectIndex].type == GameEffectType::SPEED_BOOST) {
                    // Increase the fruit falling speed
                }
                // Add more effect logic here as needed
            }
        }
    }
}

void Game::activateBonusMode() {
    bonusModeActive = true;
    bonusModeTimer = 10; // 10 seconds for bonus mode
    addGameMessage("Bonus Mode Activated!");

    // Activate a random effect during bonus mode
    std::uniform_int_distribution<> effectDistrib(0, activeEffects.size() - 1);
    int effectIndex = effectDistrib(randomEngine);
    activeEffects[effectIndex].active = true;
    activeEffects[effectIndex].duration = 10; // Match bonus mode duration
    activeEffects[effectIndex].startTime = std::chrono::system_clock::now(); // Set the start time
    addGameMessage("Activated " + gameEffectTypeToString(activeEffects[effectIndex].type) + " effect!");

    // Other bonus mode effects can be added here
}

void Game::updateBonusMode() {
    if (bonusModeActive) {
        if (bonusModeTimer <= 0) {
            bonusModeActive = false;
            addGameMessage("Bonus Mode Ended");
        } else {
            bonusModeTimer--;
        }
    }
}

void Game::triggerChallenge() {
    for (auto& challenge : challenges) {
        if (!challenge.active) {
            challenge.active = true;
            challenge.progress = 0;
            challenge.startTime = std::chrono::system_clock::now();
            addGameMessage("New Challenge: " + challenge.description);
            break;
        }
    }
}

void Game::updateChallenges() {
    auto now = std::chrono::system_clock::now();
    for (auto& challenge : challenges) {
        if (challenge.active) {
            bool challengeCompleted = false;
            if (challenge.type == ChallengeType::SPEED_CHALLENGE && challenge.progress >= challenge.target) {
                challengeCompleted = true;
            } else if (challenge.type == ChallengeType::COMBO_CHALLENGE && challenge.progress >= challenge.target) {
                challengeCompleted = true;
            } else if (challenge.type == ChallengeType::ACCURACY_CHALLENGE && challenge.progress >= challenge.target) {
                challengeCompleted = true;
            } else if (challenge.type == ChallengeType::SURVIVAL_CHALLENGE) {
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - challenge.startTime);
                if (elapsed.count() >= challenge.target) {
                    challengeCompleted = true;
                }
            } else if (challenge.type == ChallengeType::COLOR_CHALLENGE && challenge.progress >= challenge.target) {
                challengeCompleted = true;
            }

            if (challengeCompleted) {
                addGameMessage("Challenge Completed: " + challenge.description);
                // Add reward for completing the challenge (e.g., coins, score bonus, etc.)
                challenge.active = false;
            } else if (challenge.type != ChallengeType::SURVIVAL_CHALLENGE) {
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - challenge.startTime);
                if (elapsed.count() >= 60) {
                    addGameMessage("Challenge Failed: " + challenge.description);
                    challenge.active = false;
                }
            }
        }
    }
}

void Game::updateParticles() {
    auto now = std::chrono::system_clock::now();
    for (auto it = particles.begin(); it != particles.end();) {
        // Move the particle
        it->x += static_cast<int>(it->velocity_x);
        it->y += static_cast<int>(it->velocity_y);
        // Decrease lifetime
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->creationTime);
        it->lifetime = std::max(0, static_cast<int>(1000 - elapsed.count())); // Example lifetime

        if (it->lifetime <= 0 || it->x < 0 || it->x >= SCREEN_WIDTH || it->y < 0 || it->y >= SCREEN_HEIGHT) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::addParticles(int x, int y, ParticleType type, int num, int color) {
    for (int i = 0; i < num; ++i) {
        Particle particle;
        particle.x = x;
        particle.y = y;
        particle.type = type;
        particle.creationTime = std::chrono::system_clock::now();

        switch (type) {
            case ParticleType::SPARKLE:
                particle.symbol = "*";
                particle.lifetime = 500; // Lifetime in milliseconds
                particle.velocity_x = (rand() % 3 - 1) * 0.5; // Random horizontal velocity
                particle.velocity_y = (rand() % 3 - 1) * 0.5; // Random vertical velocity
                particle.color = (color == -1) ? generateRandomColor() : color;
                break;
            case ParticleType::EXPLOSION:
                particle.symbol = ".";
                particle.lifetime = 300;
                particle.velocity_x = (rand() % 5 - 2) * 0.5; // Wider range for explosion
                particle.velocity_y = (rand() % 5 - 2) * 0.5;
                particle.color = (color == -1) ? generateRandomColor() : color;
                break;
            case ParticleType::TRAIL:
                particle.symbol = "+";
                particle.lifetime = 400;
                particle.velocity_x = (rand() % 3 - 1) * 0.3;
                particle.velocity_y = (rand() % 3 - 1) * 0.3;
                particle.color = (color == -1) ? generateRandomColor() : color;
                break;
            case ParticleType::SCORE_POPUP:
                particle.symbol = std::to_string(currentFruit->points);
                particle.lifetime = 200;
                particle.velocity_x = 0;
                particle.velocity_y = -0.5; // Move upward
                particle.color = (color == -1) ? generateRandomColor() : color;
                break;
            default:
                particle.symbol = "*";
                particle.lifetime = 1000;
                particle.velocity_x = 0;
                particle.velocity_y = 0;
                particle.color = 7; // White
        }
        particles.push_back(particle);
    }
}

void Game::applyScreenShake() {
    if (screenShakeIntensity > 0) {
        int shakeX = (rand() % (2 * screenShakeIntensity + 1)) - screenShakeIntensity;
        int shakeY = (rand() % (2 * screenShakeIntensity + 1)) - screenShakeIntensity;

        // Move cursor position for shake effect
        std::cout << "\033[" << shakeY << ";" << shakeX << "H";

        screenShakeIntensity--;
    }
}

void Game::run() {
    while (true) {
        switch (currentState) {
            case GameState::MENU:
                drawMenu();
                {
                    char choice = getch();
                    switch (choice) {
                        case '1':
                            startNewGame();
                            currentState = GameState::PLAYING;
                            break;
                        case '2':
                            currentState = GameState::SHOP;
                            break;
                        case '3':
                            currentState = GameState::INSTRUCTIONS;
                            break;
                        case '4':
                            currentState = GameState::HIGH_SCORES;
                            break;
                        case '5':
                            currentState = GameState::SETTINGS;
                            break;
                        case '6':
                            return; // Exit the game
                        default:
                            break;
                    }
                }
                break;
            case GameState::SHOP:
                displayShop();
                currentState = GameState::MENU;
                break;
            case GameState::PLAYING:
                if (!running) {
                    // Reinitialize the game state if starting a new game
                    lives = MAX_LIVES;
                    score = 0;
                    level = 1;
                    combo = 0;
                    maxCombo = 0;
                    consecutiveCatches = 0;
                    comboMultiplier = 1;
                    totalFruits = 0;
                    gameSpeed = std::max(20, 250 - (level * 10) - (difficultyLevel * 25)); // Reset game speed
                    fruitY = 0; // Reset fruit position
                    fruitX = SCREEN_WIDTH / 2; // Reset fruit position
                    stats.totalFruitsCaught = 0;
                    stats.totalSpecialFruitsCaught = 0;
                    stats.totalFruitsMissed = 0;
                    stats.totalPowerUpsCollected = 0;
                    stats.totalEffectsActivated = 0;
                    stats.startTime = std::chrono::system_clock::now(); // Reset game start time
                    isPaused = false;
                    bonusModeActive = false; // Reset bonus mode
                    bonusModeTimer = 0; // Reset bonus mode timer
                    initializeBaskets();    // Reinitialize baskets
                    if (currentFruit) {    // Clear any existing fruit
                        delete currentFruit;
                        currentFruit = nullptr;
                    }
                    challenges.clear();    // Clear challenges
                    for (int i = 0; i < 3; ++i) {
                        std::uniform_int_distribution<> dist(0, static_cast<int>(ChallengeType::COLOR_CHALLENGE));
                        ChallengeType type = static_cast<ChallengeType>(dist(randomEngine));
                        challenges.emplace_back(type);
                    }
                    for (auto& effect : activeEffects) { // Reset active effects
                        effect.active = false;
                        effect.duration = 0;
                    }
                    running = true;
                }
                while (running && lives > 0) {
                    spawnFruit();
                    drawGame();
                    if (kbhit()) {
                        char input = getch();
                        if (input == 'a' || input == 'A') {
                            for (auto& basket : baskets) {
                                basket.x = std::max(basket.x - 1, basket.width / 2);
                            }
                        } else if (input == 'd' || input == 'D') {
                            for (auto& basket : baskets) {
                                basket.x = std::min(basket.x + 1, SCREEN_WIDTH - 1 - basket.width / 2);
                            }
                        } else if (input == 'p' || input == 'P') {
                            isPaused = !isPaused;
                            if (isPaused) {
                                std::cout << "\nGame Paused. Press any key to continue...\n";
                            } else {
                                addGameMessage("Game Resumed");
                            }
                            while (isPaused && !kbhit()) {
                                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                            }
                        } else if (input == 'q' || input == 'Q') {
                            running = false;
                        }
                    }

                    if (!isPaused) {
                        updateGameLogic();
                        std::this_thread::sleep_for(std::chrono::milliseconds(gameSpeed));
                    }
                }

                if (lives <= 0) {
                    currentState = GameState::GAME_OVER;
                }
                break;
            case GameState::GAME_OVER:
                drawGameOver();
                manageRecentScores();
                saveHighScore(score);
                stats.gamesPlayed++;
                checkAchievements(); // Check for achievements at the end of the game
                running = false; // Ensure the game is set to not running
                {
                    std::cout << "\nPress any key to return to the main menu...\n";
                    getch();
                    currentState = GameState::MENU;
                }
                break;
            case GameState::INSTRUCTIONS:
                drawInstructions();
                currentState = GameState::MENU;
                break;
            case GameState::HIGH_SCORES:
                drawHighScores();
                currentState = GameState::MENU;
                break;
            case GameState::SETTINGS:
                // Draw and handle settings
                currentState = GameState::MENU;
                break;
            // ... (Add other cases as needed)
        }
    }
}

void Game::manageRecentScores() {
    std::string timestamp = getCurrentTimestamp();
    recentScores.push_back(std::make_pair(score, timestamp));
    if (recentScores.size() > 5) {
        recentScores.erase(recentScores.begin());
    }
}

int main() {
    Game game;
    game.run();
    return 0;
}
