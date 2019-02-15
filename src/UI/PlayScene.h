#pragma once
#include "../Data/DataDef.h"
#include "ui/CocosGUI.h"
#include <atomic>
namespace UI
{
	class PlayScene : public cocos2d::Scene
	{
	public:
		static cocos2d::Scene* createScene();

		virtual bool init();

		CREATE_FUNC(PlayScene);
		void clear();
		void RefreshMap(float dt);
		static void ContinuousDisplay();

		void StartClickedCallback();


		cocos2d::TMXTiledMap* map_widget;

		cocos2d::ui::Button* start_btn;

		cocos2d::Label* roundLabel;
		std::atomic<bool>exit_thread_flag{ true};
		
    cocos2d::ui::EditBox* roundInfo;
		
		std::thread* displayThread;

		cocos2d::Scheduler* refresh_control;

		void Command2Actions(UI::Command* command);
		
		static PlayScene* m_pInstance;

	private:
		bool is_pause;

		
	};

}