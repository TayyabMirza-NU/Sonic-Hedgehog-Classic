#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "Animation.h"
#include "Hitbox.h"

class Collectable {
protected:
    float x, y;
    bool collected;
    Hitbox hitbox;
    int type;

public:
  
    Collectable(float w, float h)
        : x(0), y(0), collected(false), hitbox(w, h) {
    }
    virtual ~Collectable() {}

    
    virtual void init(float sx, float sy) {
        x = sx;
        y = sy;
        collected = false;
        hitbox.setPosition(x, y);
    }

    virtual void update(float dt, float offsetX, char** grid, float* PX) = 0;
    virtual void render(float offsetX, sf::RenderWindow& win) = 0;
    bool Collection(const Hitbox& character);
    int getType() {
        return type;
    }

    bool isCollected() const { return collected; }
    void setCollected(bool c) { collected = c; }
    const Hitbox& getHitbox() const { return hitbox; }
};

class RingCollectable : public Collectable {
    sf::Texture texMove, texCollect;
    Animation moveAnim, collectAnim;
    Animation* current;
    float collectTimer;
    

public:
    RingCollectable()
        : Collectable(GameConstants::CELL_SIZE / 1.5f, GameConstants::CELL_SIZE / 1.5f),
        collectTimer(0.f), current(nullptr)
    {
        type = 1;
        texMove.loadFromFile("../Data/ring.png");
        moveAnim.init(texMove, 16, 16, 4, 0.1f);
        texCollect.loadFromFile("../Data/ring.png");
        collectAnim.init(texCollect, 16, 16, 4, 0.1f);
    }

    void init(float sx, float sy) override {
        Collectable::init(sx, sy);
        collectTimer = 0.f;
        current = &moveAnim;
        hitbox.setSize(GameConstants::CELL_SIZE, GameConstants::CELL_SIZE);
        moveAnim.setPosition(x, y);
    }

    void update(float dt, float offsetX, char** , float* ) override {
        if (!collected) {
         
        }
        else {
            collectTimer += dt;
            current = &collectAnim;
        }
        current->setPosition(x, y);
        current->update(dt);
        hitbox.setPosition(x, y);
    }

    void render(float offsetX, sf::RenderWindow& win) override {
        current->draw(win, offsetX);

        hitbox.shape.setPosition(hitbox.left(), hitbox.top());
     
    }
};

class ExtraBoostCollectable : public Collectable {
    sf::Texture texMove, texCollect;
    Animation moveAnim, collectAnim;
    Animation* current;
    float collectTimer;

public:
    ExtraBoostCollectable()
        : Collectable(GameConstants::CELL_SIZE, GameConstants::CELL_SIZE),
        collectTimer(0.f), current(nullptr)
    {
        type = 2;
        texMove.loadFromFile("../Data/extraboost.png");
        moveAnim.init(texMove, 221, 215, 4, 0.1f);
        moveAnim.setScale(0.5f, 0.5f);
        texCollect.loadFromFile("../Data/boost_collect.png");
        collectAnim.init(texCollect, 20, 20, 4, 0.1f);
    }

    void init(float sx, float sy) override {
        Collectable::init(sx, sy);
        collectTimer = 0.f;
        current = &moveAnim;
        moveAnim.setPosition(x, y);
    }

    void update(float dt, float offsetX, char**, float* ) override {
        if (!collected) {

        }
        else {
            collectTimer += dt;
            current = &collectAnim;
        }
        current->setPosition(x, y);
        current->update(dt);
        hitbox.setPosition(x, y);
    }

    void render(float offsetX, sf::RenderWindow& win) override {
        current->draw(win, offsetX);
        hitbox.shape.setPosition(hitbox.left(), hitbox.top());
        
    }
};

class ExtraLifeCollectable : public Collectable {
    sf::Texture texMove, texCollect;
    Animation moveAnim, collectAnim;
    Animation* current;
    float collectTimer;

public:
    ExtraLifeCollectable()
        : Collectable(24.f, 24.f), collectTimer(0.f), current(nullptr)
    {
        type = 3;
        texMove.loadFromFile("../Data/extralife.png");
        moveAnim.init(texMove, 130, 124, 5, 0.1f);
        moveAnim.setScale(0.5f, 0.5f);
        texCollect.loadFromFile("../Data/life_collect.png");
        collectAnim.init(texCollect, 24, 24, 4, 0.1f);
    }

    void init(float sx, float sy) override {
        Collectable::init(sx, sy);
        collectTimer = 0.f;
        current = &moveAnim;
        moveAnim.setPosition(x, y);
    }

    void update(float dt, float offsetX, char** , float* ) override {
        if (!collected) {
          
        }
        else {
            collectTimer += dt;
            current = &collectAnim;
        }
        current->setPosition(x, y);
        current->update(dt);
        hitbox.setPosition(x, y);
    }

    void render(float offsetX, sf::RenderWindow& win) override {
        current->draw(win, offsetX);
        hitbox.shape.setPosition(hitbox.left(), hitbox.top());
        //win.draw(hitbox.shape);
    }
};
