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
#include <random> // Use a better random number generator


// --- Constants ---
const int SCREEN_WIDTH = 160; // Increased screen width for better visuals
const int SCREEN_HEIGHT = 40; // Increased screen height
const std::string HIGHSCORE_FILE = "highscores.txt";
const int MAX_LEVEL = 200; // Increased max level
const int MAX_LIVES = 5;
const std::vector<std::string> DIFFICULTY_LEVELS = {"Easy", "Normal", "Hard", "Hell", "Nightmare", "Ultimate"}; // More difficulty levels
const int POWERUP_CHANCE = 20; // Percentage chance of getting a powerup
const int EFFECT_CHANCE = 10; // Percentage chance of activating a random effect

// --- Additional Game Constants ---
const std::vector<std::string> THEMES = {"Classic", "Dark", "Neon", "Retro", "Future"}; 
const int MAX_MESSAGES = 10;
const int BONUS_INTERVAL = 30; // Trigger bonus every 30 seconds
const int CHALLENGE_INTERVAL = 60; // Trigger challenge every 60 seconds
const double GRAVITY_ACCELERATION = 0.5;

// --- Enums ---
enum class FruitType { APPLE, BANANA, ORANGE, GRAPE, WATERMELON, STRAWBERRY, SPECIAL };
enum class PowerupType { DOUBLE_POINTS, SLOW_MOTION, EXTRA_LIFE, MAGNET, SCORE_BOOST };
enum class GameEffectType { SPEED_BOOST, SHIELD, DOUBLE_SCORE, MAGNET, INVISIBILITY };

// --- Additional Enums ---
enum class GameState {
    MENU, PLAYING, PAUSED, GAME_OVER, HIGH_SCORES, SETTINGS, SHOP, TUTORIAL
};

enum class ParticleType {
    SPARKLE, EXPLOSION, TRAIL, SCORE_POPUP
};

enum class ShopItemType {
    BASKET_SKIN, FRUIT_SKIN, POWER_UP, BACKGROUND
};

// --- Structures ---
struct Fruit {
    FruitType type;
    std::string symbol; // Changed to std::string
    int points;
    std::string name; // Added fruit name for display
    Fruit(FruitType type, const std::string& symbol, int points, const std::string& name) 
        : type(type), symbol(symbol), points(points), name(name) {}
};

struct Basket {
    int x;
    FruitType type;
    std::string symbol; // Changed to std::string
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
    int totalFruitsMissed; // added missed fruit counter
    int totalPowerUpsCollected; // Added power-ups collected counter
    int totalEffectsActivated; // Added effects activated counter
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
};

struct Powerup {
    PowerupType type;
    int duration;
    std::string description; // Added description for better understanding
};

// --- Extended Game Effect Structure ---
struct Particle {
    int x, y;
    ParticleType type;
    std::string symbol;
    int lifetime;
    double velocity_x;
    double velocity_y;
    int color;
};

// --- Extended Shop Item Structure ---
struct ShopItem {
    ShopItemType type;
    std::string name;
    std::string description;
    int price;
    bool unlocked;
    std::string preview;
};

// --- Extended Achievement System ---
struct AchievementTier {
    std::string name;
    int requirement;
    bool claimed;
    std::string reward;
};

// --- Function Prototypes ---
int kbhit();
char getch();
void clearScreen();


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
    std::mt19937 randomEngine; // Mersenne Twister engine for better randomness

    // New member variables
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
    
    // Effect related
    std::vector<std::pair<std::string, int>> floatingTexts;
    std::vector<std::pair<int, int>> sparkles;
    int screenShakeIntensity;
    bool rainbowMode;
    
    // Gameplay related
    int dailyStreak;
    std::time_t lastPlayTime;
    int specialFruitSpawnTimer;
    std::vector<std::pair<std::string, int>> challenges;
    bool bonusModeActive;
    int bonusModeTimer;
    
    // Statistics related
    std::map<FruitType, int> fruitsCaughtByType;
    std::vector<int> scoreHistory;
    int longestStreak;
    int totalPlayTime;
    
    // Visual effect related constants
    const std::vector<std::string> BORDER_STYLES = {
        "═║╔╗╚╝",  // Single line
        "═║╔╗╚╝",  // Double line
        "─│┌┐└┘",  // Rounded corners
        "━┃┏┓┗┛"   // Bold line
    };

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
    void drawScoreBoard(); // Added function to display recent scores

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
    void manageRecentScores(); //Function to manage recent scores



public:
    Game();
    void run();
    ~Game() { delete currentFruit; }
};

// --- kbhit and getch functions (unchanged) ---
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
    return buf;
}

void clearScreen() {
    // Clears the console screen.  Implementation depends on OS.
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}


// --- Game Class Implementations ---
Game::Game() : running(true), score(0), lives(MAX_LIVES), level(1), gameSpeed(150),
             currentFruit(nullptr), fruitY(0), fruitX(SCREEN_WIDTH / 2),
             combo(0), maxCombo(0), animationFrame(0), showTutorial(true),
             hasPowerup(false), playerName("Player"), difficultyLevel(1), isPaused(false),
             comboMultiplier(1), consecutiveCatches(0), totalFruits(0), randomEngine(std::random_device{}()) {
        initializeFruits();
        initializeBaskets();
        initializeAchievements();
        initializeAnimations();
        initializeEffects();
        loadHighScores();
        stats = {0, 0, 0, 0, 0, std::chrono::system_clock::now(), 0, 0, 0};
        lastScoreTime = std::chrono::system_clock::now();

    }

// Add enum conversion functions
std::string powerupTypeToString(PowerupType type) {
    switch(type) {
        case PowerupType::DOUBLE_POINTS: return "Double Points";
        case PowerupType::SLOW_MOTION: return "Slow Motion";
        case PowerupType::EXTRA_LIFE: return "Extra Life";
        case PowerupType::MAGNET: return "Magnet";
        case PowerupType::SCORE_BOOST: return "Score Boost";
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
        default: return "Unknown";
    }
}

void Game::initializeFruits() {
    fruits.clear();
    fruits.push_back(Fruit(FruitType::APPLE, "🍎", 10, "Apple"));
    fruits.push_back(Fruit(FruitType::BANANA, "🍌", 15, "Banana"));
    fruits.push_back(Fruit(FruitType::ORANGE, "🍊", 12, "Orange"));
    fruits.push_back(Fruit(FruitType::GRAPE, "🍇", 8, "Grape"));
    fruits.push_back(Fruit(FruitType::WATERMELON, "🍉", 20, "Watermelon"));
    fruits.push_back(Fruit(FruitType::STRAWBERRY, "🍓", 18, "Strawberry"));
    fruits.push_back(Fruit(FruitType::SPECIAL, "🌟", 30, "Star"));
}

void Game::initializeBaskets() {
    int spacing = SCREEN_WIDTH / fruits.size();
    for (size_t i = 0; i < fruits.size(); ++i) {
        Basket basket;
        basket.x = i * spacing + spacing / 2;
        basket.type = (FruitType)i;
        basket.symbol = fruits[i].symbol;
        baskets.push_back(basket);
    }
}

// ... (Other initialization functions remain largely similar, with improvements for clarity and added functionality)

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
        {"Fruit Collector", "Collect all fruit types", false, 6}, // New achievement
        {"Power Master", "Collect 20 power-ups", false, 20} // New achievement

    };
}

void Game::initializeAnimations() {
    animations = {
        "✨", "💫", "🌟", "💥", "🔥", "🌪️", "🌈", "⚡️", "🚀", "🍄", "🎉", "🎊" // More animations
    };
}

void Game::initializeEffects() {
    activeEffects = {
        {GameEffectType::SPEED_BOOST, 0, "💨", false},
        {GameEffectType::SHIELD, 0, "🛡️", false},
        {GameEffectType::DOUBLE_SCORE, 0, "2x", false},
        {GameEffectType::MAGNET, 0, "🧲", false},
        {GameEffectType::INVISIBILITY, 0, "👻", false} // Added invisibility effect
    };
}

void Game::loadHighScores() {
    // ... (Implementation remains largely the same, error handling improved)
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
    // ... (Implementation remains largely the same, error handling improved)
    highScores.push_back(score);
    std::sort(highScores.begin(), highScores.end(), std::greater<int>());
    if (highScores.size() > 10) highScores.resize(10); // Keep only top 10 scores

    std::ofstream file(HIGHSCORE_FILE);
    if (file.is_open()) {
        for (int s : highScores) {
            file << s << "\n";
        }
        file.close();
    }
}



// ... (Drawing functions remain largely similar, with improvements for clarity and added functionality)

void Game::drawGameBorder() {
    std::cout << "\033[1;36m"; // Cyan color
    for (int i = 0; i < SCREEN_WIDTH + 2; ++i) std::cout << '=';
    std::cout << "\033[0m\n";
}

void Game::drawCombo() {
    if (combo > 0) {
        std::string comboText = "Combo: " + std::to_string(combo);
        if (comboMultiplier > 1) {
            comboText += " (x" + std::to_string(comboMultiplier) + ")";
        }
        std::cout << "\033[1;33m" << comboText << "\033[0m\n";
    }
}

void Game::drawGame() {
    clearScreen(); // Clear the console
    drawGameBorder();
    drawGameStats();
    std::cout << "\n";

    // Player information
    std::cout << "\033[1;34mPlayer: " << playerName << " | Score: " << score << " | Lives: ";
    for (int i = 0; i < lives; ++i) std::cout << "❤️ ";
    std::cout << " | Level: " << level << " | Difficulty: " << DIFFICULTY_LEVELS[difficultyLevel] << "\033[0m\n";

    drawCombo();
    drawEffects();
    drawProgressBar();
    drawGameMessages();
    std::cout << "\n";

    // Game area
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        std::cout << "\033[1;36m║\033[0m";
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (currentFruit && y == fruitY && x == fruitX) {
                std::cout << "\033[1;33m" << currentFruit->symbol << "\033[0m";
            } else if (y == SCREEN_HEIGHT - 1) {
                bool isBasket = false;
                for (const auto& basket : baskets) {
                    if (x == basket.x) {
                        std::cout << "\033[1;32m" << basket.symbol << "\033[0m";
                        isBasket = true;
                        break;
                    }
                }
                if (!isBasket) std::cout << ' ';
            } else {
                std::cout << ' ';
            }
        }
        std::cout << "\033[1;36m║\033[0m\n";
    }

    drawGameBorder();
    std::cout << "\n\033[1;36mControls: [A/D] Move Baskets, [P] Pause, [Q] Quit\033[0m\n";
    drawScoreBoard(); // Display recent scores
}

void Game::drawMenu() {
    // ... (Improved menu drawing)
    clearScreen();
    printCenteredText("Fruit Basket Game", SCREEN_HEIGHT / 2 - 4);
    printCenteredText("1. Start Game", SCREEN_HEIGHT / 2 - 2);
    printCenteredText("2. Instructions", SCREEN_HEIGHT / 2);
    printCenteredText("3. High Scores", SCREEN_HEIGHT / 2 + 2);
    printCenteredText("4. Exit Game", SCREEN_HEIGHT / 2 + 4);
    std::cout << "\nSelect option: ";
}

void Game::drawInstructions() {
    // ... (Improved instructions drawing)
    clearScreen();
    printCenteredText("Instructions", SCREEN_HEIGHT / 2 - 5);
    printCenteredText("Use A/D keys to move baskets", SCREEN_HEIGHT / 2 - 3);
    printCenteredText("Catch falling fruits with the correct basket", SCREEN_HEIGHT / 2 - 1);
    printCenteredText("Special fruits (🌟) give extra points", SCREEN_HEIGHT / 2 + 1);
    printCenteredText("Avoid missing fruits to keep lives", SCREEN_HEIGHT / 2 + 3);
    printCenteredText("Press any key to return to the main menu", SCREEN_HEIGHT / 2 + 5);
    getch();
}


void Game::drawHighScores() {
    // ... (Improved high score display)
    clearScreen();
    printCenteredText("High Scores", SCREEN_HEIGHT / 2 - 4);
    for (size_t i = 0; i < highScores.size(); ++i) {
        std::cout << std::setw(3) << i + 1 << ". " << highScores[i] << "\n";
    }
    printCenteredText("Press any key to return to the main menu", SCREEN_HEIGHT / 2 + 4);
    getch();
}

void Game::drawGameOver() {
    // ... (Improved game over screen)
    clearScreen();
    printCenteredText("\033[1;31mGame Over!\033[0m", SCREEN_HEIGHT / 2 - 6);
    printCenteredText("Final Score: " + std::to_string(score), SCREEN_HEIGHT / 2 - 4);
    printCenteredText("Level Reached: " + std::to_string(level), SCREEN_HEIGHT / 2 - 2);
    printCenteredText("Highest Combo: " + std::to_string(maxCombo), SCREEN_HEIGHT / 2);
    printCenteredText("Fruits Caught: " + std::to_string(stats.totalFruitsCaught), SCREEN_HEIGHT / 2 + 2);
    printCenteredText("Special Fruits: " + std::to_string(stats.totalSpecialFruitsCaught), SCREEN_HEIGHT / 2 + 4);
    printCenteredText("Fruits Missed: " + std::to_string(stats.totalFruitsMissed), SCREEN_HEIGHT / 2 + 6); // display missed fruits
    printCenteredText("Power-ups Collected: " + std::to_string(stats.totalPowerUpsCollected), SCREEN_HEIGHT / 2 + 8); // display collected power-ups
    printCenteredText("Effects Activated: " + std::to_string(stats.totalEffectsActivated), SCREEN_HEIGHT / 2 + 10); // display activated effects

    // Display unlocked achievements
    printCenteredText("\033[1;33mUnlocked Achievements:\033[0m", SCREEN_HEIGHT / 2 + 12);
    for (const auto& achievement : achievements) {
        if (achievement.unlocked) {
            std::cout << "  ★ " << achievement.name << " - " << achievement.description << "\n";
        }
    }
    std::cout << "\n";
}


void Game::drawGameStats() {
    // ... (Improved game stats display)
    auto now = std::chrono::system_clock::now();
    auto gameDuration = std::chrono::duration_cast<std::chrono::seconds>(now - stats.startTime).count();
    int scorePerMinute = (gameDuration > 0) ? static_cast<int>(round(60.0 * score / gameDuration)) : score;

    std::cout << "\033[1;36m╔═══════════════════ Game Stats ═══════════════════╗\033[0m\n";
    std::cout << "\033[1;36m║ Game Time: " << std::setw(5) << gameDuration << " seconds              ║\033[0m\n";
    std::cout << "\033[1;36m║ Score/Minute: " << std::setw(5) << scorePerMinute << "               ║\033[0m\n";
    std::cout << "\033[1;36m║ Level: " << std::setw(2) << level << "                        ║\033[0m\n";
    std::cout << "\033[1;36m╚���═════════════════════════════════════════════════╝\033[0m\n";
}

void Game::drawProgressBar() {
    //Improved progress bar
    int width = SCREEN_WIDTH / 2;
    int progress = (score % 100) * width / 100; // Increased level-up score requirement
    std::cout << "\033[1;35m[";
    for (int i = 0; i < width; ++i) {
        if (i < progress) std::cout << "=";
        else if (i == progress) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << score % 100 << "/100 to next level\033[0m\n"; // Increased level-up score requirement
}

void Game::drawGameMessages() {
    // Improved message display
    if (!gameMessages.empty()) {
        std::cout << "\033[1;33mLatest Messages:\033[0m\n";
        for (size_t i = 0; i < std::min(gameMessages.size(), size_t(3)); ++i) {
            std::cout << "  - " << gameMessages[i] << "\n";
        }
    }
}

void Game::drawEffects() {
    //Improved effects display
    std::cout << "\033[1;33mActive Effects: ";
    bool hasEffects = false;
    for (const auto& effect : activeEffects) {
        if (effect.active) {
            std::cout << effect.symbol << " (" << effect.duration << "s) ";
            hasEffects = true;
        }
    }
    if (!hasEffects) std::cout << "None";
    std::cout << "\033[0m\n";
}


void Game::printCenteredText(const std::string& text, int y) {
    int padding = (SCREEN_WIDTH - text.length()) / 2;
    std::cout << std::setw(padding + text.length()) << std::right << text << "\n";
}

void Game::spawnFruit() {
    if (!currentFruit) {
        std::uniform_int_distribution<> distrib(0, fruits.size() - 1);
        int fruitIndex = distrib(randomEngine);
        currentFruit = new Fruit(fruits[fruitIndex]);
        fruitY = 0;
        fruitX = distrib(randomEngine) % (SCREEN_WIDTH - 10) + 5;
        totalFruits++;
    }
}

void Game::updateGameSpeed() {
    // Adjust game speed based on difficulty and level
    gameSpeed = std::max(20, 250 - (level * 10) - (difficultyLevel * 25)); // Smoother speed adjustment
}

void Game::checkAchievements() {
    // Improved achievement checking logic, added new achievement checks
    std::map<FruitType, bool> fruitsCollected;
    for (auto& achievement : achievements) {
        if (!achievement.unlocked) {
            if (achievement.name == "Rookie Collector" && stats.gamesPlayed >= 1) achievement.unlocked = true;
            else if (achievement.name == "Basket Master" && maxCombo >= 10) achievement.unlocked = true;
            else if (achievement.name == "Fruit Expert" && stats.totalFruitsCaught >= 100) achievement.unlocked = true;
            else if (achievement.name == "Pro Player" && level >= 10) achievement.unlocked = true;
            else if (achievement.name == "Perfect Game" && stats.totalFruitsMissed == 0 && lives == MAX_LIVES) achievement.unlocked = true;
            else if (achievement.name == "Fruit Master" && stats.totalFruitsCaught >= 500) achievement.unlocked = true;
            else if (achievement.name == "Combo King" && maxCombo >= 20) achievement.unlocked = true;
            else if (achievement.name == "Level Challenger" && level >= 50) achievement.unlocked = true;
            else if (achievement.name == "Super Player") {
                bool allUnlocked = true;
                for (const auto& a : achievements) {
                    if (a.name != "Super Player" && !a.unlocked) {
                        allUnlocked = false;
                        break;
                    }
                }
                achievement.unlocked = allUnlocked;
            } else if (achievement.name == "Fruit Collector") {
                for (int i = 0; i < fruits.size() -1; ++i){
                    fruitsCollected[(FruitType)i] = false;
                }
                for (int i = 0; i < stats.totalFruitsCaught; ++i){
                    fruitsCollected[currentFruit->type] = true;
                }
                bool allCollected = true;
                for (int i = 0; i < fruits.size() - 1; ++i){
                    if (!fruitsCollected[(FruitType)i]){
                        allCollected = false;
                        break;
                    }
                }
                achievement.unlocked = allCollected;
            } else if (achievement.name == "Power Master" && stats.totalPowerUpsCollected >= 20) achievement.unlocked = true;
        }
    }
}

void Game::updateGameLogic() {
    if (currentFruit) {
        fruitY++;
        if (fruitY >= SCREEN_HEIGHT - 1) {
            bool caught = false;
            for (const auto& basket : baskets) {
                if (abs(basket.x - fruitX) <= 2 && basket.type == currentFruit->type) {
                    caught = true;
                    int points = currentFruit->points;

                    // Apply effects
                    for (const auto& effect : activeEffects) {
                        if (effect.active && effect.type == GameEffectType::DOUBLE_SCORE) {
                            points *= 2;
                        }
                    }

                    score += points * comboMultiplier;
                    combo++;
                    consecutiveCatches++;

                    // Update combo multiplier
                    comboMultiplier = (consecutiveCatches >= 5) ? 3 : ((consecutiveCatches >= 3) ? 2 : 1);
                    maxCombo = std::max(maxCombo, combo);
                    stats.totalFruitsCaught++;
                    if (currentFruit->type == FruitType::SPECIAL) stats.totalSpecialFruitsCaught++;

                    if (score % 100 == 0) { // Increased level-up score requirement
                        level = std::min(level + 1, MAX_LEVEL);
                        updateGameSpeed();
                        addGameMessage("Level Up! Now at level " + std::to_string(level));
                    }

                    lastScoreTime = std::chrono::system_clock::now();
                }
            }
            if (!caught) {
                lives--;
                combo = 0;
                consecutiveCatches = 0;
                comboMultiplier = 1;
                stats.totalFruitsMissed++; // Increment missed fruits counter
                addGameMessage("Missed! Lost a life");
            }
            delete currentFruit;
            currentFruit = nullptr;
            checkAchievements();
        }
    }
    updateEffects();
}


void Game::applyPowerup() {
    std::uniform_int_distribution<> distrib(1, 100);
    if (distrib(randomEngine) <= POWERUP_CHANCE) {
        hasPowerup = true;
        lastPowerupTime = std::chrono::system_clock::now();
        std::uniform_int_distribution<> powerupDistrib(0, static_cast<int>(PowerupType::SCORE_BOOST));
        currentPowerup.type = static_cast<PowerupType>(powerupDistrib(randomEngine));
        currentPowerup.duration = 10; // Adjust duration as needed
        currentPowerup.description = "Power-up: " + powerupTypeToString(currentPowerup.type); // Add description
        stats.totalPowerUpsCollected++; // Increment collected powerups counter
        addGameMessage(currentPowerup.description);
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
            effect.duration--;
            if (effect.duration <= 0) {
                effect.active = false;
                addGameMessage(gameEffectTypeToString(effect.type) + " effect ended");
            }
        }
    }
}

void Game::activateRandomEffect() {
    std::uniform_int_distribution<> distrib(1, 100);
    if (distrib(randomEngine) <= EFFECT_CHANCE) {
        std::uniform_int_distribution<> effectDistrib(0, activeEffects.size() - 1);
        int effectIndex = effectDistrib(randomEngine);
        if (!activeEffects[effectIndex].active) {
            activeEffects[effectIndex].active = true;
            activeEffects[effectIndex].duration = 10; // Adjust duration as needed
            stats.totalEffectsActivated++; // Increment activated effects counter
            addGameMessage("Activated " + gameEffectTypeToString(activeEffects[effectIndex].type) + " effect!");
        }
    }
}


void Game::run() {
    // ... (Game loop remains largely the same, with improvements for clarity and added functionality)
    bool inMenu = true;
    while (inMenu) {
        drawMenu();
        char choice = getch();
        switch (choice) {
            case '1':
                inMenu = false;
                stats.gamesPlayed++;
                stats.startTime = std::chrono::system_clock::now();
                break;
            case '2':
                drawInstructions();
                break;
            case '3':
                drawHighScores();
                break;
            case '4':
                return;
            default:
                break;
        }
    }

    while (running && lives > 0) {
        spawnFruit();
        drawGame();
        if (kbhit()) {
            char input = getch();
            if (input == 'a' || input == 'A') {
                for (auto& basket : baskets) {
                    if (basket.x > 1) basket.x--;
                }
            } else if (input == 'd' || input == 'D') {
                for (auto& basket : baskets) {
                    if (basket.x < SCREEN_WIDTH - 2) basket.x++;
                }
            } else if (input == 'p' || input == 'P') {
                isPaused = !isPaused;
                if (isPaused) std::cout << "\nGame Paused. Press any key to continue...\n";
                else addGameMessage("Game Resumed");
                while (isPaused && !kbhit()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            } else if (input == 'q' || input == 'Q') {
                running = false;
            }
        }
        if (!isPaused) {
            updateGameLogic();
            applyPowerup();
            activateRandomEffect(); // Activate random effect
            updateAnimation();
            std::this_thread::sleep_for(std::chrono::milliseconds(gameSpeed));
        }
    }

    drawGameOver();
    saveHighScore(score);
}

void Game::drawScoreBoard() {
    // Function to display the recent scores
    std::cout << "\033[1;35mRecent Scores:\033[0m\n";
    for (const auto& score : recentScores) {
      std::cout << "  " << score.second << " - " << score.first << " points\n";
    }
}

void Game::manageRecentScores() {
    // Fix pair type issue
    std::string timestamp = std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
    recentScores.push_back(std::make_pair(score, timestamp));
    if (recentScores.size() > 5) {
        recentScores.erase(recentScores.begin());
    }
}



int main() {
    srand(time(0));
    Game game;
    game.run();
    return 0;
}