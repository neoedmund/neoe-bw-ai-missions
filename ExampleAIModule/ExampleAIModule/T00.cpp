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

static bool doMine=true;
static bool doBuild=true;
static bool doTrain=true;
static bool doAttack=false;
static bool doExplore=false;

void T00::onFrame(){
	BW->drawTextScreen(5,16,"%s %s %s %s %s",
		doMine?"mine":"",
		doBuild?"build":"",
		doTrain?"train":"",
		doAttack?"attack":"",
		doExplore?"explore":"");
	funcs[step]();
	Util1::updateStatus();	
};
void T00::onUnitDestroy(BWAPI::Unit* unit){
	Util1::onUnitDestroy(unit);
};
bool T00::onSendText(std::string text){
	if (text=="mine"){doMine=!doMine;}
	else if (text=="build"){doBuild=!doBuild;}
	else if (text=="train"){doTrain=!doTrain;}
	else if (text=="att"){doAttack=!doAttack;}
	else if (text=="exp"){doExplore=!doExplore;}
	else return false;
	return true;
}
void step0(){
	Util1::initExpMap();
	step=1;
}
void step1(){

	if (doMine)Util1::productDepartment();
	if (doBuild){
		Util1::buildEnough(UnitTypes::Terran_Engineering_Bay,1);
		Util1::buildEnough(UnitTypes::Terran_Missile_Turret,1);
		Util1::buildEnough(UnitTypes::Terran_Academy,1);
		Util1::buildEnough(UnitTypes::Terran_Barracks,4);			
		Util1::buildEnough(UnitTypes::Terran_Factory,2);
		Util1::upgarade(UpgradeTypes::U_238_Shells);
		Util1::upgarade(UpgradeTypes::Terran_Infantry_Weapons);
		Util1::upgarade(UpgradeTypes::Terran_Infantry_Armor);
	}
	if (doTrain){
		int rd =rand() % 60;
		if (rd<10)	Util1::trainEnough(UnitTypes::Terran_Marine,100);
		else if (rd<20)	Util1::trainEnough(UnitTypes::Terran_Firebat,100);
		else if (rd<30)	Util1::trainEnough(UnitTypes::Terran_Wraith,100);
		else if (rd<40)	Util1::trainEnough(UnitTypes::Terran_Vulture,100);
		else if (rd<50)	Util1::trainEnough(UnitTypes::Terran_Medic,30);
		else Util1::trainEnough(UnitTypes::Terran_Dropship,2);
	}
	if (doExplore){
		US idle;
		Util1::filterOrder(Util1::getMyArmy(), Orders::PlayerGuard, idle);
		Util1::bordExplore(idle);
	}
	if (doAttack){
		Util1::attackOnSight();
		Util1::microAttack(Util1::getMyArmy());
		Util1::moveMedic();
	}
}
