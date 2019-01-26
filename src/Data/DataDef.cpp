#include "DataDef.h"
#include "../Logic/MainLogic.h"

using namespace std;

namespace UI
{
	int PLAYER_NUM = 4;
	int MAX_ROUND = 300;
	std::string logFileName = "Log/log.txt";
	
	map<string, int> TPlayer::LUTPLAYER;

	UIObject::UIObject(const UIObjectType &ut, const bool &v) : m_nType(ut), m_bVisible(v)
	{
		m_bGenerated = false;
	}

	UIObject::UIObject(UIObjectType &&ut, bool &&v) : m_nType(ut), m_bVisible(v)
	{
		m_bGenerated = false;
	}

	UIObject::~UIObject()
	{

	}

	void UIObject::SetVisible(const bool &v)
	{
		m_bVisible = v;
	}

	bool UIObject::GetVisible()
	{
		return m_bVisible;
	}

	void UIObject::SetType(const UIObjectType &ut)
	{
		m_nType = ut;
	}

	UIObjectType UIObject::GetType()
	{
		return m_nType;
	}

	bool UIObject::IsGenerated()
	{
		return m_bGenerated;
	}
	
	
	TTower::TTower() :UIObject(UIObjectType::TowerType)
	{
	}

	TTower::TTower(int towerID, int owner, int level, int blood, int recruiting, int recruitingRound, std::string recruitingType):UIObject(UIObjectType::TowerType)
	{
		this->ID = towerID;
		if(owner!=-1)
			this->owner = MainLogic::GetInstance()->players[owner];
		else
		{
			this->owner = nullptr;
		}
		this->level = TTower::TowerLEVEL(level);
		this->blood = blood;
		this->recruiting = recruiting;
		this->recruitingRound = recruitingRound;
		this->recruitingType = SoldierTypeStr2Enum(recruitingType);

		this->upgrade = false;
		this->produceSoldier = ProduceType{ false, TSoldier::NoneSoldierType };
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

	TSoldier::TSoldier() :UIObject(UIObjectType::SoldierType)
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

	TPlayer::TPlayer() : UIObject(UIObjectType::PlayerType)
	{
		if (LUTPLAYER.empty())
		{
			LUTPLAYER.emplace("Rank", 0);
			LUTPLAYER.emplace("Score", 1);
			LUTPLAYER.emplace("KillNum", 2);
			LUTPLAYER.emplace("TowerNum", 3);
			LUTPLAYER.emplace("SurvivalRound", 4);
			LUTPLAYER.emplace("SoldierNum", 5);
			LUTPLAYER.emplace("Resource", 6);
			LUTPLAYER.emplace("MaxPopulation", 7);
			LUTPLAYER.emplace("Population", 8);
		}
	}

	TPlayer::~TPlayer()
	{
	}

	void TPlayer::Generate(const std::string &strLine)
	{
		string strHolder;
		stringstream ssFormat(strLine);
		while (!ssFormat.eof())
		{
			ssFormat >> strHolder;
			switch (LUTPLAYER[strHolder])
			{
			default:
				throw exception("Unexpected identifier in line.");
				break;
			case 0:
				ssFormat >> m_nRank;
				break;
			case 1:
				ssFormat >> m_nScore;
				break;
			case 2:
				ssFormat >> m_nKillNum;
				break;
			case 3:
				ssFormat >> m_nTowerNum;
				break;
			case 4:
				ssFormat >> m_nSurvivalRound;
				break;
			case 5:
				ssFormat >> m_nSoldierNum;
				break;
			case 6:
				ssFormat >> m_nResource;
				break;
			case 7:
				ssFormat >> m_nMaxPopulation;
				break;
			case 8:
				ssFormat >> m_nPopulation;
				break;
			}
		}
		m_bGenerated = true;
	}

	void TPlayer::Clear()
	{
		
	}

	void TPlayer::UIUpdate()
	{
	}


	TSoldier::SoldierType SoldierTypeStr2Enum(std::string str)
	{
		if (str == "LightInfantry")
			return TSoldier::LightInfantry;
		else if (str == "LightArcher")
			return TSoldier::LightArcher;
		else if (str == "LightKnight")
			return TSoldier::LightKnight;
		else if (str == "Mangonel")
			return TSoldier::Mangonel;
		else if (str == "HeavyInfantry")
			return TSoldier::HeavyInfantry;
		else if (str == "HeavyArcher")
			return TSoldier::HeavyArcher;
		else if (str == "HeavyKnight")
			return TSoldier::HeavyKnight;
		else
			return TSoldier::NoneSoldierType;
	}
	MoveDirection moveDirStr2Enum(std::string str)
	{
		if (str == "UP")
			return MoveDirection::UP;
		else if (str == "DOWN")
			return MoveDirection::DOWN;
		else if (str == "LEFT")
			return MoveDirection::LEFT;
		else //if (str == "RIGHT")
			return MoveDirection::RIGHT;
	}
}


