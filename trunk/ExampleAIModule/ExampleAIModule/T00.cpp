/*
this is a general purporals handler for terran. may be used in multiplayer game.
*/
#include <BWAPI.h>
#include "utils.h"


using namespace BWAPI;


static void step0();
static void step1();


static const char* data[] = {"0 init","1 do"};
static const stepFunc funcs[] = {step0, step1};
static int step = 0;

void T00::onFrame(){
	BW->drawTextScreen(5,16,"in step:%d (%s) building depot %d",step, data[step], Util1::getBuilding(UnitTypes::Terran_Supply_Depot));
	funcs[step]();
	Util1::updateStatus();	
};
void T00::onUnitDestroy(BWAPI::Unit* unit){
	Util1::onUnitDestroy(unit);
};
void step0(){
	Util1::initExpMap();
	step=1;
}
void step1(){
	Util1::defenceDepartment();
	Util1::productDepartment();
	Util1::repairDepartment();
	Util1::buildEnough(UnitTypes::Terran_Engineering_Bay,1);
	Util1::buildEnough(UnitTypes::Terran_Missile_Turret,1);
	Util1::buildEnough(UnitTypes::Terran_Academy,1);
	Util1::buildEnough(UnitTypes::Terran_Barracks,4);			
	Util1::buildEnough(UnitTypes::Terran_Factory,2);
	Util1::upgarade(UpgradeTypes::U_238_Shells);
	Util1::upgarade(UpgradeTypes::Terran_Infantry_Weapons);
	Util1::upgarade(UpgradeTypes::Terran_Infantry_Armor);

	int rd =rand() % 60;
	if (rd<10)	Util1::trainEnough(UnitTypes::Terran_Marine,100);
	else if (rd<20)	Util1::trainEnough(UnitTypes::Terran_Firebat,100);
	else if (rd<30)	Util1::trainEnough(UnitTypes::Terran_Wraith,100);
	else if (rd<40)	Util1::trainEnough(UnitTypes::Terran_Vulture,100);
	else if (rd<50)	Util1::trainEnough(UnitTypes::Terran_Medic,30);
	else Util1::trainEnough(UnitTypes::Terran_Dropship,3);

	Util1::attackOnSight();
	US idle;
	Util1::filterOrder(Util1::getMyArmy(), Orders::PlayerGuard, idle);
	Util1::bordExplore(idle);
	Util1::microAttack(Util1::getMyArmy());
}