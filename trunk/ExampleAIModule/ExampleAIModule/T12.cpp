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
	Broodwar->enableFlag(Flag::CompleteMapInformation);
	Broodwar->printf("units dump begin");
	for each(Unit* u in Broodwar->getAllUnits()){
		Broodwar->printf("[%s]%s", u->getPlayer()->getName().c_str() ,
			u->getType().getName().c_str());
	}
	Broodwar->printf("units dump end");
	Broodwar->enableFlag(Flag::CompleteMapInformation);
	step=1;
}
static void step1(){

}
static void step2(){

}
static void step3(){}
static void step4(){}


