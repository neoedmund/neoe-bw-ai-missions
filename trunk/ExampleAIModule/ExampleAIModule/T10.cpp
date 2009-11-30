/*
code for :
Episode I
Terran:
Tutorial: Boot Camp
Objectives
- build 3 supply depots
- build a refinery
- gather 100 vespene gas

@auther neoe
@bwapi beta 2.2
*/
#include <BWAPI.h>
#include "utils.h"

using namespace BWAPI;
static BWAPI::Unit* getCommandCenter();
static int getSCVCnt();
static int getMineralCnt();
static void makeIdelWork();
static BWAPI::Unit* getNextMineral();
static void buildDepot();
static bool canBuildHere(BWAPI::TilePosition position, BWAPI::UnitType type) ;
static BWAPI::TilePosition getBuildDepotLoc();
static bool visible(BWAPI::TilePosition p, int width, int height);
static BWAPI::TilePosition getGasLoc();
static BWAPI::Unit* getRefinery();

static int mineralCnt=0;
static int step=0;
static float scvPerMin=2.1f;
static int gasWorkerCnt=0;
static int maxGasWorker=3;

void T10::onFrame(){
	// T 1 1
	//1. build 3 deposit 
	//2. build a refinery 
	//3. gather 100 gas
	BW->drawTextScreen(5,16,"in step:%d",step);
	BWAPI::Unit* commandCenter = getCommandCenter();
	Player* me = BW->self();
	if (step==0){// build 2.5*mineral piece of SCV, build deposit if necessary
		if (me->supplyUsed()+4 > me->supplyTotal() && me->supplyTotal()<200){
			buildDepot();
		}
		if (getSCVCnt()<scvPerMin*getMineralCnt()
			&& me->minerals()>=50 
			&& me->supplyUsed()<me->supplyTotal()
			&& commandCenter->getTrainingQueue().size()==0){
				commandCenter->train(UnitTypes::Terran_SCV);
		}
		if (getSCVCnt()>=scvPerMin*getMineralCnt()){
			BW->printf("scv count ok, let's go to step 1");
			//step=1;
		}

		if (me->minerals()>=100){
			UnitType type= UnitTypes::Terran_Refinery;
			BWAPI::TilePosition p=getGasLoc();
			if (p==BWAPI::TilePosition(-1,-1)){
				gasWorkerCnt=1;
				step=2;
			}else{
				for(US::const_iterator i=MYUNITS.begin();i!=MYUNITS.end();i++)
				{
					if ( (*i)->getType().isWorker() ){
						if (!visible(p,type.tileWidth(), type.tileHeight())) (*i)->rightClick(Position(p.x()*32,p.y()*32));
						else (*i)->build(Position(p.x()*32,p.y()*32),type);
						break;
					}
				}
			}
		}
	}else if (step==1){//build refinery


	}else if (step==2){
		BWAPI::Unit* refinery=getRefinery();
		if (refinery!=NULL){

			for(US::const_iterator i=MYUNITS.begin();i!=MYUNITS.end();i++)
			{
				if ( (*i)->getType().isWorker() ){

					BW->printf("assign more gas worker %d",gasWorkerCnt);
					(*i)->rightClick(refinery);
					gasWorkerCnt++;
					if (gasWorkerCnt>=maxGasWorker){
						step=3;
						break;
					}
				}
			}
		}

	}else{
	}
	makeIdelWork();

};
static BWAPI::TilePosition getGasLoc(){
	for(US::const_iterator i=BW->getGeysers().begin();i!=BW->getGeysers().end();i++)
	{
		BWAPI::TilePosition p=(*i)->getTilePosition();
		std::set<BWAPI::Unit*> units = BWAPI::BW->unitsOnTile(p.x(), p.y());
		for(std::set<BWAPI::Unit*>::iterator j = units.begin(); j != units.end(); j++)
		{ 
			if (!((*j)->getType().isBuilding() && !(*j)->isLifted()))
				return p;
		}
		if (units.size()<=1) return p;
	}
	return BWAPI::TilePosition(-1,-1);
}
static BWAPI::Unit* getRefinery(){
	for(US::const_iterator i=MYUNITS.begin();i!=MYUNITS.end();i++)
	{
		if ((*i)->getType()==UnitTypes::Terran_Refinery&&(*i)->isCompleted())
			return (*i);
	}
	return NULL;
}
static void buildDepot(){
	BWAPI::TilePosition p=getBuildDepotLoc();
	UnitType type= UnitTypes::Terran_Supply_Depot;

	if (p==BWAPI::TilePosition(-1,-1)){
		BW->printf("cannot find place to build depot");
	}else{
		for(US::const_iterator i=MYUNITS.begin();i!=MYUNITS.end();i++)
		{
			if ( (*i)->getType().isWorker() ){
				//BigAI::setCommand(*i, "build", p,UnitTypes::Terran_Supply_Depot); 
				if (!visible(p,type.tileWidth(), type.tileHeight())) (*i)->rightClick(Position(p.x()*32,p.y()*32));
				else (*i)->build(Position(p.x()*32,p.y()*32),type);
				break;
			}
		}
	}
}
static bool visible(BWAPI::TilePosition p, int width, int height){
	for (int x=p.x();x<=p.x()+width;x++){
		for (int y=p.y();y<=p.y()+height;y++){
			if (!BW->isVisible(x,y)){
				return false;
			}
		}

	}
	return true;
}

static BWAPI::TilePosition getBuildDepotLoc()
{
	int space=5;
	UnitType type= UnitTypes::Terran_Supply_Depot;
	int width=type.tileWidth();
	int height=type.tileHeight();
	for(US::const_iterator i=MYUNITS.begin();i!=MYUNITS.end();i++)
	{

		if ((*i)->getType().isBuilding()){
			for (int x=(*i)->getTilePosition().x()-width-space;x<=(*i)->getTilePosition().x()+space;x++){
				for (int y=(*i)->getTilePosition().y()-height-space;y<=(*i)->getTilePosition().y()+space;y++){
					if (canBuildHere(BWAPI::TilePosition(x,y),type)){
						return BWAPI::TilePosition(x,y);
					}
				}

			}
		}
	}
	return BWAPI::TilePosition(-1,-1);
}

static bool canBuildHere(BWAPI::TilePosition position, BWAPI::UnitType type) 
{
	return BW->canBuildHere(NULL, position, type);

}


static void makeIdelWork(){
	for(US::const_iterator i=MYUNITS.begin();i!=MYUNITS.end();i++)
	{
		Unit* mineral = getNextMineral();
		if (mineral && (*i)->getType().isWorker() && (*i)->getOrder()==Orders::PlayerGuard){
			(*i)->rightClick(mineral);
		}
	}
}
static int nextMineral=0;
BWAPI::Unit* getNextMineral(){
	int total =  BW->getMinerals().size();
	if (total==0)return NULL;
	nextMineral=(nextMineral++)% total; 
	US::const_iterator i=BW->getMinerals().begin();	
	return *i;
}
static Unit* commandCenter=NULL;
BWAPI::Unit* getCommandCenter(){
	if (commandCenter) return commandCenter;
	for(US::const_iterator i=MYUNITS.begin();i!=MYUNITS.end();i++)
	{
		if ((*i)->getType()==UnitTypes::Terran_Command_Center){
			commandCenter=*i;
			return *i;
		}
	}
	return NULL;
}
static int getSCVCnt(){
	int cnt=0;
	for(US::const_iterator i=MYUNITS.begin();i!=MYUNITS.end();i++)
	{
		if ((*i)->getType().isWorker()){
			cnt++;
		}
	}
	return cnt;
}
static int getMineralCnt(){
	int cnt=0;
	for(US::const_iterator i=BW->getAllUnits().begin();i!=BW->getAllUnits().end();i++)
	{
		if ((*i)->getType()==UnitTypes::Resource_Mineral_Field){
			cnt++;
		}
	}
	if (mineralCnt!=cnt)	BW->printf("find mineral %d", cnt);
	mineralCnt=cnt;
	return cnt;

}