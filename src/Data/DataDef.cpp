#include "DataDef.h"
#include "../Logic/MainLogic.h"

namespace UI
{
	int PLAYER_NUM = 4;
	int MAX_ROUND = 300;
	int TOWER_NUM = 19;
	std::string logFileName = "Log/log.txt";

	std::map<std::string, int>TPlayer::LUTPLAYER;
	std::map<std::string, int>TTower::LUTTOWER;
	std::map<std::string, int>TSoldier::LUTSOLDIER;

	UIObject::UIObject(const UIObjectType& uiObjectType, const bool& visiable) :m_nUIType(uiObjectType), m_bVisible(visiable)
	{
		m_bGenerated = false;
	}

	UIObject::~UIObject()
	{
		m_bGenerated = false;
	}

	void UIObject::setVisible(const bool&visible)
	{
		m_bVisible = visible;
	}

	bool UIObject::getVisible()
	{
		return m_bVisible;
	}

	void UIObject::setUIType(const UIObjectType& UIType)
	{
		m_nUIType = UIType;
	}

	UIObjectType UIObject::getUIType()
	{
		return m_nUIType;
	}

	bool UIObject::IsGenerated()
	{
		return m_bGenerated;
	}

	TPlayer::TPlayer() :UIObject(TypeAsPlayer)
	{
		if (LUTPLAYER.empty())
			LUT_INIT();
	}

	void TPlayer::LUT_INIT()
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
		std::string strHolder;
		std::stringstream ssFormat(strLine);
		while (!ssFormat.eof())
		{
			ssFormat >> strHolder;
			switch (LUTPLAYER[strHolder])
			{
			default:
				throw std::exception("Unexpected identifier in line.");
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

	TTower::TTower() : UIObject(TypeAsTower)
	{

	}


	TTower::TTower(const int& towerID, const int& owner, const int& level, const int& blood, const int& recruiting, const int& recruitingRound, const std::string& recruitingType) :UIObject(UIObjectType::TypeAsTower)
	{
		if (LUTTOWER.empty())
			LUT_INIT();
		this->m_nID = towerID;
		if (owner != -1)
			this->m_pOwner = MainLogic::GetInstance()->players[owner];
		else
		{
			this->m_pOwner = nullptr;
		}
		this->m_nLevel = level;
		this->m_nBlood = blood;
		this->m_bRecruiting = (recruiting != 0);
		this->m_nRecruitingRound = recruitingRound;
		this->m_nRecruitingType = SoldierTypeStr2Enum(recruitingType);

		this->m_bUpgrade = false;
		this->m_strctProduceSoldier = ProduceType{ false, NoneSoldierType };
	}

	TTower::~TTower()
	{
		if (LUTTOWER.empty())
			LUT_INIT();
	}

	void TTower::LUT_INIT()
	{
		if (LUTTOWER.empty())
		{
			LUTTOWER.emplace("TowerID", 0);
			LUTTOWER.emplace("Owner", 1);
			LUTTOWER.emplace("Level", 2);
			LUTTOWER.emplace("Blood", 3);
			LUTTOWER.emplace("Recruiting", 4);
			LUTTOWER.emplace("RecruitingRound", 5);
			LUTTOWER.emplace("RecruitingType", 6);
		}
	}

	void TTower::Generate(const std::string& strLine)
	{

		std::string strHolder;
		std::stringstream ssFormat(strLine);
		int temp_int;
		std::string temp_str;
		while (!ssFormat.eof())
		{
			ssFormat >> strHolder;
			switch (LUTTOWER[strHolder])
			{
			case 0:
				ssFormat >> m_nID; break;
			case 1:
				ssFormat >> temp_int;
				try
				{
					if (temp_int < 0)
						m_pOwner = nullptr;
					else
						m_pOwner = MainLogic::GetInstance()->players[temp_int];
				}
				catch (const std::exception&)
				{
					m_pOwner = nullptr;
				}
				break;
			case 2:
				ssFormat >> m_nLevel;
				break;
			case 3:
				ssFormat >> m_nBlood;
				break;
			case 4:
				ssFormat >> temp_int;
				m_bRecruiting = (temp_int != 0);
				break;
			case 5:
				ssFormat >> m_nRecruitingRound;
				break;
			case 6:
				ssFormat >> temp_str;
				m_nRecruitingType = SoldierTypeStr2Enum(temp_str);
				break;
			default:
				throw std::exception("Unexpected identifier in line.");
				break;
			}
		}
	}

	void TTower::Clear()
	{

	}

	void TTower::UIUpdate()
	{
	}

	TSoldier::TSoldier() :UIObject(TypeAsSoldier)
	{
		if (LUTSOLDIER.empty())
			LUT_INIT();
	}

	TSoldier::TSoldier(int soldierID, int owner, std::string type, int level, int blood, int x_position, int y_position)
	{
		if (LUTSOLDIER.empty())
			LUT_INIT();
		this->m_nID = soldierID;
		try
		{
			if (owner < 0)
				m_pOwner = nullptr;
			else
				this->m_pOwner = MainLogic::GetInstance()->players[owner];
		}
		catch (const std::exception&)
		{
			this->m_pOwner = nullptr;
		}
		this->m_nSoldierType = SoldierTypeStr2Enum(type);
		this->m_nLevel = level;
		this->m_nBlood = blood;
		this->m_vec2Position = cocos2d::Vec2(x_position, y_position);
		this->m_strctSoldierMove = SoldierMoveType{ false, UP, 0 };
		this->m_pVictim = nullptr;

		this->m_bFreshman = true;
		this->m_bDead = (blood <= 0);
	}

	TSoldier::~TSoldier()
	{
	}

	void TSoldier::LUT_INIT()
	{
		if (LUTSOLDIER.empty())
		{
			LUTSOLDIER.emplace("SoldierID", 0);
			LUTSOLDIER.emplace("Owner", 1);
			LUTSOLDIER.emplace("Type", 2);
			LUTSOLDIER.emplace("Level", 3);
			LUTSOLDIER.emplace("Blood", 4);
			LUTSOLDIER.emplace("X_Position", 5);
			LUTSOLDIER.emplace("Y_Position", 6);
		}
	}
	void TSoldier::Generate(const std::string&strLine)
	{
		std::string strHolder;
		std::stringstream ssFormat(strLine);
		int temp_int, x_pos = -1, y_pos = -1;
		std::string temp_str;
		while (!ssFormat.eof())
		{
			ssFormat >> strHolder;
			switch (LUTSOLDIER[strHolder])
			{
			case 0:
				ssFormat >> m_nID;
				break;
			case 1:
				ssFormat >> temp_int;
				try
				{
					if (temp_int < 0)
						m_pOwner = nullptr;
					else
						m_pOwner = MainLogic::GetInstance()->players[temp_int];
				}
				catch (const std::exception&)
				{
					m_pOwner = nullptr;//This should not happen
				}
				break;
			case 2:
				ssFormat >> temp_str;
				m_nSoldierType = SoldierTypeStr2Enum(temp_str);
				break;
			case 3:
				ssFormat >> m_nLevel;
				break;
			case 4:
				ssFormat >> m_nBlood;
				break;
			case 5:
				ssFormat >> x_pos;
				break;
			case 6:
				ssFormat >> y_pos;
				break;

			default:
				throw std::exception("Unexpected identifier in line.");
				break;
			}
		}

		m_vec2Position = cocos2d::Vec2(x_pos, y_pos);
		if (m_nBlood <= 0)
			m_bDead = true;
	}

	void TSoldier::Clear()
	{
	}

	void TSoldier::UIUpdate()
	{
	}

  int TSoldier::Info2GID() 
  {
    return SOLDIER_SET_START + ((m_pOwner->m_nID + 1) * SOLDIER_SET_COL + m_nSoldierType);
  }

	Command::Command()
	{}

	Command::~Command()
	{}






	SoldierType SoldierTypeStr2Enum(std::string str)
	{
		if (str == "LightInfantry")
			return LightInfantry;
		else if (str == "LightArcher")
			return LightArcher;
		else if (str == "LightKnight")
			return LightKnight;
		else if (str == "Mangonel")
			return Mangonel;
		else if (str == "HeavyInfantry")
			return HeavyInfantry;
		else if (str == "HeavyArcher")
			return HeavyArcher;
		else if (str == "HeavyKnight")
			return HeavyKnight;
		else
			return NoneSoldierType;
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

	void MyClear(std::stringstream & ifs)
	{
		ifs.clear();
		int last = 0;
		while (ifs.tellg())
		{
			ifs.seekg(last + 1);
			if (int(ifs.tellg()) == -1)
				break;
			last++;
		}
		ifs.clear();
		ifs.seekg(last);
		
	}
}