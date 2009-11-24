#pragma once
#include <BWAPI.h>

using namespace BWAPI;

class Util1 {
public:
	static Unit* getCommandCenter();
	static Unit* getMyUnit(UnitType type);
	static std::set<Unit*> getMyUnits(UnitType type);
	static Unit* getNearestMineral(Unit* center);
	static void trainEnoughSVC();
	static void buildEnoughSupplyDepot();

	static std::set<Unit*> getMineralNearCenter(Unit* center, double maxDistance);
	static bool visible(TilePosition p, int width, int height);
	static TilePosition getBuildLocation(UnitType type);
	static bool canBuildHere(TilePosition position, UnitType type);
	static void makeIdelWork();
	static bool buildEnough(UnitType type, size_t count);
	static bool trainEnough(UnitType type, size_t count);
	static void train(UnitType type);
	static void build(UnitType type);
	//members
	static Unit* commandCenter;
	static double svcPerMineral;
	static double nearMineralDis;
	
};
