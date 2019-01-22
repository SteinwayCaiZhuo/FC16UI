#include "StartScene.h"
#include "SimpleAudioEngine.h"
#include "../Logic/MainLogic.h"
USING_NS_CC;

namespace UI
{
	
	Scene* StartScene::createScene()
	{
		
		return StartScene::create();
	}

	bool StartScene::init()
	{
		if (!Scene::init())
		{
			return false;
		}

		auto visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 origin = Director::getInstance()->getVisibleOrigin();

		//auto closeItem = MenuItemImage::create("closeNormal.png", "closeSelected.png",CC_CALLBACK_1(....))
		auto loadFileItem = MenuItemImage::create("loadFileNormal.jpg", "loadFileSelected.jpg", CC_CALLBACK_1(StartScene::loadFileClickedCallback, this));
		
		

		return true;
	}

	void StartScene::loadFileClickedCallback(cocos2d::Ref* pSender)
	{
		MainLogic::LoadData();
	}
}