#include "MissionsAIModule.h"
#include "utils.h"
#include <BWAPI.h>

using namespace BWAPI;

void setMapHandler();


Unit* lastSelect=NULL;
void dumpSelect(){
	for each(Unit* u in Broodwar->getSelectedUnits()){
		if (u==lastSelect){break;}
		Broodwar->printf("select [%s]%s(%d) [%x] at (%d,%d)",
			u->getPlayer()->getName().c_str(),
			u->getType().getName().c_str(),
			u->getType().getID(),
			u,u->getPosition().x(),u->getPosition().y());
		lastSelect=u;
		break;
	}
}

void MissionsAIModule::onFrame()
{
	if (Broodwar->isReplay())
		return;
	mh->onFrame();
	dumpSelect();
}


#define _MF(a,b) if (strcmp(Broodwar->mapName().c_str(),a)==0) {mh = new b;return;}

void MissionsAIModule::setMapHandler(){
	_MF("Terran Tutorial",T10);
	_MF("T1) Wasteland",T11);
	_MF("T2) Backwater Station",T12);
	_MF("T3) Desperate Alliance",T13);
	_MF("Untitled Scenario",T14);
	_MF("T5) The Antigan Revolt",T15);
	Broodwar->printf("map's handler not found!");
	mh=new MapHandler;
}

void MissionsAIModule::onStart()
{
	Broodwar->sendText("Hello world! neoe");

	Broodwar->printf("The map is %s, a %d player map",
		Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());

	setMapHandler();

	// Enable some cheat flags
	Broodwar->enableFlag(Flag::UserInput);
	// Uncomment to enable complete map information
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	if (Broodwar->isReplay())
	{
		Broodwar->printf("The following players are in this replay:");
		for(std::set<Player*>::iterator p=Broodwar->getPlayers().begin();p!=Broodwar->getPlayers().end();p++)
		{
			if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
			{
				Broodwar->printf("%s, playing as a %s",(*p)->getName().c_str(),(*p)->getRace().getName().c_str());
			}
		}
	}
	else
	{
		Broodwar->printf("The match up is %s v %s",
			Broodwar->self()->getRace().getName().c_str(),
			Broodwar->enemy()->getRace().getName().c_str());
	}

}



void MissionsAIModule::onUnitCreate(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay())
		Broodwar->printf("A %s [%x] created at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	else
	{
		mh->onUnitCreate(unit);
	}
}
void MissionsAIModule::onUnitDestroy(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay())mh->onUnitDestroy(unit);
}
void MissionsAIModule::onUnitMorph(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay())mh->onUnitMorph(unit);
}
void MissionsAIModule::onUnitShow(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay()){
		Broodwar->printf("A [%s]%s(%d) [%x] spot at (%d,%d)",
			unit->getPlayer()->getName().c_str(),
			unit->getType().getName().c_str(),
			unit->getType().getID(),
			unit,unit->getPosition().x(),unit->getPosition().y());
		mh->onUnitShow(unit);
	}
}
void MissionsAIModule::onUnitHide(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay())mh->onUnitHide(unit);
}
void dumpEnemy();
bool MissionsAIModule::onSendText(std::string text)
{
	if (text=="dumpEnemy") {dumpEnemy();return false;}
	else if (text=="dumpData")
	{Util1::exportStaticData();}
	else if (text=="/show players")
	{
		showPlayers();
		return false;
	} else if (text=="/show forces")
	{
		showForces();
		return false;
	}  
	Broodwar->printf("You typed '%s'!",text.c_str());
	mh->onSendText(text);
	return false;
}

void MissionsAIModule::drawStats()
{
	US myUnits = MYUNITS;
	Broodwar->drawTextScreen(5,32,"I have %d units:",myUnits.size());
	std::map<UnitType, int> unitTypeCounts;
	for(US::iterator i=myUnits.begin();i!=myUnits.end();i++)
	{
		if (unitTypeCounts.find((*i)->getType())==unitTypeCounts.end())
		{
			unitTypeCounts.insert(std::make_pair((*i)->getType(),0));
		}
		unitTypeCounts.find((*i)->getType())->second++;
	}
	int line=3;
	for(std::map<UnitType,int>::iterator i=unitTypeCounts.begin();i!=unitTypeCounts.end();i++)
	{
		Broodwar->drawTextScreen(5,16*line,"- %d %ss",(*i).second, (*i).first.getName().c_str());
		line++;
	}
}

void MissionsAIModule::showPlayers()
{
	std::set<Player*> players=Broodwar->getPlayers();
	for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++)
	{
		Broodwar->printf("Player [%d]: %s is in force: %s",(*i)->getID(),(*i)->getName().c_str(), (*i)->getForce()->getName().c_str());
	}
}
void MissionsAIModule::showForces()
{
	std::set<Force*> forces=Broodwar->getForces();
	for(std::set<Force*>::iterator i=forces.begin();i!=forces.end();i++)
	{
		std::set<Player*> players=(*i)->getPlayers();
		Broodwar->printf("Force %s has the following players:",(*i)->getName().c_str());
		for(std::set<Player*>::iterator j=players.begin();j!=players.end();j++)
		{
			Broodwar->printf("  - Player [%d]: %s",(*j)->getID(),(*j)->getName().c_str());
		}
	}
}