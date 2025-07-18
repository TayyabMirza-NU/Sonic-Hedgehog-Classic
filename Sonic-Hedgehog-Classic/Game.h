#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Constants.h"

#include "Level.h"
#include "Player.h"
#include "Systems.h"
#include <iostream>
#include<fstream>
using namespace std;
using namespace sf;


class Game {
private:
    const int LEVEL_COUNT = 4;
 
    const int STATE_MENU = 0;
    const int STATE_SELECT = 1;
    const int STATE_PLAY = 2;
    const int STATE_EXIT = 3;
    const int STATE_LEADER = 4;
    
    RenderWindow window;
    Level* currentLevelPtr = nullptr;
    int currentLevelIndex = -1;
    Player player;
    Font font;
    Menu menu;
    LevelManager levelManager;
    Leaderboard board;
    int score = 0;
    bool isGameOver = false;
    bool complete = false;
    Text gameOverText;
    Text scoreText;
    Text healthText;
    Text winText;
    string  playerName;
    SaveManager saveMgr;

public:
    Game()
        : window(VideoMode(GameConstants::SCREEN_X, GameConstants::SCREEN_Y),
            "Sonic Heroes OOP"),
        levelManager(LEVEL_COUNT),
        saveMgr("savegame.txt")
    {


        window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(60);
        if (!font.loadFromFile("../Data/Jaro/Jaro.ttf"))
            cerr << "Failed to load font." << endl;

   
        scoreText.setFont(font);
        scoreText.setCharacterSize(40);
        scoreText.setFillColor(Color::White);
        scoreText.setPosition(10.f, 10.f);
        scoreText.setString("Score: 0");

        healthText.setFont(font);
        healthText.setCharacterSize(40);
        healthText.setFillColor(Color::White);
        healthText.setPosition(10.f, 50.f);
        healthText.setString("Score: 0");

        gameOverText.setFont(font);
        gameOverText.setCharacterSize(60);
        gameOverText.setFillColor(Color::Red);
        gameOverText.setString("GAME OVER!!");

        winText.setFont(font);
        winText.setCharacterSize(60);
        winText.setFillColor(Color::Green);
        winText.setString("YOU WON!!");
    }

    ~Game() {
        delete currentLevelPtr;
        window.close();
    }

    string getPlayerName(RenderWindow& window);

    void saveScore() {
        ofstream ofs("scores.txt", ios::app);
        if (!ofs) { cerr << "Failed to open scores file." << endl; return; }
        ofs << playerName << ' ' << score << '\n';
    }

    string intToString(int num) {
        if (num == 0) {
            return "0";
        }

        string s = "";
        while (num > 0) {
            char digit = char('0' + (num % 10));
            s = digit + s;
            num /= 10;
        }
        return s;
    }

    void loadLevel(int index) {
        delete currentLevelPtr;
        currentLevelPtr = nullptr;
        currentLevelIndex = index;
        switch (index) {
        case 0: currentLevelPtr = new Level1();    break;
        case 1: currentLevelPtr = new Level2();    break;
        case 2: currentLevelPtr = new Level3();    break;
        case 3: currentLevelPtr = new BossLevel(); break;
        default: break;
        }
        if (currentLevelPtr) {
            currentLevelPtr->setup(&player);
            player.resetForLevel();
            isGameOver = complete = false;

        }
    }

    void run() {
        int state = STATE_MENU;
        Clock frameClock;

        while (window.isOpen() && state != STATE_EXIT) {
            Event ev;
            while (window.pollEvent(ev)) {
                if (ev.type == Event::Closed) {
                    if (currentLevelPtr) {
                        SaveData d;
                        d.hp = player.getHP();
                        d.character = player.getLeaderId();

                        d.levelIndex = currentLevelIndex;
                        d.posX = player.getX();
                        d.posY = player.getY();
                        d.ringCount = currentLevelPtr->getCollectedCount();
                        d.ringsCollected = currentLevelPtr->getCollectedArray();
                        d.enemyCount = currentLevelPtr->getDefeatedCount();
                        d.enemiesDefeated = currentLevelPtr->getDefeatedArray();
                        d.score = score;
                        {
                            int len = playerName.size() < 31
                                ? playerName.size() : 31;
                            for (int i = 0; i < len; ++i)
                                d.playerName[i] = playerName[i];
                            d.playerName[len] = '\0';
                        }
                        saveMgr.saveGame(d);
                    }
                    state = STATE_EXIT;
                }

                if (state == STATE_MENU) {
                    int sel = menu.navigate(ev);
                    if (sel >= 0) {
                        switch (sel) {
                        case 0: { // Continue
                            SaveData sd;
                            if (saveMgr.loadGame(sd)) {
                             
                                playerName = sd.playerName;
                                loadLevel(sd.levelIndex);
                                player.setHP(sd.hp);
                                player.setLeaderId(sd.character);
                                player.setPosition(sd.posX, sd.posY);
                                 currentLevelPtr->setCollected(sd.ringsCollected,sd.ringCount);
                                 currentLevelPtr->setDefeated(sd.enemiesDefeated,sd.enemyCount);
                                score = sd.score;
                                scoreText.setString("Score: " + intToString(score));
                                healthText.setString("Health Points: " + intToString(player.getHP()));
                            }
                            else {
                                
                                playerName = getPlayerName(window);
                                loadLevel(0);
                            }
                            state = STATE_PLAY;
                            break;
                        }
                        case 1: // New Game
                            playerName = getPlayerName(window);
                            player.setHP(10);
                            loadLevel(0);
                            state = STATE_PLAY;
                            break;
                        case 2: // Select Level
                            state = STATE_SELECT;
                            break;
                        case 3: // Leaderboard
                            state = STATE_LEADER;
                            break;
                        case 4: // Exit
                            state = STATE_EXIT;
                            break;
                        }
                    }
                }
                // LEVEL SELECT
                else if (state == STATE_SELECT) {
                    int lvl = levelManager.navigate(ev);
                    if (lvl > 0 && lvl <= LEVEL_COUNT) {
                        playerName = getPlayerName(window);
                        loadLevel(lvl - 1);
                        state = STATE_PLAY;
                    }
                }
              
                if ((isGameOver || complete) &&state == STATE_PLAY && ev.type == Event::KeyPressed)
                {
                    state = STATE_MENU;
                }
            }


            float dt = frameClock.restart().asSeconds();
            if (dt > 0.05f) dt = 0.05f;

            window.clear();

            if (state == STATE_MENU) {
                menu.draw(window);
            }
            else if (state == STATE_SELECT) {
                levelManager.draw(window);
            }
            else if (state == STATE_LEADER) {
                board.viewScoreboard(window, font);
                state = STATE_MENU;
            }
            else if (state == STATE_PLAY && currentLevelPtr) {
                if (!isGameOver && !complete) {
                 
                    if (!player.isAlive()) {
                        isGameOver = true;
                        saveScore();
                    
                      
                        gameOverText.setPosition(
                            GameConstants::SCREEN_X / 2-3*GameConstants::CELL_SIZE,GameConstants::SCREEN_Y / 2
                        );
                    }
                 
                    else if (currentLevelPtr->isComplete()) {
                        int next = currentLevelIndex + 1;
                        if (next < LEVEL_COUNT) {
                            loadLevel(next);
                        }
                        else {
                            complete = true;
                            saveScore();
                           
                            winText.setPosition(
                                GameConstants::SCREEN_X / 2 - 3*GameConstants::CELL_SIZE, GameConstants::SCREEN_Y / 2 );
                        }
                    }

                    if (!isGameOver && !complete) {
                        char** grid = currentLevelPtr->getGrid();
                        player.update(grid, dt, currentLevelIndex);
                        scoreText.setString("Score: " + intToString(score));
                        healthText.setString("Health Points: " + intToString(player.getHP()));

                        float levelPixW = currentLevelPtr->getWidth() * GameConstants::CELL_SIZE - GameConstants::CELL_SIZE;
                        float playerCenterX = player.getX() + GameConstants::CELL_SIZE * 0.5f;
                        float halfW = GameConstants::SCREEN_X * 0.5f;
                        float offsetX;

                        if (levelPixW <= GameConstants::SCREEN_X) {
                        
                            offsetX = 0.0f;
                        }
                        else {
                            float clampedX;
                            if (playerCenterX < halfW) {
                                clampedX = halfW;
                            }
                            else if (playerCenterX > levelPixW - halfW) {
                                clampedX = levelPixW - halfW;
                            }
                            else {
                                clampedX = playerCenterX;
                            }

                 
                            offsetX = clampedX - halfW;
                        }


                        currentLevelPtr->update(dt, offsetX, player.getPX(), score);
                        currentLevelPtr->render(window, offsetX, player.getPX());
                        player.draw(window, offsetX);
                        window.draw(scoreText);
                        window.draw(healthText);
                    }
                }

             
                if (isGameOver) {
                    window.draw(gameOverText);
                }
                else if (complete) {
                    window.draw(winText);
                }
            }

            window.display();
        }
    }

};






string Game::getPlayerName(RenderWindow& window) {
    char* name = new char[31];
    name[0] = '\0';
    unsigned int length = 0;

    Text nameText("", font, 30);
    nameText.setPosition(300, 300);
    nameText.setFillColor(Color::White);

    Text instruction("Enter your name: ", font, 30);
    instruction.setPosition(300, 250);
    instruction.setFillColor(Color::White);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                delete[] name;
                return "";
            }
            else if (event.type == Event::TextEntered) {
       
                if (event.text.unicode == 8 && length > 0) {
                    --length;
                    name[length] = '\0';
                }
             
                else if (event.text.unicode >= 32
                    && event.text.unicode < 128
                    && length < 30) {
                    name[length++] = static_cast<char>(event.text.unicode);
                    name[length] = '\0';
                }
            }
            else if (event.type == Event::KeyPressed
                && event.key.code == Keyboard::Enter) {
                std::string result(name);
                delete[] name;
                return result;
            }
        }

        window.clear(Color::Black);
        nameText.setString(name);
        window.draw(instruction);
        window.draw(nameText);
        window.display();
    }

    string result(name);
    delete[] name;
    return result;
}
