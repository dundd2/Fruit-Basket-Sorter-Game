# 🍇 Fruity Frenzy: Basket Bonanza 🎮

## 🌟 Intro

Welcome to **Fruity Frenzy: Basket Bonanza**, a fun and engaging console-based game where you catch falling fruits in baskets! This game tests your reflexes, and strategic thinking, and offers a variety of features to enhance your gaming experience. With colourful graphics, challenging levels, and addictive gameplay, you're sure to have a great time.

**Important Note**: This program is designed to run exclusively on **Linux** systems due to its use of POSIX-specific functions and ANSI escape codes that are not supported on Windows.

## 🎬 Demo
![menu](https://github.com/dundd2/Fruit-Basket-Sorter-Game/blob/main/Screenshot/menu.gif)  
![intro](https://github.com/dundd2/Fruit-Basket-Sorter-Game/blob/main/Screenshot/intro.png)  
![endgame](https://github.com/dundd2/Fruit-Basket-Sorter-Game/blob/main/Screenshot/endgame.png)

## 💻 Installation Guide (Linux Only)
1. **Clone the repository**:
    ```bash
    git clone https://github.com/dundd2/Fruity-Frenzy-Basket-Bonanza.git
    cd Fruity-Frenzy-Basket-Bonanza
    ```
2. **Compile the code** (requires a C++ compiler like g++):
    ```bash
    g++ -o FruityFrenzy main.cpp -std=c++11 -pthread
    ```
3. **Run the application**:
    ```bash
    ./FruityFrenzy
    ```

## 💡 Usage Instructions

1. **Start the Application**:
    -   Launch the game by running the compiled source code.
2. **Navigate the Menu**:
    -   Use the number keys to navigate the main menu options: Start Game, Shop, Instructions, High Scores, Settings, and Exit Game.
3. **Start a New Game**:
    -   Select "Start Game" to begin playing.
4. **Control Your Baskets**:
    -   Use the `A` and `D` keys to move your baskets left and right.
    -   Catch the falling fruits with the corresponding baskets.
5. **Collect Power-ups**:
    -   Catch power-ups to gain special abilities and bonuses.
6. **Use the Shop**:
    -   Earn coins by playing and spending them in the shop to unlock new items and customize your game.
7. **Pause and Resume**:
    -   Press `P` to pause the game. Press any key to resume.
8. **Quit the Game**:
    -   Press `Q` to quit the game at any time.
9. **View High Scores**:
    -   Select "High Scores" from the main menu to see the top scores.

## 🎨 Game Design

### Fruits

| Fruit      | Symbol | Points | Description             |
| :--------- | :----- | :----- | :---------------------- |
| Apple      | 🍎     | 10     | Common fruit            |
| Banana     | 🍌     | 15     | Gives more points       |
| Orange     | 🍊     | 12     | Medium points           |
| Grape      | 🍇     | 8      | Small but quick         |
| Watermelon | 🍉     | 20     | High points             |
| Strawberry | 🍓     | 18     | Rare and valuable       |
| Special    | 🌟     | 30     | Extra points and effects |

### Baskets

-   Each basket corresponds to a type of fruit.
-   Baskets can be moved left and right using the `A` and `D` keys.
-   The width of the baskets can increase as you level up.

### Power-ups

| Power-up     | Description                                         | Symbol |
| :----------- | :-------------------------------------------------- | :----- |
| Double Points| Doubles the points earned for a short duration      | 2️⃣X   |
| Slow Motion  | Slows down the game speed                           | ⏱️     |
| Extra Life   | Grants an additional life                           | ❤️     |
| Magnet       | Attracts fruits towards the correct basket          | 🧲     |
| Score Boost  | Instantly adds a flat score boost                  | 💯     |
| Freeze Time  | Temporarily freezes the falling fruits              | ❄️     |

### Game Effects

| Effect        | Description                                                  | Symbol |
| :------------ | :----------------------------------------------------------- | :----- |
| Speed Boost   | Increases the fruit falling speed                            | 💨     |
| Shield        | Prevents losing a life on the next miss                      | 🛡️     |
| Double Score  | Doubles the score earned for a short duration                | 2️⃣X   |
| Magnet        | Attracts fruits towards the correct basket                   | 🧲     |
| Invisibility  | Makes the baskets invisible                                  | 👻     |
| Color Shift   | Changes the colours of the game elements                      | 🎨     |

### Challenges

| Challenge          | Description                                   | Target |
| :----------------- | :-------------------------------------------- | :----- |
| Speed Challenge    | Catch 50 fruits in under 60 seconds           | 50     |
| Combo Challenge    | Achieve a combo of 30                        | 30     |
| Accuracy Challenge | Catch 100 fruits without missing more than 5 | 100    |
| Survival Challenge | Survive for 120 seconds                      | 120    |
| Color Challenge    | Catch 20 red fruits in a row                  | 20     |


## 🎯 What This Program Does
### Core Gameplay Features

-   **Dynamic Fruit System**: Catch various fruits like apples 🍎, bananas 🍌, oranges 🍊, grapes 🍇, watermelons 🍉, and strawberries 🍓, each with unique points and behaviors.
-   **Special Fruits**: Encounter special fruits 🌟 that offer extra points and unique challenges.
-   **Basket System**: Use different baskets to catch fruits, each with specific widths and behaviours.
-   **Level Progression**: Advance through levels with increasing difficulty, speed, and complexity.
-   **Power-ups**: Collect power-ups like Double Points, Slow Motion, Extra Life, Magnet, Score Boost, and Freeze Time to enhance your gameplay.
-   **Game Effects**: Experience various effects like Speed Boost, Shield, Double Score, Magnet, Invisibility, and Color Shift.
-   **Achievements**: Unlock achievements as you play, adding a layer of challenge and reward.
-   **High Scores**: Compete with yourself and others by aiming for the highest score.
-   **Game Statistics**: Track your progress with detailed stats, including total fruits caught, special fruits caught, highest combo, and more.
-   **Shop System**: Earn coins and spend them in the shop to unlock new items, baskets, and backgrounds.
-   **Challenges**: Take on various challenges like Speed Challenge, Combo Challenge, Accuracy Challenge, Survival Challenge, and Color Challenge.
-   **Particles and Animations**: Enjoy visually appealing particles and animations that make the game more dynamic and fun.
-   **Daily Streaks and Bonuses**: Keep playing daily to earn streaks and bonuses.
-   **Responsive Controls**: Use simple keyboard controls to move your baskets and interact with the game.
-   **Pause and Resume**: Pause the game at any time and resume when you're ready.

### User Experience

-   **Colorful Text-Based Interface**: Engaging and visually appealing text-based graphics with color-coded elements.
-   **Immersive Gameplay**: Dynamic game elements, including screen shake and visual effects, for a more immersive experience.
-   **Intuitive Controls**: Simple and responsive controls for easy gameplay.
-   **Detailed Feedback**: Clear and concise feedback on your performance with detailed game stats and messages.
  
## 💫 Technical Skills Demonstrated

### Programming

-   **C++ Core**: Utilizes object-oriented programming, file I/O, string manipulation, and the standard library.
-   **Data Structures**: Implements vectors, maps, and custom classes for managing game entities.
-   **Error Handling**: Includes basic input validation and exception handling.
-   **State Management**: Tracks player progress, inventory, and game states.

### Game Development

-   **Game Loop**: Implements a main game loop to handle user input, game logic, and rendering.
-   **Entity-Component System**: Uses a basic form of ECS with Rooms, Items, and NPCs as entities.
-   **AI and Pathfinding**: Basic AI for NPCs, with hostile NPCs engaging in combat.
-   **Narrative Design**: Branching dialogues and story progression based on player choices.

### Code Style

-   **Consistent Indentation**: 4 spaces for indentation.
-   **Meaningful Names**: Descriptive variable and function names.
-   **Comments**: Clear comments to explain complex logic.
-   **Short Functions**: Functions are kept short and focused on a single task.

### Integration

-   **File System**: Save and load game states using file I/O operations.
-   **Console Effects**: ANSI escape codes for text colourization.
-   **User Interface**: Text-based UI with clear instructions and feedback.

## 🙏 Acknowledgments
 Special thanks to the open-source community for providing valuable resources and tools.

## 📜 License

This project is licensed under the MIT LICENSE.
