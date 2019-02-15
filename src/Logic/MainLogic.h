#pragma once

#include "cocos2d.h"
#include "../Data/DataDef.h"

namespace UI {


	class MainLogic : public cocos2d::Application
	{
		enum GameState
		{
			GAME_NOT_START,
			GAME_RUNNING,
			GAME_PAUSE
		};
	public:
		GameState gameState;
		int gameRound;
		int playerAlive;
		float speed;
	public:
		std::ofstream logFileStream;

		std::string loadFileName;
		std::ifstream ifsGameResult;




		std::vector<UI::TTower*>towers;
		std::map<int, UI::TSoldier*>soldiers;
		std::vector<UI::TPlayer*>players;
		std::vector<UI::Command*>commands;
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

		std::string GetFileDialogName();
		void LoadData();
		bool LogicUpdate();
		void parseLines(const std::string& mark_type, const int& mark_lines);
		void UIUpdate(cocos2d::Scene* playscene);
		void clearData();
		void initData();
		void WriteLog(const std::string& message);
		void WriteInfo(const UI::TPlayer*player);
		void WriteInfo(const UI::TSoldier*soldier);
		void WriteInfo(const UI::TTower*tower);
		void WriteInfo(const UI::Command*command);

		void StartScene2PlayScene();
		void PlayScene2StartScene();
	
		//Singleton.
	public:
		static MainLogic* m_pInstance;
	
		static MainLogic* GetInstance();
		static void DestroyInstance();
	};

}