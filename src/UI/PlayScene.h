#pragma once
#include "../Data/DataDef.h"


namespace UI
{
	class PlayScene : public cocos2d::Scene
	{
	public:
		static cocos2d::Scene* createScene();

		virtual bool init();

		CREATE_FUNC(PlayScene);
	};

}