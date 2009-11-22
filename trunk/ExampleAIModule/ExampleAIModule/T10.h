#pragma once
#include <BWAPI.h>

void T10();
BWAPI::Unit* getCommandCenter();
int getSCVCnt();
int getMineralCnt();
void makeIdelWork();
BWAPI::Unit* getNextMineral();
void buildDepot();
bool canBuildHere(BWAPI::TilePosition position, BWAPI::UnitType type) ;
BWAPI::TilePosition getBuildDepotLoc();
bool visible(BWAPI::TilePosition p, int width, int height);
BWAPI::TilePosition getGasLoc();