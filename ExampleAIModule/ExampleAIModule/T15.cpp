/*
code for :
Episode I Terran: 5  Revolution

Objectives
- bring kerrigan to the antigan command center.
- defend the antigan rebels
- raynor and kerrigan must servive.
- destroy conferederate base(added)


@auther neoe
@bwapi r1661
*/
#include <BWAPI.h>
#include "utils.h"


using namespace BWAPI;


static void step0();
static void step1();
static void step2();
static void step3();
static void step4();
static void step5();


static const char* data[6] = {"0 init",
"1 meet kerrigan at (149,1233)",
"2 destroy Terran Missile Turret(2112,800)",
"3 kerrigan  cloak and goto command center (2720,464), then kerrigan and raynor go to safe base(2797,534)",
"4 train SVC",
"5 denfence and destroy conferederate base"};
static Position turrent1p = Position(1900,750);
static const stepFunc funcs[6] = {step0, step1, step2, step3, step4, step5};
static int step = 0;

void T15::onFrame(){
	BW->drawTextScreen(5,16,"in step:%d (%s) building depot %d",step, data[step], Util1::getBuilding(UnitTypes::Terran_Supply_Depot));
	funcs[step]();
	Util1::updateStatus();
	
};
void T15::onUnitDestroy(BWAPI::Unit* unit){
	Util1::onUnitDestroy(unit);
};
static Unit* kerrigan=NULL;
static Unit* getKerrigan(){
	if (kerrigan)return kerrigan;
	for each(Unit* u in Util1::getMyUnits(UnitTypes::Unknown)){
		if (u->getEnergy()>10){
			kerrigan=u;
			BW->printf("kerr %s", u->getType().isCloakable()?"C":"NG");
			return kerrigan;
		}
	}
	BW->printf("oops, cannot find Kerrigan");
	return NULL;
}
static US step2moving;

void step0(){
	Util1::initExpMap();
	step=1;
}
void step1(){
	US army;
	Util1::filterOrder(MYUNITS, Orders::PlayerGuard, army);
	if (army.size()>0){
		for each(Unit* u in army) u->attackMove(Position(149,1233));
	}
	US heroes = Util1::getMyUnits(UnitTypes::Unknown);
	if (heroes.size()==2){		
		step=2;
	}
}
static void step2(){
	Unit* k = getKerrigan();
	if (k->getDistance(Position(2112,800))<200){
		US turrets=Util1::getUnits(UnitTypes::Terran_Missile_Turret, Util1::getEnemyUnits());
		if (turrets.size()==0){
			for each(Unit* u in MYUNITS){
				u->rightClick(Position(1800,800));
			}
			k->cloak();
			k->rightClick(Position(2720,464));
			step=3;
		}
	}else{
		US en = Util1::getEnemyUnits();
		if (en.size()>0 ){
			Util1::filterOrder(MYUNITS, 
				Orders::PlayerGuard, step2moving);//with idle ones
			if (step2moving.size()>0){
				BW->printf("attack %d", step2moving.size());
				Unit* e= * en.begin();
				Position p=(e)->getPosition();
				for each(Unit* u in step2moving)
					u->attackMove(p);
				if((e->getType().isMechanical()|| e->getType().isRobotic())
					&&!e->getType().isBuilding()&&!e->isLockedDown()){
					k->useTech(TechTypes::Lockdown,e);
				}
				step2moving.clear();
			}
		}else{
			US army;
			Util1::filterOrder(MYUNITS, Orders::PlayerGuard, army);
			if (army.size()>0){
			for each(Unit* u in army) u->attackMove(turrent1p);
			BW->printf("moving %d", army.size());
			step2moving.insert(army.begin(), army.end());
			}
		}
	}
}
static void attackOnSight();
static void step3(){
	Unit* cc = Util1::getMyUnit(UnitTypes::Terran_Command_Center);
	if (cc){
		US heroes = Util1::getMyUnits(UnitTypes::Unknown);
		for each(Unit* u in heroes) u->attackMove(Position(2797,534));
		getKerrigan()->decloak();
		step=4;
	}
}
static void step4(){
	Util1::defenceDepartment();
	Util1::productDepartment();
	if(BW->self()->minerals()>400 || Util1::getMyUnits(UnitTypes::Terran_SCV).size()>=Util1::svcPerMineral*
		Util1::getMyControlledMineralCnt()){
			step=5;
	}
	attackOnSight();
}
static void step5(){
	Util1::defenceDepartment();
	Util1::productDepartment();
	Util1::repairDepartment();

	Util1::buildEnough(UnitTypes::Terran_Engineering_Bay,1);
	Util1::buildEnough(UnitTypes::Terran_Academy,1);
	Util1::buildEnough(UnitTypes::Terran_Barracks,4);			
	Util1::buildEnough(UnitTypes::Terran_Factory,2);
	Util1::upgarade(UpgradeTypes::U_238_Shells);
	Util1::upgarade(UpgradeTypes::Terran_Infantry_Weapons);
	Util1::upgarade(UpgradeTypes::Terran_Infantry_Armor);

	int rd =rand() % 100;
	if (rd<20)	Util1::trainEnough(UnitTypes::Terran_Marine,50);
	else if (rd<40)	Util1::trainEnough(UnitTypes::Terran_Firebat,50);
	else if (rd<60)	Util1::trainEnough(UnitTypes::Terran_Wraith,100);
	else Util1::trainEnough(UnitTypes::Terran_Dropship,3);

	attackOnSight();
}
US getAirAttacker(){
	US a;
	Util1::filterOrder(Util1::getMyUnits(UnitTypes::Terran_Marine), Orders::PlayerGuard, a);
	Util1::filterOrder(Util1::getMyUnits(UnitTypes::Terran_Wraith), Orders::PlayerGuard, a);
	return a;
}
US getGroundAttacker(){
	US a;
	Util1::filterOrder(Util1::getMyUnits(UnitTypes::Terran_Firebat), Orders::PlayerGuard, a);
	Util1::filterOrder(Util1::getMyUnits(UnitTypes::Terran_Vulture), Orders::PlayerGuard, a);
	return a;
}
US getMyArmy(){
	US a;
	US air=getAirAttacker();
	US gd=getGroundAttacker();
	a.insert(air.begin(), air.end());
	a.insert(gd.begin(), gd.end());
	return a;
}
static void attackOnSight(){
	US en = Util1::getEnemyUnits();
	
	if (en.size()>0){
		Unit* e1 = *en.begin();
		US army;
		if (e1->getType().isFlyer()){
			army=getAirAttacker();
		}else{
			army=getMyArmy();
		}
		if (army.size()>0){
		for each(Unit* u in army) u->attackMove(e1->getPosition());
		Util1::attack(e1, army);
		}
	}
}