/*
code for :
Episode I Terran: 3
Desperate Alliance
Objectives
- survive 30 minutes

-todo
stop block move


@auther neoe
@bwapi r1646
*/
#include <BWAPI.h>
#include "utils.h"


using namespace BWAPI;

static void step0();
static void step1();

static const char* data[2] = {"init","defence"};
static const stepFunc funcs[2] = {step0, step1};
static int step = 0;

void T13::onFrame(){
		BW->drawTextScreen(5,16,"in step:%d (%s)",step, data[step]);
		funcs[step]();
		Util1::setExpMap();
};

void step0(){
	Util1::initExpMap();
	step=1;
}
static void attackOnSight();
void step1(){
	Util1::defenceDepartment();
	Util1::productDepartment();
	if(Util1::getMyUnits(UnitTypes::Terran_SCV).size()>=Util1::svcPerMineral*
		BW->getMinerals().size()){
			int rd = rand() % 30;
			if (rd<10)	Util1::trainEnough(UnitTypes::Terran_Marine,100);
			else if (rd<20) Util1::trainEnough(UnitTypes::Terran_Firebat,100);
			else Util1::trainEnough(UnitTypes::Terran_Vulture,100);
			Util1::buildEnough(UnitTypes::Terran_Barracks,6);			
			Util1::buildEnough(UnitTypes::Terran_Factory,3);
			Util1::upgarade(UpgradeTypes::U_238_Shells);
			Util1::upgarade(UpgradeTypes::Terran_Infantry_Weapons);
			Util1::upgarade(UpgradeTypes::Terran_Infantry_Armor);
			if(rd<10)Util1::buildEnough(UnitTypes::Terran_Missile_Turret,10);
	}
	attackOnSight();	
	US v= Util1::getMyUnits(UnitTypes::Terran_Vulture);
	if(v.size()>3)	Util1::bordExplore(v); 

}
static void attackOnSight(){
	US am = Util1::getMyUnits(UnitTypes::Terran_Marine);
	US af = Util1::getMyUnits(UnitTypes::Terran_Firebat);
	US av = Util1::getMyUnits(UnitTypes::Terran_Vulture);
	US eu = BW->enemy()->getUnits();
	if (eu.size()>0){
		int md= 200*(am.size()+af.size()+av.size()-eu.size());
		US::iterator enemyi=eu.begin();
		Unit* enemy = *enemyi;
		int go = 0;
		if(enemy->getType().isFlyer())
		{for each(Unit* u in am) 
		if (u->getOrder()==Orders::PlayerGuard && u->getDistance(enemy)<md) 
		{u->attackMove(enemy->getPosition());
		go++;}
		}else{
			for each(Unit* u in am) 
				if (u->getOrder()==Orders::PlayerGuard && u->getDistance(enemy)<md) 
				{u->attackMove(enemy->getPosition());
			go++;}
			for each(Unit* u in af) 
				if (u->getOrder()==Orders::PlayerGuard && u->getDistance(enemy)<md) 
				{u->attackMove(enemy->getPosition());
			go++;}
			for each(Unit* u in av) 
				if (u->getOrder()==Orders::PlayerGuard && u->getDistance(enemy)<md) 
				{u->attackMove(enemy->getPosition());
			go++;}
		}

		if (go>0){
			Position p = enemy->getPosition();
			int w = 1;
			BW->drawBox(CoordinateType::Map, p.x() * w, p.y() * 8, p.x() * w + w, p.y() * w + 8, Colors::Red, false);
			BW->printf("attack (%d,%d) %d", p.x(), p.y(), go);
		}
	}
}