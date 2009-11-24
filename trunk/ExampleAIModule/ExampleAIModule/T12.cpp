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

typedef void (*stepFunc)();

static const char* data[5] = {"debug info","find another base", "train forces", "attack", "finish"};
static const stepFunc funcs[5] = {step0, step1, step2, step3, step4};
static int step = 0;

void T12(){
	Broodwar->drawTextScreen(5,16,"in step:%d (%s)",step, data[step]);
	funcs[step]();
}

static void step0(){
	Broodwar->printf("units dump begin");
	for each(Unit* u in Broodwar->getAllUnits()){
		Broodwar->printf("[%s]%s(%d)", u->getPlayer()->getName().c_str() ,
			u->getType().getName().c_str(),
			u->getType().getID());
	}
	Broodwar->printf("units dump end");
	step=1;
}

static void step1(){
	//[Antiga]Terran Command Center [57e07c0] has been spotted at (224,144)
	Unit* raynor=NULL;
	for each(Unit* u in Broodwar->self()->getUnits()){
		if (u->getType().getID()==229) raynor = u;
	}
	Position ATCC = Position(224,144);
	if (raynor) {
		if (raynor->getOrder()==Orders::PlayerGuard) {
			raynor->rightClick(ATCC);
			Broodwar->printf("Raynor Go.");
		}
		double dis = raynor->getDistance(ATCC);
		//Broodwar->printf("distance %.1f", dis);
		if (dis<100) step=2;
	}
}
static void step2(){
	Broodwar->printf("enemy units dump begin");
	for each(Unit* u in Broodwar->enemy()->getUnits()){
		Broodwar->printf("[%s]%s(%d)", u->getPlayer()->getName().c_str() ,
			u->getType().getName().c_str(),
			u->getType().getID());
	}
	Broodwar->printf("units dump end");
	step=3;
}
static void step3(){}
static void step4(){}


