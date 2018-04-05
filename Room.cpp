#include "Room.h"

Room::Room(Ogre::SceneManager* mSceneMgr, Physics* mPhys, int wallSize):
	wall(mSceneMgr, mPhys, wallSize),
	brickCount(0)
{
	this->mPhys = mPhys;
	this->mSceneMgr = mSceneMgr;
	wall.createWalls();
	ball = new Ball(mSceneMgr, mPhys);
	paddle1 = NULL;
	paddle2 = NULL;
	//brick = new Brick(mSceneMgr, mPhys);
	scoreWall = new ScoreWall(mSceneMgr, mPhys, wallSize);
	scoreWall->createScoreWall();
}

void Room::setupSingle() {
	ball->createBall(Ogre::Vector3(0,0,-60));
	ball->getRigidBody()->applyCentralImpulse(btVector3(0,10,75));
	paddle1 = new Paddle(mSceneMgr, mPhys, 30, 15, Ogre::Vector3(0,0,-75), -90);
	generateBricks(0);
}

void Room::setupMulti() {
	ball->createBall(Ogre::Vector3(0,0,0));
	ball->getRigidBody()->applyCentralImpulse(btVector3(0,10,75));
	paddle1 = new Paddle(mSceneMgr, mPhys, 30, 15, Ogre::Vector3(0,0,-75), -90);
	paddle2 = new Paddle(mSceneMgr, mPhys, 30, 15, Ogre::Vector3(0,0,75), 90);
}

ScoreWall* Room::getScoreWall() {
	return scoreWall;
}

Paddle* Room::getPaddle1() {
	return paddle1;
}

Paddle* Room::getPaddle2() {
	return paddle2;
}

Ball* Room::getBall() {
	return ball;
}

std::list<Brick*>& Room::getBricks() {
	return bricks;
}

int& Room::getBrickCount() {
	return brickCount;
}

void Room::reset() {
	// Reset ball
	ball->reset();
	// Reset paddle
	paddle1->reset();
}

void Room::resetMultiplayer(int roundNum) {
	// Reset ball
	ball->resetMultiplayer(roundNum);
	// Reset paddles
	paddle1->reset();
	paddle2->reset();
}

void Room::generateBricks(int roundNum) {
	int brickSize = 20;

	int greenChance;
	int yellowChance;
	int orangeChance;
	int brickDensity;
	setProbability(roundNum,greenChance,yellowChance,orangeChance,brickDensity);
	for(int x = -70; x <= 70; x += brickSize) {
		for(int y = -70; y <= 70; y += brickSize) {
			for(int z = 20; z <= 60; z += brickSize) {
				int createChance = rand() % 100;
				bool createBrick = createChance < brickDensity;
				if(createBrick) {
					Brick* brick = new Brick(mSceneMgr,mPhys);
					int brickHealth = rand() % 100;
					if(brickHealth < greenChance) {
						brick->createBrick(Ogre::Vector3(x,y,z),3);
					} else if (brickHealth < yellowChance) {
						brick->createBrick(Ogre::Vector3(x,y,z),2);
					} else {
						brick->createBrick(Ogre::Vector3(x,y,z),1);
					}
					bricks.push_back(brick);
					brickCount++;
				}
			}
		}
	}
}

void Room::setProbability(int roundNum, int& greenChance, int& yellowChance, int& orangeChance, int& brickDensity) {
	if(roundNum == 0) {
		greenChance = 20;
		yellowChance = 70;
		orangeChance = 100;
		brickDensity = 10;
	} else if(roundNum == 1) {
		greenChance = 25;
		yellowChance = 75;
		orangeChance = 100;
		brickDensity = 12;
	} else if(roundNum == 2) {
		greenChance = 30;
		yellowChance = 80;
		orangeChance = 100;
		brickDensity = 14;
	} else if(roundNum == 3) {
		greenChance = 35;
		yellowChance = 85;
		orangeChance = 100;
		brickDensity = 17;
	} else if(roundNum == 4) {
		greenChance = 40;
		yellowChance = 85;
		orangeChance = 100;
		brickDensity = 20;
	} else if(roundNum >= 5) {
		greenChance = 45;
		yellowChance = 85;
		orangeChance = 100;
		brickDensity = 25;
	} else if(roundNum >= 7) {
		greenChance = 55;
		yellowChance = 85;
		orangeChance = 100;
		brickDensity = 35;
	} else if(roundNum >= 9) {
		greenChance = 60;
		yellowChance = 90;
		orangeChance = 100;
		brickDensity = 45;
	} else if(roundNum >= 14) {
		greenChance = 75;
		yellowChance = 90;
		orangeChance = 100;
		brickDensity = 55;
	} else if(roundNum >= 19) {
		greenChance = 85;
		yellowChance = 95;
		orangeChance = 100;
		brickDensity = 75;
	}
}
