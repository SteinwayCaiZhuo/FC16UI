#include "StartScene.h"
#include "SimpleAudioEngine.h"
#include "../Logic/MainLogic.h"
#include "ui/CocosGUI.h"
#include "ui/UIButton.h"

USING_NS_CC;

namespace UI
{
	
	Scene* StartScene::createScene()
	{
		//auto layer = 
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

		auto startSprite = Sprite::create("start.jpg");
		startSprite->setScale(0.5);
		startSprite->setPosition(500, 300);
		this->addChild(startSprite, 0);
		
		ui::Button* openFileButton = ui::Button::create();
		openFileButton->setTitleText("Select a File");
		openFileButton->setPosition(Vec2(600, 400));
		openFileButton->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
			switch (type) {
			case cocos2d::ui::Widget::TouchEventType::ENDED:
				this->loadFileClickedCallback(sender);
				break;
			default:
				break;
			}
		});

		this->addChild(openFileButton, 1);
		return true;
	}

	void StartScene::loadFileClickedCallback(cocos2d::Ref* pSender)
	{
		MainLogic::GetInstance()->WriteLog("Load File Clicked Call back");
		MainLogic::GetInstance()->LoadData();
		
	}
}

