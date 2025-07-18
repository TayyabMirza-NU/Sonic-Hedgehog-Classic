#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include "Constants.h"
#include "Animation.h"
#include "Hitbox.h"
#include "Projectile.h"
#include <iostream>

using namespace sf;

class Enemy {
protected:
    float x = 0, y = 0;
    float vx = 0, vy = 0;

    float speed = 0;

    int health = 0;
    bool dead = false;
    float patrolMinX = 0, patrolMaxX = 0;

    float appearAtX = 0;
    bool activated = false;

    Music enemyDeadSound;
		

    Animation anim;
    Hitbox hitbox;

public:
    Enemy()
        : hitbox(GameConstants::CELL_SIZE, GameConstants::CELL_SIZE)
    {
        enemyDeadSound.openFromFile("../Data/destroy.wav");
        enemyDeadSound.setVolume(50.f);
    
    }

    virtual void init(float sx, float sy, float spd, int hp,float mn, float mx, float ax){
        x = sx; y = sy;
        speed = spd; vx = spd; vy = 0;

        patrolMinX = mn; patrolMaxX = mx;
        appearAtX = ax; dead = false; activated = false;

        anim.setPosition(x, y);
        hitbox.setSize(GameConstants::CELL_SIZE, GameConstants::CELL_SIZE);
        hitbox.setPosition(x, y);
    }

    virtual void update(float dt, float offsetX, char** grid, float PX)
    {
        if (!activated && (appearAtX - offsetX < GameConstants::SCREEN_X))
            activated = true;
        if (!activated || dead) return;
  
        x += vx * dt;
        if (x < patrolMinX) {
            x = patrolMinX;
            vx = speed;
        }
        else if (x > patrolMaxX) {
            x = patrolMaxX;
            vx = -speed; 
        }

        anim.setPosition(x, y);
        anim.update(dt);

        hitbox.setPosition(x, y);
    }

    virtual void render(float offsetX, RenderWindow& win)
    {
        if (!activated || dead) 
            return;

        anim.draw(win, offsetX);

        hitbox.shape.setPosition(hitbox.left(), hitbox.top());
     
    }

    virtual void takeDamage(int dmg)
    {
        health -= dmg;
        if (health <= 0) {
			enemyDeadSound.play();
            dead = true;
        }
    }

    bool isActivated() const { 
        return activated; 
    }

    bool isDead()      const { 
        return dead; 
    }

    float getX() const { 
        return x; 
    }

    float getY() const { 
        return y; 
    }

    const Hitbox& getHitbox() const { 
        return hitbox; 
    }
    float getAppearX() const { 
        return appearAtX; 
    }
    void setDead() {
        dead = true;
    }

    virtual ~Enemy() {}
};



class EggStinger : public Enemy {
    sf::Texture flyTex, diveTex;
    Animation flyAnim, diveAnim, flybackAnim;
    Animation* current = nullptr;

    float startY = 0.f;
    bool  isDiving = false;
    bool  divingDown = true;

    int NUM_COLS, NUM_ROWS;
    static constexpr float HALF_H = GameConstants::CELL_SIZE;

    float patrolL;
    float patrolR;
    bool movingRight = true;
    float lastDiveX = 0.f;

    int targetCol = -1;

public:
    EggStinger() {
        if (!flyTex.loadFromFile("../Data/finalbossfly.png"))
            std::cerr << "Failed to load flying texture" << std::endl;
        if (!diveTex.loadFromFile("../Data/finalbossGU.png"))
            std::cerr << "failed to load diving texture" << std::endl;

        flyAnim.init(flyTex, 115, 80, 4, 0.2f,2.0);
        diveAnim.init(diveTex, 95, 109, 2, 0.2f,2.0);
        flybackAnim.init(diveTex, 95, 127, 2, 0.2f,2.0);

        current = &flyAnim;
    }

    void init(float, float, float spd, int dmg, float, float, float) override {
        Enemy::init(0, GameConstants::CELL_SIZE + 2, spd, dmg, 0, 0, 0);
        activated = true;
        dead = false;
        health =20;
        x = GameConstants::SCREEN_X * 0.5f;
        y = startY = GameConstants::CELL_SIZE + 2;
        patrolL = 65;

        // reduce right limit by one cell (block)
        patrolR = GameConstants::SCREEN_X - GameConstants::CELL_SIZE * 2;
        movingRight = true;
        NUM_COLS = GameConstants::SCREEN_X / GameConstants::CELL_SIZE;
        NUM_ROWS = GameConstants::SCREEN_Y / GameConstants::CELL_SIZE;
        isDiving = false; divingDown = true;
        current->reset();

    }

    void update(float dt, float offsetX, char** grid, float) override {
        if (!activated || dead) return;

        const float halfCell = GameConstants::CELL_SIZE * 0.5f;

        if (!isDiving) {
            x += (movingRight ? speed : -speed) * dt;
            if (x < patrolL)  
                x = patrolL;
            if (x > patrolR) 
                x = patrolR;

            if ((movingRight && x >= patrolR) || (!movingRight && x <= patrolL)) {

                isDiving = true;
                divingDown = true;
                current = &diveAnim;
                current->reset();
                lastDiveX = x;
            }
        }
        else if (divingDown) {
            y += 100 * dt;

            float centerX = x + halfCell;
            int col = int(centerX / GameConstants::CELL_SIZE);
            int row = int((y + 128) / GameConstants::CELL_SIZE);

            if (col < 0) 
                col = 0;
            if (col >= NUM_COLS) 
                col = NUM_COLS - 1;
            if (row < 0) 
                row = 0;
            if (row >= NUM_ROWS) 
                row = NUM_ROWS - 1;

            bool broke = false;
            if (grid[row][col] == 'w') {
       
                grid[row][col] = ' ';
                y = row * GameConstants::CELL_SIZE - 128;
                broke = true;

                float middle = (patrolL + patrolR) * 0.5f;
                if (lastDiveX > middle) {
                    float newRight = patrolR - GameConstants::CELL_SIZE;
                    patrolR = (newRight > patrolL ? newRight : patrolL);
                    movingRight = false;
                }
                else {
                    float newLeft = patrolL + GameConstants::CELL_SIZE;
                    patrolL = (newLeft < patrolR ? newLeft : patrolR);
                    movingRight = true;
                }
            }
            if (broke || row == NUM_ROWS - 1) {
                divingDown = false;
                current = &flybackAnim;
                current->reset();
            }
        }
        else {
            y -= 80 * dt;
            if (y <= startY) {
                y = startY;
                isDiving = false;
                current = &flyAnim;
                current->reset();
            
            }
        }
        cout << x /64<< "     " << y /64<< endl;

   
        current->setPosition(x-64, y);
        current->update(dt);
        hitbox.setPosition(x + offsetX,y + GameConstants::CELL_SIZE
        );
    }


    void render(float offsetX, sf::RenderWindow& win) override {
        if (!activated || dead) return;
        current->draw(win, offsetX);
        Enemy::render(offsetX, win);
    }
};

// =====================================


// BatBrain
class BatBrain : public Enemy {
    Texture texFlyRight, texFlyLeft;
    Texture swoopD, swoopU;
    Animation flyAnimRight, flyAnimLeft;
    Sprite swoopSpriteD, swoopSpriteU;
    Animation* current;
    float startX;
    float hoverY;
    float patrolRange = 100.f;
    float patrolSpeed = 50.f;
    float swoopDepth = 200.f;
    bool goingRight = true;
    bool swooping = false;
    bool returning = false;

public:
    BatBrain() {
        health = 3;
        if (!texFlyRight.loadFromFile("../Data/batbrainF.gif"))
            std::cerr << "Error: batbrainF_right.gif failed to load\n";
        if (!texFlyLeft.loadFromFile("../Data/batbrainF.gif"))
            std::cerr << "Error: batbrainF_left.gif failed to load\n";

        flyAnimRight.init(texFlyRight, 42, 47, 3, 0.2f);
        flyAnimLeft.init(texFlyLeft, 42, 47, 3, 0.2f);

        if (!swoopD.loadFromFile("../Data/batbrainD.gif"))
            std::cerr << "Error: batbrainD.gif failed to load\n";
        swoopSpriteD.setTexture(swoopD);
        swoopSpriteD.setScale(2.5f, 2.5f);

        current = &flyAnimRight;
    }

    void init(float sx, float sy, float spd, int dmg,
        float mn, float mx, float ax) override {
        Enemy::init(sx, sy, spd, dmg, mn, mx, ax);
        startX = sx; hoverY = sy;
        goingRight = true; swooping = false; returning = false;
        current = &flyAnimRight; current->reset();
    }

    void update(float dt, float offsetX, char** grid, float PX) override {
        Enemy::update(dt, offsetX, grid, PX);
        if (!activated || dead) return;

        float px = PX;
        float distX = std::fabs(px - x);
        if (!swooping && !returning && distX < 200.f)
            swooping = true;

        if (swooping) {
            y += speed * dt * 2.5f;
            if (y >= hoverY + swoopDepth) {
                y = hoverY + swoopDepth;
                swooping = false; returning = true;
            }
        }
        else if (returning) {
            y -= speed * dt * 2.5f;
            if (y <= hoverY) {
                y = hoverY; returning = false;
            }
        }
        else {
            if (goingRight) {
                x += patrolSpeed * dt;
                if (x >= startX + patrolRange) 
                    goingRight = false;
            }
            else {
                x -= patrolSpeed * dt;
                if (x <= startX - patrolRange) 
                    goingRight = true;
            }
        }

        if (!swooping && !returning) {
            current = goingRight ? &flyAnimRight : &flyAnimLeft;
            current->setPosition(x, y);
            current->update(dt);
        }
        else {
            swoopSpriteD.setPosition(x - offsetX, y);
        }
    }

    void render(float offsetX, sf::RenderWindow& win) override {
        if (!activated || dead) 
            return;
        if (!swooping && !returning)
            current->draw(win, offsetX);
        else
            win.draw(swoopSpriteD);

        Enemy::render(offsetX, win);
    }
};


class BeeBot : public Enemy {

    Texture texFlyRight, texFlyLeft;
    Animation flyRightAnim, flyLeftAnim;

    Texture texAttackRight, texAttackLeft;
    Animation attackRightAnim, attackLeftAnim;

    int damage = 0;
    float baseY;
    static const int MAX_SHOTS{8};
    float attackDir = 1.0f;
    float shootTimer = 0.f;
    const float shootInterval = 1.f;
    float attackTime = 0.f;
    const float attackDuration = 0.5f;
    bool attacking = false;

    sf::Texture projTex;
    Projectile* shots[MAX_SHOTS];

public:
    BeeBot() {
        health = 5;
        if (!texFlyRight.loadFromFile("../Data/beebotFR.gif"))
            std::cerr << "Failed to load beebotFR.gif" << std::endl;
        flyRightAnim.init(texFlyRight, 45, 42, 2, 0.2f);

        if (!texFlyLeft.loadFromFile("../Data/beebotFL.gif"))
            std::cerr << "Failed to load beebotFL.gif" << std::endl;
        flyLeftAnim.init(texFlyLeft, 56, 40, 4, 0.1f);

        if (!texAttackRight.loadFromFile("../Data/beebotFLA.gif"))
            std::cerr << "Failed to load beebotFLA.gif" << std::endl;
        attackRightAnim.init(texAttackRight, 40, 54, 2, 0.2f);

        if (!texAttackLeft.loadFromFile("../Data/beebotFRA.gif"))
            std::cerr << "Failed to load beebotFRA.gif" << std::endl;
        attackLeftAnim.init(texAttackLeft, 70, 67, 2, 0.2f);

        if (!projTex.loadFromFile("../Data/beebotAB.gif"))
            std::cerr << "Failed to load bee_shot.png" << std::endl;

        for (int i = 0; i < MAX_SHOTS; ++i)
            shots[i] = nullptr;
    }

    void init(float sx, float sy, float spd, int dmg, float mn, float mx, float ax) override {
        Enemy::init(sx, sy, spd * 1.2f, dmg, mn, mx, ax);
        shootTimer = shootInterval;
		attackDir = 1.f; 
    
        if (x >= patrolMaxX) {
            vx = -speed; 
        }
        else if (x <= patrolMinX) {
            vx = speed; 
        }
        else {
            vx = speed; 
        }
        damage = dmg;
        baseY = sy;
    }


    void update(float dt, float offsetX, char** grid, float PX) override {
        Enemy::update(dt, offsetX, grid, PX);
        if (!activated || dead) return;

     
        x += vx * dt;
        if (x < patrolMinX) { x = patrolMinX; vx = speed; }
        else if (x > patrolMaxX) { x = patrolMaxX; vx = -speed; }
        y = baseY + sin(x * 0.01f) * 50.f;

      
        shootTimer += dt;
        if (shootTimer >= shootInterval) {
            shootTimer = 0.f;
            attacking = true;
            attackTime = 0.f;
            attackDir = (vx >= 0) ? 1.f : -1.f; 
        }

        if (attacking && attackTime < dt) {
            for (int i = 0; i < MAX_SHOTS; ++i) {
                if (!shots[i]) {
                    float spawnX = x + 56.f; 
                    float spawnY = y + 65.f; 

                    shots[i] = new Projectile(
                        &projTex,
                        spawnX,
                        spawnY,
                        attackDir * 500.f,  
                        125.f,            
                        damage
                    );
                    break;
                }
            }
        }

        if (attacking) {
            attackTime += dt;
            if (attackTime >= attackDuration)
                attacking = false;
        }

        for (int i = 0; i < MAX_SHOTS; ++i) {
            if (shots[i]) {
                shots[i]->update(dt);
                if (!shots[i]->isAlive()) {
                    delete shots[i];
                    shots[i] = nullptr;
                }
            }
        }
	
 
        if (attacking) {
            if (attackDir > 0) {
                attackRightAnim.setPosition(x - (45.f - 40.f), y);
                attackRightAnim.update(dt);
            }
            else {
                attackLeftAnim.setPosition(x - (70.f - 56.f), y);
                attackLeftAnim.update(dt);
            }
        }
        else {
            if (vx > 0) {
                flyRightAnim.setPosition(x, y);
                flyRightAnim.update(dt);
            }
            else {
               
                flyLeftAnim.setPosition(x, y);
                flyLeftAnim.update(dt);
            }
        }
    }

	Projectile* getShot(int i) {
		return shots[i];
	}


    void render(float offsetX, RenderWindow& win) override {
        if (!activated || dead) return;
        if (attacking) {
            if (attackDir > 0) 
                attackRightAnim.draw(win, offsetX);
            else
                attackLeftAnim.draw(win, offsetX);
        }
        else {
            if (vx >= 0)
                flyRightAnim.draw(win, offsetX);
            else
                flyLeftAnim.draw(win, offsetX);
        }
        Enemy::render(offsetX, win);


        for (int i = 0; i < MAX_SHOTS; ++i) {
            if (shots[i] != nullptr && shots[i]->isAlive()) {
                shots[i]->render(offsetX, win);
            }
        }
    }

    ~BeeBot() {
        for (int i = 0; i < MAX_SHOTS; ++i)
            delete shots[i];
    }

    
};



// Motobug
class Motobug : public Enemy {
    sf::Texture texRight, texLeft;
    Animation walkRightAnim, walkLeftAnim;
    Animation* current;
public:
    Motobug() {
        health = 4;
        if (!texRight.loadFromFile("../Data/motobugMR.gif")) 
            std::cerr << "motobugMR fail" << std::endl;
        if (!texLeft.loadFromFile("../Data/motobugML.gif")) 
            std::cerr << "motobugML fail" << std::endl;
        walkRightAnim.init(texRight, 43, 40, 4, 0.2f);
        walkLeftAnim.init(texLeft, 49, 39, 4, 0.2f);
        current = &walkRightAnim;
    }

    void init(float sx, float sy, float spd, int dmg, float mn, float mx, float ax) override {
        Enemy::init(sx, sy, spd, dmg, mn, mx, ax);
        current = &walkRightAnim;
        hitbox.setSize(GameConstants::CELL_SIZE * 2, GameConstants::CELL_SIZE * 2);
    }

    void update(float dt, float offsetX, char** grid, float PX) override {
        Enemy::update(dt, offsetX, grid, PX);
        if (!activated || dead) 
            return;

        if (fabs(PX - x) > 100.f) { vx = (PX > x ? speed : -speed); x += vx * dt; }

        Animation* next = (vx >= 0) ? &walkRightAnim : &walkLeftAnim;
        if (next != current) { next->reset(); current = next; }
        current->setPosition(x, y);
        current->update(dt);
    }

    void render(float offsetX, RenderWindow& win) override {
        if (!activated || dead)
            return;
        current->draw(win, offsetX);
        Enemy::render(offsetX, win);
    }
};


class CrabMeat : public Enemy {
    Texture tex;
    Animation walkAnim;
    float shootTimer = 0.f;
public:
    CrabMeat() {
        health = 2;
        if (!tex.loadFromFile("../Data/crabmeatM.gif")) std::cerr << "crab_meat fail" << endl;
        walkAnim.init(tex, 55, 44, 3, 0.15f, 3);
    }


    void init(float sx, float sy, float spd, int dmg, float mn, float mx, float ax) override {
        Enemy::init(sx, sy, spd, dmg, mn, mx, ax);
        hitbox.setSize(GameConstants::CELL_SIZE * 2, GameConstants::CELL_SIZE * 2);
    }

    void update(float dt, float offsetX, char** grid, float PX) override {
        Enemy::update(dt, offsetX, grid, PX);

        if (!activated || dead) 
            return;
        x += vx * dt;
        if (x < patrolMinX || x > patrolMaxX) 
            vx = -vx;
        shootTimer += dt;
        if (shootTimer >= 3.f) { 
            shootTimer = 0.f;
        }
        walkAnim.setPosition(x, y);
        walkAnim.update(dt);
    }

    void render(float offsetX, sf::RenderWindow& win) override {
        if (!activated || dead) return;
        walkAnim.draw(win, offsetX);
        Enemy::render(offsetX, win);
    }
};

