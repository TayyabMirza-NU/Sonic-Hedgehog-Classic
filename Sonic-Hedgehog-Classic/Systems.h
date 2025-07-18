// Systems.h
#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "Enemy.h"
#include<iostream>
#include<fstream>

using namespace sf;

using namespace std;


class Menu {
private:
    static const int OPTIONS_COUNT = 5;
    Font font;
    Text options[OPTIONS_COUNT];
    int selected{ 0 };
    const char* labels[OPTIONS_COUNT] = { "Continue","New Game", "Select Level", "Leaderboard", "Exit"};

    Texture backgroundTexture;
    Sprite backgroundSprite;

    Text title;

    SoundBuffer beepBuffer;
    Sound beepSound;

    Music backgroundMusic;

public:
    Menu() {
        font.loadFromFile("../Data/Jaro/Jaro.ttf");

        backgroundTexture.loadFromFile("../Data/menubg2.jpg");
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(0.4f, 0.4f);

        beepBuffer.loadFromFile("../Data/select.wav");
        beepSound.setBuffer(beepBuffer);

        backgroundMusic.openFromFile("../Data/m1.wav");

        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(50.f);
        backgroundMusic.play();


        title.setFont(font);
        title.setString("Developed by FANC");
        title.setCharacterSize(40);
        title.setFillColor(Color::Cyan);
        title.setStyle(Text::Bold);
        title.setPosition(200.f, 20.f);

        for (int i = 0; i < OPTIONS_COUNT; ++i) {
            options[i] = Text(labels[i], font, 36);
            options[i].setPosition(310.f, 150.f + i * 60.f);
            options[i].setFillColor(i == selected ? Color::Yellow : Color::White);
        }
    }

    int navigate(const Event& ev) {
        bool moved = false;
        if (ev.type == Event::KeyPressed) {
            if (ev.key.code == Keyboard::Up && selected > 0) {
                selected--;
                moved = true;
            }
            if (ev.key.code == Keyboard::Down && selected + 1 < OPTIONS_COUNT) {
                selected++;
                moved = true;
            }
            if (ev.key.code == Keyboard::Enter) {
                backgroundMusic.stop();
                return selected;

            }

            if (moved) 
                beepSound.play();

            for (int i = 0; i < OPTIONS_COUNT; ++i)
                options[i].setFillColor(i == selected ? Color::Yellow : Color::White);
        }
        return -1;
    }

    void draw(RenderWindow& win) {
        win.draw(backgroundSprite);
        win.draw(title);
        for (int i = 0; i < OPTIONS_COUNT; ++i)
            win.draw(options[i]);
    }
};



struct SaveData {
    int    hp;
    int   character;
    int    levelIndex;
    float  posX, posY;
    int    ringCount;
    int* ringsCollected;
    int    enemyCount;
    int* enemiesDefeated;
    int    score;
    char   playerName[31];

    SaveData()
        : hp(0), levelIndex(0), posX(0.f), posY(0.f),
        ringCount(0), ringsCollected(nullptr),
        enemyCount(0), enemiesDefeated(nullptr),
        score(0)
    {
        playerName[0] = '\0';
    }

    ~SaveData() {
        delete[] ringsCollected;
        delete[] enemiesDefeated;
    }
};

class SaveManager {
    const char* filename;
public:
    SaveManager(const char* file = "savegame.txt"): filename(file){}

    int stringToInt(const string& s) {
        int result = 0;
        int i = 0;
     
        while (s[i]) {
            result = result * 10 + (s[i] - '0');
            ++i;
        }
        return result;
    }

    bool loadGame(SaveData& d) {
        ifstream input(filename);
        if (!input.is_open()) 
            return false;

        char buf[15];

        input.getline(buf, 15);
        if (input.fail()) return false;
        d.hp = stringToInt(buf);

        // character name
        input.getline(buf, 15);
        if (input.fail()) return false;
        d.character = stringToInt(buf);

        // levelIndex
        input.getline(buf, 15);
        d.levelIndex = stringToInt(buf);

        // posX
        input.getline(buf, 15);
        d.posX = std::atof(buf);

        // posY
        input.getline(buf, 15);
        d.posY = std::atof(buf);

        // ringCount
        input.getline(buf, 15);
        d.ringCount = stringToInt(buf);
        if (d.ringCount > 0) {
            d.ringsCollected = new int[d.ringCount];
            for (int i = 0; i < d.ringCount; ++i) {
                input.getline(buf, 15);
                d.ringsCollected[i] = stringToInt(buf);
            }
        }

        // enemyCount
        input.getline(buf, 15);
        d.enemyCount = stringToInt(buf);
        if (d.enemyCount > 0) {
            d.enemiesDefeated = new int[d.enemyCount];
            for (int i = 0; i < d.enemyCount; ++i) {
                input.getline(buf, 15);
                d.enemiesDefeated[i] = stringToInt(buf);
            }
        }

        // score
        input.getline(buf, 15);
        d.score = stringToInt(buf);

        // playerName
        input.getline(d.playerName, 31);
        if (input.fail()) 
            return false;

        return true;
    }

    void saveGame(const SaveData& d) {
        std::ofstream out(filename, std::ios::trunc);
        if (!out.is_open()) 
            return;

        out << d.hp << '\n';
        out << d.character << '\n';
        out << d.levelIndex << '\n';
        out << d.posX << '\n';
        out << d.posY << '\n';

        out << d.ringCount << '\n';
        for (int i = 0; i < d.ringCount; ++i)
            out << d.ringsCollected[i] << '\n';

        out << d.enemyCount << '\n';
        for (int i = 0; i < d.enemyCount; ++i)
            out << d.enemiesDefeated[i] << '\n';

        out << d.score << '\n';
        out << d.playerName << '\n';

        out.close();
    }
};



class Leaderboard {
private:
    int topScores[10];
    int scoreCount{ 0 };
    
    const char* filename;

    Texture backgroundTexture;
    Sprite backgroundSprite;

public:
    Leaderboard(const char* file = "scores.txt") :filename(file){
        if (!backgroundTexture.loadFromFile("../Data/menubg2.jpg"))
            std::cerr << "Failed to load background image for leaderboard\n";
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(0.4f, 0.4f);
    }

    void addScore(const string& name, int score);

    void viewScoreboard(RenderWindow& window, Font& font) {
        ifstream file(filename);
        string lines[100];
        int scores[100];
        int count = 0;

        if (file.is_open()) {
            while (file >> lines[count] >> scores[count]) {
                count++;
            }
            file.close();
        }

        for (int i = 0; i < count - 1; i++) {
            for (int j = i + 1; j < count; j++) {
                if (scores[i] < scores[j]) {
                    int tempScore = scores[i];
                    scores[i] = scores[j];
                    scores[j] = tempScore;

                    string tempLine = lines[i];
                    lines[i] = lines[j];
                    lines[j] = tempLine;
                }
            }
        }

        window.clear();
        window.draw(backgroundSprite);

        Text title("Scoreboard", font, 50);
        title.setPosition(300, 50);
        title.setFillColor(Color::Cyan);
        window.draw(title);

        for (int i = 0; i < count && i < 10; ++i) {
            Text scoreText(lines[i] + ": \t\t\t\t\t\t" + to_string(scores[i]), font, 30);
            scoreText.setPosition(300, 150 + i * 40);
            scoreText.setFillColor(Color::White);
            window.draw(scoreText);
        }

        Text instruction("Press ESC to return to menu", font, 20);
        instruction.setPosition(300, 600);
        instruction.setFillColor(Color::White);
        window.draw(instruction);

        window.display();

        while (true) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    window.close();
                    return;
                }
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                    return;
                }
            }
        }
    }
};


class LevelManager {
private:
    int choice{ 1 };
    int maxLevels;

    Texture backgroundTexture;
    Sprite  backgroundSprite;

   
    Font font;
    Text prompt;

    Text* levelTexts;

public:
    LevelManager(int maxLv)
        : maxLevels(maxLv)
    {
        
        if (!backgroundTexture.loadFromFile("../Data/menubg2.jpg"))
            std::cerr << "LevelManager: failed to load menubg2.jpg\n";
        backgroundSprite.setTexture(backgroundTexture);
  
        backgroundSprite.setScale(0.4f, 0.4f);

       
        if (!font.loadFromFile("../Data/Jaro/Jaro.ttf"))
            std::cerr << "LevelManager: failed to load Jaro.ttf\n";

     
        prompt.setFont(font);
        prompt.setString("Select Level (1-" + std::to_string(maxLevels) + ") and press Enter");
        prompt.setCharacterSize(50);
        prompt.setFillColor(Color::White);
        prompt.setPosition(150.f, 200.f);

        levelTexts = new Text[maxLevels];
        for (int i = 0; i < maxLevels; ++i) {
            levelTexts[i].setFont(font);
            levelTexts[i].setString(to_string(i + 1));
            levelTexts[i].setCharacterSize(50);
            levelTexts[i].setPosition(300.f + i * 80.f, 300.f);
            levelTexts[i].setFillColor((i + 1) == choice ? Color::Yellow : Color::White);
        }
    }

    ~LevelManager() {
        delete[] levelTexts;
    }

    int navigate(const Event& ev) {
        if (ev.type == Event::KeyPressed) {
            if (ev.key.code == Keyboard::Left && choice > 1) {
                choice--;
            }
            else if (ev.key.code == Keyboard::Right && choice < maxLevels) {
                choice++;
            }
            else if (ev.key.code == Keyboard::Enter) {
                return choice;
            }
         
            for (int i = 0; i < maxLevels; ++i) {
                levelTexts[i].setFillColor((i + 1) == choice
                    ? Color::Yellow
                    : Color::White);
            }
        }
        return -1;
    }

    void draw(RenderWindow& win) {
        win.draw(backgroundSprite);
        win.draw(prompt);
        for (int i = 0; i < maxLevels; ++i) {
            win.draw(levelTexts[i]);
        }
    }
};


