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
#define EM(x,y) (*expMap)[x][y]
	void setExpMap();
	void initExpMap();
	void bordExplore(std::set<Unit*> army);

	void upgarade(UpgradeType ut);
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

	bool * operator[]( size_t row )
	{
		return iData+iCols*row;
	}

private:
	bool *   iData;
	size_t  iRows;
	size_t  iCols;
};
std::string toString(int value);
std::string toString(bool value);
void append(FILE *log, std::string data);
typedef void (*stepFunc)();