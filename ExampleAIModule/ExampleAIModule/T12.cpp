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
static void setexpMap();
static Array2D* expMap;

#define EM(x,y) (*expMap)[x][y]

void T12(){
	Broodwar->drawTextScreen(5,16,"in step:%d (%s)",step, data[step]);
	funcs[step]();
	setexpMap();
}
static void setexpMap(){
	for(int x=0; x<Broodwar->mapWidth();x++){
		for(int y=0; y<Broodwar->mapHeight();y++){
			if(!(*expMap)[x][y] && Broodwar->visible(x,y)) (*expMap)[x][y]=true;
		}
	}
}
static void step0(){
	Broodwar->printf("units dump begin");
	for each(Unit* u in Broodwar->getAllUnits()){
		Broodwar->printf("[%s]%s(%d)", u->getPlayer()->getName().c_str() ,
			u->getType().getName().c_str(),
			u->getType().getID());
	}
	Broodwar->printf("units dump end");
	expMap = new Array2D(Broodwar->mapWidth(), Broodwar->mapHeight());
	for(int x=0; x<Broodwar->mapWidth();x++){
		for(int y=0; y<Broodwar->mapHeight();y++){
			(*expMap)[x][y]=false;
		}
	}
	step=1;
}

static void step1(){
	//[Antiga]Terran Command Center [57e07c0] has been spotted at (224,144)
	Unit* raynor=NULL;
	for each(Unit* u in Broodwar->self()->getUnits()){
		if (u->getType().getID()==229) raynor = u;
	}

	if (raynor) {
		Position ATCC = Position(224,144);
		if (raynor->getOrder()==Orders::PlayerGuard) {
			raynor->rightClick(ATCC);
			Broodwar->printf("Raynor Go.");
		}
		double dis = raynor->getDistance(ATCC);
		//Broodwar->printf("distance %.1f", dis);
		if (dis<100) step=2;
	}
	Util1::productDepartment();
	Util1::trainEnough(UnitTypes::Terran_Marine,100);

}
static Position getUnexploredPos(){
	for (int x=0;x<Broodwar->mapWidth();x++)
		for (int y=0;y<Broodwar->mapHeight();y++)
			if (!EM(x,y)&&
				(EM(x-1,y)
				||EM(x+1,y)
				||EM(x,y-1)
				||EM(x,y+1)
				||EM(x-1,y-1)
				||EM(x+1,y+1)
				||EM(x+1,y-1)
				||EM(x-1,y+1)
				)){
					EM(x,y)=true;
					return Position(TilePosition(x,y));
			}
			return Position(-1,-1);
}
static void bordExplore(){
	std::set<Unit*> marines = Util1::getMyUnits(UnitTypes::Terran_Marine);
	std::set<Unit*> firebats = Util1::getMyUnits(UnitTypes::Terran_Firebat);


	Position p = getUnexploredPos();
	if (p==Position(-1,-1))return;
	int go = 0;

	for each(Unit* u in marines) 
		if (u->getOrder()==Orders::PlayerGuard) 
		{
			u->attackMove(p);go++;
			Broodwar->drawLine(CoordinateType::Map, 
				u->getPosition().x(),u->getPosition().y(),
				p.x(), p.y(), Colors::Green);
		}
		for each(Unit* u in firebats)
			if (u->getOrder()==Orders::PlayerGuard)	{
				u->attackMove(p);go++;
				Broodwar->drawLine(CoordinateType::Map, 
					u->getPosition().x(),u->getPosition().y(),
					p.x(), p.y(), Colors::Green);
			}
			if (go>0){
				int w =1;
				Broodwar->drawBox(CoordinateType::Map, p.x() * w, p.y() * w, p.x() * w + 8, p.y() * w + 8, Colors::Orange, false);
				Broodwar->printf("explore (%d,%d) %d", p.x(), p.y(), go);
			}
}
static void step2(){
	Util1::defenceDepartment();
	Util1::productDepartment();
	if(Util1::getMyUnits(UnitTypes::Terran_SCV).size()>=Util1::svcPerMineral*
		Broodwar->getMinerals().size()){
			if (rand() % 10<=6)	Util1::trainEnough(UnitTypes::Terran_Marine,100);
			else Util1::trainEnough(UnitTypes::Terran_Firebat,100);
	}
	std::set<Unit*> marines = Util1::getMyUnits(UnitTypes::Terran_Marine);
	std::set<Unit*> firebats = Util1::getMyUnits(UnitTypes::Terran_Firebat);
	if (Broodwar->enemy()->getUnits().size()>0){
		std::set<Unit*>::iterator enemyi=Broodwar->enemy()->getUnits().begin();
		Unit* enemy = *enemyi;
		int go = 0;
		for each(Unit* u in marines) 
			if (u->getOrder()==Orders::PlayerGuard) 
			{u->attackMove(enemy->getPosition());
		go++;}
		for each(Unit* u in firebats)
			if (u->getOrder()==Orders::PlayerGuard) 
			{u->attackMove(enemy->getPosition());
		go++;}
		if (go>0){
			Position p = enemy->getPosition();
			int w = 1;
			Broodwar->drawBox(CoordinateType::Map, p.x() * w, p.y() * 8, p.x() * w + w, p.y() * w + 8, Colors::Red, false);
			Broodwar->printf("attack (%d,%d) %d", p.x(), p.y(), go);
		}
	}else{
		int go = 0;
		for each(Unit* u in marines) if (u->getOrder()==Orders::PlayerGuard) 
		{
			go++;
		}
		for each(Unit* u in firebats) if (u->getOrder()==Orders::PlayerGuard) 
		{
			go++;
		}
		if (go>0){
			bordExplore();
		}
	}

	if (marines.size()+firebats.size()>30){
		Position boss = Position(1224,160);
		for each(Unit* u in marines) 	 if (u->getOrder()==Orders::PlayerGuard) 	
			u->attackMove(boss);
		for each(Unit* u in firebats) if (u->getOrder()==Orders::PlayerGuard) 
			u->attackMove(boss);
	}
}
static void step3(){}
static void step4(){}
void dumpEnemy(){
	Broodwar->printf("enemy units dump begin");
	for each(Unit* u in Broodwar->enemy()->getUnits()){
		Broodwar->printf("[%s]%s(%d)", u->getPlayer()->getName().c_str() ,
			u->getType().getName().c_str(),
			u->getType().getID());
	}
	Broodwar->printf("enemy units dump end");
}