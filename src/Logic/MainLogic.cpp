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
		gameState = GameState::GAME_NOT_START;
		mapFile = "gameMap.png";
		loadFileName = "./EnResult.txt";
		logFileStream.open("Log/log.txt", std::ios::out);
		logFileStream << "\n\n\n\n\n\n\n\nStarting...\n";
		playerAlive = 4;
		framesPerRound = 60;//Default setting
		gameRound = 0;
		
		logFileStream << "MainLogic ptr is " << this<<std::endl;

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
		
		
		
		// TODO : Try to deal with open failure.
		ifsGameResult.open(loadFileName, std::ios::in);
		if (!ifsGameResult.is_open()) return;
		MainLogic::GetInstance()->WriteLog("Succesfully loaded the file");
		//ifsGameResult.close();

		StartScene2PlayScene();
	}

	void MainLogic::LogicUpdate()
	{
		
		std::string strLine;
		std::string mark_type;
		int mark_lines;
		std::stringstream strstrm;
		while (true)
		{
			getline(ifsGameResult, strLine);
			strstrm.clear();
			strstrm << strLine;
			strstrm >> mark_type >> mark_lines;
			strstrm.clear();
			if (mark_type.empty()||mark_type == "RoundEnd")
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
	}

	void MainLogic::parseLines(const std::string&mark_type, const int&mark_lines)
	{
		std::string strLine;
		std::stringstream strstrm;
		
		if (mark_type == "RoundBegin")
		{
			if (mark_lines != 1)
				throw std::exception("Round info should be given in one line");
			getline(ifsGameResult, strLine);
			strstrm.clear();
			strstrm << strLine;
			strstrm >> gameRound;
			strstrm.clear();
		}
		else if (mark_type == "PlayerAlive")
		{
			if (mark_lines != 1)
				throw std::exception("PlayerAlive info should be given in one line");
			getline(ifsGameResult, strLine);
			strstrm.clear();
			strstrm << strLine;
			strstrm >> playerAlive;
			strstrm.clear();
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
				strstrm.clear();
				strstrm << strLine;
				strstrm >> temp_str >> id;
				strstrm.clear();
				getline(ifsGameResult, strLine);
				logFileStream.close();
				logFileStream.open("result.txt", std::ios::app);
				players[id]->Generate(strLine);
			}
			MainLogic::GetInstance()->WriteLog("Successfully loaded PlayerInfo");
		}
		else if (mark_type == "TowerInfo")
		{
			for (int i = 0; i < mark_lines; i++)
			{
				getline(ifsGameResult, strLine);
				towers[i]->Generate(strLine);
			}
		}
		else if (mark_type == "SoldierInfo")
		{
			int maxSoldierID;
			if (soldiers.size() <= 0)
				maxSoldierID = -1;
			else
				maxSoldierID =(*soldiers.cend()).first;
			
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
				strstrm.clear();
				strstrm << strLine;
				strstrm >> mark_player_id >> mark_commands_lines;
				for (int j = 0; j < mark_commands_lines; j++)
				{
					getline(ifsGameResult, strLine);
					strstrm.clear();
					strstrm << strLine;
					strstrm >> mark_type_command;
					newCommand = new Command();

					try
					{
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
						}
					}
					catch (const std::exception&)
					{
						delete newCommand;
						newCommand = nullptr;
					}
				}
			}
		}
	}

	void MainLogic::clearData()
	{
		WriteLog("ClearData...");
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
		logFileStream << message <<"   #### WHILE players.size =  "<<players.size()<< "\n";
	}
	void MainLogic::StartScene2PlayScene()
	{
		Director::getInstance()->replaceScene(PlayScene::createScene());
	}
}