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
		LONGLONG delayPerRound;	//ms
	private:
		std::ofstream logFile;
		
		std::string loadFileName;
		std::vector<std::stringstream*> loadFileStream;


		std::vector<UI::UIObject*>uiObjects;

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

		std::string getFileDialogName();
		void LoadData();
		void LogicUpdate();
		void UIUpdate();
		void clearData();
		void WriteLog(std::string& message);
	};

}