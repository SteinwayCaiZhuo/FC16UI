#pragma once
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include "cocos2d.h"
#include <tchar.h>
#include <windows.h>

namespace UI
{
	extern std::string logFileName;
	extern int MAX_ROUND;
	extern int PLAYER_NUM;
	extern int TOWER_NUM;
	
	
	enum UIObjectType
	{
		TypeAsNone,
		TypeAsPlayer,
		TypeAsTower,
		TypeAsSoldier
	};

	enum MoveDirection
	{
		UP = 1
		, DOWN = 2
		, LEFT = 3
		, RIGHT = 4
	};
	MoveDirection moveDirStr2Enum(std::string str);

	enum SoldierType
	{
		LightInfantry = 1  //Çá²½±ø
		, LightArcher = 2   //Çá¹­±ø
		, LightKnight = 3   //ÇáÆï±ø
		, Mangonel = 4      //Í¶Ê¯»ú
		, HeavyInfantry = 5 //ÖØ²½±ø
		, HeavyArcher = 6   //ÖØ¹­±ø
		, HeavyKnight = 7   //ÖØÆï±ø
		, NoneSoldierType
	};
	SoldierType SoldierTypeStr2Enum(std::string str);

	struct SoldierMoveType
	{
		bool m_bMove;
		MoveDirection m_nMoveDirection;
		int m_nMoveDistance;
	};

	struct ProduceType
	{
		bool m_bProduce;
		SoldierType m_nSoldierType;
	};

	enum CommandType
	{
		Attack       //(SoldierID, target_x, target_y)
		, Move       //(SoldierID, moveDirection, moveDistance)
		, Upgrade    //(TowerID)
		, Produce    //(TowerID, SoldierType)
	};

	enum TLandForm
	{
		Road,
		Forest,
		River,
		Mountain,
		Dorm,
		Classroom
	};

	class UIObject
	{
	public:
		UIObjectType m_nUIType;
		bool m_bVisible;
		bool m_bGenerated;
	
	public:
		//Constructor & Destructor
		UIObject(const UIObjectType& UIType = TypeAsNone, const bool& visible = true);
		
		virtual ~UIObject();
		//	Properties
		void setVisible(const bool&);
		bool getVisible();
		void setUIType(const UIObjectType&);
		UIObjectType getUIType();
		bool IsGenerated();

		//	Interfaces
		virtual void Generate(const std::string&) = 0;
		virtual void Clear() = 0;
		virtual void UIUpdate() = 0;
		virtual void LUT_INIT() = 0;
		
	};

	class TPlayer;
	class TTower;
	class TSoldier;

	class TPlayer: public UIObject
	{
	private:
		static std::map<std::string, int>LUTPLAYER;
	public:
		int m_nID;
		std::vector<TTower*>m_vecrHalls;
		std::vector<TSoldier*>m_vecrSoldiers;
		int m_nRank;
		int m_nResource;
		int m_nScore;
		int m_nKillNum;
		int m_nTowerNum;
		int m_nSurvivalRound;
		int m_nMaxPopulation;
		int m_nPopulation;
		int m_nSoldierNum;

		TPlayer();
		virtual ~TPlayer();
		virtual void Generate(const std::string&);
		virtual void Clear();
		virtual void UIUpdate();
		virtual void LUT_INIT();
	};

	class TSoldier : public UIObject
	{
	private:
		static std::map<std::string, int>LUTSOLDIER;
	public:
		int m_nID;
		TPlayer* m_pOwner;
		int m_nBlood;
		SoldierType m_nSoldierType;
		cocos2d::Vec2 m_vec2Position;
		int m_nLevel;
		SoldierMoveType m_strctSoldierMove;
		UIObject* m_pVictim;
		bool m_bFreshman;
		bool m_bDead;
		
		//	Default Constructor
		TSoldier();
		//	Specified constructor, use this to contruct new soldier from result.txt
		TSoldier(int soldierID, int owner, std::string type, int level, int blood, int x_position, int y_position);
		~TSoldier();
		virtual void Generate(const std::string&strLine);
		virtual void Clear();
		virtual void UIUpdate();
		virtual void LUT_INIT();
	};

	

	class TTower: public UIObject
	{
	private:
		static std::map<std::string, int>LUTTOWER;
	public:
		
		ProduceType m_strctProduceSoldier;
		int m_nID;
		TPlayer* m_pOwner;
		int m_nLevel;
		int m_nBlood;
		
		bool m_bRecruiting;
		SoldierType m_nRecruitingType;
		int m_nRecruitingRound;
		bool m_bUpgrade; //
		

		cocos2d::Vec2 m_vec2Position;

		TTower();
		TTower(const int& towerID, const int& owner, const int& level, const int& blood, const int& recruiting, const int& recruitingRound, const std::string& recruitingType);
		~TTower();
		virtual void Generate(const std::string&strLine);
		virtual void Clear();
		virtual void UIUpdate();
		virtual void LUT_INIT();
		
	};

	

	class Command 
	{
	

	public:
		
		
		CommandType m_nCommandType;

		
		//Move
		UIObject* m_pMoveSoldier;
		MoveDirection m_nMoveDirection;
		int m_nMoveDistance;

		//Attack
		UIObject* m_pAttackObject;
		UIObject* m_pVictimObject;

		//Upgrade
		UIObject* m_pUpgradeTower;
		
		//Produce
		UIObject* m_pProduceTower;
		SoldierType produceSoldierType;

	
		Command();
		~Command();
	};

	void MyClear(std::stringstream& ifs);
}