#pragma once

#include <SFML/Graphics.hpp>


using namespace sf;

struct Hitbox {
    float width{ 0 }, height{ 0 };
    RectangleShape shape;

    Hitbox() {
        shape.setFillColor(Color::Transparent);
        shape.setOutlineColor(Color::Red);
        shape.setOutlineThickness(1.f);
    }
    Hitbox(float w, float h)
        : width(w), height(h) {
        shape.setSize({ w, h });
        shape.setFillColor(Color::Transparent);
        shape.setOutlineColor(Color::Red);
        shape.setOutlineThickness(1.f);
    }

    void setSize(float w, float h) {
        width = w;
        height = h;
        shape.setSize({ w, h });
    }
    void setPosition(float x, float y) {
        shape.setPosition(x, y);
    }
    float left() const { 
        return shape.getPosition().x; 
    }
    float top()    const { 
        return shape.getPosition().y; 
    }
    float right()  const { 
        return left()+width; 
    }
    float bottom() const {
        return top()+height; 
    }
};

class Platform {

public:
    static bool checkCollision(const Hitbox& hb, char** grid,int width) {
        int leftCell = int(hb.left() / GameConstants::CELL_SIZE);
        int rightCell = int((hb.right() - 1) / GameConstants::CELL_SIZE);
        int bottomCell = int((hb.bottom() - 1) / GameConstants::CELL_SIZE);
      
        if (bottomCell < 0 || bottomCell >= 14)
            return false;
        for (int col = leftCell; col <= rightCell; ++col) {
            if (col < 0 || col >= width) 
                continue;

            if (grid[bottomCell][col] == 'w'|| grid[bottomCell][col] == 'p') 
                return true;
        }
        return false;
    }
    
    static bool checkFlyingCollision(Hitbox& Tails, Hitbox& other) {
        float L1 = Tails.left();
        float R1 = Tails.right();
        float T1 = Tails.top();
        float B1 = Tails.bottom();

        float L2 = other.left();
        float R2 = other.right();
        float T2 = other.top();
        float B2 = other.bottom();

        float tailsHeight = B1 - T1;
        float knucklesHeight = B2 - T2;

        float tailsLowerQuarterStart = B1 - (tailsHeight * 0.125f);  // Lower 12.5% of Tails
        float knucklesUpperQuarterEnd = T2 + (knucklesHeight * 0.125f); // Top 12.5% of Knuckles

        return (R1 > L2) && (L1 < R2) && (tailsLowerQuarterStart < knucklesUpperQuarterEnd) && (B1 > T2);
    }
    static bool checkEnemyCollision(const Hitbox& hb, const Hitbox& enemy) {

        float L1 = hb.left();
        float R1 = hb.right();
        float T1 = hb.top();
        float B1 = hb.bottom();

        float L2 = enemy.left();
        float R2 = enemy.right();
        float T2 = enemy.top();
        float B2 = enemy.bottom();

        if (R1 < L2 || L1 > R2 || B1 < T2 || T1 > B2) {
  
            return false;
        }

        return true; 
    }

};