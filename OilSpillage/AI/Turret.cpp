#include "../States/PlayingGameState.h"
#include "Turret.h"

Turret::Turret()
{
	this->sightRange = 10;
	this->setPosition(Vector3(-15.0f, 0.0f, 0.0f));
	setUpActor();
	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
	turretAngle = 90;
	this->calculateTarget(turretAngle);

	this->defaultStats = VehicleStats::AITurret;
	this->updatedStats = this->defaultStats;

	this->health = this->updatedStats.maxHealth;
}

Turret::Turret(float x, float z)
	: Actor(x, z, nullptr)
{
	//this->setColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
	this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	this->sightRange = 10.f;
	turretAngle = 90;
	this->calculateTarget(turretAngle);
	setUpActor();
	vecForward.Normalize();
	this->body.setPosition(this->position);
	this->body.setScale(this->scale);
	this->body.mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Turret");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Turret1");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Dummy_Turret"));
	this->body.setMaterial(Game::getGraphics().getMaterial("Entities/Dummy_Turret"));
	Game::getGraphics().addToDraw(&this->body);

	this->weapon = AIWeapon::machineGun;

	this->defaultStats = VehicleStats::AITurret;
	this->updatedStats = this->defaultStats;

	this->health = this->updatedStats.maxHealth;
}

Turret::~Turret()
{
	Game::getGraphics().removeFromDraw(&this->body);
}

void Turret::update(float dt, Vector3 targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;
	this->root->func();

	//rotateTowards();

	if ((this->position - targetPos).Length() <= 5 && this->health > 0)
	{
		changeHealth(-100);
	}

	for (int i = 0; i < bulletCount; i++)
	{
		Game::getGraphics().removeFromDraw(this->bullets[i].obj);

		if (this->bullets[i].timeLeft > 0.0f)
		{
			if ((this->bullets[i].obj->getPosition() - this->targetPos).Length() < 0.5f)
			{
				static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->changeHealth(-5);
				this->bullets[i].timeLeft = 0;

			}
			else
			{
				Game::getGraphics().addParticle(this->position - this->vecForward, Vector3(0, 0, 0), 1, 1, 1);
				this->bullets[i].timeLeft -= deltaTime;
				this->bullets[i].obj->move(this->bullets[i].dir * this->bullets[i].speed * deltaTime);
				Game::getGraphics().addToDraw(this->bullets[i].obj);
			}
		}
	}
}

bool Turret::hasGroup()
{
	return true;
}

void Turret::setForwardVector(Vector3 forward)
{
	this->vecForward = forward;
}

void Turret::setSightRange(int range)
{
	this->sightRange = range;
}

void Turret::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Turret::inRange, std::ref(*this)));

	Behavior& rotate = bt.getAction();
	rotate.addAction(std::bind(&Turret::rotateTowards, std::ref(*this)));

	Behavior& shoot = bt.getAction();
	shoot.addAction(std::bind(&Turret::shoot, std::ref(*this)));

	Behavior& idle = bt.getAction();
	idle.addAction(std::bind(&Turret::idle, std::ref(*this)));

	Behavior& lineOf = bt.getAction();
	lineOf.addAction(std::bind(&Turret::lineOfSight, std::ref(*this)));

	//Attack shoot;

	root->addChildren(sequence);
	root->addChildren(idle);
	sequence.addChildren(inRange);
	sequence.addChildren(lineOf);
	sequence.addChildren(rotate);
	sequence.addChildren(shoot);
}

void Turret::followPath()
{
}
void Turret::updateBoid(float deltatime)
{
}

Status Turret::rotateTowards()
{
	Vector3 targetToSelf = (targetPos - position);
	//targetToSelf.Normalize();

	if ((targetToSelf).Dot(vecForward) < 0.8)
	{
		vecForward -= (targetToSelf * deltaTime) / 0.1;
		vecForward.Normalize();

		float newRot = atan2(this->vecForward.x, this->vecForward.z);
		this->setRotation(Vector3(0, newRot - (XM_PI / 2), 0));
	}
	return Status::SUCCESS;
}

Status Turret::inRange()
{
	Status status;
	if ((getPosition() - targetPos).Length() > this->sightRange)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
	}
	return status;
}

Status Turret::lineOfSight()
{
	Status status = Status::FAILURE;
	Vector3 dir = this->position - targetPos;
	dir.Normalize();
	float value = dir.Dot(vecForward);
	float angle = std::abs(acos(value));
	if (angle <= XM_PIDIV2)
	{
		status = Status::SUCCESS;
	}
	return status;

}
void Turret::calculateTarget(float& angle)
{
	float radians = angle * (3.14f / 180.f);
	float x = cos(radians) * vecForward.x + sin(radians) * vecForward.z;
	float y = cos(radians) * vecForward.x - sin(radians) * vecForward.z;
	this->idleTarget = Vector3(x, 0.0f, y);
}
Status Turret::idle()
{
	/*calculate new target position if are getting close to the point*/
	if (idleTarget.Dot(vecForward) <= 0.0f)
	{
		turretAngle += 90;
		this->calculateTarget(turretAngle);
	}
	else
	{	vecForward -= (idleTarget * deltaTime) / 1.01;
		vecForward.Normalize();

		float newRot = atan2(this->vecForward.x, this->vecForward.z);
		this->setRotation(Vector3(0, newRot - (XM_PIDIV2), 0));
	}
	return Status::SUCCESS;
}

