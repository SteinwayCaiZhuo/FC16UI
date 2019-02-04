#include "PlayScene.h"
#include "../Logic/MainLogic.h"
#include "ui/CocosGUI.h"
//TODO delete iostream
#include <iostream>
USING_NS_CC;


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
	map_widget->setScale(Director::getInstance()->getOpenGLView()->getFrameSize().height / map_widget->getContentSize().height);
  
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
	this->schedule(schedule_selector(UI::PlayScene::RefreshMap), 2.0f);
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

void UI::PlayScene::StartClickedCallback()
{
	MainLogic::GetInstance()->GameStart();
	start_btn->setTitleText("stop");
	is_pause = false;
	this->_scheduler->resumeTarget(this);
}

void UI::PlayScene::Command2Actions(UI::Command* command)
{
  //TODO fix all command show at a time
  TMXLayer* soldiers = map_widget->getLayer("soldiers");

  if (command->m_nCommandType == Attack) 
  {
    UI::TSoldier* attacker = dynamic_cast<UI::TSoldier*>(command->m_pAttackObject);
    UI::TSoldier* victim_s = dynamic_cast<UI::TSoldier*>(command->m_pVictimObject);
    UI::TTower* victim_t = dynamic_cast<UI::TTower*>(command->m_pVictimObject);

    if (attacker == nullptr)  
    {
      MainLogic::GetInstance()->WriteLog("attacker null");
      return;
    }

    soldiers->getTileAt(attacker->m_vec2Position)->runAction(
      Blink::create(0.5f, 3)
    );

    //pause 0.5sec
    soldiers->getTileAt(attacker->m_vec2Position)->runAction(
      MoveBy::create(0.5f, Vec2(0.f, 0.f))
    );

    if (victim_s != nullptr)       
    {
      soldiers->getTileAt(victim_s->m_vec2Position)->runAction(
        MoveBy::create(1.f, Vec2(0.f, 0.f))
      );
      soldiers->getTileAt(victim_s->m_vec2Position)->runAction(
        Blink::create(1.f, 3)
      );
    }

    if (victim_t != nullptr) {
      soldiers->getTileAt(victim_t->m_vec2Position)->runAction(
        MoveBy::create(1.f, Vec2(0.f, 0.f))
      );
      soldiers->getTileAt(victim_t->m_vec2Position)->runAction(
        Blink::create(1.f, 3)
      );
    }

    return;
  }

  if (command->m_nCommandType == Move) 
  {
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
    Action* move = MoveBy::create(0.3f, delta * soldiers->getMapTileSize().height);
    auto move_soldier = soldiers->getTileAt(mover->m_vec2Position);
    move_soldier->runAction(move);
    return;
  }

  //TODO tower action
  if (command->m_nCommandType == Upgrade) 
  {
    return;
  }

  if (command->m_nCommandType == Produce) 
  {
    return;
  }
}

