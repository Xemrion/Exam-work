#include "../States/PlayingGameState.h"
#include "Actor.h"

Actor::Actor()
{
	this->velocity = Vector3(10.0f, 0.0f, 10.0f);
	this->position = Vector3(0, 0.0f, 0);
	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
	this->points = 0;
}

Actor::Actor(float x, float z, Physics* physics)
{
	this->velocity = Vector3(10.0f, 0.0f, 10.0f);
	this->position = Vector3(x, -1.0f, z);
	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
	this->points = 0;
	this->stunTimer = 0;
	this->fireTimer = 0;
}
Actor::~Actor()
{
	for (int i = 0; i < 2; i++)
	{
		Game::getGraphics().addParticle("explosion", 1, 1, position, Vector4(0.0f, 0.0f, 0.0f, 10.0f), 0.5f);
		Game::getGraphics().addParticle("explosion", 1, 1, position, Vector4(0.0f, 0.0f, 0.0f, 10.0f), 0.5f);
		Game::getGraphics().addParticle("explosion", 1, 1, position, Vector4(0.0f, 0.0f, 0.0f, 10.0f), 0.5f);
		Game::getGraphics().addParticle("explosion", 1, 1, position, Vector4(0.0f, 0.0f, 0.0f, 10.0f), 0.5f);
	}
	for (int i = 0; i < 24; i++)
	{
		Game::getGraphics().addParticle(position, Vector3(0.0f), 1, 3);
		Game::getGraphics().addParticle2(position, Vector3(0.0f), 1, 3);
	}
}

void Actor::update(float dt, const Vector3& targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;
	if (root != nullptr && stunTimer <= 0 && unStunned)
	{
		this->root->func();
	}
	else if(stunTimer <= 0 && !unStunned)
	{
		unStunned = true;
		setColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	}
	if (isHit)
	{
		
		setColor(Vector4(getColor().x / (1 + 15.0f * deltaTime), getColor().y, getColor().z, 1));
		if (getColor().x <= 0.01f)
		{
			isHit = false;
		}
	}
	stunTimer -= dt;
	particleTimer -= deltaTime;
}

float Actor::getHealth() const
{
	return this->health;
}

float Actor::getMaxHealth() const
{
	return this->stats.maxHealth;
}

void Actor::setHealth(float health)
{
	this->health = std::clamp(health, 0.0f, this->stats.maxHealth);
}

void Actor::setMaxHealth(float health)
{
	this->stats.maxHealth = health;
}

void Actor::changeHealth(float amount)
{
	if (amount < 0) {
		isHit = true;
		Sound::play("HitSound.wav");
	}
	setColor(Vector4(max(getColor().x + -amount * 0.1f, 0), getColor().y, getColor().z, 1));
	this->health = std::clamp(this->health + amount, 0.0f, this->stats.maxHealth);
	Game::getGraphics().addParticle2(this->getPosition(), Vector3(0, 0, 0), 1, 0.5f);
}

bool Actor::isDead() const
{
	return this->health <= 0;
}

int Actor::getPoints()
{
	return points;
}

void Actor::setPoints(int amount)
{
	this->points = amount;
}

void Actor::scaling(float& stat, float ratio)
{
	for (int i = 0; i < Game::getGameInfo().nrOfClearedStages; i++)
	{
		//�kar exponentiellt?
		stat *= ratio;
	}
}

void Actor::onFire()
{
	if(fireTimer > 0.0 && !isDead())
	{
		changeHealth(-10*deltaTime);
		fireTimer -= deltaTime;
		if(particleTimer <= 0.0f)
		{
			for (int i = 0; i < 10; i++)
			{
				Game::getGraphics().addParticle("fire", 1, 1, position, Vector4(0.0f, 0.0f, 0.0f, 10.0f), 0.5f);
			}
			particleTimer = 0.1f;
		}
	}
}

void Actor::setFire(float timer)
{
	fireTimer = timer;
}

void Actor::setStun(float timer)
{
	if(stunTimer <= 0)
	{
		stunTimer = timer;
		setColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
		unStunned = false;
	}
}
