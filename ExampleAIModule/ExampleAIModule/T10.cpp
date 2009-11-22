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
#include "T10.h"

using namespace BWAPI;

static int mineralCnt=0;
static int step=0;
static float scvPerMin=2.1f;

void T10(){
	// T 1 1
	//1. build 3 deposit 
	//2. build a refinery 
	//3. gather 100 gas
	Broodwar->drawTextScreen(5,16,"in step:%d",step);
	BWAPI::Unit* commandCenter = getCommandCenter();
	Player* me = Broodwar->self();
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
			Broodwar->printf("scv count ok, let's go to step 1");
			//step=1;
		}

		if (me->minerals()>=100){
		UnitType type= UnitTypes::Terran_Refinery;
		BWAPI::TilePosition p=getGasLoc();
		if (p==BWAPI::TilePosition(-1,-1)){
			step=2;
		}else{
			for(std::set<Unit*>::iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
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
		BWAPI::TilePosition p=getGasLoc();
		int cnt=0;
		int maxGasWorker=3;
		for(std::set<Unit*>::iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
		{
			if ( (*i)->getType().isWorker() ){
				(*i)->rightClick(Position(p.x()*32,p.y()*32));
				cnt++;
				if (cnt>=maxGasWorker){
					break;
				}
			}
		}
		step=3;
	}else{
	}
	makeIdelWork();
}
BWAPI::TilePosition getGasLoc(){
	for(std::set<Unit*>::iterator i=Broodwar->getGeysers().begin();i!=Broodwar->getGeysers().end();i++)
	{
		BWAPI::TilePosition p=(*i)->getTilePosition();
		std::set<BWAPI::Unit*> units = BWAPI::Broodwar->unitsOnTile(p.x(), p.y());
		for(std::set<BWAPI::Unit*>::iterator j = units.begin(); j != units.end(); j++)
		{ 
			if (!((*j)->getType().isBuilding() && !(*j)->isLifted()))
				return p;
		}
		if (units.size()<=1) return p;
	}
	return BWAPI::TilePosition(-1,-1);
}
void buildDepot(){
	BWAPI::TilePosition p=getBuildDepotLoc();
	UnitType type= UnitTypes::Terran_Supply_Depot;

	if (p==BWAPI::TilePosition(-1,-1)){
		Broodwar->printf("cannot find place to build depot");
	}else{
		for(std::set<Unit*>::iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
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
bool visible(BWAPI::TilePosition p, int width, int height){
	for (int x=p.x();x<=p.x()+width;x++){
		for (int y=p.y();y<=p.y()+height;y++){
			if (!Broodwar->visible(x,y)){
				return false;
			}
		}

	}
	return true;
}

BWAPI::TilePosition getBuildDepotLoc()
{
	int space=5;
	UnitType type= UnitTypes::Terran_Supply_Depot;
	int width=type.tileWidth();
	int height=type.tileHeight();
	for(std::set<Unit*>::iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
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

bool canBuildHere(BWAPI::TilePosition position, BWAPI::UnitType type) 
{
	return Broodwar->canBuildHere(NULL, position, type);

}


void makeIdelWork(){
	for(std::set<Unit*>::iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		Unit* mineral = getNextMineral();
		if (mineral && (*i)->getType().isWorker() && (*i)->getOrder()==Orders::PlayerGuard){
			(*i)->rightClick(mineral);
		}
	}
}
static int nextMineral=0;
BWAPI::Unit* getNextMineral(){
	int total =  Broodwar->getMinerals().size();
	if (total==0)return NULL;
	nextMineral=(nextMineral++)% total; 
	std::set<Unit*>::iterator i=Broodwar->getMinerals().begin();	
	return *i;
}
static Unit* commandCenter=NULL;
BWAPI::Unit* getCommandCenter(){
	if (commandCenter) return commandCenter;
	for(std::set<Unit*>::iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if ((*i)->getType()==UnitTypes::Terran_Command_Center){
			commandCenter=*i;
			return *i;
		}
	}
	return NULL;
}
int getSCVCnt(){
	int cnt=0;
	for(std::set<Unit*>::iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if ((*i)->getType().isWorker()){
			cnt++;
		}
	}
	return cnt;
}
int getMineralCnt(){
	int cnt=0;
	for(std::set<Unit*>::iterator i=Broodwar->getAllUnits().begin();i!=Broodwar->getAllUnits().end();i++)
	{
		if ((*i)->getType()==UnitTypes::Resource_Mineral_Field){
			cnt++;
		}
	}
	if (mineralCnt!=cnt)	Broodwar->printf("find mineral %d", cnt);
	mineralCnt=cnt;
	return cnt;

}