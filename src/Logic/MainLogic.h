#pragma once

#include "cocos2d.h"
#include "../Data/DataDef.h"

namespace UI 
{
	class MainLogic : public cocos2d::Application
	{
	public:
		enum GameState
		{
			GAME_NOT_START,
			GAME_RUNNING,
			GAME_PAUSE
		};
		GameState gameState;
		int gameRound;
		int playerAlive;
		LONGLONG delayPerRound;
		std::ofstream logFile;
		std::string loadFileName;
		std::ifstream ifsGameResult;
		std::vector<UI::TTower*>towers;
		std::vector<UI::TSoldier*>soldiers;
		std::vector<UI::TPlayer*>players;
		std::string mapFile;
	public:
		MainLogic();
		virtual ~MainLogic();
		virtual void initGLContextAttrs();
		virtual bool applicationDidFinishLaunching();
		virtual void applicationDidEnterBackground();
		virtual void applicationWillEnterForeground();
		void GameStart();
		void GameLoop();
		void GameOver();
		void GamePause();
		void GameResume();
		void UIUpdate();
		std::string GetFileNameByDialog();
		void LoadData();
		void LogicUpdate();
		void clearData();
	public:
		// Singleton.
		static MainLogic *m_pInstance;
		static MainLogic *GetInstance();
		static void DestroyInstance();
	};

}