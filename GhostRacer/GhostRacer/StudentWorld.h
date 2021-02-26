#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

// Define the edges of the road
const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
// Border Line math for correct positioning
const double n_borderLine = VIEW_HEIGHT / SPRITE_HEIGHT;
const double m_borderLine = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
const int newBorderY = VIEW_HEIGHT - SPRITE_HEIGHT;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    // Based off of functions from GameWorld
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    // Add an actor to the vector
    void addActor(Actor* a) { allActors.push_back(a); };
    // Return a pointer to the player
    Player* getPlayer() { return m_player; };
    // Set m_deadPed to true if a pedestrian has been killed
    void pedKilled() { m_deadPed = true; };
    // Increment m_soulsSaved
    void soulSaved() { m_soulsSaved++; };
    // Check if the holy water projectile has hit anything, and act accordingly
    bool checkProjectileCollision(Actor* proj);
    bool actorInRangeOfCab(ZombieCab* cab, int dir);
private:
    bool beatenLevel() { return (m_soulsSaved >= getLevel() * 2 + 5); };
    void addZombieCab();
    Player* m_player;
    std::vector<Actor*> allActors;
    double m_distSinceLastWhiteBorderLine;
    int m_soulsSaved;
    bool m_deadPed;
    int m_bonus;
};

#endif // STUDENTWORLD_H_
