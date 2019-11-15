#pragma once
#include "Car_Spitfire.h"
class ChaseCar : public Spitfire
{
private:
protected:
	virtual void followPath();
public:
	ChaseCar();
	ChaseCar(float x, float z,Physics* physics);
	~ChaseCar();
	void update(float dt, const Vector3& targetPos);

};