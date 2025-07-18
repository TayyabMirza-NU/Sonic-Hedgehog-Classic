#pragma once
#include <SFML/Graphics.hpp>
#include<iostream>
using namespace std;

// ======================== Animation ========================
class Animation {
private:
    sf::Sprite sprite;
    sf::IntRect frames[15];
    int frameCount;
    int currentFrame;
    float frameTime;
    float elapsedTime;
    float scale;
public:
    Animation()
        : frameCount(0), currentFrame(0),
        frameTime(0.1f), elapsedTime(0),
        scale(2.5f) {
    }

    void init(sf::Texture& tex, int fw, int fh, int count, float tpf, float scale = 2.5f, int offsetX = 0) {
        sprite.setTexture(tex);
        sprite.setScale(scale, scale);
        frameCount = count;
        for (int i = 0; i < count && i < 15; ++i)
            frames[i] = sf::IntRect(offsetX + i * fw, 0, fw, fh);
        frameTime = tpf;
        currentFrame = 0;
        elapsedTime = 0;
        sprite.setTextureRect(frames[0]);
    }

    void reset() {
        currentFrame = 0;
        elapsedTime = 0;
        if (frameCount > 0)
            sprite.setTextureRect(frames[0]);
    }

    void update(float dt) {
        elapsedTime += dt;
        if (elapsedTime >= frameTime && frameCount > 0) {
            elapsedTime -= frameTime;
            currentFrame = (currentFrame + 1) % frameCount;
            sprite.setTextureRect(frames[currentFrame]);
        }
    }
    int x, y;
    void setPosition(float x, float y) {
        sprite.setPosition(x, y);
		this->x = x;
		this->y = y;    
    }

    void setScale(float sx, float sy) {
        sprite.setScale(sx, sy);
    }
  
    void draw(sf::RenderWindow& win, float offsetX) {
        sprite.move(-offsetX, 0.f);
        win.draw(sprite);
      
    }
};