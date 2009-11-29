#pragma once
#include <BWAPI.h>

using namespace BWAPI;

class Array2D;

#define US std::set<Unit*>
#define BW Broodwar
#define MYUNITS Broodwar->self()->getUnits()


namespace Util1 {

	Unit* getCommandCenter();
	Unit* getMyUnit(UnitType type);
	US getUnits(UnitType type, US all);
	US getMyUnits(UnitType type);

	Unit* getNearestUnit(Unit* c, double maxDistance, UnitType filter, US all);
	Unit* getNearestUnit(Position p, double maxDistance, UnitType filter, US all);
	void trainEnoughSVC();
	void buildEnoughSupplyDepot();
	US getUnitsNearCenter(Unit* c, 
		double maxDistance, 
		UnitType filter,										  
		US all);
	bool visible(TilePosition p, int width, int height);
	TilePosition getBuildLocation(UnitType type);
	bool canBuildHere(TilePosition position, UnitType type);
	void makeIdelWork();
	bool buildEnough(UnitType type, size_t count);
	bool trainEnough(UnitType type, size_t count);
	void train(UnitType type);
	void build(UnitType type);
	int getMyControlledMineralCnt();
	void microAttack(US army);
	
	void defenceDepartment();
	void productDepartment();
	void repairDepartment();
	void buildGas();
	void havestGas();
	bool isGasBuilt(Unit* u);
	void exportStaticData();



#define EM(x,y) expMap->get(x,y)
	int getBuilding(UnitType type);
	void attack(Unit* enemy, US army);
	void onUnitDestroy(Unit* unit);
	void setExpMap();
	void initExpMap();
	void updateStatus();
	void bordExplore(US army);
	void upgarade(UpgradeType ut);
	void filterOrder(US const &src, Order filter, US &addto);
	US getEnemyUnits();
	US getNeutralUnits();
	void attackOnSight();
	US getMyArmy();
	void moveMedic();
	bool buildAddon(UnitType addon ,size_t maxcnt, UnitType body);
	
		// explore
	
	static Array2D* expMap;
	static std::map<TilePosition,US> exploring;
	static std::map<Unit*,US> attacking;
	static std::map<Unit*,UnitType> building;
	static std::map<Unit*,int> buildingTime;
	static std::map<Unit*,Unit*> repairing;
	static std::map<Unit*,int> repairingTime;
	static std::set<std::pair<Unit*,Unit*>> microAttacking;
	static std::set<TilePosition> builtLoc;
	static int buidSpace=4;
	//members
	static Unit* commandCenter = NULL;
	static double svcPerMineral = 1.7;
	static double svcPerGas = 2;
	static double nearMineralDis = 500;
	static std::map<Unit*, US> gasWorkers;

};
class Array2D
{
public:
	Array2D( size_t rows, size_t cols )
		: iData( new bool[rows*cols] )
		, iRows( rows )
		, iCols( cols )
	{
	}

	// Add copy and assignment etc.

	~Array2D()
	{
		delete [] iData;
	}

	bool get(size_t x, size_t y)
	{
		if (x<0||x>=iRows||y<0||y>=iCols)return false;
		return iData[y+iCols*x];
	}
	void set(size_t x, size_t y, bool v){
		if (x<0||x>=iRows||y<0||y>=iCols)return;
		iData[y+iCols*x]=v;
	}

private:
	bool *   iData;
	size_t  iRows;
	size_t  iCols;
};
std::string toString(int value);
std::string toString(bool value);
void append(FILE *log, std::string data);


class MapHandler{
public:
	MapHandler(){};
	virtual void onFrame(){};
	virtual bool onSendText(std::string text){return false;};
	virtual void onUnitCreate(BWAPI::Unit* unit){};
	virtual void onUnitDestroy(BWAPI::Unit* unit){};
	virtual void onUnitMorph(BWAPI::Unit* unit){};
	virtual void onUnitShow(BWAPI::Unit* unit){};
	virtual void onUnitHide(BWAPI::Unit* unit){};
	virtual void onEnd(bool isWinner){};
	virtual void onPlayerLeft(BWAPI::Player* player){};
	virtual void onNukeDetect(BWAPI::Position target){};
	virtual void onUnitRenegade(BWAPI::Unit* unit){};
};


typedef void (*stepFunc)();

class T00: public MapHandler{
	void onFrame();
	void onUnitDestroy(BWAPI::Unit* unit);
	bool onSendText(std::string text);
};
class T10: public MapHandler{void onFrame();};
class T11: public MapHandler{void onFrame();};
class T12: public MapHandler{void onFrame();void onUnitDestroy(BWAPI::Unit* unit);};
class T13: public MapHandler{void onFrame();};
class T14: public MapHandler{void onFrame();};
class T15: public MapHandler{void onFrame();void onUnitDestroy(BWAPI::Unit* unit);};

class SortClass1{
public:
	static Unit* center;
	static bool sortPredicate(Unit* a,  Unit* b)
	{
		return a->getDistance(center) < b->getDistance(center);
	};
};

US US_diff(US const &src, US const &b);