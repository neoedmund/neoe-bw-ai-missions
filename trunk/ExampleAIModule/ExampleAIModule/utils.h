#pragma once
#include <BWAPI.h>

using namespace BWAPI;

class Array2D;

namespace Util1 {

	Unit* getCommandCenter();
	Unit* getMyUnit(UnitType type);
	std::set<Unit*> getUnits(UnitType type, std::set<Unit*> all);
	std::set<Unit*> getMyUnits(UnitType type);

	Unit* getNearestUnit(Unit* c, double maxDistance, UnitType filter, std::set<Unit*> all);
	Unit* getNearestUnit(Position p, double maxDistance, UnitType filter, std::set<Unit*> all);
	void trainEnoughSVC();
	void buildEnoughSupplyDepot();
	std::set<Unit*> getUnitsNearCenter(Unit* c, 
		double maxDistance, 
		UnitType filter,										  
		std::set<Unit*> all);
	bool visible(TilePosition p, int width, int height);
	TilePosition getBuildLocation(UnitType type);
	bool canBuildHere(TilePosition position, UnitType type);
	void makeIdelWork();
	bool buildEnough(UnitType type, size_t count);
	bool trainEnough(UnitType type, size_t count);
	void train(UnitType type);
	void build(UnitType type);

	void defenceDepartment();
	void productDepartment();
	void buildGas();
	void havestGas();
	bool isGasBuilt(Unit* u);
	bool isHavestingGas(Unit* u);
	void exportStaticData();

	// explore
	static Array2D* expMap;
	static std::map<TilePosition,std::set<Unit*>> exploring;
	static std::map<Unit*,std::set<Unit*>> attacking;
#define EM(x,y) expMap->get(x,y)
	void attack(Unit* enemy, std::set<Unit*> army);
	void onUnitDestroy(Unit* unit);
	void setExpMap();
	void initExpMap();
	void bordExplore(std::set<Unit*> army);
	void upgarade(UpgradeType ut);
	void filterOrder(std::set<Unit*> const &src, Order filter, std::set<Unit*> &addto);
	std::set<Unit*> getEnemyUnits();
	std::set<Unit*> getNeutralUnits();
	//members
	static Unit* commandCenter = NULL;
	static double svcPerMineral = 1.7;
	static double svcPerGas = 3;
	static double nearMineralDis = 500;
	static std::map<Unit*, std::set<Unit*>> gasWorkers;

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
	virtual bool onSendText(std::string text){return true;};
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

class T10: public MapHandler{void onFrame();};
class T11: public MapHandler{void onFrame();};
class T12: public MapHandler{void onFrame();void onUnitDestroy(BWAPI::Unit* unit);};
class T13: public MapHandler{void onFrame();};
class T14: public MapHandler{void onFrame();};


	class SortClass1{public:
		static Unit* center;
		static bool sortPredicate(Unit* a,  Unit* b)
		{
			return a->getDistance(center) < b->getDistance(center);
		};
	};