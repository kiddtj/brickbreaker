#include "ScoreWall.h"


ScoreWall::ScoreWall(Ogre::SceneManager* scnMgr, Physics* mPhys, int wallSize) { 
    this->scnMgr = scnMgr;
    mPhysics = mPhys;
    this->wallSize = wallSize;
    squareSize = ((double) wallSize) / 3;
    activeGoal = NULL;
    score = 0;
    scoreOther = 0;

    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane(
        "goalMesh",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 
        squareSize, squareSize, 20, 20, 
        true, 
        1, 5, 5, 
        Ogre::Vector3::UNIT_Z);

    Ogre::MeshManager::getSingleton().createPlane(
        "scoreMesh",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 
        wallSize, wallSize, 20, 20, 
        true, 
        1, 5, 5, 
        Ogre::Vector3::UNIT_Z);
}

void ScoreWall::createScoreWall() {
    Ogre::Entity* scoreWallEntity = scnMgr->createEntity("scoreMesh");
    scoreWallEntity->setVisible(false);
    scoreWallEntity->setMaterialName("Colors/Blue");

    Ogre::SceneNode* scoreWallNode = scnMgr->getRootSceneNode()->createChildSceneNode("scoreWall");
    scoreWallNode->attachObject(scoreWallEntity);
    scoreWallNode->pitch(Ogre::Radian(Ogre::Degree(-90)));
    scoreWallNode->translate(Ogre::Vector3(0,0,wallSize/2));

    goals.push_back(Goal(scnMgr, mPhysics, squareSize, Ogre::Vector3(0,0,0)));
    goals.push_back(Goal(scnMgr, mPhysics, squareSize, Ogre::Vector3(squareSize,0,0)));
    goals.push_back(Goal(scnMgr, mPhysics, squareSize, Ogre::Vector3(-squareSize,0,0)));
    goals.push_back(Goal(scnMgr, mPhysics, squareSize, Ogre::Vector3(0,0,squareSize)));
    goals.push_back(Goal(scnMgr, mPhysics, squareSize, Ogre::Vector3(squareSize,0,squareSize)));
    goals.push_back(Goal(scnMgr, mPhysics, squareSize, Ogre::Vector3(-squareSize,0,squareSize)));
    goals.push_back(Goal(scnMgr, mPhysics, squareSize, Ogre::Vector3(0,0,-squareSize)));
    goals.push_back(Goal(scnMgr, mPhysics, squareSize, Ogre::Vector3(squareSize,0,-squareSize)));
    goals.push_back(Goal(scnMgr, mPhysics, squareSize, Ogre::Vector3(-squareSize,0,-squareSize)));
}

void ScoreWall::allOff() { 
    for(int i = 0; i < goals.size(); ++i)
        goals[i].off();
}

void ScoreWall::pickGoal() { 
    int i = rand() % goals.size();
    if(activeGoal) {
        activeGoal->off();
        while(&(goals[i]) == activeGoal)
            i = rand() % goals.size();
    }
    activeGoal = &(goals[i]);
    int j = rand() % 3;
    activeGoal->createBrick(j);
}

bool ScoreWall::decHealth(){
    return activeGoal->decHealth();
}

void ScoreWall::increaseScore(int increment) {
    score += increment;
}

void ScoreWall::increaseScoreOther() {
    scoreOther++;
}

int ScoreWall::getScore() {
    return score;
}

int ScoreWall::getScoreOther() {
    return scoreOther;
}

void ScoreWall::resetScore() {
    score = 0;
}

void ScoreWall::resetScoreOther() {
    scoreOther = 0;
}
