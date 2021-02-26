#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
//
// Actor Class
//	 Members of this class:
//
class Actor : public GraphObject
{
public:
	Actor(int imageID, double x, double y, int dir, double size, int depth, StudentWorld* world);
	virtual void doSomething() = 0;
	// Functions that are NOT going to be overwritten
	double getHorizSpeed() const { return m_horizSpeed; };
	double getVertSpeed() const { return m_vertSpeed; };
	bool isAlive() const { return m_alive; };
	StudentWorld* getWorld() const { return m_world; };
	void setDead() { m_alive = false; };
	void setHorizSpeed(double hs) { m_horizSpeed = hs; };
	void setVertSpeed(double vs) { m_vertSpeed = vs; };
	bool doesOverlap(Actor* other) const;
	bool outOfBounds() const;
	// Functions that WILL be overwritten
	virtual bool isCollisionAvoidanceWorthy() const { return false; };
	virtual bool isAffectedByProjectile() const { return false; };
	virtual void interactWithProjectile() {};
	void move();
private:
	StudentWorld* m_world;
	bool m_alive;
	double m_horizSpeed;
	double m_vertSpeed;
};
//
// Sentient Class
//	 Derived from Actor Class
//	 Members of this class: 
//		-Have health
//		-Can take damage
//		-Are collision avoidance worthy
//
class Sentient : public Actor
{
public:
	Sentient(int imageID, double x, double y, int dir, double size, int depth, StudentWorld* world, int health);
	int getHealth() { return m_health; };
	void changeHealth(int h) { m_health += h; };
	virtual bool isCollisionAvoidanceWorthy() const { return true; };
private:
	int m_health;
};
//
// Player Class
//
class Player : public Sentient
{
public:
	Player(StudentWorld* world);
	virtual void doSomething();
	void shootHolyWater();
	void refillSprays(int n) { m_spraysLeft += n; };
	int getSprays() const { return m_spraysLeft; };
	void spin();
private:
	int m_spraysLeft;
};
//
// Zombie Cab Class
//
class ZombieCab : public Sentient
{
public:
	ZombieCab(double x, double y, double speed, int lane, StudentWorld* world);
	virtual bool isAffectedByProjectile() const { return true; };
	virtual void interactWithProjectile();
	virtual void doSomething();
	void setLane(int lane) { m_lane = lane; };
	int getLane() const { return m_lane; };
private:
	bool m_damagedPlayer;
	int m_lane;
	int m_movementPlanDistance;
};
//
// Pedestrian Class
//	 Derived from the Sentient class
//	 Members of this class:
//		-Move in a particular way
//		-Have movement plan distance
//
class Pedestrian : public Sentient
{
public:
	Pedestrian(int imageID, double x, double y, int size, StudentWorld* world);
	virtual bool isAffectedByProjectile() const { return true; };
	int getMovementPlanDistance() const { return m_movementPlanDistance; };
	void setMovementPlanDistance(int d) { m_movementPlanDistance = d; };
	void updateMovementPlan();
private:
	int m_movementPlanDistance;
};
//
// Human Pedestrian Class
//
class HumanPedestrian : public Pedestrian
{
public:
	HumanPedestrian(double x, double y, StudentWorld* world);
	virtual void doSomething();
	virtual void interactWithProjectile();
private:
};
//
// Zombie Pedestrian Class
//
class ZombiePedestrian : public Pedestrian
{
public:
	ZombiePedestrian(double x, double y, StudentWorld* world);
	virtual void doSomething();
	virtual void interactWithProjectile();
private:
	int m_tickToGrunt;
};
//
// Activated Object class
//	 Members of this class:
//		-Move in a particular way
//		-Interact with the player upon contact
//
class ActivatedObject : public Actor
{
public:
	ActivatedObject(int imageID, double x, double y, int size, StudentWorld* world);
	virtual void doSomething();
	// Initialise default behaviours for the activated objects (can/will be overridden by derived classes)
	virtual bool doesRotateValue() const { return false; };
	virtual bool spinPlayerValue() const { return false; };
	virtual bool selfDestructsValue() const { return true; };
	virtual int getHealthValue() const { return 0; };
	virtual int getSpraysValue() const { return 0; };
	virtual int getScoreValue() const { return 0; };
	virtual int getSoundValue() const { return SOUND_GOT_GOODIE; };
	void interactWithPlayer();
	void interactWithProjectile() { setDead(); };
private:
};
//
// Oil Slick Class
//
class OilSlick : public ActivatedObject
{
public:
	OilSlick(double x, double y, StudentWorld* world);
	virtual bool selfDestructsValue() const { return false; };
	virtual int getSoundValue() const { return SOUND_OIL_SLICK; };
	virtual bool spinPlayerValue() const { return true; };
private:
};
//
// Healing Goodie Class
//
class HealingGoodie : public ActivatedObject
{
public:
	HealingGoodie(double x, double y, StudentWorld* world);
	virtual int getHealthValue() const { return 10; };
	virtual int getScoreValue() const { return 250; };
	virtual bool isAffectedByProjectile() const { return true; };
private:
};
//
// Holy Water Goodie Class
//
class HolyWaterGoodie : public ActivatedObject
{
public:
	HolyWaterGoodie(double x, double y, StudentWorld* world);
	virtual int getSpraysValue() const { return 10; };
	virtual int getScoreValue() const { return 50; };
	virtual bool isAffectedByProjectile() const { return true; };
private:
};
//
// Soul Goodie Class
//
class SoulGoodie : public ActivatedObject
{
public:
	SoulGoodie(double x, double y, StudentWorld* world);
	virtual int getSoundValue() const { return SOUND_GOT_SOUL; };
	virtual int getScoreValue() const { return 100; };
	virtual bool doesRotateValue() const { return true; };
private:
};
//
// Projectile Class
//
class Projectile : public Actor
{
public:
	Projectile(double x, double y, int dir, StudentWorld* world);
	virtual void doSomething();
private:
	double m_distTraveled;
};
//
// Border Line Class
//
class BorderLine : public Actor
{
public:
	BorderLine(double x, double y, int imageID, StudentWorld* world);
	virtual void doSomething();
private:
};
#endif // ACTOR_H_
