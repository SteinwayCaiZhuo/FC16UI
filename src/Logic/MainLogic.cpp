// TODO FIX:cannot add command of Player1 when Player0 dont have command and Player2 have command
#include "MainLogic.h"
#include "../UI/StartScene.h"
#include "../UI/PlayScene.h"

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

static cocos2d::Size designResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

namespace UI
{

	MainLogic* MainLogic::m_pInstance = nullptr;

	

	MainLogic::MainLogic()
	{
		resetDirectory();
		gameState = GameState::GAME_NOT_START;
		mapFile = "gameMap.png";
		loadFileName = "result.txt";
		logFileStream.open("Log/log.txt", std::ios::out);
		logFileStream << "Starting..\n";
		playerAlive = 4;
		speed = 0.4;//Default setting
		gameRound = 0;

		logFileStream << "MainLogic ptr is " << this << std::endl;

		


		for (int i = 0; i < PLAYER_NUM; i++)
		{
			players.push_back(new TPlayer());
			logFileStream << "Add new player: " << players[i] << std::endl;
		}
		for (int i = 0; i < TOWER_NUM; i++)
			towers.push_back(new TTower());
	}


	MainLogic::~MainLogic()
	{
#if USE_AUDIO_ENGINE
		AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
		SimpleAudioEngine::end();
#endif
		logFileStream << "MainLogic destruction..\n";
		logFileStream.close();
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
		//director->setDisplayStats(true);

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
		MainLogic::clearData();
		

	}

	void MainLogic::GameLoop()
	{

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
			MainLogic::clearData();
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

	void MainLogic::UIUpdate(cocos2d::Scene* playScene)
	{
	}

	std::string MainLogic::GetFileDialogName()
	{
		constexpr int nMaxFileName = 256;
		char filename[nMaxFileName] = "";
		TCHAR buffer[nMaxFileName] = _T("");
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lpstrFile = buffer;
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFilter = _T("All Files\0*.*\0*.csv\0*.txt\0");
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		ofn.nMaxFile = nMaxFileName;
		ofn.nFilterIndex = 1;
		ofn.lpstrTitle = _T("读取比赛结果文件");
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = _T(".\\");
		if (GetOpenFileName(&ofn) == TRUE)
		{
			wcstombs(filename, ofn.lpstrFile, nMaxFileName);
		}
		MainLogic::GetInstance()->WriteLog("FILENAME IS " + std::string(filename));

		return std::string(filename);
	}
	void MainLogic::LoadData()
	{
		loadFileName = GetFileDialogName();
		//loadFileName = "result.txt";
		if (!loadFileName.size())
		{
			return;
		}



		ifsGameResult.open(loadFileName, std::ios::in);
		if (!ifsGameResult.is_open()) return;
		MainLogic::GetInstance()->WriteLog("Succesfully loaded the file");
		//ifsGameResult.close();

		StartScene2PlayScene();
	}

	bool MainLogic::LogicUpdate()
	{

		std::string strLine;
		std::string mark_type;
		int mark_lines;
		std::stringstream strstrm("test");
		while (true)
		{
			getline(ifsGameResult, strLine);
			
			//End of game
			if (strLine.empty())
				return false;

			MyClear(strstrm);
			strstrm << strLine;
			strstrm >> mark_type >> mark_lines;
			MyClear(strstrm);
			WriteLog("Got mark_type is: " + mark_type);
			if (mark_type.empty() || mark_type == "RoundEnd")
				break;
			else
			{
				try
				{
					parseLines(mark_type, mark_lines);
				}
				catch (std::exception&)
				{
					MainLogic::GetInstance()->WriteLog("Logic Update error!");
				}
			}

		}
		return true;
	}

	void MainLogic::parseLines(const std::string&mark_type, const int&mark_lines)
	{
		std::string strLine;
		std::stringstream strstrm("init");

		if (mark_type == "RoundBegin")
		{
			if (mark_lines != 1)
				throw std::exception("Round info should be given in one line");
			getline(ifsGameResult, strLine);
			MyClear(strstrm);
			strstrm << strLine;
			strstrm >> gameRound;
			MyClear(strstrm);
			WriteLog("in Game round: " + std::to_string(gameRound) + "\n");
		}
		else if (mark_type == "PlayerAlive")
		{
			if (mark_lines != 1)
				throw std::exception("PlayerAlive info should be given in one line");
			getline(ifsGameResult, strLine);
			MyClear(strstrm);
			strstrm << strLine;
			strstrm >> playerAlive;
			MyClear(strstrm);
		}
		else if (mark_type == "PlayerInfo")
		{
			if (players.size() < PLAYER_NUM)
			{
				clearData();
				initData();

			}
			if (mark_lines > PLAYER_NUM)
				throw std::exception("PlayerInfo should be given less than 4 lines");
			std::string temp_str;
			int id;

			for (int i = 0; i < mark_lines; i++)
			{
				getline(ifsGameResult, strLine);
				MyClear(strstrm);
				strstrm << strLine;
				strstrm >> temp_str >> id;
				MyClear(strstrm);
				getline(ifsGameResult, strLine);

				players[id]->Generate(strLine);
				players[id]->m_nID = id;
			}

		}
		else if (mark_type == "TowerInfo")
		{
			for (int i = 0; i < mark_lines; i++)
			{
				getline(ifsGameResult, strLine);
				towers[i]->Generate(strLine);
        towers[i]->SetVec2Position(i);
			}
		}
		else if (mark_type == "SoldierInfo")
		{
			int maxSoldierID = maxKey(soldiers);

			//Clear old soldiers
			for (auto item : soldiers)
			{
				delete item.second;
			}
			soldiers.clear();

			//Generate new soldiers
			for (int i = 0; i < mark_lines; i++)
			{
				getline(ifsGameResult, strLine);
				try
				{
					TSoldier* newSoldier = new TSoldier();
					newSoldier->Generate(strLine);
					soldiers.emplace(newSoldier->m_nID, newSoldier);

					if (newSoldier->m_nID > maxSoldierID)
						newSoldier->m_bFreshman = true;

				}
				catch (const std::exception&)
				{

				}
			}


		}

		//Generate Command
		else if (mark_type == "CommandsInfo")
		{
			std::string mark_type_command, temp_str;
			int mark_player_id, mark_commands_lines, mark_info_command;

			for (auto item : commands)
				delete item;
			commands.clear();
			UI::Command* newCommand = nullptr;




			for (int i = 0; i < mark_lines; i++)
			{
				getline(ifsGameResult, strLine);
				MyClear(strstrm);
				strstrm << strLine;
				strstrm >> mark_player_id >> mark_commands_lines;
				MyClear(strstrm);

				for (int j = 0; j < mark_commands_lines; j++)
				{
					getline(ifsGameResult, strLine);
					MyClear(strstrm);
					strstrm << strLine;
					strstrm >> mark_type_command;

					newCommand = new Command();

					try
					{
						newCommand->m_pOwner = players[mark_player_id];
						if (mark_type_command == "Move")
						{
							newCommand->m_nCommandType = UI::CommandType::Move;

							//SoldierID
							strstrm >> mark_type_command >> mark_info_command;
							newCommand->m_pMoveSoldier = soldiers[mark_info_command];
							//Direction
							strstrm >> mark_type_command >> temp_str;
							newCommand->m_nMoveDirection = moveDirStr2Enum(temp_str);
							//Distance
							strstrm >> mark_type_command >> mark_info_command;
							newCommand->m_nMoveDistance = mark_info_command;

							newCommand->m_pMoveSoldier->m_strctSoldierMove = UI::SoldierMoveType{ true, newCommand->m_nMoveDirection, newCommand->m_nMoveDistance };

						}
						else if (mark_type_command == "Attack")
						{
							newCommand->m_nCommandType = UI::CommandType::Attack;
							//SoldierID
							strstrm >> mark_type_command >> mark_info_command;
							newCommand->m_pAttackObject = soldiers[mark_info_command];
							//VicType
							strstrm >> mark_type_command >> temp_str;
							strstrm >> mark_type_command >> mark_info_command;
							if (temp_str == "Soldier")
								newCommand->m_pVictimObject = soldiers[mark_info_command];
							else if (temp_str == "Tower")
								newCommand->m_pVictimObject = towers[mark_info_command];
							newCommand->m_pAttackObject->m_pVictim = newCommand->m_pVictimObject;
						}
						else if (mark_type_command == "Upgrade")
						{
							newCommand->m_nCommandType = UI::CommandType::Upgrade;
							strstrm >> temp_str >> mark_info_command;
							newCommand->m_pUpgradeTower = towers[mark_info_command];

						}
						else if (mark_type_command == "Produce")
						{
							newCommand->m_nCommandType = UI::CommandType::Produce;

							strstrm >> temp_str >> mark_info_command;
							newCommand->m_pProduceTower = towers[mark_info_command];
							strstrm >> temp_str >> mark_type_command;
							newCommand->m_nProduceSoldierType = UI::SoldierTypeStr2Enum(mark_type_command);
						}
					}

					catch (const std::exception&)
					{
						delete newCommand;
						newCommand = nullptr;
					}

					MainLogic::GetInstance()->commands.push_back(newCommand);
				}
			}
			MyClear(strstrm);
		}
	}

	void MainLogic::clearData()
	{
		WriteLog("ClearData..");
		loadFileName = "";

		for (auto item : players)
			delete item;
		players.clear();
		for (auto item : towers)
			delete item;
		towers.clear();
		for (auto item : soldiers)
			delete item.second;
		soldiers.clear();
		for (auto item : commands)
			delete item;
		commands.clear();
	}

	void MainLogic::initData()
	{
		for (int i = 0; i < PLAYER_NUM; i++)
			players.push_back(new TPlayer());
		for (int i = 0; i < TOWER_NUM; i++)
			towers.push_back(new TTower());
		WriteLog("Initializing data...");
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

	void MainLogic::WriteLog(const std::string& message)
	{
		logFileStream << message << std::endl;
	}
	void MainLogic::StartScene2PlayScene()
	{
		Director::getInstance()->replaceScene(PlayScene::createScene());
		
	}
	void MainLogic::PlayScene2StartScene()
	{
		Director::getInstance()->replaceScene(StartScene::createScene());
	}
}