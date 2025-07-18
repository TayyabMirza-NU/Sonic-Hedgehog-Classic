#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "level.h"
#include "Enemy.h"
#include "Collectable.h"

#include<iostream>
using namespace sf;
using namespace std;




class TileMap {


public:
   
    char**& populate(char**& grid, int height, int width, int levelId) {
        const char l1Grid[14][200] = {
             "                                                                                                                                                                                                       ",
             "                                                                                                                                                                                                       ",
             "                                              ppppp                                                                                                                                                    ",
             "                                              b   b                                                                                                                    pppppp                          ",
             "                                              b   b                                                                                                                                                    ",
             "                              w         pppp  ppppp                                     pppp                w                                                                                          ",
             "                              w                                                                             w                              wwwwwwwwwwwwww                                              ",
             "                    pppp  ppppw                                     pppp  pppp                              b                              bbbbbbbbbbbbww                                              ",
             "                              w                                                                      ppp    b                              bbbbbbb     ww                                              ",
             "   pppp                       w                             ppp                                   ppp       bppp  pppp                     bbbbbbb     ww            ppppp       ppppp                ",
             "                              w                                                                             w                              bbbbbbb     ww                                         w w  ",
             "                              w                                                                             w                              bbbbbbb     ww                                        w w w ",
             "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwww     wwwwwwwwwwwwwwwwwwwwwwwwwwww   wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww     wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww",
             "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeee     eeeeeeeeeeeeeeeeeeeeeeeeeeee   eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee     eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
        };
        const char l2Grid[14][250] = {
          "                                                                                                                                                                                                        ppppp                                           ",
          "                                                                                                                                                                                                        b   b                         b                 ",
          "                                              ppppp                                                                                                                                                     b   b                         b                 ",
          "                                                                                                                                                                       pppppp                           b   b                         b                 ",
          "                                                                      w                                                                                                                                 ppppp                         b                 ",
          "                                       pppp  ppppp                  w                    bbbbb  pppp                                                                                                    w   w                         b                 ",
          "                                                                  w                                                                         wwwwwwwwwwwwww                                              b   b                         b                 ",
          "                                                                w    pppp  pppp                                                             eeeeeeeeeeeeee                                              ppppp                   ppppppppppp             ",
          "                                                              w                                              b                                                                                          w   w                   eeeeeeeeeee             ",
          "                                                           w                                            wppp  pppp                                               ppppp       ppppp                      b   b                                           ",
          "                                                                                                           w                                                                                       w w  ppppp                                           ",
          "                                                                                                           w                                                                                      w w w w   w                                           ",
          "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwww     wwwwwwwwwwwwwwwwwwwwwwwwwwww   wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww     wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww",
          "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeee     eeeeeeeeeeeeeeeeeeeeeeeeeeee   eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee     eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
        };


        const char l3Grid[14][300] = {
        "                                                                                                                                                                                                       ppppp                         pppppp                                                                ",
        "                                                                                                                                                                                                       b   b                         b   b                                                                 ",
        "                                              ppppppppppppp                                                                                                                                                                          w   w                                                                 ",
        "                                              b   b                                                                                                                    pppppp                          b   b                         b   b                                                                 ",
        "                                              b   b                                                                                                                                                    ppppp                         ppppp                                                                 ",
        "                                              b                                   pppp  pppp                                                                                                                                         w   w           ppppppp        w             w                        ",
        "                                              b                                                               w                              wwwwwwwwwwwwww                                                                          b   b           b     b         w             w                       ",
        "                          wwwww               b                        pppp  pppp                             b                              bbbbbbbbbbbbww                                    wwwww                                 ppppp            b   b           w     pppp    w                      ",
        "                                              b                                                               b                                                                                w   w                                 w   w             b b             w             w                     ",
        "                                              b             ppp                                             wppp  pppp                        ww            ppppp       ppppp                  e   e                                 e   e              b               w             w                    ",
        "                                               b                                                           w                                                                              w w  ppppp                                 ppppp              b                w             w                   ",
        "                                                b                                                         w                                 ww                                           w w w w   w                                 w   w             b  b               w          wwww                  ",
        "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwww     wwwwwwwwwwwwwwwwwwwwwwwwwwww   wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww     wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww",
        "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeee     eeeeeeeeeeeeeeeeeeeeeeeeeeee   eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee     eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
        };

        const char l4Grid[14][20] = {
             "                   ",
             "                   ",
             "                   ",
             "                   ",
             "                   ",
             "                   ",
             "                   ",
             "                   ",
             "                   ",
             "                   ",
             "                   ",
             " wwwwwwwwwwwwwwwww ",
             "                   ",
             "                   ",
        };

        switch (levelId) {
        case 1:

            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    grid[i][j] = l1Grid[i][j];
                }
            }
            break;

        case 2:
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    grid[i][j] = l2Grid[i][j];
                }
            }
            break;

        case 3:
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    grid[i][j] = l3Grid[i][j];
                }
            }
        break;
        case 4:
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    grid[i][j] = l4Grid[i][j];
                }
            }
            break;

        default:

            for (int j = 0; j < width; ++j)
                if (j % 5 != 0)
                    grid[height - 6][j] = 'p';
      
            {
                int bx = (width / 2) - 3;
                if (bx >= 0 && bx < width)
                    grid[height - 6][bx] = 'b';
            }
            break;
        }

        return grid;
    }



    // Allocate and populate enemies for a given level
    Enemy** populateEnemies(int levelId, int& enemyCount) {
        switch (levelId) {
        case 1: enemyCount = 8; break;
        case 2: enemyCount = 12; break;
        case 3: enemyCount = 20; break;
        case 4: enemyCount = 1; break;
        default:
            enemyCount = 0;
            return nullptr;
        }

        Enemy** enemies = new Enemy * [enemyCount];

        int cols1[8] = { 10,20,40,45,60,70,90,120 };
        int rows1[8] = { 6, 3, 3, 10, 3, 2, 3, 10 };

        int cols2[12] = { 20,30,35,45,60,80,100,95, 120,140,170,220 };
        int rows2[12] = { 10,10,4,10,10,10,2,10 ,10,2,10 };

        int cols3[20] = { 10,20,30,40,50,60,65,70,80,95 ,110,170,220,250,280,310,320,400,450,500 };
        int rows3[20] = { 10,10,2,10,10,10,4,10,10,10 ,4,10,10,10,6,2,9,3,7,10 };

        int cols4 = 10;
        int rows4 = 4;

        for (int i = 0; i < enemyCount; ++i) {
            float x, y, minX, maxX, appearX;
            Enemy* e = nullptr;
            if (levelId == 1) {
                x = cols1[i] * GameConstants::CELL_SIZE;
                y = rows1[i] * GameConstants::CELL_SIZE;
                minX = x - 2 * GameConstants::CELL_SIZE;
                maxX = x + 2 * GameConstants::CELL_SIZE;
                appearX = x + GameConstants::CELL_SIZE;
                switch (i % 4) {
                case 0: e = new BatBrain(); break;
                case 1: e = new BeeBot();   break;
                case 2: e = new CrabMeat(); break;
                case 3: e = new Motobug(); break;
                }
            }
            else if (levelId == 2) {
                x = cols2[i] * GameConstants::CELL_SIZE;
                y = rows2[i] * GameConstants::CELL_SIZE;
                minX = x - 3 * GameConstants::CELL_SIZE;
                maxX = x + 3 * GameConstants::CELL_SIZE;
                appearX = x;
                switch (i % 4) {
                case 0: e = new CrabMeat(); break;
                case 1: e = new Motobug();   break;
                case 2: e = new BeeBot(); break;
                case 3: e = new CrabMeat(); break;
                }
            }
            else if (levelId == 3) {
                x = cols3[i] * GameConstants::CELL_SIZE;
                y = rows3[i] * GameConstants::CELL_SIZE;
                minX = x - 3 * GameConstants::CELL_SIZE;
                maxX = x + 3 * GameConstants::CELL_SIZE;
                appearX = x;
                switch (i % 4) {
                case 0: e = new CrabMeat(); break;
                case 1: e = new Motobug();   break;
                case 2: e = new BeeBot(); break;
                case 3: e = new CrabMeat(); break;
                }
            }
            else if (levelId = 4) {
                x = cols4*GameConstants::CELL_SIZE;
                y = rows4 * GameConstants::CELL_SIZE;
                minX = x - 4 * GameConstants::CELL_SIZE;
                maxX = x + 4 * GameConstants::CELL_SIZE;
                appearX = x + GameConstants::CELL_SIZE;
                e = new EggStinger; 
            }

            e->init(x, y,
                (levelId == 1 ? 80.f : levelId == 2 ? 90.f : 100.f),
                (levelId == 1 ? 10 : levelId == 2 ? 15 : 20),
                minX, maxX, appearX);
            enemies[i] = e;
        }
        cout << " 1 " << endl;
        return enemies;
    }

    Collectable** populateCollectables(int levelId, int& collectableCount) {
        switch (levelId) {
        case 1: collectableCount = 3; break;
        case 2: collectableCount = 5; break;
        case 3: collectableCount = 7; break;
        default:
            collectableCount = 0;
            return nullptr;
        }

        Collectable** items = new Collectable * [collectableCount];
        int cols1[3] = { 12,28,48 };
        int rows1[3] = { 2,3,1 };
        int cols2[5] = { 20,34,44,60,70 };
        int rows2[5] = { 1,2,3,2,1 };
        int cols3[7] = { 10,22,35,45,55,65,75 };
        int rows3[7] = { 3,2,4,1,3,2,1 };

        for (int i = 0; i < collectableCount; ++i) {
            float x, y;
            if (levelId == 1) {
                x = cols1[i] * GameConstants::CELL_SIZE;
                y = rows1[i] * GameConstants::CELL_SIZE;
            }
            else if (levelId == 2) {
                x = cols2[i] * GameConstants::CELL_SIZE;
                y = rows2[i] * GameConstants::CELL_SIZE;
            }
            else {
                x = cols3[i] * GameConstants::CELL_SIZE;
                y = rows3[i] * GameConstants::CELL_SIZE;
            }

            Collectable* c = nullptr;
            int type = i % 3;
            if (type == 0)               c = new RingCollectable();
            else if (type == 1)          c = new ExtraBoostCollectable();
            else                          c = new ExtraLifeCollectable();

            c->init(x, y);
            items[i] = c;
        }
        cout << "2" << endl;
        return items;
    }
};

