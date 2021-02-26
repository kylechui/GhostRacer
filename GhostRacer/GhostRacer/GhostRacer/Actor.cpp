#define _USE_MATH_DEFINES
#include <cmath>
#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//
//	Actor member functions
//
Actor::Actor(int imageID, double x, double y, int dir, double size, int depth, StudentWorld* world) :
	GraphObject(imageID, x, y, dir, size, depth),
	m_world(world),
	m_alive(true),
	m_horizSpeed(0),
	m_vertSpeed(-4)
{
}
bool Actor::doesOverlap(Actor* other) const
{
	double delX = abs(this->getX() - other->getX());
	double delY = abs(this->getY() - other->getY());
	double totalRadius = this->getRadius() + other->getRadius();
	return (delX < (totalRadius * 0.25) && delY < (totalRadius * 0.6));
}
bool Actor::outOfBounds() const
{
	return (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT);
}
void Actor::move()
{
	double vertSpeed = m_vertSpeed - getWorld()->getPlayer()->getVertSpeed();
	double horizSpeed = m_horizSpeed;
	double newY = getY() + vertSpeed;
	double newX = getX() + horizSpeed;
	moveTo(newX, newY);
}
//
// Sentient member functions
//
Sentient::Sentient(int imageID, double x, double y, int dir, double size, int depth, StudentWorld* world, int health) :
	Actor(imageID, x, y, dir, size, depth, world),
	m_health(health)
{
}
//
// Player member functions
//
Player::Player(StudentWorld* world) :
	Sentient(IID_GHOST_RACER, 128, 32, 90, 4, 0, world, 100),
	m_spraysLeft(10)
{
	setVertSpeed(0);
}
void Player::doSomething()
{
	// Define keypress variable
	int ch;
	// If health is zero or less, return immediately
	if (getHealth() <= 0)
	{
		setDead();
		getWorld()->playSound(SOUND_PLAYER_DIE);
		return;
	}
	// If colliding with the left side
	if (getX() <= LEFT_EDGE)
	{
		if (getDirection() > 90)
		{
			changeHealth(-10);
		}
		setDirection(82);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	// If colliding with the right side
	else if (getX() >= RIGHT_EDGE)
	{
		if (getDirection() < 90)
		{
			changeHealth(-10);
		}
		setDirection(98);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	// Register key presses
	else if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_SPACE:
			if (m_spraysLeft > 0) shootHolyWater();
			break;
		// Movement
		case KEY_PRESS_LEFT:
			if (getDirection() < 114) setDirection(getDirection() + 8);
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() > 66) setDirection(getDirection() - 8);
			break;
		case KEY_PRESS_UP:
			if (getVertSpeed() < 5) setVertSpeed(getVertSpeed()+1);
			break;
		case KEY_PRESS_DOWN:
			if (getVertSpeed() > -1) setVertSpeed(getVertSpeed()-1);
			break;
		default:
			break;
		}
	}
	// Move the player
	double maxShiftPerTick = 4.0;
	int curDir = getDirection();
	double deltaX = cos(curDir * M_PI / 180) * maxShiftPerTick;
	double curX = getX();
	double curY = getY();
	moveTo(curX + deltaX, curY);
}
void Player::shootHolyWater()
{
	// Calculate where to spawn the holy water projectile
	double newX = getX() + SPRITE_HEIGHT * cos(getDirection() * M_PI / 180);
	double newY = getY() + SPRITE_HEIGHT * sin(getDirection() * M_PI / 180);
	// Add the projectile to the world and play the spray sound
	getWorld()->addActor(new Projectile(newX, newY, getDirection(), getWorld()));
	getWorld()->playSound(SOUND_PLAYER_SPRAY);
	m_spraysLeft--;
}
void Player::spin()
{
	// Spin when hitting an oil slick
	int spinAmount = randInt(5, 20);
	if (randInt(0, 1) == 0)
		spinAmount *= -1;
	setDirection(getDirection() + spinAmount);
	if (getDirection() < 60)
		setDirection(60);
	if (getDirection() > 120)
		setDirection(120);
}
//
// Zombie Cab member functions
//
ZombieCab::ZombieCab(double x, double y, double speed, int lane, StudentWorld* world) :
	Sentient(IID_ZOMBIE_CAB, x, y, 90, 4, 0, world, 3),
	m_damagedPlayer(false),
	m_lane(0),
	m_movementPlanDistance(0)
{
	setVertSpeed(speed);
}
void ZombieCab::interactWithProjectile()
{
	changeHealth(-1);
	if (getHealth() <= 0)
	{
		setDead();
		getWorld()->playSound(SOUND_VEHICLE_DIE);
		// Random one in five chance to spawn an oil slick upon death
		if (randInt(0, 4) == 0)
			getWorld()->addActor(new OilSlick(getX(), getY(), getWorld()));
		getWorld()->increaseScore(200);
		return;
	}
	else
		getWorld()->playSound(SOUND_VEHICLE_HURT);
}
void ZombieCab::doSomething()
{
	// Check if not alive
	if (!isAlive())
		return;
	// If not overlapping with the player, reset the damage boolean
	if (!doesOverlap(getWorld()->getPlayer()))
		m_damagedPlayer = false;
	// Otherwise, if the cab has not damaged the player, damage the player
	else if (!m_damagedPlayer)
	{
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		getWorld()->getPlayer()->changeHealth(-20);
		// Change the car's direction/speed based on whether it is to the left or right of the player
		// Check if it is to the left (or same X coordinate)
		if (getX() <= getWorld()->getPlayer()->getX())
		{
			setHorizSpeed(-5);
			setDirection(120 + randInt(0, 19));
		}
		// Otherwise, the zombie cab is to the right
		else
		{
			setHorizSpeed(5);
			setDirection(60 - randInt(0, 19));
		}
		m_damagedPlayer = true;
	}
	// Move the cab
	move();
	// Check if out of bounds
	if (outOfBounds())
		setDead();
	// If the cab is faster than the player
	// and there is a collision avoidance worthy actor within 96 pixels in front of it, slow down
	if (getVertSpeed() >= getWorld()->getPlayer()->getVertSpeed() && getWorld()->actorInRangeOfCab(this, 1))
	{
		setVertSpeed(getVertSpeed() - 0.5);
		return;
	}
	// If the cab is slower than the player
	// and there is a collision avoidance worthy actor within 96 pixels behind it, speed up
	if (getVertSpeed() <= getWorld()->getPlayer()->getVertSpeed() && getWorld()->actorInRangeOfCab(this, -1))
	{
		setVertSpeed(getVertSpeed() + 0.5);
		return;
	}
	// Decrement the cab's movement plan distance
	m_movementPlanDistance--;
	// Return if the movement plan distance is greater than zero
	if (m_movementPlanDistance > 0)
		return;
	// Pick a new movement plan for the cab, update cab's speed
	m_movementPlanDistance = randInt(4, 32);
	setVertSpeed(getVertSpeed() + randInt(-2, 2));
}
//
// Pedestrian member functions
//
Pedestrian::Pedestrian(int imageID, double x, double y, int size, StudentWorld* world) :
	Sentient(imageID, x, y, 0, size, 0, world, 2),
	m_movementPlanDistance(0)
{
}
void Pedestrian::updateMovementPlan()
{
	// Choose a new non-zero horizontal speed
	int potHorizSpeed = randInt(-3, 3);
	while (potHorizSpeed == 0)
		potHorizSpeed = randInt(-3, 3);
	setHorizSpeed(potHorizSpeed);
	// Choose a new movement plan distance
	setMovementPlanDistance(randInt(4, 32));
	// Update the pedestrian direction
	if (getHorizSpeed() < 0)
		setDirection(180);
	else
		setDirection(0);
}
//
// Human Pedestrian member functions
//
HumanPedestrian::HumanPedestrian(double x, double y, StudentWorld* world) :
	Pedestrian(IID_HUMAN_PED, x, y, 2, world)
{
}
void HumanPedestrian::doSomething()
{
	// Check if not alive
	if (!isAlive())
		return;
	// If there is a collision with the player
	if (doesOverlap(getWorld()->getPlayer()))
	{
		// Decrease the number of lives and tell the world that the player lost
		getWorld()->decLives();
		getWorld()->pedKilled();
		setDead();
		return;
	}
	// Move the human
	move();
	// Check if out of bounds
	if (outOfBounds())
	{
		setDead();
		return;
	}
	// Decrement the movement plan distance
	setMovementPlanDistance(getMovementPlanDistance() - 1);
	// If the distance is greater than zero, return
	if (getMovementPlanDistance() > 0)
		return;
	// Otherwise, pick a new movement plan
	else
		updateMovementPlan();
}
void HumanPedestrian::interactWithProjectile()
{
	setHorizSpeed(getHorizSpeed() * -1);
	setDirection(180 - getDirection());
	getWorld()->playSound(SOUND_PED_HURT);
}
//
// Zombie Pedestrian member functions
//
ZombiePedestrian::ZombiePedestrian(double x, double y, StudentWorld* world) :
	Pedestrian(IID_ZOMBIE_PED, x, y, 3, world),
	m_tickToGrunt(0)
{
}
void ZombiePedestrian::doSomething()
{
	// Check if not alive
	if (!isAlive())
		return;
	// Check for collision with the player
	if (doesOverlap(getWorld()->getPlayer()))
	{
		getWorld()->getPlayer()->changeHealth(-5);
		changeHealth(-2);
		if (getHealth() <= 0)
			setDead();
		return;
	}
	// Check if the zombie is in front of the player and in range
	double delX = getX() - getWorld()->getPlayer()->getX();
	if (abs(delX) <= 30 && getY() > getWorld()->getPlayer()->getY())
	{
		// Set the new direction of the zombie
		setDirection(270);
		// Make the zombie "chase" the player
		if (delX < 0)
			setHorizSpeed(1);
		else if (delX > 0)
			setHorizSpeed(-1);
		else
			setHorizSpeed(0);
		// Decrement the ticks left to grunt
		m_tickToGrunt--;
		// If the ticks left is non-positive, grunt and reset the counter to 20
		if (m_tickToGrunt <= 0)
		{
			getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			m_tickToGrunt = 20;
		}
	}
	// Move the zombie
	move();
	// Check if out of bounds
	if (outOfBounds())
	{
		setDead();
		return;
	}
	// If the distance is positive, decrement and return
	if (getMovementPlanDistance() > 0)
	{
		setMovementPlanDistance(getMovementPlanDistance() - 1);
		return;
	}
	// Otherwise pick a new movement plan
	else
	{
		updateMovementPlan();
	}
}
void ZombiePedestrian::interactWithProjectile()
{
	changeHealth(-1);
	if (getHealth() <= 0)
	{
		setDead();
		getWorld()->playSound(SOUND_PED_DIE);
		if (!doesOverlap(getWorld()->getPlayer()))
		{
			if (randInt(0, 4) == 0)
				getWorld()->addActor(new HealingGoodie(getX(), getY(), getWorld()));
		}
		getWorld()->increaseScore(150);
	}
	else
		getWorld()->playSound(SOUND_PED_HURT);
}
//
// Activated Object member functions
//
ActivatedObject::ActivatedObject(int imageID, double x, double y, int size, StudentWorld* world) :
	Actor(imageID, x, y, 0, size, 2, world)
{
}
void ActivatedObject::doSomething()
{
	// Move the object
	move();
	// Check if out of bounds
	if (outOfBounds())
	{
		setDead();
		return;
	}
	// Interact with the player
	if (doesOverlap(getWorld()->getPlayer()))
	{
		interactWithPlayer();
	}
	// Rotate if necessary
	if (doesRotateValue())
		setDirection(getDirection() + 10);
		
}
void ActivatedObject::interactWithPlayer()
{
	// Define a temporary pointer to the player for convenience
	Player* p = getWorld()->getPlayer();
	// Increase the score
	getWorld()->increaseScore(getScoreValue());
	// Update the health and remove any overflow, if necessary
	p->changeHealth(getHealthValue());
	if (p->getHealth() > 100)
		p->changeHealth(100 - p->getHealth());
	// Increase sprays, if necessary
	p->refillSprays(getSpraysValue());
	// Spin the player, if necessary
	if (spinPlayerValue())
		p->spin();
	// Destroy the object, if necessary
	if (selfDestructsValue())
		setDead();
	// Play the appropriate sound
	getWorld()->playSound(getSoundValue());
	// Save a soul, if necessary
	if (doesRotateValue())
		getWorld()->soulSaved();
}
//
// Oil Slick member functions
//
OilSlick::OilSlick(double x, double y, StudentWorld* world) :
	ActivatedObject(IID_OIL_SLICK, x, y, randInt(2, 5), world)
{
}
//
// Healing Goodie member functions
//
HealingGoodie::HealingGoodie(double x, double y, StudentWorld* world) :
	ActivatedObject(IID_HEAL_GOODIE, x, y, 1, world)
{
}
//
// Holy Water Goodie member functions
//
HolyWaterGoodie::HolyWaterGoodie(double x, double y, StudentWorld* world) :
	ActivatedObject(IID_HOLY_WATER_GOODIE, x, y, 2, world)
{
	setDirection(90);
}
//
// Soul Goodie member functions
//
SoulGoodie::SoulGoodie(double x, double y, StudentWorld* world) :
	ActivatedObject(IID_SOUL_GOODIE, x, y, 4, world)
{
}
//
// Holy Water Projectile member functions
//
Projectile::Projectile(double x, double y, int dir, StudentWorld* world) :
	Actor(IID_HOLY_WATER_PROJECTILE, x, y, dir, 1, 1, world),
	m_distTraveled(0)
{
}
void Projectile::doSomething()
{
	// Check if not alive
	if (!isAlive())
		return;
	// Check for interactions with other actors
	if (getWorld()->checkProjectileCollision(this))
		setDead();
	// Move forwards
	moveForward(SPRITE_HEIGHT);
	m_distTraveled += SPRITE_HEIGHT;
	// Check if out of bounds
	if (outOfBounds())
	{
		setDead();
		return;
	}
	// Check if it has traveled 160 pixels
	if (m_distTraveled >= 160)
	{
		setDead();
	}
}
//
// Border Line member functions
//
BorderLine::BorderLine(double x, double y, int color, StudentWorld* world) :
	Actor(color, x, y, 0, 2, 2, world)
{
}
void BorderLine::doSomething()
{
	// Move the borderline
	move();
	// Check if out of bounds
	if (outOfBounds())
	{
		setDead();
		return;
	}
}