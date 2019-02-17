#include "cocos2d.h"
#include "../Data/DataDef.h"
#include "PlayScene.h"
#include "../Logic/MainLogic.h"
#include "ui/CocosGUI.h"
//TODO delete iostream
#include <iostream>
#include <thread>
#include <chrono>
USING_NS_CC;

UI::PlayScene* UI::PlayScene::m_pInstance = nullptr;

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
	//resetDirectory();
	displayThread = nullptr;

	m_pInstance = this;

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	map_widget = TMXTiledMap::create("test.tmx");
	if (map_widget == nullptr)
	{
	}
	else
	{
		map_widget->setScale(Director::getInstance()->getOpenGLView()->getFrameSize().height / map_widget->getContentSize().height);

		this->addChild(map_widget, 0);
	}
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

	roundLabel = Label::createWithTTF("Game Not Start", "fonts/Marker Felt.ttf", 24);
	roundLabel->setPosition(Vec2(900, 640));
	this->addChild(roundLabel);
	//this->schedule(schedule_selector(UI::PlayScene::RefreshMap), 2.0f);

  roundInfo = ui::EditBox::create(Size(200, 400), "start.jpg");
  roundInfo->setPosition(Vec2(900, 300));
  roundInfo->setFontSize(11);
  this->addChild(roundInfo);
	
	this->is_pause = true;

  //TODO temp load to enable load those picture in member function, try to fix it
  auto up_arrow = Sprite::create("upgrade.png");
  up_arrow->setPosition(Vec2(1300, 300));
  up_arrow->setVisible(false);
  this->addChild(up_arrow, 1);

  auto new_sign = Sprite::create("produce.png");
  new_sign->setPosition(Vec2(1300, 300));
  new_sign->setVisible(false);
  this->addChild(new_sign, 1);

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
			soldiers->setTileGID(SOLDIER_SET_START, Vec2(i, j));
		}
	}
	for (std::map<int, UI::TSoldier*>::iterator i = MainLogic::GetInstance()->soldiers.begin();
		i != MainLogic::GetInstance()->soldiers.end(); ++i) {
		//TODO : whether need coordinates change
		//soldiers->setTileGID((*i).second->Info2GID(), (*i).second->m_vec2Position);	
		soldiers->setTileGID((*i).second->Info2GID(), (*i).second->m_vec2Position);
		//background->setTileGID(4, (*i).second->m_vec2Position);
	}


	for (std::vector<UI::Command*>::iterator i = MainLogic::GetInstance()->commands.begin();
		i != MainLogic::GetInstance()->commands.end(); ++i) {
		Command2Actions(*i);
	}
	is_pause = true;
	this->_scheduler->pauseTarget(this);
}

void UI::PlayScene::ContinuousDisplay()
{
	while (UI::PlayScene::m_pInstance->is_pause == false)
	{
		MainLogic::GetInstance()->WriteLog("RefreshMap began");

		//refreshmap
		if (!MainLogic::GetInstance()->LogicUpdate())
		{
			MainLogic::GetInstance()->WriteLog("logic update failed");
			break;
		}
		MainLogic::GetInstance()->WriteLog("Logic update finished ");
		TMXLayer* background = UI::PlayScene::m_pInstance->map_widget->getLayer("background");
		TMXLayer* soldiers = UI::PlayScene::m_pInstance->map_widget->getLayer("soldiers");

		//clear
		for (int i = 0; i < UI::PlayScene::m_pInstance->map_widget->getMapSize().width; i++) {
			for (int j = 0; j < UI::PlayScene::m_pInstance->map_widget->getMapSize().width; j++) {
				soldiers->setTileGID(SOLDIER_SET_START, Vec2(i, j));
			}
		}
		for (std::map<int, UI::TSoldier*>::iterator i = MainLogic::GetInstance()->soldiers.begin();
			i != MainLogic::GetInstance()->soldiers.end(); ++i) {
			//TODO : whether need coordinates change
			soldiers->setTileGID((*i).second->Info2GID(), (*i).second->m_vec2Position);
		}

		MainLogic::GetInstance()->WriteLog("Beginning command actions");
		//auto delay1 = DelayTime::create(0.001);
		//auto delay2 = delay1->clone();
		//auto my_prevSeq = Sequence::create(delay1,delay2);
		//auto my_newSeq= my_prevSeq;
		for (auto item:MainLogic::GetInstance()->commands) {
			
			//my_newSeq = Sequence::create(my_prevSeq, cocos2d::CallFunc::create([&]() {
			//	UI::PlayScene::m_pInstance->Command2Actions(*i);
			//}), DelayTime::create(1));
			//my_prevSeq = my_newSeq;
			UI::PlayScene::m_pInstance->Command2Actions(item);
		}
		//my_newSeq = Sequence::create(my_prevSeq, nullptr);
		//UI::PlayScene::m_pInstance->runAction(my_newSeq);


		UI::PlayScene::m_pInstance->removeChild(UI::PlayScene::m_pInstance->roundLabel);
		UI::PlayScene::m_pInstance->roundLabel->release();
		UI::PlayScene::m_pInstance->roundLabel = Label::createWithTTF("Round "+ std::to_string(MainLogic::GetInstance()->gameRound), "fonts/Marker Felt.ttf", 24);
		UI::PlayScene::m_pInstance->roundLabel->setPosition(Vec2(900, 640));
		UI::PlayScene::m_pInstance->addChild(UI::PlayScene::m_pInstance->roundLabel);

		std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(int(MainLogic::GetInstance()->speed*1000)));
	}
	//MainLogic::GetInstance()->PlayScene2StartScene();
}

void UI::PlayScene::StartClickedCallback()
{
	
	if (start_btn->getTitleText() == "start")
	{
		MainLogic::GetInstance()->GameStart();
		start_btn->setTitleText("stop");
		is_pause = false;
		//std::thread t(&UI::PlayScene::ContinuousDisplay, this);
		std::thread t([] {
			MainLogic::GetInstance()->WriteLog("My thread");
			UI::PlayScene::ContinuousDisplay();
		});
		displayThread = &t;
		t.detach();
	}
	else if (start_btn->getTitleText() == "stop")
	{
		if (displayThread != nullptr)
		{
			displayThread->~thread();
			displayThread = nullptr;
		}
		start_btn->setTitleText("start");

	}
	//this->_scheduler->resumeTarget(this);
}

void UI::PlayScene::Command2Actions(UI::Command* command)
{
	TMXLayer* soldiers = map_widget->getLayer("soldiers");
  TMXLayer* background = map_widget->getLayer("background");

	if (command->m_nCommandType == Attack)
	{
		MainLogic::GetInstance()->WriteLog("Command2Actions----Attack");

		UI::TSoldier* attacker = dynamic_cast<UI::TSoldier*>(command->m_pAttackObject);
		UI::TSoldier* victim_s = dynamic_cast<UI::TSoldier*>(command->m_pVictimObject);
		UI::TTower* victim_t = dynamic_cast<UI::TTower*>(command->m_pVictimObject);

		if (attacker == nullptr)
		{
			MainLogic::GetInstance()->WriteLog("attacker null");
			return;
		}
		auto action1 = Blink::create(MainLogic::GetInstance()->speed*0.5f, 1);
		soldiers->getTileAt(attacker->m_vec2Position)->runAction(action1);
		while(action1->isDone() == false);
    
		if (victim_s != nullptr)
		{
			auto action2 = MoveBy::create(MainLogic::GetInstance()->speed*1.f, Vec2(0.f, 0.f));
			soldiers->getTileAt(victim_s->m_vec2Position)->runAction(action2);
			while (action2->isDone() == false);

			auto action3 = Blink::create(MainLogic::GetInstance()->speed*1.f, 3);
			soldiers->getTileAt(victim_s->m_vec2Position)->runAction(action3);
			while (action3->isDone()== false);
		}

		if (victim_t != nullptr) {
			auto action4 = MoveBy::create(MainLogic::GetInstance()->speed*1.f, Vec2(0.f, 0.f));	
			soldiers->getTileAt(victim_t->m_vec2Position)->runAction(action4);
			while (action4->isDone() == false);

			auto action5 = Blink::create(MainLogic::GetInstance()->speed*1.f, 3);
      Vec2 center = victim_t->m_vec2Position;
			background->getTileAt(center)->runAction(action5);
      //TODO find a better way
      background->getTileAt(Vec2(center.x - 1, center.y - 1))->runAction(action5->clone());
      background->getTileAt(Vec2(center.x - 1, center.y + 0))->runAction(action5->clone());
      background->getTileAt(Vec2(center.x - 1, center.y + 1))->runAction(action5->clone());
      background->getTileAt(Vec2(center.x + 0, center.y - 1))->runAction(action5->clone());
      background->getTileAt(Vec2(center.x + 0, center.y + 1))->runAction(action5->clone());
      background->getTileAt(Vec2(center.x + 1, center.y - 1))->runAction(action5->clone());
      background->getTileAt(Vec2(center.x + 1, center.y + 0))->runAction(action5->clone());
      background->getTileAt(Vec2(center.x + 1, center.y + 1))->runAction(action5->clone());
			while (action5->isDone() == false);
		}
    
		return;
	}

	if (command->m_nCommandType == Move)
	{
		MainLogic::GetInstance()->WriteLog("Command2Actions----Move");

		UI::TSoldier* mover = dynamic_cast<UI::TSoldier*>(command->m_pMoveSoldier);

		if (mover == nullptr) {
			MainLogic::GetInstance()->WriteLog("mover null");
			return;
		}
		Vec2 delta(0, 0);
		switch (command->m_nMoveDirection) {
		case LEFT: {
			delta.x = -command->m_nMoveDistance;
			break;
		}
		case RIGHT: {
			delta.x = command->m_nMoveDistance;
			break;
		}
		case DOWN: {
			delta.y = -command->m_nMoveDistance;
			break;
		}
		case UP: {
			delta.y = command->m_nMoveDistance;
			break;
		}
		default:
			MainLogic::GetInstance()->WriteLog("error move type");
		}
		Action* move = MoveBy::create(MainLogic::GetInstance()->speed*0.3f, delta * soldiers->getMapTileSize().height);
		
		auto move_soldier = soldiers->getTileAt(mover->m_vec2Position);
		move_soldier->runAction(move);
		while (move->isDone() == false);
		return;
	}

	//TODO tower action
	if (command->m_nCommandType == Upgrade)
	{
    MainLogic::GetInstance()->WriteLog("Command2Actions----Upgrade");

    auto up_arrow = Sprite::create("upgrade.png");
    if (up_arrow == nullptr) 
    {
      MainLogic::GetInstance()->WriteLog("Cannot create upgrade arrow sprite");
    }
    //up_arrow->setScale(map_widget->getScale());
    this->addChild(up_arrow, 2);
    //TODO FIX anchor error
    up_arrow->setAnchorPoint(Vec2(0.4, 0));
    up_arrow->setPosition(background->getTileAt(command->m_pUpgradeTower->m_vec2Position)->getPosition() * map_widget->getScale());
    auto up = MoveBy::create(MainLogic::GetInstance()->speed*1.f, Vec2(0, 5));
    auto back = MoveBy::create(0.01f, Vec2(0, 5));
    auto up_actions = Sequence::createWithTwoActions(up, back);
    
    up_arrow->runAction(up);
    while (up->isDone() == false);
    up_arrow->setVisible(false);

    //TODO find a release way to save memory
    /*up_arrow->runAction(up_actions);
    while (up_actions->isDone());
    up_arrow->setVisible(false);*/

    /*while (up->isDone() == false);
    up_arrow->runAction(back);
    while (back->isDone() == false);
    up_arrow->setVisible(false);*/

    //up_arrow->release();
    //delete up_arrow;
		return;
	}

	if (command->m_nCommandType == Produce)
	{

    MainLogic::GetInstance()->WriteLog("Command2Actions----Produce");

    auto new_sign = Sprite::create("produce.png");
    if (new_sign == nullptr) {
      MainLogic::GetInstance()->WriteLog("Cannot create produce arrow sprite");
    }
    //up_arrow->setScale(map_widget->getScale());
    this->addChild(new_sign, 2);
    //TODO FIX anchor error
    new_sign->setAnchorPoint(Vec2(0.4, 0));
    new_sign->setPosition(background->getTileAt(command->m_pProduceTower->m_vec2Position)->getPosition() * map_widget->getScale());
    auto up = MoveBy::create(MainLogic::GetInstance()->speed*1.f, Vec2(0, 5));
    auto back = MoveBy::create(0.01f, Vec2(0, 5));
    auto up_actions = Sequence::createWithTwoActions(up, back);

    new_sign->runAction(up);
    while (up->isDone() == false);
    new_sign->setVisible(false);

		return;
	}
}