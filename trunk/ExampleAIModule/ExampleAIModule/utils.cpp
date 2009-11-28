#include "utils.h"
#include <set>
#include <stdio.h>
#include <sstream>
#include <string>

using namespace BWAPI;


//Util::Logger* log = new Util::FileLogger("neoe-ai", Util::LogLevel::MicroDetailed);
namespace Util1 {
	Unit* getMyUnit(UnitType type){
		for(US::iterator i=MYUNITS.begin();i!=MYUNITS.end();i++)
		{
			if ((*i)->getType()==type){
				return *i;
			}
		}
		return NULL;
	}


	void makeIdelWork(){
		US cmds  = getMyUnits(UnitTypes::Terran_Command_Center);
		std::list<Unit*> orderedCmds;
		orderedCmds.assign (  cmds.begin(), cmds.end());

		for each(Unit* svc in getMyUnits(UnitTypes::Terran_SCV))
		{
			if (svc->getOrder()==Orders::PlayerGuard){
				SortClass1::center=svc;
				orderedCmds.sort(SortClass1::sortPredicate);
				for each(Unit* cmd in orderedCmds){
					Unit* mineral = getNearestUnit(cmd, 100000, UnitTypes::Resource_Mineral_Field,
						Broodwar->getAllUnits());	
					if (mineral){
						svc->rightClick(mineral);
						break;
					}
				}
			}
		}
	}
	US getMyUnits(UnitType type){
		US us;
		for each(Unit* u in MYUNITS){if (u->getType()==type){
			us.insert(u);
		}}
		return us;
	}
	US getUnits(UnitType type, US all){
		US us;
		for each(Unit* u in all){if (u->getType()==type){
			us.insert(u);
		}}
		return us;
	}
	Unit* getCommandCenter(){
		if (commandCenter==NULL) commandCenter=getMyUnit(UnitTypes::Terran_Command_Center);
		return commandCenter;
	}
	Unit* getNearestUnit(Position p, double maxDistance, UnitType filter,US all){
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
	Unit* getNearestUnit(Unit* c, double maxDistance, UnitType filter,US all){
		return getNearestUnit(c->getPosition(), maxDistance,filter,all);
	}

	void trainEnoughSVC(){
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

	US getUnitsNearCenter(Unit* c, 
		double maxDistance, UnitType filter,
		US all){
			US res;
			for each( Unit* u in all){
				double dis;
				if (u->getType()==filter
					&& (dis=c->getDistance(u)) < maxDistance){
						res.insert(u);
				}
			}
			return res;
	}
	
	int getBuildingSupplyDepot(){
		std::map<Unit* ,UnitType>::iterator it;
		int c=0;
		for ( it=building.begin() ; it != building.end(); it++ ){
			if ((*it).second==UnitTypes::Terran_Supply_Depot)c++;
		}
		return c;		
	}

	void buildEnoughSupplyDepot(){
		Player* me = Broodwar->self();
		if (me->minerals()>=100 && me->supplyUsed()+4 > me->supplyTotal()+8*getBuildingSupplyDepot()
			&& me->supplyTotal()<200){
			build(UnitTypes::Terran_Supply_Depot);
		}
	}
	double getBuildScore(TilePosition tp){
		double score=0;
		Position p = Position(tp);
		for each(Unit* u in MYUNITS){
			if (u->getType().isBuilding()){
				score-=u->getDistance(p);
			}
		}
		return score;
	}
	TilePosition getBuildLocation(UnitType type){
		int space=4;
		int width=type.tileWidth();
		int height=type.tileHeight();
		double maxScore=-1000000000;
		TilePosition res=TilePosition(-1,-1);
		for(US::iterator i=MYUNITS.begin();i!=MYUNITS.end();i++)
		{

			if ((*i)->getType().isBuilding()){
				for (int x=(*i)->getTilePosition().x()-width-space;x<=(*i)->getTilePosition().x()+space;x++){
					for (int y=(*i)->getTilePosition().y()-height-space;y<=(*i)->getTilePosition().y()+space;y++){
						if (canBuildHere(TilePosition(x,y),type)){
							double score = getBuildScore(TilePosition(x,y));
							if (score>maxScore){
								maxScore=score;
								res=TilePosition(x,y);
							}
						}
					}

				}
			}
		}
		return res;
	}

	bool visible(TilePosition p, int width, int height){
		for (int x=p.x();x<=p.x()+width;x++){
			for (int y=p.y();y<=p.y()+height;y++){
				if (!Broodwar->visible(x,y)){
					return false;
				}
			}

		}
		return true;
	}


	bool canBuildHere(TilePosition position, UnitType type) 
	{
		return Broodwar->canBuildHere(NULL, position, type);
	}

	void build(UnitType type){
		if (Broodwar->self()->minerals()<type.mineralPrice()
			|| Broodwar->self()->gas()<type.gasPrice()) return;
		TilePosition p=getBuildLocation(type);
		if (p==TilePosition(-1,-1)){
			Broodwar->printf("cannot find place to build %s", type.getName().c_str());
		}else{
			US SCVs = getMyUnits(UnitTypes::Terran_SCV);
			US buildingSCV;
			filterOrder(SCVs, Orders::BuildTerran, buildingSCV);
			US ava = US_diff(SCVs, buildingSCV);
			
			Unit* u = getNearestUnit(Position(p), 1000000, UnitTypes::Terran_SCV, ava);
			if(u){	
				building.insert(std::make_pair(u, type));
				if (!visible(p,type.tileWidth(), type.tileHeight())) u->rightClick(Position(p.x()*32,p.y()*32));
				else u->build(Position(p),type);
			}else{
				Broodwar->drawTextScreen(5,1,"cannot find SVC to build %s at (%d,%d) scvs(%d)", type.getName().c_str(),
					p.x(), p.y(), ava.size());		
			}
		}
	}

	void train(UnitType type){
		if (Broodwar->self()->minerals()<type.mineralPrice()
			|| Broodwar->self()->gas()<type.gasPrice()) return;
		const UnitType* builder = type.whatBuilds().first;	
		int v =type.whatBuilds().second;
		if (v==0) return;
		for(US::iterator i=MYUNITS.begin();i!=MYUNITS.end();i++)
		{
			if ( (*i)->getType()==*builder  && !(*i)->isTraining()){
				(*i)->train(type);
			}
		}	
	}


	bool buildEnough(UnitType type, size_t count){
		US us = getMyUnits(type);
		if (us.size()>=count) return true;
		build(type);
		return false;
	}
	bool trainEnough(UnitType type, size_t count){
		US us = getMyUnits(type);
		if (us.size()>=count)return true;
		train(type);
		return false;
	}

	void defenceDepartment(){

	}
	void productDepartment(){
		buildEnoughSupplyDepot();
		trainEnoughSVC();
		makeIdelWork();
		buildGas();
		havestGas();
	}
	void havestGas(){
		if (Broodwar->self()->minerals()<100)return;//mining first
		for each(Unit* refinery in getMyUnits(UnitTypes::Terran_Refinery)) if (refinery->isCompleted()){		
			US workers = gasWorkers[refinery];
			US dead;
			for each(Unit* worker in workers){
				if (!worker->exists()) dead.insert(worker);
			}
			if (workers.size()-dead.size()<svcPerGas){
				double distance= nearMineralDis;
				Unit* scv=NULL;
				for each(Unit* u in MYUNITS){
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
	bool isHavestingGas(Unit* u){
		Order o = u->getOrder();
		if (o==Orders::MoveToGas 
			|| o==Orders::WaitForGas 
			|| o==Orders::HarvestGas 
			|| o==Orders::ReturnGas )return true;
		return false;
	}
	void buildGas(){
		if (Broodwar->self()->minerals()<100)return;
		for each(Unit* cmdCenter in getMyUnits(UnitTypes::Terran_Command_Center)){				
			US  gases = getUnitsNearCenter(cmdCenter,
				nearMineralDis,UnitTypes::Resource_Vespene_Geyser, 
				Broodwar->getAllUnits());
			for each(Unit* gas in gases){
				if (!isGasBuilt(gas)){
					Unit* scv = getNearestUnit(gas->getPosition(), 
						nearMineralDis, UnitTypes::Terran_SCV, MYUNITS);
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

	bool isGasBuilt(Unit* geyser){
		TilePosition p=geyser->getTilePosition();
		US units = Broodwar->unitsOnTile(p.x(), p.y());
		for each(Unit* j in units)
		{ 
			if ((j->getType().isBuilding() && !j->isLifted()))
				return false;
		}
		if (units.size()<=1) return true;
		return false;
	}
	/**
	* Exports static data about UnitTypes, TypeTypes, and UpgradeTypes to a text 
	* file name "TypeData.txt"
	*/
	void exportStaticData() {

		FILE *typeData = 0;
		typeData = fopen("TypeData.txt", "w");

		// Export unit type data
		append(typeData, "UnitTypes");
		append(typeData, "-id,race,name,mins,gas,hitPoints,shields,energy,buildTime,canAttack,canMove,width,height,supply,supplyProvided,sight,groundMaxRange,groundMinRange,groundDamage,airRange,airDamage,isBuilding,isFlyer,isSpellCaster,isWorker,whatBuilds");

		std::set<UnitType> types = UnitTypes::allUnitTypes();
		for(std::set<UnitType>::iterator i=types.begin();i!=types.end();i++)
		{
			int id = i->getID();
			std::string race = i->getRace().getName();
			std::string name = i->getName();
			int minerals = i->mineralPrice();
			int gas = i->gasPrice();
			int hitPoints = i->maxHitPoints();
			int shields = i->maxShields();
			int energy = i->maxEnergy();
			int buildTime = i->buildTime();
			bool canAttack = i->canAttack();
			bool canMove = i->canMove();
			int width = i->tileWidth();
			int height = i->tileHeight();
			int supplyRequired = i->supplyRequired();
			int supplyProvided = i->supplyProvided();
			int sightRange = i->sightRange();
			int groundMaxRange = i->groundWeapon()->maxRange();
			int groundMinRange = i->groundWeapon()->minRange();
			int groundDamage = i->groundWeapon()->damageAmount();
			int airRange = i->airWeapon()->maxRange();
			int airDamage = i->airWeapon()->damageAmount();
			bool isBuilding = i->isBuilding();
			bool isFlyer = i->isFlyer();
			bool isSpellCaster = i->isSpellcaster();
			bool isWorker = i->isWorker();
			int whatBuilds = i->whatBuilds().first->getID();

			std::string unitType(" UnitType");
			unitType += ":" + toString(id)
				+ ":" + race
				+ ":" + name
				+ ":" + toString(minerals)
				+ ":" + toString(gas)
				+ ":" + toString(hitPoints)
				+ ":" + toString(shields)
				+ ":" + toString(energy)
				+ ":" + toString(buildTime)
				+ ":" + toString(canAttack)
				+ ":" + toString(canMove)
				+ ":" + toString(width)
				+ ":" + toString(height)
				+ ":" + toString(supplyRequired)
				+ ":" + toString(supplyProvided)
				+ ":" + toString(sightRange)
				+ ":" + toString(groundMaxRange)
				+ ":" + toString(groundMinRange)
				+ ":" + toString(groundDamage)
				+ ":" + toString(airRange)
				+ ":" + toString(airDamage)
				+ ":" + toString(isBuilding)
				+ ":" + toString(isFlyer)
				+ ":" + toString(isSpellCaster)
				+ ":" + toString(isWorker)
				+ ":" + toString(whatBuilds);

			append(typeData, unitType);
		}

		// Export tech types
		append(typeData, "TechTypes");
		append(typeData, "-id,name,whatResearches,minerals,gas");

		std::set<TechType> tektypes = TechTypes::allTechTypes();
		for(std::set<TechType>::iterator i=tektypes.begin();i!=tektypes.end();i++)
		{
			int id = i->getID();
			std::string name = i->getName();
			int whatResearchesID = i->whatResearches()->getID(); 
			int mins = i->mineralPrice();
			int gas = i->gasPrice();

			std::string techType(" TechType"); 
			techType += ":" + toString(id)
				+ ":" + name
				+ ":" + toString(whatResearchesID)
				+ ":" + toString(mins)
				+ ":" + toString(gas);

			append(typeData, techType);
		}

		// Export upgrade types
		append(typeData, "UpgradeTypes");
		append(typeData, "-id,name,whatUpgrades,repeats,minBase,minFactor,gasBase,gasFactor");

		std::set<UpgradeType> upTypes = UpgradeTypes::allUpgradeTypes();
		for(std::set<UpgradeType>::iterator i=upTypes.begin();i!=upTypes.end();i++)
		{
			int id = i->getID();
			std::string name = i->getName();
			int whatUpgradesID = i->whatUpgrades()->getID(); // unit type id of what researches it
			int repeats = i->maxRepeats();
			int minBase = i->mineralPriceBase();
			int minFactor = i->mineralPriceFactor();
			int gasBase = i->gasPriceBase();
			int gasFactor = i->gasPriceFactor();

			std::string upgradeType(" UpgradeType"); 
			upgradeType += ":" + toString(id)
				+ ":" + name
				+ ":" + toString(whatUpgradesID)
				+ ":" + toString(repeats)
				+ ":" + toString(minBase)
				+ ":" + toString(minFactor)
				+ ":" + toString(gasBase)
				+ ":" + toString(gasFactor);

			append(typeData, upgradeType);
		}
	}
	void updateBuilding(){
		std::map<Unit* ,UnitType>::iterator it;

		for ( it=building.begin() ; it != building.end(); it++ ){
			Unit* u=(*it).first;
			if (!u->exists()||u->getOrder()==Orders::PlayerGuard){
				building.erase(u);
			}
		}

	}
	
	void updateStatus(){
		setExpMap();
		updateBuilding();
	}
	void setExpMap(){
		for(int x=0; x<Broodwar->mapWidth();x++){
			for(int y=0; y<Broodwar->mapHeight();y++){
				if(!expMap->get(x,y) && Broodwar->visible(x,y)) {
					expMap->set(x,y,true);
					//stop exploring visited spot
					for each(Unit* u in exploring[TilePosition(x,y)])
						u->stop();
					exploring.erase(TilePosition(x,y));
				}
			}
		}
		US en = getEnemyUnits();
		if (en.size()>0){//stop exploring when enemy seen
			std::map<TilePosition,US>::iterator it;
			for ( it=exploring.begin() ; it != exploring.end(); it++ ){
				for each(Unit* u in (*it).second)
					u->stop();

			}
			exploring.clear();
		}

	}

	void initExpMap(){
		expMap = new Array2D(Broodwar->mapWidth(), Broodwar->mapHeight());
		for(int x=0; x<Broodwar->mapWidth();x++){
			for(int y=0; y<Broodwar->mapHeight();y++){
				expMap->set(x,y,false);
			}
		}
	}

	TilePosition getUnexploredPos(US army){
		TilePosition p = TilePosition(-1,-1);
		double min=1e+20;
		for (int x=0;x<Broodwar->mapWidth();x++)
			for (int y=0;y<Broodwar->mapHeight();y++)
				if (!EM(x,y)&&Broodwar->walkable(x*4,y*4)&&
					(EM(x-1,y)
					||EM(x+1,y)
					||EM(x,y-1)
					||EM(x,y+1)
					||EM(x-1,y-1)
					||EM(x+1,y+1)
					||EM(x+1,y-1)
					||EM(x-1,y+1)
					)){
						//expMap->set(x,y,true);
						TilePosition p2=TilePosition(x,y);
						double  dis=0;
						for each(Unit* u in army) dis+=u->getDistance(Position(p2));
						if (dis<min){
							min=dis;
							p=p2;
						}
				}
				return p;
	}
	void attack(Unit* enemy, US army){
		for each(Unit* u in army) u->attackMove(enemy->getPosition());
		US a = attacking[enemy];
		a.insert(army.begin(), army.end());
	}
	void onUnitDestroy(Unit* unit){
		US ens = getEnemyUnits();
		US a = attacking[unit];
		if (ens.size()==0){
			for each(Unit* u in a) u->stop();
		}else{
			for each(Unit* u in a) u->attackMove((*ens.begin())->getPosition());
		}
	}
	void bordExplore(US army1){
		US army ;
		filterOrder(army1, Orders::PlayerGuard, army);
		if (army.size()==0)return;
		TilePosition p = Util1::getUnexploredPos(army);
		if (p==TilePosition(-1,-1))return;
		Position p1 = Position(p);
		for each(Unit* u in army) {
			u->attackMove(p);
			Broodwar->drawLine(CoordinateType::Map, 
				u->getPosition().x(),u->getPosition().y(),
				p1.x(), p1.y(), Colors::Green);

		}
		exploring.insert(std::make_pair(p, army));


		Broodwar->drawBox(CoordinateType::Map, p1.x() , p1.y()  , p1.x()   + 8, p1.y() + 8, Colors::Orange, false);
		Broodwar->printf("explore (%d,%d) %d %s", p1.x(), p1.y(), army.size(), EM(p.x(), p.y())?"T":"F");

	}
	void upgarade(UpgradeType ut){
		const UnitType* ft = ut.whatUpgrades();
		Unit* f=getMyUnit(*ft);
		if(f){
			if(Broodwar->self()->minerals()>=ut.mineralPriceBase()
				&&Broodwar->self()->gas()>=ut.gasPriceBase()
				&&!f->isUpgrading()) f->upgrade(ut);
		}
	}

	void filterOrder(US const &src, Order filter, US &addto){
		for each(Unit* u in src) if (u->getOrder()==filter) 
		{
			addto.insert(u);
		}
	}
	US getEnemyUnits(){
		US res;
		for each(Unit* u in Broodwar->getAllUnits()){
			if (u->getPlayer()->isEnemy(Broodwar->self()))res.insert(u);
		}
		return res;
	}
	US getNeutralUnits(){
		US res;
		for each(Unit* u in Broodwar->getAllUnits()){
			if (u->getPlayer()->isNeutral()) res.insert(u);
		}
		return res;
	}
	int getMyControlledMineralCnt(){
		int t=0;
		for each(Unit* cc in getMyUnits(UnitTypes::Terran_Command_Center)){

			t += getUnitsNearCenter(cc,
				nearMineralDis,UnitTypes::Resource_Mineral_Field, 
				Broodwar->getAllUnits()).size();
		}
		return t;
	}
	void repairDepartment(){
		if (Broodwar->self()->minerals()>100){
			for each(Unit* u in MYUNITS){
				if (u->getType().isBuilding() && u->getHitPoints()<u->getInitialHitPoints()){
					Unit* scv=getNearestUnit(u->getPosition(), 100000, UnitTypes::Terran_SCV,MYUNITS);
					if (scv && !scv->isConstructing()){
						scv->rightClick(u);
					}
				}
			}
		}
	}
}
/**
* Utiliity function for int to string conversion.
*/
std::string toString(int value) 
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

/**
* Utiliity function for bool to string conversion.
*/
std::string toString(bool value) 
{
	if (value) return std::string("1");
	else return std::string("0");
}
/**
* Utility function for appending data to a file.
*/
void append(FILE *log, std::string data) {
	data += "\n";
	fprintf(log, (char*)data.c_str());
	fflush(log);
}
Unit* SortClass1::center=NULL;

US US_diff(US const &src, US const &b){
	US a;
	for each(Unit* u in src) a.insert(u);
	for each(Unit* u in b){
		US::iterator i = a.find(u);
		if(i!=a.end()) a.erase(i);
	}
	return a;
}