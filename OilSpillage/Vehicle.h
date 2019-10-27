#ifndef VEHICLE_H
#define VEHICLE_H

#include "GameObject.h"
#include "Weapon.h"
#include "VehicleStats.h"

using namespace DirectX::SimpleMath;

class Vehicle
{
private:
	GameObject* vehicle;
	GameObject* bodyRotation;
	GameObject* bodyRotationPoint;
	btScalar mRotation;
	GameObject* mountedWeapon;//Mounted on top of the car
	GameObject* wheel1;
	GameObject* wheel2;
	GameObject* wheel3;
	GameObject* wheel4;
	btGeneric6DofSpring2Constraint* spring1;
	btGeneric6DofSpring2Constraint* spring2;
	btGeneric6DofSpring2Constraint* spring3;
	btGeneric6DofSpring2Constraint* spring4;
	GameObject* vehicleBody1;
	btPoint2PointConstraint* pointJoint;
	//btRaycastVehicle* vehicleBody;
	btRaycastVehicle::btVehicleTuning	m_tuning;
	btVehicleRaycaster* m_vehicleRayCaster;
	btCollisionShape* m_wheelShape;

	int health;

	static const int bulletCount = 32;
	float leftoverTime;
	Weapon weapon;
	Stats defaultStats;
	Stats updatedStats;

	struct Bullet
	{
		Vector3 dir;
		float speed = 0.0f;
		float timeLeft = 0.0f;
		GameObject* obj = nullptr;
	} bullets[bulletCount];
	float gunRotation;
	Vector3 bodyPivot;
	DirectX::XMFLOAT2 velocity;
	float strength;
	float add;

	DirectX::XMFLOAT3 accelerator;
	float acceleratorTempX;
	float acceleratorTempZ;

	float targetRotation;
	int drivingMode;
	float topSpeed;
	float counter = 0.0f;
	float rotateAcceleration = 0.0f;
	float rotationSmoother = 1.0f;
	float tempTargetRotation;
	bool rotationDirection;
	float cameraDistance;
	Vector3 accelForce;

	
	Vector2 curDir;
	Vector2 currentDir;
	float velocitySimple;
	float velocitySpeed;
	class Physics* physics;
public:
	Vehicle();
	virtual ~Vehicle();

	void init(Physics *physics);
	void update(float deltaTime);
	void updateWeapons(float deltaTime);
	GameObject* getVehicle() { return this->vehicle; }
	GameObject* getVehicleBody1() { return this->vehicleBody1; }
	float getAcceleratorX();

	void setDrivingMode(int i);
	bool getDrivingMode();
	Vector3 getVelocity();
	float getVelocitySpeed();
	float getRotator();
	float getCameraDistance(float deltaTime);
	void setAccelForce(Vector3 accelForce, float deltaTime);
	//void onCollision(Vector2 direction);

	const int& getHealthRef() const;
	int getHealth() const;
	int getMaxHealth() const;
	void setHealth(int health);
	void setMaxHealth(int maxHealth);
	void resetHealth();
	void changeHealth(int amount);
	bool isDead() const;

	float getPitch(DirectX::XMVECTOR Quaternion);
	float getYaw(DirectX::XMVECTOR Quaternion);
	float getRoll(DirectX::XMVECTOR Quaternion);
	float getHeading(Quaternion qt);
};

#endif // !VEHICLE_H
