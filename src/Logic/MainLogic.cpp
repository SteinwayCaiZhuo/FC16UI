#include "MainLogic.h"
#include "../UI/StartScene.h"
#include <tchar.h>
#include <windows.h>
#include <thread>
#include <chrono>

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

USING_NS_CC;

using namespace std;

static cocos2d::Size designResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

namespace UI
{
	MainLogic *MainLogic::m_pInstance = nullptr;

	MainLogic::MainLogic()
	{
		gameState = GameState::GAME_NOT_START;
		mapFile = "gameMap.png";
		loadFileName = "./EnResult.txt";
		playerAlive = 4;
		delayPerRound = 100;
		gameRound = 0;
		towers = *(new vector<UI::TTower*>());
		soldiers = *(new vector<UI::TSoldier*>());
		players = *(new vector<UI::TPlayer*>());
	}

	MainLogic::~MainLogic()
	{
#if USE_AUDIO_ENGINE
		AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
		SimpleAudioEngine::end();
#endif
	}

	void MainLogic::initGLContextAttrs()
	{
	}

	bool MainLogic::applicationDidFinishLaunching()
	{
		auto director = Director::getInstance();
		auto glview = director->getOpenGLView();
		if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
			glview = GLViewImpl::createWithRect("FC16UI1.0", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
			glview = GLViewImpl::create("FC16UI1.0");
#endif
			director->setOpenGLView(glview);
		}

		// turn on display FPS
		director->setDisplayStats(true);

		// set FPS. the default value is 1.0/60 if you don't call this
		director->setAnimationInterval(1.0f / 60);

		// Set the design resolution
		glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
		auto frameSize = glview->getFrameSize();
		// if the frame's height is larger than the height of medium size.
		if (frameSize.height > mediumResolutionSize.height)
		{
			director->setContentScaleFactor(MIN(largeResolutionSize.height / designResolutionSize.height, largeResolutionSize.width / designResolutionSize.width));
		}
		// if the frame's height is larger than the height of small size.
		else if (frameSize.height > smallResolutionSize.height)
		{
			director->setContentScaleFactor(MIN(mediumResolutionSize.height / designResolutionSize.height, mediumResolutionSize.width / designResolutionSize.width));
		}
		// if the frame's height is smaller than the height of medium size.
		else
		{
			director->setContentScaleFactor(MIN(smallResolutionSize.height / designResolutionSize.height, smallResolutionSize.width / designResolutionSize.width));
		}

		auto scene = StartScene::createScene();
		director->runWithScene(scene);
		return true;
	}

	void MainLogic::applicationDidEnterBackground()
	{
		Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
		AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
		SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
	}

	void MainLogic::applicationWillEnterForeground()
	{
		Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
		AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
		SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
		SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
	}

	void MainLogic::GameStart()
	{
		if (gameState != GameState::GAME_NOT_START)
			return;

		gameRound = 0;
		gameState = GameState::GAME_RUNNING;
		clearData();
		while (gameRound < MAX_ROUND)
		{
			GameLoop();
		};
		GameOver();
	}

	void MainLogic::GameLoop()
	{
		if (gameState != GameState::GAME_RUNNING)
		{
			return;
		}
		else
		{
			LogicUpdate();
			UIUpdate();
			this_thread::sleep_for(chrono::milliseconds(delayPerRound));
		}
	}

	void MainLogic::GameOver()
	{
		if (gameState == GameState::GAME_NOT_START)
		{
			return;
		}

		else
		{
			gameRound = 0;
			gameState = GameState::GAME_NOT_START;
			clearData();
		}

	}

	void MainLogic::GamePause()
	{
		if (gameState != GameState::GAME_RUNNING)
			return;
		else
		{
			gameState = GameState::GAME_PAUSE;
		}
	}

	void MainLogic::GameResume()
	{
		if (gameState != GameState::GAME_PAUSE)
			return;
		else
		{
			gameState = GameState::GAME_RUNNING;
		}
	}

	void MainLogic::UIUpdate()
	{
	}

	string MainLogic::GetFileNameByDialog()
	{
		constexpr int nMaxFileName = 256;
		char filename[nMaxFileName] = "";
		TCHAR buffer[nMaxFileName] = _T("");
		OPENFILENAME ofn;
		ofn.lpstrFile = buffer;
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFilter = _T("All\0*.*\0*.csv\0*.txt\0");
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		ofn.nMaxFile = nMaxFileName;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = _T("读取比赛结果文件");
		ofn.lpstrInitialDir = _T(".\\");
		if (GetOpenFileName(&ofn) == TRUE)
		{
			wcstombs(filename, ofn.lpstrFile, nMaxFileName);
		}
		return filename;
	}
	void MainLogic::LoadData()
	{
		loadFileName = GetFileNameByDialog();
		if (!loadFileName.size())
		{
			return;
		}
		// TODO : Try to deal with open failure.
		ifsGameResult.open(loadFileName, ios::in);
		// if (!ifsGameResult.is_open()) return;
		// ifsGameResult.close();
	}

	void MainLogic::LogicUpdate()
	{
		// NEED ENTIRE REVIEW & RECONSTITUTION.
		// WATCH OUT.
		/*
		FILE* fp;
		int playerID;
		int a, b, c, d, e, f, g, o, p;
		char h[30], m[30];
		if (fscanf_s(fp, "Round %d\n", &gameRound) != 2)
		{
			GameOver();
		}
		fscanf_s(fp, "PlayerAlive: %d\n", &playerAlive);
		

		for (int i = 0; i < PLAYER_NUM; i++)
		{
			fscanf_s(fp, "Player");
			fscanf_s(fp, "%d", &playerID);
			fscanf_s(fp, "Info\n");
			
			fscanf_s(fp, "Rank %d Score %d KillNum %d TowerNum %d SurvivalRound %d SoldierNum %d Resource %d MaxPopulation %d Population %d\n", &a, &b, &c, &d, &e, &f, &g, &o, &p);
			players[playerID]->rank = a;
			players[playerID]->score = b;
			players[playerID]->killNum = c;
			players[playerID]->towerNum = d;
			players[playerID]->survivalRound = e;
			players[playerID]->soldierNum = f;
			players[playerID]->resource = g;
			players[playerID]->maxPopulation = o;
			players[playerID]->population = p;
		}
		
		//Towers
		for (auto item : towers)
			delete item;
		towers.clear();
		TTower* newTower;

		fscanf_s(fp, "TowerInfo\n");
		while(fscanf_s(fp, "TowerID %d Owner %d Level %d Blood %d Recruiting %d RecruitingRound %d RecruitingType %s\n",
				&a, &b, &c, &d, &e, &f, h, 20)==7)
		{
			newTower = new TTower(a, b, c, d, e, f, string(h));
			towers.push_back(newTower);
		}
		fscanf_s(fp, "SoldierInfo\n");
		for (auto item : soldiers)
			delete item;
		soldiers.clear();
		TSoldier* newSoldier=nullptr;
		while (fscanf_s(fp, "SoldierID %d Owner %d Type %s Level %d Blood %d X_Position %d Y_Position %d\n", &a, &b, h, 20, &d, &e, &f, &g) == 7)
		{
			newSoldier = new TSoldier(a, b, h, d, e, f, g);
			soldiers.push_back(newSoldier);
		}

		fscanf_s(fp, "CommandsInfo\n");
		
		for (int i = 0; i < PLAYER_NUM; i++)
		{
			if (fscanf_s(fp, "Player%dCommands\n", &playerID) != 1)
				break;
			while (true)
			{
				if (fscanf_s(fp, "Command Move SoldierID %d Direction %s Distance %d\n", &a, h, 20, &b)==3)
				{
					try
					{
						soldiers[a]->soldierMove.move = true;
						soldiers[a]->soldierMove.moveDirection = moveDirStr2Enum(string(h));
						soldiers[a]->soldierMove.moveDistance = b;
					}
					catch (const exception&)
					{
						//Nothing
					}
					continue;
				}
				else if (fscanf_s(fp, "Command Attack SoldierID %d VicType %s VictimID %d\n", &a, h, 20, &b) == 3)
				{
					try
					{
						if (string(h) == "Soldier")
							soldiers[a]->victim = soldiers[b];
						else if (string(h) == "Tower")
							soldiers[a]->victim = towers[b];
					}
					catch (const exception&)
					{
						//Nothing
					}
					continue;
				}
				else if (fscanf_s(fp, "Command Upgrade TowerID %d\n", &a) == 1)
				{
					try
					{
						towers[a]->upgrade = true;
					}
					catch (const exception&)
					{

					}
					
					continue;
				}
				else if (fscanf_s(fp, "Command Produce TowerID %d Type %s\n", &a, h, 20) == 2)
				{
					try
					{
						towers[a]->produceSoldier.is_produce = true;
						towers[a]->produceSoldier.soldierType = SoldierTypeStr2Enum(string(h));
					}
					catch (const exception&)
					{

					}
				}
				else
				{
					break;
				}
			}
		}*/
	}

	void MainLogic::clearData()
	{
		loadFileName = "";			
	}
	MainLogic * MainLogic::GetInstance()
	{
		return m_pInstance;
	}
	void MainLogic::DestroyInstance()
	{
		// No delete for stack object
		// REVIEW NEEDED.
	}
}