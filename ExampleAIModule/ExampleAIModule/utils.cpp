#include "utils.h"
#include <set>
using namespace BWAPI;


//Util::Logger* Util1::log = new Util::FileLogger("neoe-ai", Util::LogLevel::MicroDetailed);
namespace Util1 {
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
		for(std::set<Unit*>::iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
		{
			if ((*i)->getType().isWorker() && (*i)->getOrder()==Orders::PlayerGuard){
				Unit* mineral = getNearestUnit((*i), nearMineralDis, UnitTypes::Resource_Mineral_Field,
					Broodwar->getAllUnits());	
				if (mineral)
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
	std::set<Unit*> Util1::getUnits(UnitType type, std::set<Unit*> all){
		std::set<Unit*> us;
		for each(Unit* u in all){if (u->getType()==type){
			us.insert(u);
		}}
		return us;
	}
	Unit* Util1::getCommandCenter(){
		if (commandCenter==NULL) commandCenter=getMyUnit(UnitTypes::Terran_Command_Center);
		return commandCenter;
	}
	Unit* Util1::getNearestUnit(Position p, double maxDistance, UnitType filter,std::set<Unit*> all){
		Unit* nearest=NULL;
		double distance = maxDistance;

		for each(Unit* u in all){
			double dis;
			if (u->getType()==filter && (dis=u->getDistance(p)) < distance){
				distance = dis;
				nearest=u;
			}
		}
		return nearest;
	}
	Unit* Util1::getNearestUnit(Unit* c, double maxDistance, UnitType filter,std::set<Unit*> all){
		return Util1::getNearestUnit(c->getPosition(), maxDistance,filter,all);
	}

	void Util1::trainEnoughSVC(){
		Player* me = Broodwar->self();	
		if (me->minerals()>=50 
			&& me->supplyUsed()<me->supplyTotal()){
				for each(Unit* cmdCenter in getMyUnits(UnitTypes::Terran_Command_Center)){
					if (cmdCenter->getTrainingQueue().size()==0){
						int mineralCnt = getUnitsNearCenter(cmdCenter,
							nearMineralDis,UnitTypes::Resource_Mineral_Field, 
							Broodwar->getAllUnits()).size();
						int svcCnt = getUnitsNearCenter(cmdCenter,nearMineralDis,
							UnitTypes::Terran_SCV, Broodwar->getAllUnits()).size();
						if (mineralCnt*svcPerMineral>svcCnt)	{
							Broodwar->printf("train SVC by %x (S%d/M%d)", cmdCenter, svcCnt, mineralCnt);
							cmdCenter->train(UnitTypes::Terran_SCV);
						}
					}}}
	}

	std::set<Unit*> Util1::getUnitsNearCenter(Unit* c, 
		double maxDistance, UnitType filter,
		std::set<Unit*> all){
			std::set<Unit*> res;
			for each( Unit* u in all){
				double dis;
				if (u->getType()==filter
					&& (dis=c->getDistance(u)) < maxDistance){
						res.insert(u);
				}
			}
			return res;
	}
	void Util1::buildEnoughSupplyDepot(){
		Player* me = Broodwar->self();
		if (me->minerals()>=100 && me->supplyUsed()+4 > me->supplyTotal() && me->supplyTotal()<200){
			build(UnitTypes::Terran_Supply_Depot);
		}
	}

	TilePosition Util1::getBuildLocation(UnitType type){
		int space=4;
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
			Unit* u = getNearestUnit(Position(p), nearMineralDis, UnitTypes::Terran_SCV, Broodwar->self()->getUnits());
			if(u){			
				if (!visible(p,type.tileWidth(), type.tileHeight())) u->rightClick(Position(p.x()*32,p.y()*32));
				else u->build(Position(p),type);
			}else{
				Broodwar->printf("cannot find SVC to build %s at (%d,%d)", type.getName().c_str(),
					p.x(), p.y());		
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

	void Util1::defenceDepartment(){

	}
	void Util1::productDepartment(){
		Util1::buildEnoughSupplyDepot();
		Util1::trainEnoughSVC();
		Util1::makeIdelWork();
		Util1::buildGas();
		Util1::havestGas();
	}
	void Util1::havestGas(){
		if (Broodwar->self()->minerals()<100)return;//mining first
		for each(Unit* refinery in getMyUnits(UnitTypes::Terran_Refinery)) if (refinery->isCompleted()){		
			std::set<Unit*> workers = gasWorkers[refinery];
			std::set<Unit*> dead;
			for each(Unit* worker in workers){
				if (!worker->exists()) dead.insert(worker);
			}
			if (workers.size()-dead.size()<svcPerGas){
				double distance= nearMineralDis;
				Unit* scv=NULL;
				for each(Unit* u in Broodwar->self()->getUnits()){
					double dis;
					if (u->getType()==UnitTypes::Terran_SCV
						&& !isHavestingGas(u)
						&& (dis=u->getDistance(refinery)) < distance){
							distance = dis;
							scv=u;
					}
				}
				if(scv){	
					scv->rightClick(refinery);
					gasWorkers[refinery].insert(scv);
					Broodwar->printf("add gas worker %x", scv);
				}else{
					Broodwar->printf("cannot found near SVC to havest gas");
				}
			}
		}
	}
	bool Util1::isHavestingGas(Unit* u){
		Order o = u->getOrder();
		if (o==Orders::MoveToGas 
			|| o==Orders::WaitForGas 
			|| o==Orders::HarvestGas 
			|| o==Orders::ReturnGas )return true;
		return false;
	}
	void Util1::buildGas(){
		if (Broodwar->self()->minerals()<100)return;
		for each(Unit* cmdCenter in getMyUnits(UnitTypes::Terran_Command_Center)){				
			std::set<Unit*>  gases = getUnitsNearCenter(cmdCenter,
				nearMineralDis,UnitTypes::Resource_Vespene_Geyser, 
				Broodwar->getAllUnits());
			for each(Unit* gas in gases){
				if (!Util1::isGasBuilt(gas)){
					Unit* scv = getNearestUnit(gas->getPosition(), 
						nearMineralDis, UnitTypes::Terran_SCV, Broodwar->self()->getUnits());
					if(scv){	
						UnitType type = UnitTypes::Terran_Refinery;
						TilePosition p =  gas->getTilePosition();
						if (!visible(p, type.tileWidth(), type.tileHeight()))
							scv->rightClick(Position(p));
						else {
							scv->build(Position(p),type);
						}
					}
				}
			}
		}
	}

	bool Util1::isGasBuilt(Unit* geyser){
		TilePosition p=geyser->getTilePosition();
		std::set<Unit*> units = Broodwar->unitsOnTile(p.x(), p.y());
		for each(Unit* j in units)
		{ 
			if ((j->getType().isBuilding() && !j->isLifted()))
				return false;
		}
		if (units.size()<=1) return true;
		return false;
	}
}