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

void T14::onFrame(){
	Broodwar->drawTextScreen(5,16,"in step:%d (%s)",step, data[step]);
	funcs[step]();
	Util1::setExpMap();
};

void step0(){
	Util1::initExpMap();
	step=1;
}
static void attackOnSight();
void step1(){	
	US v= MYUNITS;
	if(v.size()>0)	Util1::bordExplore(v); 
	attackOnSight();
}
static void attackOnSight(){
	US eu = Util1::getEnemyUnits();
	US army = MYUNITS;
	US	ar2;
	Util1::filterOrder(army, Orders::PlayerGuard,ar2);
	if (ar2.size()>0){
		if (eu.size()>0){
			US::iterator enemyi=eu.begin();
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
			US nes =Util1::getNeutralUnits();
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