#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "Animation.h"
#include <cmath>
#include "Hitbox.h"
#include <iostream>;

using namespace std;

class Tails;

class Character {
protected:

    float posX{ 100.f };
    float posY{ 100.f };
    bool  isOnGround{ false };
    float velocityX{ 0.f };
    float velocityY{ 0.f };
    float gravityAcceleration{ 0.7f };
    float terminalVelocity{ 20.f };
    float movementSpeed{ 5.f };
    int   spriteWidth, spriteHeight;
    int   hitboxOffsetX, hitboxOffsetY;
    Hitbox hb;
    Music jumpSound;
    int groundWidth;
    friend class Tails;
    bool hanging{ false };
    float accel = 0.1f;
    bool invincible{ false };
    const float followDist = 150.f;
    const float jumpStrength = 10.f;    
    const float bounceFactor = 0.2f;


public:
    Character() {
        jumpSound.openFromFile("../Data/jump.wav");
        jumpSound.setVolume(50.f);

    }

    virtual ~Character() = default;

    virtual void handleInput() = 0;


    void reSpawn(float x) {
        posX = x;
        posY = 200;
        velocityX = 0.f;
        velocityY = 0.f;
    }

    virtual void horizontalCollision(char** levelGrid) {
        posX += velocityX;
        if (posX <= 0) {
            posX = 0;
        }
        if (velocityX != 0.f) {

            float midY = posY + spriteHeight * 0.5f;
            int   rowMid = int(midY / GameConstants::CELL_SIZE);
            int colLeft = int((posX + hitboxOffsetX) / GameConstants::CELL_SIZE);
            int colRight = int((posX + spriteWidth - hitboxOffsetX) / GameConstants::CELL_SIZE);

            if (rowMid >= 0 && rowMid < 14) {
            
                if (velocityX > 0.f &&
                    (levelGrid[rowMid][colRight] == 'w' || levelGrid[rowMid][colRight] == 'b'))
                {
                    posX -= velocityX;  
                }
                else if (velocityX < 0.f &&
                    (levelGrid[rowMid][colLeft] == 'w' || levelGrid[rowMid][colLeft] == 'b'))
                {
                    posX -= velocityX;
                }
            }
        }
    }


    virtual void update(char** levelGrid, bool isControlled, float dt, Character* leader, const int& currentLevel) {
        if (isControlled && hanging) {
            hanging = false;
            terminalVelocity = 20.f;
        }

        if (!isControlled && leader) {
            if (hanging) {
                velocityX = leader->velocityX;
                velocityY = leader->velocityY;
                isOnGround = false;
            }
            else {

                if ((leader->velocityX > 0 && leader->posX - posX > followDist) || (leader->velocityX < 0 && leader->posX - posX < -followDist))
                {
                    velocityX = leader->velocityX;
                }
                else if (leader->posX - posX > followDist) {
                    velocityX = movementSpeed;  
                }
                else if (leader->posX - posX < -followDist) {
                    velocityX = -movementSpeed; 
                }
                else {
                    velocityX = 0.f;
                }


                if (!leader->isOnGround && isOnGround) {
                    velocityY = leader->velocityY;
                    isOnGround = false;
                }
            }
        }

 



        if (!isControlled && posY > GameConstants::SCREEN_Y + 1000) {
            reSpawn(leader->getX());
        }



        hb.setPosition(posX + hitboxOffsetX, posY + hitboxOffsetY);
    }

    virtual void draw(sf::RenderWindow& win, float offsetX) = 0;

    virtual void carryingOthers(Character*& sonic, Character*& knuckles) {}

    int getX() const {
        return int(posX);
    }
    void resetChar() {
        posX = 100;
        posY = 100;
    }
    void setAttributes(float gravity, float speed, float accel, int width) {
        gravityAcceleration = gravity;
        this->accel = accel;
        groundWidth = width;
    }
    void boost(float gravity, float speed, float accel) {
        gravityAcceleration = gravity;
        this->accel = accel;

    }
    void setPosition(float x, float y) {
        posX = x;
        posY = y;
    }
    int getY() const {
        return int(posY);
    }
    void setInvincible(bool inv) {
        invincible = inv;
    }


    float getVelocityX() const {
        return velocityX;
    }
    float* getPX() {
        return &posX;
    }
    void bounce() {
       
        float effectiveVelocity = max(abs(velocityY) * bounceFactor, jumpStrength);
        velocityY = -effectiveVelocity;
        isOnGround = false;
   
        velocityX *= -3 * bounceFactor;
    }
    Hitbox& getHitbox() {
        return hb;
    }
    bool onGround() const {
        return isOnGround;
    }
};

// ======================== Sonic ========================
class Sonic : public Character {
private:
    Animation stillL, stillR, walkL, walkR, runL, runR, atkL, atkR, hangL, hangR;
    Animation* current{ nullptr };
    Texture tStillL, tStillR, tWalkL, tWalkR, tRunL, tRunR, tAtkL, tAtkR, thangL, thangR;

    const float maxSpeed = 18.f;  

public:
    Sonic() {
        jumpSound.openFromFile("../Data/jump.wav");
        spriteWidth = int(40 * 2.5f);
        spriteHeight = int(40 * 2.5f);
        hitboxOffsetX = 20;
        hitboxOffsetY = 1;
        hb.setSize(spriteWidth - 2 * hitboxOffsetX,
            spriteHeight - hitboxOffsetY);

        tStillL.loadFromFile("../Data/0left_Still.png");    stillL.init(tStillL, 40, 40, 1, 0.f);
        tStillR.loadFromFile("../Data/0right_Still.png");   stillR.init(tStillR, 40, 40, 1, 0.f);
        tWalkL.loadFromFile("../Data/0jog_left.png");      walkL.init(tWalkL, 40, 40, 8, 0.09f);
        tWalkR.loadFromFile("../Data/0jog_right.png");     walkR.init(tWalkR, 40, 40, 8, 0.09f);
        tRunL.loadFromFile("../Data/0left_run.png");       runL.init(tRunL, 40, 40, 6, 0.08f);
        tRunR.loadFromFile("../Data/0right_run.png");      runR.init(tRunR, 40, 40, 6, 0.08f);
        tAtkL.loadFromFile("../Data/0upL.png");            atkL.init(tAtkL, 40, 35, 3, 0.12f);
        tAtkR.loadFromFile("../Data/0upR.png");            atkR.init(tAtkR, 40, 35, 3, 0.12f);
        thangL.loadFromFile("../Data/0hang_left.png");            hangL.init(thangL, 49, 44, 6, 0.12f);
        thangR.loadFromFile("../Data/0hang_right.png");            hangR.init(thangR, 49, 44, 6, 0.12f);

        current = &stillR;
    }
    void handleInput() override {


        if (Keyboard::isKeyPressed(Keyboard::A)) {
            if (!invincible && velocityX > 0.f) velocityX = 0.f; // stop if moving right
            velocityX -= accel;
            if (velocityX < -maxSpeed)
                velocityX = -maxSpeed;
        }
        // Accelerate right
        else if (Keyboard::isKeyPressed(Keyboard::D)) {
            if (!invincible && velocityX < 0.f) 
                velocityX = 0.f;

            velocityX += accel;
            if (velocityX > maxSpeed)
                velocityX = maxSpeed;
        }
        else {
            if (velocityX > 0.f) {
                velocityX -= accel;
                if (velocityX < 0.f) velocityX = 0.f;
            }
            else if (velocityX < 0.f) {
                velocityX += accel;
                if (velocityX > 0.f) velocityX = 0.f;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Space) && isOnGround) {
            jumpSound.play();
            velocityY = -20.f;
            isOnGround = false;
        }
    }


    void update(char** levelGrid, bool isControlled, float dt, Character* leader, const int& curLevel) override {
        Character::update(levelGrid, isControlled, dt, leader, curLevel);
        if (isControlled)
            handleInput();


        horizontalCollision(levelGrid);
        //posX += velocityX;
        //if (velocityX != 0.f) {
        //    float midY = posY + spriteHeight * 0.5f;
        //    int rowMid = int(midY / GameConstants::CELL_SIZE);
        //    int colLeft = int((posX + hitboxOffsetX) / GameConstants::CELL_SIZE);
        //    int colRight = int((posX + spriteWidth - hitboxOffsetX) / GameConstants::CELL_SIZE);
        //    if (rowMid >= 0 && rowMid < 14) {
        //        if (velocityX > 0 && levelGrid[rowMid][colRight] == 'w') {
        //            posX -= velocityX;
        //            //velocityX = 0.f; // stop on collision
        //        }
        //        if (velocityX < 0 && levelGrid[rowMid][colLeft] == 'w')
        //        {
        //            posX -= velocityX;
        //            //velocityX = 0.f; // stop on collision
        //        }
        //    }
        //}

        float nextY = posY + velocityY;
        hb.setPosition(posX + hitboxOffsetX, nextY + hitboxOffsetY);
        if (velocityY >= 0 && Platform::checkCollision(hb, levelGrid, groundWidth)) {
            isOnGround = true;
            velocityY = 0.f;
            int landedRow = int(hb.bottom() / GameConstants::CELL_SIZE);
            posY = landedRow * GameConstants::CELL_SIZE - (spriteHeight - hitboxOffsetY);
        }
        else {
            isOnGround = false;
            posY = nextY;
            velocityY = std::min(velocityY + gravityAcceleration, terminalVelocity);
        }



        if (hanging && !onGround())
        {

            current = (velocityX < 0 ? &hangL : &hangR);
        }
        else if (!onGround())
            current = (velocityX < 0 ? &atkL : &atkR);
        else if (std::fabs(velocityX) > movementSpeed * 1.5f)
            current = (velocityX < 0 ? &runL : &runR);
        else if (std::fabs(velocityX) > 0.f)
            current = (velocityX < 0 ? &walkL : &walkR);
        else {
            if (current == &walkL || current == &runL || current == &atkL || current == &hangL)
                current = &stillL;

            else if (current == &walkR || current == &runR || current == &atkR || current == &hangR)
                current = &stillR;

        }
        current->setPosition(posX, posY);
        current->update(dt);
    }


    void draw(sf::RenderWindow& win, float offsetX) override {
        hb.shape.setPosition(posX + hitboxOffsetX - offsetX, posY + hitboxOffsetY);
        //win.draw(hb.shape);
        current->draw(win, offsetX);
    }
};
// ============================= Tails ========================
class Tails : public Character {
private:
    Animation stillL, stillR, walkL, walkR, runL, runR, atkL, atkR, flyL, flyR;
    Animation* current{ nullptr };
  

    const float maxSpeed = 10.f;

    Clock flightTime;
    bool flying{ false };

    Music FlySound;

    sf::Texture tStillL, tStillR, tWalkL, tWalkR, tRunL, tRunR, tAtkL, tAtkR, tflyL, tflyR;
public:
    Tails() {
        FlySound.openFromFile("../Data/Flying.wav");
        FlySound.setVolume(50.f);

        spriteWidth = int(40 * 2.5f);
        spriteHeight = int(40 * 2.5f);
        hitboxOffsetX = 20;
        hitboxOffsetY = 10;
        hb.setSize(spriteWidth - 1.9 * hitboxOffsetX,
            spriteHeight - 1.8 * hitboxOffsetY);

        tStillL.loadFromFile("../Data/2still_left.png"); stillL.init(tStillL, 60, 60, 1, 0.f, 1.4f);
        tStillR.loadFromFile("../Data/2still_right.png"); stillR.init(tStillR, 60, 60, 1, 0.f, 1.4f);
        tWalkL.loadFromFile("../Data/2jog_left.png");    walkL.init(tWalkL, 49, 35, 6, 0.09f);
        tWalkR.loadFromFile("../Data/2jog_right.png");   walkR.init(tWalkR, 49, 35, 6, 0.09f);
        tRunL.loadFromFile("../Data/2run_left.png");       runL.init(tRunL, 49, 35, 8, 0.08f);
        tRunR.loadFromFile("../Data/2run_right.png");      runR.init(tRunR, 49, 35, 8, 0.08f);
        tAtkL.loadFromFile("../Data/2upL.png");            atkL.init(tAtkL, 48.6f, 48, 6, 0.12f);
        tAtkR.loadFromFile("../Data/2upR.png");            atkR.init(tAtkR, 48.6f, 48, 6, 0.12f);
        tflyL.loadFromFile("../Data/2fly_left.png");            flyL.init(tflyL, 57.f, 39, 4, 0.1);
        tflyR.loadFromFile("../Data/2fly_right.png");            flyR.init(tflyR, 57.f, 39, 4, 0.1);

        current = &stillR;
    }


    void handleInput() override {
        static bool flight = false;      
        static bool spacePressed = false;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            if (!invincible && velocityX > 0)
            {
                velocityX = 0;
            }
            velocityX -= accel;
            if (velocityX < -maxSpeed)
                velocityX = -maxSpeed;
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            if (!invincible && velocityX < 0)
            {
                velocityX = 0;
            }
            velocityX += accel;
            if (velocityX > maxSpeed)
                velocityX = maxSpeed;
        }
        else {
            if (velocityX > 0.f) {
                velocityX -= accel;
                if (velocityX < 0.f) velocityX = 0.f;
            }
            else if (velocityX < 0.f) {
                velocityX += accel;
                if (velocityX > 0.f) velocityX = 0.f;
            }
        }


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (isOnGround) {
                flight = true;
                jumpSound.play();
                velocityY = -20.f;
                isOnGround = false;
            }
            else if (flight && !spacePressed) {
                flying = true;
                flightTime.restart();
                FlySound.play();
                FlySound.setLoop(true);
            }
            else if (flight && flightTime.getElapsedTime().asSeconds() > 7.f) {
                flying = false; 
                FlySound.setLoop(false);
            }
            spacePressed = true; 
        }
        else {
            if (flying) {
                flying = false; 
            }
            FlySound.setLoop(false);
            spacePressed = false; 
        }

        if (flying) {
            velocityY = -0.5f; 
        }
    }


    void carryingOthers(Character*& sonic, Character*& knuckles) {
        
        if (!flying) {
            sonic->terminalVelocity = 20.f;
            knuckles->terminalVelocity = 20.f;
            knuckles->hanging = false;
            sonic->hanging = false;
            return;
        }

       
        if (Platform::checkFlyingCollision(this->hb, sonic->hb)) {
            
            sonic->posX = posX;
            sonic->posY = posY + hb.height - 5.f;
            sonic->velocityX = velocityX;
            sonic->velocityY = velocityY;
            sonic->isOnGround = false;
            sonic->hanging = true; 
            sonic->terminalVelocity = 0.f;

          
            if (Platform::checkFlyingCollision(sonic->hb, knuckles->hb)) {
                
                knuckles->posX = sonic->posX;
                knuckles->posY = sonic->posY + sonic->hb.height - 20.f;
                knuckles->velocityX = velocityX;
                knuckles->velocityY = velocityY;
                knuckles->hanging = true;
                knuckles->isOnGround = false;
                knuckles->terminalVelocity = 0.f;
            }
            else {
                knuckles->terminalVelocity = 20.f;
                knuckles->hanging = false;
            }
        }

        else if (Platform::checkFlyingCollision(this->hb, knuckles->hb)) {
   
            knuckles->posX = posX;
            knuckles->posY = posY + 70.f;    
            knuckles->velocityX = velocityX;
            knuckles->velocityY = velocityY;
            knuckles->isOnGround = false;
            knuckles->hanging = true;
            knuckles->terminalVelocity = 0.f;

            if (Platform::checkFlyingCollision(knuckles->hb, sonic->hb)) {

                sonic->posX = knuckles->posX;
                sonic->posY = knuckles->posY + knuckles->hb.height;
                sonic->velocityX = velocityX;
                sonic->velocityY = velocityY;
                sonic->isOnGround = false;
                sonic->hanging = true;
                sonic->terminalVelocity = 0.f;
            }
            else {
               
                sonic->terminalVelocity = 20.f;
            }
        }
     
        else {
            sonic->terminalVelocity = 20.f;
            knuckles->terminalVelocity = 20.f;
            knuckles->hanging = false;
            sonic->hanging = false;
        }
    }




    void update(char** levelGrid, bool isControlled, float dt, Character* leader, const int& curLevel) override {
        Character::update(levelGrid, isControlled, dt, leader, curLevel);
        if (isControlled)
            handleInput();

        if (!isControlled)
            flying = false;




        horizontalCollision(levelGrid);

        //posX += velocityX;
        //if (velocityX != 0.f) {
        //    float midY = posY + spriteHeight * 0.5f;
        //    int rowMid = int(midY / GameConstants::CELL_SIZE);
        //    int colLeft = int((posX + hitboxOffsetX) / GameConstants::CELL_SIZE);
        //    int colRight = int((posX + spriteWidth - hitboxOffsetX) / GameConstants::CELL_SIZE);
        //    if (rowMid >= 0 && rowMid < 14) {
        //        if (velocityX > 0 && levelGrid[rowMid][colRight] == 'w') posX -= velocityX;
        //        if (velocityX < 0 && levelGrid[rowMid][colLeft] == 'w') posX -= velocityX;
        //    }
        //}

        
        float nextY = posY + velocityY;
        hb.setPosition(posX + hitboxOffsetX, nextY + hitboxOffsetY);
        if (velocityY >= 0 && Platform::checkCollision(hb, levelGrid, groundWidth)) {
            isOnGround = true;
            velocityY = 0.f;
            int landedRow = int(hb.bottom() / GameConstants::CELL_SIZE);
            posY = landedRow * GameConstants::CELL_SIZE - (spriteHeight - hitboxOffsetY);
        }
        else {
            isOnGround = false;
            posY = nextY;
            velocityY = std::min(velocityY + gravityAcceleration, terminalVelocity);
        }




        if (flying && !onGround()) {

            current = (velocityX < 0 ? &flyL : &flyR);
        }
        else if (!onGround())
            current = (velocityX < 0 ? &atkL : &atkR);
        else if (std::fabs(velocityX) > movementSpeed * 1.5f)
            current = (velocityX < 0 ? &runL : &runR);
        else if (std::fabs(velocityX) > 0.f)
            current = (velocityX < 0 ? &walkL : &walkR);
        else {
            if (current == &walkL || current == &runL || current == &atkL || current == &flyL)
                current = &stillL;

            else if (current == &walkR || current == &runR || current == &atkR || current == &flyR)
                current = &stillR;


        }
        current->setPosition(posX, posY);
        current->update(dt);
    }

    void draw(sf::RenderWindow& win, float offsetX) override {
        hb.shape.setPosition(posX + hitboxOffsetX - offsetX, posY + hitboxOffsetY);
        //win.draw(hb.shape);
        current->draw(win, offsetX);
    }
};






// ========================= Knuckles =============================
class Knuckles : public Character {
private:
    Animation stillL, stillR, walkL, walkR, runL, runR, atkL, atkR, hangL, hangR;
    Animation* current{ nullptr };

    Texture tStillL, tStillR, tWalkL, tWalkR, tRunL, tRunR, tAtkL, tAtkR, thangL, thangR;
    const float maxSpeed = 12.f;
public:
    Knuckles() {

        spriteWidth = int(35 * 2.5f);
        spriteHeight = int(49 * 2.5f);
        hitboxOffsetX = 15;
        hitboxOffsetY = 12;
        hb.setSize(spriteWidth - 2 * hitboxOffsetX,
            spriteHeight - 1.8 * hitboxOffsetY);

        tStillL.loadFromFile("../Data/1left_Still.png");  stillL.init(tStillL, 46, 49, 1, 0.f);
        tStillR.loadFromFile("../Data/1right_Still.png"); stillR.init(tStillR, 46, 49, 1, 0.f);

        tWalkL.loadFromFile("../Data/1jog_left.png");     walkL.init(tWalkL, 49, 46, 12, 0.09f);
        tWalkR.loadFromFile("../Data/1jog_right.png");    walkR.init(tWalkR, 49, 46, 12, 0.09f);
        tRunL.loadFromFile("../Data/1left_run.png");       runL.init(tRunL, 48, 40, 8, 0.08f);
        tRunR.loadFromFile("../Data/1right_run.png");      runR.init(tRunR, 48, 40, 8, 0.08f);
        tAtkL.loadFromFile("../Data/1upL.png");            atkL.init(tAtkL, 49, 48, 6, 0.12f);
        tAtkR.loadFromFile("../Data/1upR.png");            atkR.init(tAtkR, 49, 48, 6, 0.12f);
        thangL.loadFromFile("../Data/1hang_left.png");            hangL.init(thangL, 49, 48, 6, 0.12f);
        thangR.loadFromFile("../Data/1hang_right.png");            hangR.init(thangR, 49, 48, 6, 0.12f);

        current = &stillR;
    }


    void horizontalCollision(char** levelGrid) override {
        posX += velocityX;

        if (velocityX != 0.f) {
            float midY = posY + spriteHeight * 0.5f;
            int   rowMid = int(midY / GameConstants::CELL_SIZE);
            int colLeft = int((posX + hitboxOffsetX) / GameConstants::CELL_SIZE);
            int colRight = int((posX + spriteWidth - hitboxOffsetX) / GameConstants::CELL_SIZE);

            if (rowMid >= 0 && rowMid < 14) {
           
                if (velocityX > 0.f)
                {
                    if (levelGrid[rowMid][colRight] == 'w')
                        posX -= velocityX;
                    else if (levelGrid[rowMid][colRight] == 'b') {
                        levelGrid[rowMid][colRight] = 'd';
                    }
                }
                else if (velocityX < 0.f) {

                    if (levelGrid[rowMid][colLeft] == 'w')
                        posX -= velocityX;
                    else if (levelGrid[rowMid][colLeft] == 'b') {
                        levelGrid[rowMid][colLeft] = 'd';
                    }
                }
            }
        }
    }

    void handleInput() override {

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            if (velocityX > 0)
            {
                velocityX = 0;
            }
            velocityX -= accel;
            if (velocityX < -maxSpeed)
                velocityX = -maxSpeed;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            if (velocityX < 0)
            {
                velocityX = 0;
            }
            velocityX += accel;
            if (velocityX > maxSpeed)
                velocityX = maxSpeed;
        }
        else {
            if (velocityX > 0.f) {
                velocityX -= accel;
                if (velocityX < 0.f) velocityX = 0.f;
            }
            else if (velocityX < 0.f) {
                velocityX += accel;
                if (velocityX > 0.f) velocityX = 0.f;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && isOnGround) {
            velocityY = -20.f;
            isOnGround = false;
            jumpSound.play();
        }
    }

    void update(char** levelGrid, bool isControlled, float dt, Character* leader, const int& curLevel) override {
        Character::update(levelGrid, isControlled, dt, leader, curLevel);
        if (isControlled)
            handleInput();




        horizontalCollision(levelGrid);
        //posX += velocityX;
        //if (velocityX != 0.f) {
        //    float midY = posY + spriteHeight * 0.5f;
        //    int rowMid = int(midY / GameConstants::CELL_SIZE);
        //    int colLeft = int((posX + hitboxOffsetX) / GameConstants::CELL_SIZE);
        //    int colRight = int((posX + spriteWidth - hitboxOffsetX) / GameConstants::CELL_SIZE);
        //    if (rowMid >= 0 && rowMid < 14) {
        //        if (velocityX > 0 && levelGrid[rowMid][colRight] == 'w') posX -= velocityX;
        //        if (velocityX < 0 && levelGrid[rowMid][colLeft] == 'w') posX -= velocityX;
        //    }
        //}

 
        float nextY = posY + velocityY;
        hb.setPosition(posX + hitboxOffsetX, nextY + hitboxOffsetY);
        if (velocityY >= 0 && Platform::checkCollision(hb, levelGrid, groundWidth)) {
            isOnGround = true;
            velocityY = 0.f;
            int landedRow = int(hb.bottom() / GameConstants::CELL_SIZE);
            posY = landedRow * GameConstants::CELL_SIZE - (spriteHeight - hitboxOffsetY);
        }
        else {
            isOnGround = false;
            posY = nextY;
            velocityY = min(velocityY + gravityAcceleration, terminalVelocity);
        }






        if (hanging && !onGround())
        {
            cout << velocityX << endl;
            current = (velocityX < 0 ? &hangL : &hangR);
        }
        else if (!onGround())
            current = (velocityX < 0 ? &atkL : &atkR);
        else if (std::fabs(velocityX) > movementSpeed * 1.5f)
            current = (velocityX < 0 ? &runL : &runR);
        else if (std::fabs(velocityX) > 0.f)
            current = (velocityX < 0 ? &walkL : &walkR);
        else {
            if (current == &walkL || current == &runL || current == &atkL || current == &hangL)
                current = &stillL;

            else if (current == &walkR || current == &runR || current == &atkR || current == &hangR)
                current = &stillR;


        }



        current->setPosition(posX, posY);
        current->update(dt);
    }

    void draw(sf::RenderWindow& win, float offsetX) override {
        hb.shape.setPosition(posX + hitboxOffsetX - offsetX, posY + hitboxOffsetY);
        //win.draw(hb.shape);
        current->draw(win, offsetX);
    }
};





















