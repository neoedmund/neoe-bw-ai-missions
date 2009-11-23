#pragma once
#include <BWAPI.h>

void T10();
static BWAPI::Unit* getCommandCenter();
static int getSCVCnt();
static int getMineralCnt();
static void makeIdelWork();
static BWAPI::Unit* getNextMineral();
static void buildDepot();
static bool canBuildHere(BWAPI::TilePosition position, BWAPI::UnitType type) ;
static BWAPI::TilePosition getBuildDepotLoc();
static bool visible(BWAPI::TilePosition p, int width, int height);
static BWAPI::TilePosition getGasLoc();
static BWAPI::Unit* getRefinery();