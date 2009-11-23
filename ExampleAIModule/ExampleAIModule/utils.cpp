#include "utils.h"
#include <set>
using namespace BWAPI;

Unit* Util1::commandCenter = NULL;
double Util1::svcPerMineral = 2.1;
double Util1::nearMineralDis = 500;

Unit* Util1::getMyUnit(UnitType type){
	for(std::set<Unit*>::iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if ((*i)->getType()==type){
			return *i;
		}
	}
	return NULL;
}
void Util1::makeIdelWork(){
	Unit* mineral = getNearestMineral(getCommandCenter());
	for(std::set<Unit*>::iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if (mineral && (*i)->getType().isWorker() && (*i)->getOrder()==Orders::PlayerGuard){
			(*i)->rightClick(mineral);
		}
	}
}
std::set<Unit*> Util1::getMyUnits(UnitType type){
	std::set<Unit*> us;
	for each(Unit* u in Broodwar->self()->getUnits()){if (u->getType()==type){
		us.insert(u);
	}}
	return us;
}
Unit* Util1::getCommandCenter(){
	if (commandCenter==NULL) commandCenter=getMyUnit(UnitTypes::Terran_Command_Center);
	return commandCenter;
}
Unit* Util1::getNearestMineral(Unit* c){
	Unit* nearest=NULL;
	double distance = 100000;

	for each(Unit* u in Broodwar->getAllUnits()){
		double dis;
		if (u->getType()==UnitTypes::Resource_Mineral_Field && (dis=c->getDistance(u)) < distance){
			distance = dis;
			nearest=u;
		}
	}

	return nearest;
}

void Util1::trainEnoughSVC(){
	Player* me = Broodwar->self();	
	if (me->minerals()>=50 
		&& me->supplyUsed()<me->supplyTotal()
		&& getCommandCenter()->getTrainingQueue().size()==0){
			int mineralCnt = getMineralNearCenter(getCommandCenter(),nearMineralDis).size();
			int svcCnt = getMyUnits(UnitTypes::Terran_SCV).size();
			if (mineralCnt*svcPerMineral>svcCnt)	{
				getCommandCenter()->train(UnitTypes::Terran_SCV);
			}
	}
}
std::set<Unit*> Util1::getMineralNearCenter(Unit* c, double maxDistance){
	std::set<Unit*> minerals;
	for each( Unit* u in Broodwar->getAllUnits()){
		double dis;
		if (u->getType()==UnitTypes::Resource_Mineral_Field 
			&& (dis=c->getDistance(u)) < maxDistance){
				minerals.insert(u);
		}
	}
	return minerals;
}
void Util1::buildEnoughSupplyDepot(){
	Player* me = Broodwar->self();
	if (me->minerals()>=100 && me->supplyUsed()+4 > me->supplyTotal() && me->supplyTotal()<200){
		build(UnitTypes::Terran_Supply_Depot);
	}
}

TilePosition Util1::getBuildLocation(UnitType type){
	int space=5;
	int width=type.tileWidth();
	int height=type.tileHeight();
	for(std::set<Unit*>::iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{

		if ((*i)->getType().isBuilding()){
			for (int x=(*i)->getTilePosition().x()-width-space;x<=(*i)->getTilePosition().x()+space;x++){
				for (int y=(*i)->getTilePosition().y()-height-space;y<=(*i)->getTilePosition().y()+space;y++){
					if (canBuildHere(TilePosition(x,y),type)){
						return TilePosition(x,y);
					}
				}

			}
		}
	}
	return TilePosition(-1,-1);
}

bool Util1::visible(TilePosition p, int width, int height){
	for (int x=p.x();x<=p.x()+width;x++){
		for (int y=p.y();y<=p.y()+height;y++){
			if (!Broodwar->visible(x,y)){
				return false;
			}
		}

	}
	return true;
}


bool Util1::canBuildHere(TilePosition position, UnitType type) 
{
	return Broodwar->canBuildHere(NULL, position, type);

}

void Util1::build(UnitType type){
	if (Broodwar->self()->minerals()<type.mineralPrice()
		|| Broodwar->self()->gas()<type.gasPrice()) return;
	TilePosition p=getBuildLocation(type);
	if (p==TilePosition(-1,-1)){
		Broodwar->printf("cannot find place to build %s", type.getName().c_str());
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

void Util1::train(UnitType type){
	if (Broodwar->self()->minerals()<type.mineralPrice()
		|| Broodwar->self()->gas()<type.gasPrice()) return;
	const UnitType* builder = type.whatBuilds().first;	
	int v =type.whatBuilds().second;
	if (v==0) return;
	for(std::set<Unit*>::iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if ( (*i)->getType()==*builder  && !(*i)->isTraining()){
			(*i)->train(type);
		}
	}	
}


bool Util1::buildEnough(UnitType type, size_t count){
	std::set<Unit*> us = getMyUnits(type);
	if (us.size()>=count) return true;
	build(type);
	return false;
}
bool Util1::trainEnough(UnitType type, size_t count){
	std::set<Unit*> us = getMyUnits(type);
	if (us.size()>=count)return true;
	train(type);
	return false;
}