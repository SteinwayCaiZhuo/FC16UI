#include "PlayScene.h"
#include "../Logic/MainLogic.h"
#include "ui/CocosGUI.h"
//TODO delete iostream
#include <iostream>
USING_NS_CC;
#define SOLDIER_SER_START 17

cocos2d::Scene* UI::PlayScene::createScene()
{
	return PlayScene::create();
}

bool UI::PlayScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	map_widget = TMXTiledMap::create("test.tmx");
	map_widget->setPosition(-200, -200);
	map_widget->setScale(0.35f);
	this->addChild(map_widget, 0);

	start_btn = ui::Button::create();
	start_btn->setTitleText("start");
	start_btn->setPosition(Vec2(980, 600));
	start_btn->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type) {
		case cocos2d::ui::Widget::TouchEventType::ENDED:
			this->StartClickedCallback();
			break;
		default:
			break;
		}
	});

	this->addChild(start_btn, 1);
	MainLogic::GetInstance()->WriteLog("before shedule");
	this->schedule(schedule_selector(UI::PlayScene::RefreshMap), 0.1f);
	MainLogic::GetInstance()->WriteLog("After this->schedule");
	this->is_pause = true;
	return true;
}

void UI::PlayScene::RefreshMap(float dt)
{
	MainLogic::GetInstance()->WriteLog("RefreshMap began");
	if (is_pause) {
		this->_scheduler->pauseTarget(this);
		return;
	}
	// TODO this is a show state part, delete it later
	if (start_btn->getTitleText() == "refresh") {
		start_btn->setTitleText("again refresh");
	}
	else {
		start_btn->setTitleText("refresh");
	}

	//refreshmap
	
	MainLogic::GetInstance()->LogicUpdate();
	
	TMXLayer* background = map_widget->getLayer("background");
	TMXLayer* soldiers = map_widget->getLayer("soldiers");

	//clear
	for (int i = 0; i < map_widget->getMapSize().width; i++) {
		for (int j = 0; j < map_widget->getMapSize().width; j++) {
			soldiers->setTileGID(SOLDIER_SER_START, Vec2(i, j));
		}
	}
	for (std::map<int, UI::TSoldier*>::iterator i = MainLogic::GetInstance()->soldiers.begin();
		i != MainLogic::GetInstance()->soldiers.end(); ++i) {
		//TODO : 
		//soldiers->setTileGID((*i).second->Info2GID(), (*i).second->m_vecPosition);
	
	}

	is_pause = true;
	this->_scheduler->pauseTarget(this);
}

void UI::PlayScene::StartClickedCallback()
{
	MainLogic::GetInstance()->WriteLog("Start clicked");
	MainLogic::GetInstance()->GameStart();
	start_btn->setTitleText("stop");
	is_pause = false;
	this->_scheduler->resumeTarget(this);
}




