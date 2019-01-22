#include "StartScene.h"
#include "SimpleAudioEngine.h"
#include "../Logic/MainLogic.h"
USING_NS_CC;

namespace UI
{
	StartScene::StartScene(MainLogic* mainLogic):mainLogic(mainLogic)
	{
		if (nullptr == mainLogic)
		{
			
		}

	}
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
		auto loadFileItem = MenuItemImage::create("loadFileNormal.png", "loadFileSelected.png", CC_CALLBACK_1(StartScene::loadFileClickedCallback, this));
		
		

		return true;
	}

	void StartScene::loadFileClickedCallback(cocos2d::Ref* pSender)
	{
		mainLogic->LoadData();
	}
}