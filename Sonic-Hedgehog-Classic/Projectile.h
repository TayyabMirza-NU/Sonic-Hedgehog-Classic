#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "Hitbox.h"

class Projectile {
private:
    Sprite sprite;
    Texture* texture;
    float x, y;
    float vx, vy;
    float lifeTimer = 0.f;
    const float lifeMax = 2.0f;
    bool alive = true;
    int damage;

    Hitbox hitbox;    

public:
    Projectile(sf::Texture* tex,
        float startX, float startY,
        float velX, float velY,
        int dmg)
        : texture(tex), x(startX), y(startY),
        vx(velX), vy(velY), damage(dmg),
        hitbox(GameConstants::CELL_SIZE / 2.0f, GameConstants::CELL_SIZE / 2.0f)  
    {
        sprite.setTexture(*texture);
        sprite.setPosition(x, y);
        sprite.setScale(1.0f, 1.0f);

    
        hitbox.setPosition(x, y);
    }

    void update(float dt) {
 
        x += vx * dt;
        y += vy * dt;
        sprite.setPosition(x, y);

     
        hitbox.setPosition(x, y);

       
        lifeTimer += dt;
        if (lifeTimer >= lifeMax)
            alive = false;
    }

    void render(float offsetX, sf::RenderWindow& win) {
    
        sprite.setPosition(x - offsetX, y);
        win.draw(sprite);
        sprite.setPosition(x, y);

       
        hitbox.shape.setPosition(hitbox.left(), hitbox.top());
     
    }

    bool isAlive() const { 
        return alive; 
    }
    int  getDamage() const { 
        return damage; 
    }

    const Hitbox& getHitbox() const {
        return hitbox;
    }
};
