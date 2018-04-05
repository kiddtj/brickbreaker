/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/
Tutorial Framework (for Ogre 1.9)
http://www.ogre3d.org/wiki/
-----------------------------------------------------------------------------
*/

#include <string> 
#include <sstream>

#include "TutorialApplication.h"

//---------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
{
}
//---------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{
}

//---------------------------------------------------------------------------
bool TutorialApplication::keyPressed(const OIS::KeyEvent& ke) 
{ 
    Ogre::Node* paddleNode = room->getPaddle1()->getNode();
    if(mNetRole == BaseApplication::CLIENT)
        paddleNode = room->getPaddle2()->getNode();

    Ogre::Real x = mDirection.x;
    Ogre::Real y = mDirection.y;
    Ogre::Real z = mDirection.z;  

    CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(ke.text);
    CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown((CEGUI::Key::Scan)ke.key);

    Ogre::Real move = 200;
    switch (ke.key)
    {
        case OIS::KC_ESCAPE: 
            mShutDown = true;
            break;
        case OIS::KC_UP:
            //y += move;
            mPitch = Ogre::Radian(Ogre::Degree(-.05));
            break;
        case OIS::KC_DOWN:
            //y -= move;
            mPitch = Ogre::Radian(Ogre::Degree(.05));
            break;
        case OIS::KC_LEFT:
            //x += move;
            mRoll = Ogre::Radian(Ogre::Degree(.05));
            break;
        case OIS::KC_RIGHT:
            //x -= move;
            mRoll = Ogre::Radian(Ogre::Degree(-.05));
            break;
        case OIS::KC_W:
            mPitch = Ogre::Radian(Ogre::Degree(-.05));
            break;
        case OIS::KC_A:
            mRoll = Ogre::Radian(Ogre::Degree(.05));
            break;
        case OIS::KC_S:
            mPitch = Ogre::Radian(Ogre::Degree(.05));
            break;
        case OIS::KC_D:
            mRoll = Ogre::Radian(Ogre::Degree(-.05));
            break;
         case OIS::KC_SPACE:
            if(mGameMode != BaseApplication::IN_MENU) {
                if(mHit)
                   break;
                mSound->play(Sound::SOUND_WHIFF);
                mHit = true;
                mHitFrames = mHitMaxFrames;
            }
            break;
        default:
            break;
    }
  return true; 
}
 
bool TutorialApplication::keyReleased(const OIS::KeyEvent& ke) 
{ 
    Ogre::Node* paddleNode = room->getPaddle1()->getNode();
    if(mNetRole == BaseApplication::CLIENT)
        paddleNode = room->getPaddle2()->getNode();

    Ogre::Real x = mDirection.x;
    Ogre::Real y = mDirection.y;
    Ogre::Real z = mDirection.z;

    Ogre::Real move = 200;
    switch (ke.key)
    {
        case OIS::KC_ESCAPE: 
            mShutDown = true;
            break;
        case OIS::KC_UP:
            mPitch = 0;
            break;
        case OIS::KC_DOWN:
            mPitch = 0;
            break;
        case OIS::KC_LEFT:
            mRoll = 0;
            break;
        case OIS::KC_RIGHT:
            mRoll = 0;
            break;
        case OIS::KC_W:
            mPitch = 0;;
            break;
        case OIS::KC_A:
            mRoll = 0;
            break;
        case OIS::KC_S:
            mPitch = 0;
            break;
        case OIS::KC_D:
            mRoll = 0;
            break;
        case OIS::KC_RETURN:
            if(mGameState == BaseApplication::STOPPED && mGameMode == BaseApplication::SINGLE) {
                startLabel->hide();
                youMissedLabel->hide();
                mGameState = BaseApplication::RUNNING;
            }
            break;
        case OIS::KC_R:
            if(mGameState == BaseApplication::STOPPED && mGameMode == BaseApplication::SINGLE) {
                restartGame();
            }
            break;
        default:
            break;
    } 
  return true; 
}

bool TutorialApplication::mouseMoved(const OIS::MouseEvent &arg)
{
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
    if(mGameState == BaseApplication::RUNNING){
        mDirection.x = -arg.state.X.rel *0.5; //* -50;
        mDirection.y = -arg.state.Y.rel *0.5; //* -50;
    }
    return true;
}


void TutorialApplication::createScene(void)
{
    srand(time(NULL));
    mSceneMgr->setAmbientLight(Ogre::ColourValue(.35, .35, .35));
    Ogre::Light* light = mSceneMgr->createLight("MainLight");
    light->setPosition(0, 75, 0);

    setupGUI();

    room = new Room(mSceneMgr, mPhysics, 200);
    scoreWall = room->getScoreWall();
}

bool TutorialApplication::initServer() {
    if(!mNetMgr->initNetManager()) {
        return false;
    }
    mNetMgr->addNetworkInfo(PROTOCOL_TCP, NULL, 8080);
    if(!mNetMgr->startServer()) {
        return false;    
    }
    mNetMgr->acceptConnections();
    return true;
}

bool TutorialApplication::initClient(const char* hostname) {
    if(!mNetMgr->initNetManager()) {
        return false;
    }
    mNetMgr->addNetworkInfo(PROTOCOL_TCP, hostname, 8080);
    if(!mNetMgr->startClient()) {
        return false;
    }
    mNetMgr->messageServer(PROTOCOL_TCP, "hello socket", 32);
    return true;
}

bool TutorialApplication::menu(const CEGUI::EventArgs &e) {
    // Show menu
    menuBox->show();

    // Pause game
    mGameState = BaseApplication::PAUSED;
    return true;
}

bool TutorialApplication::sound(const CEGUI::EventArgs &e) {
    // Toggle sound
    mSound->toggle();

    // Change text
    if(mSound->getIsOn()) {
        mainSoundButton->setText("Sound Off");
        soundButton->setText("Sound Off");
    }
    else {
        mainSoundButton->setText("Sound On");
        soundButton->setText("Sound On");
    }

    return true;
}

bool TutorialApplication::resume(const CEGUI::EventArgs &e) {
    // Hide menu
    menuBox->hide();
    startLabel->hide();
    // Resume game
    mGameState = BaseApplication::RUNNING;
    return true;
}

bool TutorialApplication::restart(const CEGUI::EventArgs &e) {
    restartGame();
    // Show label
    startLabel->show();
    // Hide Menu
    menuBox->hide();
    return true;
}

bool TutorialApplication::quit(const CEGUI::EventArgs &e) {
    mShutDown = true;
    return true;
}

bool TutorialApplication::single(const CEGUI::EventArgs &e) {
    // Hide Menu
    mainMenuBox->hide();
    // Set game mode
    mGameMode = BaseApplication::SINGLE;
    // Show start label, menu button, and score box
    startLabel->show();
    menuButton->show();
    scoreBox->show();
    roundBox->show();
    livesBox->show();
    updateRoundLabel();

    // Start game
    start();

    return true;
}

bool TutorialApplication::multi(const CEGUI::EventArgs &e) {
    // Hide Menu
    mainMenuBox->hide();
    // Show Multi Menu
    multiMenuBox->show();

    return true;
}

bool TutorialApplication::search(const CEGUI::EventArgs &e) {
    // Hide Menu
    multiMenuBox->hide();

    searchBox->activate();
    // Show Multi Menu
    searchMenu->show();

    return true;
}


bool TutorialApplication::server(const CEGUI::EventArgs &e) {
    // Hide Menu
    multiMenuBox->hide();

    // waiting label
    waitingBox->show();
    ipLabel->setText("Your IP Address: " + mNetMgr->getIPstring());
    ipLabel->show();

    // Set net role
    mNetRole = BaseApplication::SERVER;
    // Init Server
    initServer();
    // Set waiting for client flag to true
    mWaiting = true;
    
    return true;
}

bool TutorialApplication::client(const CEGUI::EventArgs &e) {
    // Hide Menu
    multiMenuBox->hide();

    CEGUI::String ip_addr = searchBox->getText();

    // Init Client and send message to server
    if(!initClient(ip_addr.c_str()))
        std::cout << "\n\nFAILED TO CONNECT... TRY AGAIN\n\n";
    // Set net role
    mNetRole = BaseApplication::CLIENT;
    // Show Score Box
    multiScoreBox->show();
    // Set game mode
    mGameMode = BaseApplication::MULTI;
    // Start game

    searchButton->hide();
    searchMenu->hide();
    searchBox->hide();

    start();

    return true;
}

void TutorialApplication::start() {
    if(mGameMode == BaseApplication::SINGLE) {  // Setup single player scene
        room->setupSingle();
        
        Ogre::Node* paddleNode = room->getPaddle1()->getNode();
        mCamera1->lookAt(paddleNode->getPosition());
    } else { // Setup multi player scene
        room->setupMulti();
        if(mNetRole == BaseApplication::SERVER) {
            mViewport->setCamera(mCamera2);
            mViewport->setCamera(mCamera1);
            Ogre::Node* paddleNode = room->getPaddle1()->getNode();
            mCamera1->lookAt(paddleNode->getPosition());
            mTimeToRound = 3;
            roundTimerLabel->show();
        } else {
            mViewport->setCamera(mCamera2);
            Ogre::Node* paddleNode = room->getPaddle2()->getNode();
            mCamera2->lookAt(paddleNode->getPosition());

            room->getBall()->getRigidBody()->setActivationState(DISABLE_SIMULATION);
            room->getPaddle1()->getRigidBody()->setActivationState(DISABLE_SIMULATION);
            room->getPaddle2()->getRigidBody()->setActivationState(DISABLE_SIMULATION);
            mTimeToRound = 3;
            roundTimerLabel->show();
        }
    }
}

void TutorialApplication::updateScoreLabel() {
    int score = scoreWall->getScore();
    std::stringstream ss;
    ss << score;
    if(mGameMode == BaseApplication::MULTI)
        p1ScoreLabel->setText(ss.str());
    else 
        scoreLabel->setText(ss.str());
}

void TutorialApplication::updateScoreLabelOther() {
    int scoreOther = scoreWall->getScoreOther();
    std::stringstream ss;
    ss << scoreOther;
    p2ScoreLabel->setText(ss.str());
}

void TutorialApplication::updateRoundLabel() {
    std::stringstream ss;
    ss << mRoundNum + 1;
    roundLabel->setText(ss.str());
}

void TutorialApplication::updateLivesLabel() {
    std::stringstream ss;
    ss << mLivesNum;
    livesLabel->setText(ss.str());
}

void TutorialApplication::restartGame() {
    // Stop game
    mGameState = BaseApplication::STOPPED;
    dRoll2 = 0;
    dPitch2 = 0;
    dRoll1 = 0;
    dPitch1 = 0;
    // Reset score
    scoreWall->resetScore();
    updateScoreLabel();
    // Reset round
    mRoundNum = 0;
    updateRoundLabel();
    // Reset lives
    mLivesNum = 5;
    updateLivesLabel();
    // Reset room
    room->reset();
    room->generateBricks(mRoundNum);
    // Re-center Camera
    mCamera1->setPosition(Ogre::Vector3(0,0,-100));
    // Hide gameOverLabel       
    gameOverLabel->hide();
    youMissedLabel->hide();
    tooSlowLabel->hide();
    // Show label
    startLabel->show();
}

void TutorialApplication::gameOver(bool ballStopped) {
    // Show label
    gameOverLabel->show();
    if(ballStopped)
        tooSlowLabel->show();
    else
        youMissedLabel->show();
}

void TutorialApplication::roundOverSingle() {
    mGameState = BaseApplication::STOPPED;
    dRoll2 = 0;
    dPitch2 = 0;
    dRoll1 = 0;
    dPitch1 = 0;
    scoreWall->increaseScore((mRoundNum+1)*50);
    updateScoreLabel();
    mRoundNum++;
    updateRoundLabel();
    room->reset();
    mCamera1->setPosition(Ogre::Vector3(0,0,-100));
    room->getBricks().clear();
    room->generateBricks(mRoundNum);
    startLabel->show();
}

void TutorialApplication::roundOverMulti(int ballPos, bool ballStopped) {
    // This method only called by server
    int scoreType = -1;
    if(ballPos <= -80) {
        youMissedLabel->show();
        scoreWall->increaseScoreOther();
        updateScoreLabelOther();
        scoreType = BaseApplication::SCORE_CLIENT;
    } else if(ballPos >= 80) {
        youScoredLabel->show();
        scoreWall->increaseScore(1);
        updateScoreLabel();
        mSound->play(Sound::SOUND_SCORE);
        scoreType = BaseApplication::SCORE_SERVER;
    } else if(ballStopped) {
        drawLabel->show();
        scoreType = BaseApplication::SCORE_DRAW;
    }

    if(scoreWall->getScore() == 3) {
        // Build gameover packet and send to client
        UpdatePacket packet;
        packet.packetType = BaseApplication::PACKET_GAME;
        packet.scoreType = scoreType;
        packet.soundToPlay = -1;
        packet.ballPos = Ogre::Vector3();
        packet.ballRot = Ogre::Quaternion();
        packet.paddlePos = Ogre::Vector3();
        packet.paddleRot = Ogre::Quaternion();
        char toSend[sizeof(packet)];
        std::memcpy(&toSend[0],&packet,sizeof(packet));
        mNetMgr->messageClient(PROTOCOL_TCP,0,toSend,sizeof(packet));
        youScoredLabel->hide();
        youWinLabel->show();
        mTimeToRound = std::numeric_limits<double>::max();
    } else if(scoreWall->getScoreOther() == 3) {
        // Build gameover packet and send to client
        UpdatePacket packet;
        packet.packetType = BaseApplication::PACKET_GAME;
        packet.scoreType = scoreType;
        packet.soundToPlay = -1;
        packet.ballPos = Ogre::Vector3();
        packet.ballRot = Ogre::Quaternion();
        packet.paddlePos = Ogre::Vector3();
        packet.paddleRot = Ogre::Quaternion();
        char toSend[sizeof(packet)];
        std::memcpy(&toSend[0],&packet,sizeof(packet));
        mNetMgr->messageClient(PROTOCOL_TCP,0,toSend,sizeof(packet));
        youMissedLabel->hide();
        youLoseLabel->show();
        mTimeToRound = std::numeric_limits<double>::max();
    } else {
        // Build roundover packet and send to client
        UpdatePacket packet;
        packet.packetType = BaseApplication::PACKET_ROUND;
        packet.scoreType = scoreType;
        packet.soundToPlay = -1;
        packet.ballPos = Ogre::Vector3();
        packet.ballRot = Ogre::Quaternion();
        packet.paddlePos = Ogre::Vector3();
        packet.paddleRot = Ogre::Quaternion();
        char toSend[sizeof(packet)];
        std::memcpy(&toSend[0],&packet,sizeof(packet));
        mNetMgr->messageClient(PROTOCOL_TCP,0,toSend,sizeof(packet));
        ++mRoundNum;
        room->resetMultiplayer(mRoundNum);
        mCamera1->setPosition(Ogre::Vector3(0,0,-100));
        mTimeToRound = 3;
        roundTimerLabel->show();
    }
}

void TutorialApplication::setupGUI() {
    gui = new GUI();

    CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();      
    sheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");
    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);

    // Start Label
    startLabel = wmgr.createWindow("Vanilla/Label", "CEGUIDemo/StartLabel");
    startLabel->setFont("Jura-Regular");
    startLabel->setText("Press ENTER / RETURN to start");
    startLabel->setSize(CEGUI::USize(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.1, 0)));
    startLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35, 0), CEGUI::UDim(0.45, 0)));
    startLabel->hide();
    sheet->addChild(startLabel);

    // Game Over Label
    gameOverLabel = wmgr.createWindow("Vanilla/Label", "CEGUIDemo/GameOverLabel");
    gameOverLabel->setFont("Jura-Regular");
    gameOverLabel->setText("Press R to restart");
    gameOverLabel->setSize(CEGUI::USize(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.1, 0)));
    gameOverLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35, 0), CEGUI::UDim(0.45, 0)));
    gameOverLabel->hide();
    sheet->addChild(gameOverLabel);

    // Too Slow! Label
    tooSlowLabel = wmgr.createWindow("Vanilla/Label", "CEGUIDemo/TooSlowLabel");
    tooSlowLabel->setFont("Jura-Regular");
    tooSlowLabel->setText("Too Slow!");
    tooSlowLabel->setSize(CEGUI::USize(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.1, 0)));
    tooSlowLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35, 0), CEGUI::UDim(0.65, 0)));
    tooSlowLabel->hide();
    sheet->addChild(tooSlowLabel);

    // You Missed! Label
    youMissedLabel = wmgr.createWindow("Vanilla/Label", "CEGUIDemo/YouMissedLabel");
    youMissedLabel->setFont("Jura-Regular");
    youMissedLabel->setText("You Missed!");
    youMissedLabel->setSize(CEGUI::USize(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.1, 0)));
    youMissedLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35, 0), CEGUI::UDim(0.65, 0)));
    youMissedLabel->hide();
    sheet->addChild(youMissedLabel);

    // You Scored! Label
    youScoredLabel = wmgr.createWindow("Vanilla/Label", "CEGUIDemo/YouScoredLabel");
    youScoredLabel->setFont("Jura-Regular");
    youScoredLabel->setText("You Scored!");
    youScoredLabel->setSize(CEGUI::USize(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.1, 0)));
    youScoredLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35, 0), CEGUI::UDim(0.65, 0)));
    youScoredLabel->hide();
    sheet->addChild(youScoredLabel);

    // Draw! Label
    drawLabel = wmgr.createWindow("Vanilla/Label", "CEGUIDemo/DrawLabel");
    drawLabel->setFont("Jura-Regular");
    drawLabel->setText("Draw!");
    drawLabel->setSize(CEGUI::USize(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.1, 0)));
    drawLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35, 0), CEGUI::UDim(0.65, 0)));
    drawLabel->hide();
    sheet->addChild(drawLabel);

    // You Win! Label
    youWinLabel = wmgr.createWindow("Vanilla/Label", "CEGUIDemo/YouWinLabel");
    youWinLabel->setFont("Jura-Regular");
    youWinLabel->setText("You Win!");
    youWinLabel->setSize(CEGUI::USize(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.1, 0)));
    youWinLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35, 0), CEGUI::UDim(0.65, 0)));
    youWinLabel->hide();
    sheet->addChild(youWinLabel);

    // You Lose! Label
    youLoseLabel = wmgr.createWindow("Vanilla/Label", "CEGUIDemo/YouLoseLabel");
    youLoseLabel->setFont("Jura-Regular");
    youLoseLabel->setText("You Lose!");
    youLoseLabel->setSize(CEGUI::USize(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.1, 0)));
    youLoseLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35, 0), CEGUI::UDim(0.65, 0)));
    youLoseLabel->hide();
    sheet->addChild(youLoseLabel);

    // Round Timer Label
    roundTimerLabel = wmgr.createWindow("Vanilla/Label", "CEGUIDemo/RoundTimerLabel");
    roundTimerLabel->setFont("Jura-Regular");
    roundTimerLabel->setText("Round starts in 3");
    roundTimerLabel->setSize(CEGUI::USize(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.1, 0)));
    roundTimerLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35, 0), CEGUI::UDim(0.45, 0)));
    roundTimerLabel->hide();
    sheet->addChild(roundTimerLabel);

    // Menu Button
    menuButton = wmgr.createWindow("Vanilla/Button", "CEGUIDemo/MenuButton");
    menuButton->setFont("Jura-Regular");
    menuButton->setText("Menu");
    menuButton->setSize(CEGUI::USize(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.05, 0)));
    menuButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::menu, this));
    menuButton->hide();
    sheet->addChild(menuButton);

    // Score Box
    scoreBox = wmgr.createWindow("Vanilla/FrameWindow", "CEGUIDemo/ScoreBox");
    scoreBox->setFont("Jura-Regular");
    scoreBox->setText("SCORE");
    scoreBox->setSize(CEGUI::USize(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.125, 0)));
    scoreBox->setPosition(CEGUI::UVector2(CEGUI::UDim(0.85, 0), CEGUI::UDim(0.8, 0)));
    scoreLabel = scoreBox->createChild("Vanilla/Label", "CEGUIDemo/ScoreBox/ScoreLabel");
    scoreLabel->setFont("Jura-Regular");
    scoreLabel->setText("0");
    scoreLabel->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(0.5, 0)));
    scoreLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.02, 0), CEGUI::UDim(0.25, 0)));
    scoreBox->hide();
    sheet->addChild(scoreBox);

    // Round Box
    roundBox = wmgr.createWindow("Vanilla/FrameWindow", "CEGUIDemo/RoundBox");
    roundBox->setFont("Jura-Regular");
    roundBox->setText("ROUND");
    roundBox->setSize(CEGUI::USize(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.125, 0)));
    roundBox->setPosition(CEGUI::UVector2(CEGUI::UDim(0.75, 0), CEGUI::UDim(0.8, 0)));
    roundLabel = roundBox->createChild("Vanilla/Label", "CEGUIDemo/RoundBox/RoundLabel");
    roundLabel->setFont("Jura-Regular");
    roundLabel->setText("1");
    roundLabel->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(0.5, 0)));
    roundLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.02, 0), CEGUI::UDim(0.25, 0)));
    roundBox->hide();
    sheet->addChild(roundBox);

    // Lives Box
    livesBox = wmgr.createWindow("Vanilla/FrameWindow", "CEGUIDemo/LivesBox");
    livesBox->setFont("Jura-Regular");
    livesBox->setText("LIVES");
    livesBox->setSize(CEGUI::USize(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.125, 0)));
    livesBox->setPosition(CEGUI::UVector2(CEGUI::UDim(0.05, 0), CEGUI::UDim(0.8, 0)));
    livesLabel = livesBox->createChild("Vanilla/Label", "CEGUIDemo/LivesBox/LivesLabel");
    livesLabel->setFont("Jura-Regular");
    livesLabel->setText("5");
    livesLabel->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(0.5, 0)));
    livesLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.02, 0), CEGUI::UDim(0.25, 0)));
    livesBox->hide();
    sheet->addChild(livesBox);

    // Multiplayer Score Box
    multiScoreBox = wmgr.createWindow("Vanilla/FrameWindow", "CEGUIDemo/MultiScoreBox");
    multiScoreBox->setFont("Jura-Regular");
    multiScoreBox->setText("SCORE");
    multiScoreBox->setSize(CEGUI::USize(CEGUI::UDim(0.21, 0), CEGUI::UDim(0.25, 0)));
    multiScoreBox->setPosition(CEGUI::UVector2(CEGUI::UDim(0.79, 0), CEGUI::UDim(0.75, 0)));
    p1ScoreBox = multiScoreBox->createChild("Vanilla/FrameWindow", "CEGUIDemo/p1ScoreBox");
    p1ScoreBox->setFont("Jura-Regular");
    p1ScoreBox->setText("You");
    p1ScoreBox->setSize(CEGUI::USize(CEGUI::UDim(0.2, 0), CEGUI::UDim(0.7, 0)));
    p1ScoreBox->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0, 0), CEGUI::UDim(0.1, 0)));
    p1ScoreLabel = p1ScoreBox->createChild("Vanilla/Label", "CEGUIDemo/ScoreBox/p1ScoreLabel");
    p1ScoreLabel->setFont("Jura-Regular");
    p1ScoreLabel->setText("0");
    p1ScoreLabel->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(0.5, 0)));
    p1ScoreLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.02, 0), CEGUI::UDim(0.25, 0)));
    p2ScoreBox = multiScoreBox->createChild("Vanilla/FrameWindow", "CEGUIDemo/p2ScoreBox");
    p2ScoreBox->setFont("Jura-Regular");
    p2ScoreBox->setText("Opponent");
    p2ScoreBox->setSize(CEGUI::USize(CEGUI::UDim(0.2, 0), CEGUI::UDim(0.7, 0)));
    p2ScoreBox->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5, 0), CEGUI::UDim(0.1, 0)));
    p2ScoreLabel = p2ScoreBox->createChild("Vanilla/Label", "CEGUIDemo/ScoreBox/p2ScoreLabel");
    p2ScoreLabel->setFont("Jura-Regular");
    p2ScoreLabel->setText("0");
    p2ScoreLabel->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(0.5, 0)));
    p2ScoreLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.02, 0), CEGUI::UDim(0.25, 0)));
    multiScoreBox->hide();
    sheet->addChild(multiScoreBox);

    // Main Menu
    mainMenuBox = wmgr.createWindow("Vanilla/FrameWindow", "CEGUIDemo/MainMenu");
    mainMenuBox->setFont("Jura-Regular");
    mainMenuBox->setText("Main Menu");
    mainMenuBox->setSize(CEGUI::USize(CEGUI::UDim(0.2, 0), CEGUI::UDim(0.6, 0)));
    mainMenuBox->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.2, 0)));

    // Single Player Button
    singleButton = mainMenuBox->createChild("Vanilla/Button", "CEGUIDemo/Menu/SinglePlayerButton");
    singleButton->setFont("Jura-Regular");
    singleButton->setText("Single Player");
    singleButton->setSize(CEGUI::USize(CEGUI::UDim(0.8, 0), CEGUI::UDim(0.1, 0)));
    singleButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.1, 0)));
    singleButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::single, this));

    // Multi Player Button
    multiButton = mainMenuBox->createChild("Vanilla/Button", "CEGUIDemo/Menu/MultiPlayerButton");
    multiButton->setFont("Jura-Regular");
    multiButton->setText("Multi Player");
    multiButton->setSize(CEGUI::USize(CEGUI::UDim(0.8, 0), CEGUI::UDim(0.1, 0)));
    multiButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.3, 0)));
    multiButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::multi, this));

    // Main Menu Sound Button
    mainSoundButton = mainMenuBox->createChild("Vanilla/Button", "CEGUIDemo/Menu/MainSoundButton");
    mainSoundButton->setFont("Jura-Regular");
    mainSoundButton->setText("Sound Off");
    mainSoundButton->setSize(CEGUI::USize(CEGUI::UDim(0.8, 0), CEGUI::UDim(0.1, 0)));
    mainSoundButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.5, 0)));
    mainSoundButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::sound, this));

    // Multi Menu
    multiMenuBox = wmgr.createWindow("Vanilla/FrameWindow", "CEGUIDemo/MultiMenu");
    multiMenuBox->setFont("Jura-Regular");
    multiMenuBox->setText("Multi Player");
    multiMenuBox->setSize(CEGUI::USize(CEGUI::UDim(0.2, 0), CEGUI::UDim(0.6, 0)));
    multiMenuBox->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.2, 0)));
    multiMenuBox->hide();

    // Become a Host Button
    serverButton = multiMenuBox->createChild("Vanilla/Button", "CEGUIDemo/Menu/ServerButton");
    serverButton->setFont("Jura-Regular");
    serverButton->setText("Become a Host...");
    serverButton->setSize(CEGUI::USize(CEGUI::UDim(0.8, 0), CEGUI::UDim(0.2, 0)));
    serverButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.1, 0)));
    serverButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::server, this));

    // Host waiting for an opponent box
    waitingBox = wmgr.createWindow("Vanilla/FrameWindow", "CEGUIDemo/WaitingBox");
    waitingBox->setFont("Jura-Regular");
    waitingBox->setText("Waiting on an opponent...");
    waitingBox->setSize(CEGUI::USize(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.1, 0)));
    waitingBox->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35, 0), CEGUI::UDim(0.45, 0)));
    waitingBox->hide();
    sheet->addChild(waitingBox);

    // Your ip label
    ipLabel = wmgr.createWindow("Vanilla/Label", "CEGUIDemo/IPLabel");
    ipLabel->setFont("Jura-Regular");
    ipLabel->setText("Your IP Address");
    ipLabel->setSize(CEGUI::USize(CEGUI::UDim(0.5, 0), CEGUI::UDim(0.1, 0)));
    ipLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.65, 0)));
    ipLabel->hide();
    sheet->addChild(ipLabel);

    // Search for host window
    searchMenu = wmgr.createWindow("Vanilla/FrameWindow", "CEGUIDemo/SearchMenu");
    searchMenu->setFont("Jura-Regular");
    searchMenu->setText("Search for IP Address");
    searchMenu->setSize(CEGUI::USize(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.2, 0)));
    searchMenu->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.3, 0)));
    searchMenu->hide();
    sheet->addChild(searchMenu);

    // Search for host text box
    searchBox = static_cast<CEGUI::Editbox*>(searchMenu->createChild("Vanilla/Editbox", "SearchMenu/searchBox"));
    searchBox->setSize(CEGUI::USize(CEGUI::UDim(0.5, 0), CEGUI::UDim(0.35, 0)));
    searchBox->setPosition(CEGUI::UVector2(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.05, 0)));
    searchBox->setText("");
    searchBox->setTextParsingEnabled(true);
    searchBox->setEnabled(true);

    // Search for host button
    searchButton = searchMenu->createChild("Vanilla/Button", "CEGUIDemo/searchMenu/SearchButton");
    searchButton->setSize(CEGUI::USize(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.35, 0)));
    searchButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35, 0), CEGUI::UDim(0.5, 0)));
    searchButton->setText("Search");
    searchButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::client, this));
    
    // Search for Host Button
    clientButton = multiMenuBox->createChild("Vanilla/Button", "CEGUIDemo/Menu/ClientButton");
    clientButton->setFont("Jura-Regular");
    clientButton->setText("Search for a Host...");
    clientButton->setSize(CEGUI::USize(CEGUI::UDim(0.8, 0), CEGUI::UDim(0.2, 0)));
    clientButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.3, 0)));
    clientButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::search, this));

    // In Game Menu
    menuBox = wmgr.createWindow("Vanilla/FrameWindow", "CEGUIDemo/Menu");
    menuBox->setFont("Jura-Regular");
    menuBox->setText("Menu");
    menuBox->setSize(CEGUI::USize(CEGUI::UDim(0.2, 0), CEGUI::UDim(0.6, 0)));
    menuBox->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.2, 0)));
    menuBox->hide();

    // Resume Button
    resumeButton = menuBox->createChild("Vanilla/Button", "CEGUIDemo/Menu/QuitButton");
    resumeButton->setFont("Jura-Regular");
    resumeButton->setText("Resume");
    resumeButton->setSize(CEGUI::USize(CEGUI::UDim(0.8, 0), CEGUI::UDim(0.1, 0)));
    resumeButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.1, 0)));
    resumeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::resume, this));

    // Toggle Sound Button
    soundButton = menuBox->createChild("Vanilla/Button", "CEGUIDemo/Menu/SoundButton");
    soundButton->setFont("Jura-Regular");
    soundButton->setText("Sound Off");
    soundButton->setSize(CEGUI::USize(CEGUI::UDim(0.8, 0), CEGUI::UDim(0.1, 0)));
    soundButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.3, 0)));
    soundButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::sound, this));

    // Restart Button
    restartButton = menuBox->createChild("Vanilla/Button", "CEGUIDemo/Menu/RestartButton");
    restartButton->setFont("Jura-Regular");
    restartButton->setText("Restart");
    restartButton->setSize(CEGUI::USize(CEGUI::UDim(0.8, 0), CEGUI::UDim(0.1, 0)));
    restartButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.5, 0)));
    restartButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::restart, this));

    // Quit Button
    quitButton = menuBox->createChild("Vanilla/Button", "CEGUIDemo/Menu/QuitButton");
    quitButton->setFont("Jura-Regular");
    quitButton->setText("Quit");
    quitButton->setSize(CEGUI::USize(CEGUI::UDim(0.8, 0), CEGUI::UDim(0.1, 0)));
    quitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.7, 0)));
    quitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TutorialApplication::quit, this));

    sheet->addChild(menuBox);
    sheet->addChild(mainMenuBox);
    sheet->addChild(multiMenuBox);
}

//---------------------------------------------------------------------------

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"Button
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        TutorialApplication app;

        try {
            app.go();
        } catch(Ogre::Exception& e)  {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occurred: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------
