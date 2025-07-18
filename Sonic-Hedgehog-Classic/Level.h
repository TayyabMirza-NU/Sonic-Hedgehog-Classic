#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Collectable.h"
#include "Systems.h"
#include "Enemy.h"
#include "Player.h"
#include "TileMap.h"
#include "Character.h"
#include "Hitbox.h"



class Level {
protected:
    char** grid = nullptr;
    TileMap map;
    Sprite wallSprite, wall2Sprite, platformSprite;
    Texture wallTex, wall2Tex, platformTex;
    Player* player = nullptr;
    Enemy** enemies = nullptr;
    int enemyCount = 0;
    Collectable** collectables = nullptr;
    int collectableCount = 0;
    Music collectSound;
    int id = 0;
    int width = 0;
    int height = 0;
    bool completed = false;

    bool playMusic{ false };
    Texture bgTex;
    Sprite bgSprite;
    Music bgMusic;

    void initTileTextures() {
        wallTex.loadFromFile("../Data/brick1.png"); wallSprite.setTexture(wallTex);
        wall2Tex.loadFromFile("../Data/brick3.png"); wall2Sprite.setTexture(wall2Tex);
        platformTex.loadFromFile("../Data/brick2.png"); platformSprite.setTexture(platformTex);
    }

    void initCollectSound() {
        collectSound.openFromFile("../Data/Ring.wav");
        collectSound.setVolume(50.f);
    }

    

public:
    Level()
    {
        initTileTextures();
        initCollectSound();
        

    }
    virtual ~Level() {
       
        if (grid) {
            for (int i = 0; i < height; ++i) delete[] grid[i];
            delete[] grid;
        }
        if (enemies) {
            for (int i = 0; i < enemyCount; ++i) delete enemies[i];
            delete[] enemies;
        }
        if (collectables) {
            for (int i = 0; i < collectableCount; ++i) delete collectables[i];
            delete[] collectables;
        }
    }

    virtual void setup(Player* P) {

        if (grid) {
            for (int i = 0; i < height; ++i) delete[] grid[i];
            delete[] grid;
        }
        if (enemies) {
            for (int i = 0; i < enemyCount; ++i) delete enemies[i];
            delete[] enemies;
        }
        if (collectables) {
            for (int i = 0; i < collectableCount; ++i) delete collectables[i];
            delete[] collectables;
        }
        
        player = P;
        completed = false;

        grid = new char* [height];
        for (int i = 0; i < height; ++i)
            grid[i] = new char[width];
        grid = map.populate(grid, height, width, id);
        enemies = map.populateEnemies(id, enemyCount);
        collectables = map.populateCollectables(id, collectableCount);


    }

    bool isComplete() const { return completed; }

    int getCollectedCount() const {
        int count = 0;
        for (int i = 0; i < collectableCount; i++)
        {
            if (collectables[i]->isCollected())
                count++;
        }
        return count;
    }

    int* getCollectedArray() {
        int* array = new int[getCollectedCount()];
        for (int i = 0; i < getCollectedCount(); i++)
        {
            if(collectables[i]->isCollected())
                array[i] = i;
        }
        return array;
    }

    int getDefeatedCount() const {
        int count = 0;
        for (int i = 0; i < enemyCount; i++)
        {
            if (enemies[i]->isDead())
                count++;
        }
        return count;
    }

    int* getDefeatedArray() {
        int* array = new int[getDefeatedCount()];
        for (int i = 0; i < getDefeatedCount(); i++)
        {
            if (enemies[i]->isDead())
                array[i] = i;
        }
        return array;
    }

    void setCollected(int* collected, int count) {
        for (int i = 0; i < count; i++)
        {
            collectables[i]->setCollected(true);
        }
    }
    void setDefeated(int* dead, int count) {
        for (int i = 0; i < count; i++)
        {
            enemies[i]->setDead();
        }
    }

    

    void CollisionsDetection(int& score) {
        Character* const* chars = player->getChars();
        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < enemyCount; i++) {
                if (enemies[i] && enemies[i]->isActivated() && !enemies[i]->isDead()) { 
                    

                    if (Platform::checkEnemyCollision(chars[j]->getHitbox(), enemies[i]->getHitbox())) {
                        if (!chars[j]->onGround()) {
                            enemies[i]->takeDamage(1);
                            score += 100;
                            break;
                        }
                        else {
                            player->takeDamage(j);
                            break;
                        }
                    }
                }
            }

            for (int i = 0; i < collectableCount; i++) {
                Collectable* c = collectables[i];
                if (c && !c->isCollected() &&
                    Platform::checkEnemyCollision(chars[j]->getHitbox(), c->getHitbox()))
                {
                    c->setCollected(true);
                    collectSound.play();

                    switch (c->getType()) {
                    case 1:  score += 300;
                             break;
                    case 2:   player->boost(); 
                              break;
                    case 3:  player->incrementHP();
                        break;
                       
                    }
                }
            }



            for (int i = 0; i < enemyCount; ++i) {
                Enemy* e = enemies[i];
                if (e != nullptr && e->isActivated() && !e->isDead()) {
                    BeeBot* bee = dynamic_cast<BeeBot*>(e);
                    if (bee != nullptr) {
                        for (int k = 0; k < 8; ++k) {
                            Projectile* p = bee->getShot(k);
                            if (p != nullptr && p->isAlive()) {
                                if (Platform::checkEnemyCollision(chars[j]->getHitbox(),p->getHitbox())){
                                    player->takeDamage(j);

                                }
                            }
                        }
                    }
                }
            }

        }

    }



    virtual void update(float dt, float offsetX, float* PX, int& score) {}

    virtual void render(RenderWindow& win, float offsetX, float* PX ){
        win.draw(bgSprite);

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                char tile = grid[i][j];
                Sprite* spr = nullptr;
                switch (tile) {
                case 'w': spr = &wallSprite; break;
                case 'p': spr = &platformSprite; break;
                case 'b': case 'z': spr = &wallSprite; break;
                case 'e': spr = &wall2Sprite; break;
                default: continue;
                }
                spr->setPosition(j * GameConstants::CELL_SIZE - offsetX,
                    i * GameConstants::CELL_SIZE);
                win.draw(*spr);
            }
        }
        for (int i = 0; i < collectableCount; ++i)
            if (collectables[i] && !collectables[i]->isCollected())
                collectables[i]->render(offsetX, win);
        for (int i = 0; i < enemyCount; ++i)
            if (enemies[i] && !enemies[i]->isDead() && enemies[i]->isActivated())
                enemies[i]->render(offsetX, win);
    }

    int getWidth() {
        return width;

    }
    char** getGrid() {
        return grid;
    }

};


class Level1 : public Level {
public:
    Level1() {
        bgTex.loadFromFile("../Data/leve1.jpg");
        bgSprite.setTexture(bgTex);
        bgSprite.setScale(0.35f, 0.35f);
        bgMusic.openFromFile("../Data/labrynth.ogg");
        id = 1;
        height = 14;
        width = 200;
       
    }

    void setup(Player* playerPtr) override{
  
        Level::setup(playerPtr);
        player->setAttributes(0.8f, 5, 0.1, width);
        
         
    }
    void update(float dt, float offsetX, float* PX, int& score)override {
        if (!playMusic) {
            bgMusic.play();
            playMusic = true;
        }


        for (int i = 0; i < enemyCount; ++i)
            if (enemies[i] && !enemies[i]->isDead())
                enemies[i]->update(dt, offsetX, grid, player->getX());
        for (int i = 0; i < collectableCount; ++i)
            if (collectables[i])
                collectables[i]->update(dt, offsetX, grid, PX);
        float finishX = width * GameConstants::CELL_SIZE - GameConstants::CELL_SIZE;
        if (player->getX() > finishX)
            completed = true;
        CollisionsDetection(score);
    }
};
class Level2 : public Level {
public:
    Level2() {
        bgTex.loadFromFile("../Data/level2.jpg");
        bgSprite.setTexture(bgTex);
        bgSprite.setScale(0.22f, 0.22f);
        bgMusic.openFromFile("../Data/m2.wav");
        id = 2;
        height = 14;
        width = 250;
    }

    void setup(Player* playerPtr) override {

        Level::setup(playerPtr);
        
    }
    void update(float dt, float offsetX, float* PX, int& score)override {
        player->setAttributes(0.8f, 5, 0.2f, width);
        if (!playMusic) {
            bgMusic.play();
            playMusic = true;
        }


        for (int i = 0; i < enemyCount; ++i)
            if (enemies[i] && !enemies[i]->isDead())
                enemies[i]->update(dt, offsetX, grid, player->getX());
        for (int i = 0; i < collectableCount; ++i)
            if (collectables[i])
                collectables[i]->update(dt, offsetX, grid, PX);
        float finishX = width * GameConstants::CELL_SIZE - GameConstants::CELL_SIZE;
        if (player->getX() > finishX)
            completed = true;
        CollisionsDetection(score);
    }
};
class Level3 : public Level {
public:
    Level3() {
        bgTex.loadFromFile("../Data/level3.jpg");
        bgSprite.setTexture(bgTex);
        bgSprite.setScale(0.9f, 0.9f);
        bgMusic.openFromFile("../Data/m4.wav");
        bgMusic.setVolume(20);
        id = 3;
        height = 14;
        width = 300;
    }

    void setup(Player* playerPtr) override {

        Level::setup(playerPtr);
    }
    void update(float dt, float offsetX, float* PX, int& score)override {
        player->setAttributes(0.5, 5, 0.1f, width);
        if (!playMusic) {
            bgMusic.play();
            playMusic = true;
        }


        for (int i = 0; i < enemyCount; ++i)
            if (enemies[i] && !enemies[i]->isDead())
                enemies[i]->update(dt, offsetX, grid, player->getX());
        for (int i = 0; i < collectableCount; ++i)
            if (collectables[i])
                collectables[i]->update(dt, offsetX, grid, PX);
        float finishX = width * GameConstants::CELL_SIZE - GameConstants::CELL_SIZE;
        if (player->getX() > finishX)
            completed = true;
        CollisionsDetection(score);
    }
};


class BossLevel : public Level {
public:
    BossLevel() {
        bgTex.loadFromFile("../Data/leve1.jpg");
        bgSprite.setColor(Color::Magenta);
        bgSprite.setScale(0.35f, 0.35f);
        bgSprite.setTexture(bgTex);

        bgMusic.openFromFile("../Data/m1.wav");
        id = 4;
        height = 14;
        width = 19;
    }

    void setup(Player* playerPtr) override {

        Level::setup(playerPtr);
    }

    void EnemyCollisionsDetection(int& score) {
        Character* const* chars = player->getChars();
        for (int j = 0; j < 3; j++)
        {
   
                if (enemies[0] && !enemies[0]->isDead()) { 
                    

                    if (Platform::checkEnemyCollision(chars[j]->getHitbox(), enemies[0]->getHitbox())) {
                        cout << "Boosss" << endl;
                        if (!chars[j]->onGround() && !player->isInvincible()) {
                            enemies[0]->takeDamage(1);
                            player->bounceInvincible(j);
                            score += 500;
                            break;
                        }
                        else {
                            player->takeDamage(j);
                            break;
                        }
                    }
                }
            
        }
    }
    void update(float dt, float offsetX, float* PX, int& score)override {
        player->setAttributes(0.8, 5, 0.1f, width);
        if (!playMusic) {
            bgMusic.play();
            playMusic = true;
        }


       
            if (enemies[0] && !enemies[0]->isDead())
                enemies[0]->update(dt, offsetX, grid, player->getX());

            if (enemies[0]->isDead())
                completed = true;
        
        EnemyCollisionsDetection(score);
    }
};







