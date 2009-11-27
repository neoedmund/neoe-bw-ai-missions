/*
code for :
Episode I Terran: 1 Wasteland
Objectives
- find Raynor
- build a barracks
- train 10 marines

@auther neoe
@bwapi beta 2.2
*/
#include <BWAPI.h>
#include "utils.h"
#include <string>

using namespace BWAPI;

static void step0();
static void step1();
static void step2();
static void step3();
static void makeIdelWork();
static void moveMarine();
static BWAPI::Unit* getNextMineral();


static const char* data[4] = {"goto base","build barrack", "train marines", "finish"};
static int step = 0;
void T11::onFrame(){
	Broodwar->drawTextScreen(5,16,"in step:%d (%s)",step, data[step]);
	if (step==0){
		step0();
	}else if (step==1){
		step1();
	}else if (step==2){
		step2();
	}else if (step==3){
		step3();
	}else{}
};

static void step0(){
	Util1::makeIdelWork();
	moveMarine();
	Util1::buildEnoughSupplyDepot();
	Util1::trainEnoughSVC();	
	size_t mineralCnt = Util1::getUnitsNearCenter(Util1::getCommandCenter(),Util1::nearMineralDis,
		UnitTypes::Resource_Mineral_Field, Broodwar->getAllUnits()).size();
	size_t svcCnt = Util1::getMyUnits(UnitTypes::Terran_SCV).size();
	if (mineralCnt*Util1::svcPerMineral<=svcCnt)	{
		step=1;
	}
}
static void step1(){
	if (Util1::buildEnough(UnitTypes::Terran_Barracks, 2)){
		step=2;
	}
	Util1::makeIdelWork();
}
static void step2(){
	if (Util1::trainEnough(UnitTypes::Terran_Marine, 12)){
		step=3;
	}
	Util1::makeIdelWork();
}
static void step3(){}


static void moveMarine(){
	Unit* c = Util1::getCommandCenter();
		
	for(std::set<Unit*>::iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if (c && (*i)->getType()!=UnitTypes::Terran_SCV && (*i)->getOrder()==Orders::PlayerGuard){
			(*i)->attackMove(c->getPosition());
		}
	}

}
