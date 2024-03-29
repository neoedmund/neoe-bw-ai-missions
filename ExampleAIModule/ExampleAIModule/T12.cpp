/*
code for :
Episode I Terran: 2 Blackwater station
Objectives
- Eradicate the alien infestation.
- Raynor must servive

@auther neoe
@bwapi r1646
*/
#include <BWAPI.h>
#include "utils.h"
#include <string>


using namespace BWAPI;

static void step0();
static void step1();
static void step2();
static void step3();
static void step4();



static const char* data[5] = {"debug info","find another base", "train forces", "attack", "finish"};
static const stepFunc funcs[5] = {step0, step1, step2, step3, step4};
static int step = 0;


void T12::onFrame(){
	BW->drawTextScreen(5,16,"in step:%d (%s) building depot %d",step, data[step], Util1::getBuilding(UnitTypes::Terran_Supply_Depot));
	funcs[step]();
	Util1::updateStatus();
};

void T12::onUnitDestroy(BWAPI::Unit* unit){
	Util1::onUnitDestroy(unit);
};

static void step0(){
	BW->printf("units dump begin");
	for each(Unit* u in BW->getAllUnits()){
		BW->printf("[%s]%s(%d)", u->getPlayer()->getName().c_str() ,
			u->getType().getName().c_str(),
			u->getType().getID());
	}
	BW->printf("units dump end");
	Util1::initExpMap();
	step=1;
}

static void step1(){
	//[Antiga]Terran Command Center [57e07c0] has been spotted at (224,144)
	Unit* raynor=NULL;
	for each(Unit* u in MYUNITS){
		if (u->getType().getID()==229) raynor = u;
	}

	if (raynor) {
		Position ATCC = Position(224,144);
		if (raynor->getOrder()==Orders::PlayerGuard) {
			raynor->rightClick(ATCC);
			BW->printf("Raynor Go.");
		}
		double dis = raynor->getDistance(ATCC);
		//BW->printf("distance %.1f", dis);
		if (dis<100) step=2;
	}
	Util1::productDepartment();
	Util1::trainEnough(UnitTypes::Terran_Marine,100);

}

static void step2(){
	Util1::defenceDepartment();
	Util1::productDepartment();
	if(Util1::getMyUnits(UnitTypes::Terran_SCV).size()>=Util1::svcPerMineral*
		BW->getMinerals().size()){
			Util1::upgarade(UpgradeTypes::U_238_Shells);
			if (rand() % 10<=6)	Util1::trainEnough(UnitTypes::Terran_Marine,100);
			else Util1::trainEnough(UnitTypes::Terran_Firebat,100);
	}
	US marines = Util1::getMyUnits(UnitTypes::Terran_Marine);
		US firebats = Util1::getMyUnits(UnitTypes::Terran_Firebat);
		US army;
		Util1::filterOrder(marines, Orders::PlayerGuard, army);
		Util1::filterOrder(firebats, Orders::PlayerGuard, army);
	if (BW->enemy()->getUnits().size()>0){
		US::const_iterator enemyi=BW->enemy()->getUnits().begin();
		Unit* enemy = *enemyi;
		if (army.size()>0){
			Util1::attack(enemy, army);
			Position p = enemy->getPosition();
			BW->drawBox(CoordinateType::Map, p.x() , p.y() , p.x()  + 8, p.y()  + 8, Colors::Red, false);
			BW->printf("attack (%d,%d) %d", p.x(), p.y(), army.size());		
		}
	}else{
		US marines = Util1::getMyUnits(UnitTypes::Terran_Marine);
		US firebats = Util1::getMyUnits(UnitTypes::Terran_Firebat);
		US army;
		Util1::filterOrder(marines, Orders::PlayerGuard, army);
		Util1::filterOrder(firebats, Orders::PlayerGuard, army);
		if (army.size()>0){
			Util1::bordExplore(army);
		}
	}

	if (marines.size()+firebats.size()>30){
		Position boss = Position(1224,160);
		for each(Unit* u in marines) if (u->getOrder()==Orders::PlayerGuard) 	
			u->attackMove(boss);
		for each(Unit* u in firebats) if (u->getOrder()==Orders::PlayerGuard) 
			u->attackMove(boss);
	}
}
static void step3(){}
static void step4(){}
void dumpEnemy(){
	BW->printf("enemy units dump begin");
	for each(Unit* u in BW->enemy()->getUnits()){
		BW->printf("[%s]%s(%d)", u->getPlayer()->getName().c_str() ,
			u->getType().getName().c_str(),
			u->getType().getID());
	}
	BW->printf("enemy units dump end");
}