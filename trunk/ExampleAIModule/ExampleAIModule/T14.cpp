/*
code for :
Episode I Terran: 4  The Jacobs Installation

Objectives
- retrieve data discs from Confederate network.
- Raynor must survive

info:
A [Epsilon Squadron]Unknown(229) [4f50088] spotted at (976,192) 


@auther neoe
@bwapi r1656
*/
#include <BWAPI.h>
#include "utils.h"


using namespace BWAPI;

static void step0();
static void step1();

static const char* data[2] = {"init","go"};
static const stepFunc funcs[2] = {step0, step1};
static int step = 0;
void dumpSelect();
void T14::onFrame(){
	Broodwar->drawTextScreen(5,16,"in step:%d (%s)",step, data[step]);
	funcs[step]();
	Util1::setExpMap();
	dumpSelect();
};
Unit* lastSelect=NULL;
void dumpSelect(){
	for each(Unit* u in Broodwar->getSelectedUnits()){
		if (u==lastSelect){break;}
		Broodwar->printf("select (%d,%d) %d,%s,%s",u->getPosition().x(),
			u->getPosition().y(),
			u->getType().getID(), u->getPlayer()->getName().c_str(),
			u->getType().canMove()?"M":"S");
		lastSelect=u;
		break;
	}
}
void step0(){
	Util1::initExpMap();
	step=1;
}
static void attackOnSight();
void step1(){	
	std::set<Unit*> v= Broodwar->self()->getUnits();
	if(v.size()>0)	Util1::bordExplore(v); 
	attackOnSight();
}
void attackOnSight(){
	std::set<Unit*> eu = Util1::getEnemyUnits();
	std::set<Unit*> army = Broodwar->self()->getUnits();
	std::set<Unit*>	ar2;
	Util1::filterOrder(army, Orders::PlayerGuard,ar2);
	if (ar2.size()>0){
		if (eu.size()>0){
			std::set<Unit*>::iterator enemyi=eu.begin();
			Unit* enemy = *enemyi;
			Position p = enemy->getPosition();
			for each(Unit* u in ar2){
				u->attackMove(p);
			}
			if(ar2.size()>0){
				int w=enemy->getType().tileWidth()*32;
				Broodwar->drawBox(CoordinateType::Map, p.x(),p.y() , p.x()  + w, p.y() + w, Colors::Red, false);
				Broodwar->printf("attack %d (%d,%d) %d", enemy->getInitialHitPoints() ,p.x(), p.y(), ar2.size());
			}
		}else{
			std::set<Unit*> nes =Util1::getNeutralUnits();
			for each(Unit* u in army){
				for each(Unit* ne in nes){
					if (u->getDistance(ne)<u->getType().groundWeapon()->maxRange())
					{
						u->attackUnit(ne);
						break;
					}
				}
			}
		}
	}
}