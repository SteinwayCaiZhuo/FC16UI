#include "DataDef.h"

namespace UI
{
	const int PLAYER_NUM = 4;
	const int MAX_ROUND = 300;
	std::string logFileName;
	
	UIObject::UIObject(UIObjectType uiObjectType, bool visiable):uiObjectType(uiObjectType), visiable(visiable){}

	UIObject::~UIObject()
	{
	}
	
	
	TTower::TTower() :UIObject(UIObject ::UIObjectType::TowerType)
	{
	}

	TTower::~TTower()
	{
	}

	void TTower::clear()
	{
	}

	void TTower::UIUpdate()
	{
	}

	TSoldier::TSoldier() :UIObject(UIObject::UIObjectType::SoldierType)
	{
	}

	TSoldier::~TSoldier()
	{
	}

	void TSoldier::clear()
	{
	}

	void TSoldier::UIUpdate()
	{
	}

	TPlayer::TPlayer() :UIObject(UIObject::UIObjectType::PlayerType)
	{
	}

	TPlayer::~TPlayer()
	{
	}

	void TPlayer::clear()
	{
	}

	void TPlayer::UIUpdate()
	{
	}

}
