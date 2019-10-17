#include "Attacker.h"

Attacker::Attacker()
{
	setUpActor();
}

Attacker::Attacker(float x, float z, AStar* aStar)
	:Actor(x, z, aStar)
{
	this->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	setUpActor();
}

Attacker::~Attacker()
{
}

void Attacker::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();
	Selector& selector = bt.getSelector();
	Sequence& seq2 = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Attacker::inRange, std::ref(*this)));
	Behavior& chase = bt.getAction();
	chase.addAction(std::bind(&Attacker::setChaseState, std::ref(*this)));
	Behavior& roam = bt.getAction();
	roam.addAction(std::bind(&Attacker::setRoamState, std::ref(*this)));
	Behavior& shoot = bt.getAction();
	shoot.addAction(std::bind(&Attacker::shoot, std::ref(*this)));
	Behavior& enemyNear = bt.getAction();
	enemyNear.addAction(std::bind(&Attacker::enemyNear, std::ref(*this)));

	root->addChildren(sequence);
	root->addChildren(roam);

	sequence.addChildren(enemyNear);
	sequence.addChildren(selector);
	
	selector.addChildren(seq2);
	selector.addChildren(chase);

	seq2.addChildren(inRange);
	seq2.addChildren(shoot);
}