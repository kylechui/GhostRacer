#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath) : 
    GameWorld(assetPath),
    m_player(nullptr),
    m_soulsSaved(0),
    m_deadPed(false),
    m_bonus(5000)
{
    m_distSinceLastWhiteBorderLine = 0;
}
StudentWorld::~StudentWorld()
{
    cleanUp();
}
int StudentWorld::init()
{
    // Initialize some constants
    m_soulsSaved = 0;
    m_deadPed = false;
    m_bonus = 5000;
    // Create the player
    m_player = new Player(this);
    addActor(m_player);
    // Add the yellow border lines
    for (int i = 0; i < n_borderLine; i++)
    {
        addActor(new BorderLine(LEFT_EDGE, 1.0 * i * SPRITE_HEIGHT, IID_YELLOW_BORDER_LINE, this));
        addActor(new BorderLine(RIGHT_EDGE, 1.0 * i * SPRITE_HEIGHT, IID_YELLOW_BORDER_LINE, this));
    }
    // Add the white border lines
    for (int i = 0; i < m_borderLine; i++)
    {
        addActor(new BorderLine(LEFT_EDGE + ROAD_WIDTH / 3.0, i * 4.0 * SPRITE_HEIGHT, IID_WHITE_BORDER_LINE, this));
        addActor(new BorderLine(RIGHT_EDGE - ROAD_WIDTH / 3.0, i * 4.0 * SPRITE_HEIGHT, IID_WHITE_BORDER_LINE, this));
    }
    // Define the distance since the last white border line
    m_distSinceLastWhiteBorderLine = newBorderY - (m_borderLine - 1) * 4 * SPRITE_HEIGHT;
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // Iterate through all of the active actors
    vector<Actor*>::iterator it = allActors.begin();
    while (it != allActors.end())
    {
        if ((*it)->isAlive())
        {
            (*it)->doSomething();
        }
        if (!m_player->isAlive() || m_deadPed)
        {
            return GWSTATUS_PLAYER_DIED;
        }
        // If the player has beaten the level, add the bonus and return
        if (beatenLevel())
        {
            increaseScore(m_bonus);
            return GWSTATUS_FINISHED_LEVEL;
        }
        it++;
    }
    // Deletes dead objects
    it = allActors.begin();
    while (it != allActors.end())
    {
        if (!((*it)->isAlive()))
        {
            delete* it;
            allActors.erase(it);
            it = allActors.begin();
        }
        else
            it++;
    }
    // Add border lines
    m_distSinceLastWhiteBorderLine += (4 + m_player->getVertSpeed());
    if (m_distSinceLastWhiteBorderLine >= SPRITE_HEIGHT)
    {
        addActor(new BorderLine(LEFT_EDGE, newBorderY, IID_YELLOW_BORDER_LINE, this));
        addActor(new BorderLine(RIGHT_EDGE, newBorderY, IID_YELLOW_BORDER_LINE, this));
    }
    if (m_distSinceLastWhiteBorderLine >= 4.0 * SPRITE_HEIGHT)
    {
        addActor(new BorderLine(LEFT_EDGE + ROAD_WIDTH / 3.0, newBorderY, IID_WHITE_BORDER_LINE, this));
        addActor(new BorderLine(RIGHT_EDGE - ROAD_WIDTH / 3.0, newBorderY, IID_WHITE_BORDER_LINE, this));
        m_distSinceLastWhiteBorderLine = 0;
    }
    // Add oil slicks
    int chanceOilSlick = max(150 - getLevel() * 10, 40);
    if (randInt(0, chanceOilSlick - 1) == 0)
        addActor(new OilSlick(randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, this));
    // Add holy water goodies
    int chanceOfHolyWater = 100 + 10 * getLevel();
    if (randInt(0, chanceOfHolyWater - 1) == 0)
        addActor(new HolyWaterGoodie(randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, this));
    // Add holy water goodies
    int chanceOfLostSoul = 100;
    if (randInt(0, chanceOfLostSoul - 1) == 0)
        addActor(new SoulGoodie(randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, this));
    // Add human pedestrians
    int chanceHumanPed = max(200 - getLevel() * 10, 30);
    if (randInt(0, chanceHumanPed - 1) == 0)
        addActor(new HumanPedestrian(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this));
    // Add zombie pedestrians
    int chanceZombiePed = max(100 - getLevel() * 10, 20);
    if (randInt(0, chanceZombiePed - 1) == 0)
        addActor(new ZombiePedestrian(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this));
    // Add zombie cabs
    int chanceVehicle = max(100 - getLevel() * 10, 20);
    if (randInt(0, chanceVehicle - 1) == 0)
        addZombieCab();
    // Decrease the bonus
    m_bonus--;
    // Update display text
    ostringstream status;
    status << "Score: " << getScore();
    status << "  ";
    status << "Lvl: " << getLevel();
    status << "  ";
    status << "Souls2Save: " << getLevel() * 2 + 5 - m_soulsSaved;
    status << "  ";
    status << "Lives: " << getLives();
    status << "  ";
    status << "Health: " << m_player->getHealth();
    status << "  ";
    status << "Sprays: " << m_player->getSprays();
    status << "  ";
    status << "Bonus: " << m_bonus;
    setGameStatText(status.str());
    // Player has not died nor beaten the level, so continue
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::addZombieCab()
{
    bool checkedOne = false;
    bool checkedTwo = false;
    bool checkedThree = false;
    // Iterate through the three lanes
	int curLane = randInt(-1, 1);
    // While there's at least one lane that hasn't been checked yet
    while (!checkedOne || !checkedTwo || !checkedThree)
    {
        // Continually pick a new lane until you find one that hasn't been checked yet
        while ((curLane == -1 && checkedOne) || (curLane == 0 && checkedTwo) || (curLane == 1 && checkedThree))
			curLane = randInt(-1, 1);
        switch (curLane)
        {
        case -1:
            checkedOne = true;
            break;
        case 0:
            checkedTwo = true;
            break;
        case 1:
            checkedThree = true;
            break;
        default:
            break;
        }
        double leftBoundary = ROAD_CENTER + ROAD_WIDTH / 3.0 * curLane  - ROAD_WIDTH / 6.0;
        double rightBoundary = ROAD_CENTER + ROAD_WIDTH / 3.0 * curLane + ROAD_WIDTH / 6.0;
        // Iterate through all of the actors and find the ones closest to the top and bottom
        // in the desired lane
        vector<Actor*>::iterator it = allActors.begin();
        Actor* closestToBottom = nullptr;
        Actor* closestToTop = nullptr;
        while (it != allActors.end())
        {
            // Only bother checking the y coordinate if the actor is in the current lane
            // and collision avoidance-worthy
            if ((*it)->getX() >= leftBoundary && (*it)->getX() < rightBoundary && (*it)->isCollisionAvoidanceWorthy())
            {
                // Update the actors closest to the bottom and top of the lane, if necessary
                if (closestToBottom == nullptr || closestToTop == nullptr)
                {
                    closestToBottom = *it;
                    closestToTop = *it;
                }
                else
                {
                    if ((*it)->getY() < closestToBottom->getY())
                        closestToBottom = *it;
                    if ((*it)->getY() > closestToTop->getY())
                        closestToTop = *it;
                }
            }
            it++;
        }
        if (closestToBottom == nullptr || closestToBottom->getY() > VIEW_HEIGHT / 3)
        {
            addActor(new ZombieCab(ROAD_CENTER + ROAD_WIDTH / 3.0 * curLane, SPRITE_HEIGHT / 2.0, m_player->getVertSpeed() + randInt(2, 4), this));
            break;
        }
        if (closestToTop == nullptr || closestToTop->getY() < VIEW_HEIGHT * 2 / 3)
        {
            addActor(new ZombieCab(ROAD_CENTER + ROAD_WIDTH / 3.0 * curLane, VIEW_HEIGHT - SPRITE_HEIGHT / 2.0, m_player->getVertSpeed() - randInt(2, 4), this));
            break;
        }
    }
}
// The direction specifies whether to check in front or behind the cab,
// 1 is in front, -1 is behind
bool StudentWorld::actorInRangeOfCab(ZombieCab* cab, int dir)
{
    vector<Actor*>::iterator it = allActors.begin();
    // Define constants for the left and right edges of the cab's lane
	double leftBoundary = ROAD_CENTER + ROAD_WIDTH / 3.0 * cab->getLane() - ROAD_WIDTH / 6.0;
	double rightBoundary = ROAD_CENTER + ROAD_WIDTH / 3.0 * cab->getLane() + ROAD_WIDTH / 6.0;
    // Iterate through the actors
    while (it != allActors.end())
    {
        // If the actor is collision avoidance worthy
        // and not the zombie cab
        // and within 96 pixels of the cab in the indicated direction
        // and in the same lane as the cab, return true
        if ((*it)->isCollisionAvoidanceWorthy()
            && *it != cab
            && abs((*it)->getY() - (cab->getY() + 48.0 * dir)) <= 48
            && (*it)->getX() >= leftBoundary
            && (*it)->getX() < rightBoundary)
        {
            // Furthermore, if the object is in front of the cab return true
            // Otherwise return true only if the actor is not the player
            if (dir == 1 || (*it) != m_player)
                return true;
        }
        it++;
    }
    return false;
}

bool StudentWorld::checkProjectileCollision(Actor* proj)
{
    vector<Actor*>::iterator it = allActors.begin();
    while (it != allActors.end())
    {
        // Check that the object is affected by projectiles
        // and the objects overlap 
        if ((*it)->isAffectedByProjectile() && proj->doesOverlap(*it))
        {
            (*it)->interactWithProjectile();
            return true;
        }
        it++;
    }
    return false;
}

void StudentWorld::cleanUp()
{
    vector<Actor*>::iterator it = allActors.begin();
    while (it != allActors.end())
    {
        delete* it;
        allActors.erase(it);
        it = allActors.begin();
    }
}
