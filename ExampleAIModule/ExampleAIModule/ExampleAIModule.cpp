#include "ExampleAIModule.h"
#include "utils.h"
#include <BWAPI.h>

using namespace BWAPI;


void ExampleAIModule::onFrame()
{
	if (Broodwar->isReplay())
		return;
	drawStats();
	mapfunc();
}

static void noaction(){}

#define _MF(a,b) if (strcmp(Broodwar->mapName().c_str(),a)==0) {mapfunc = b;return;}

void ExampleAIModule::setMapFunc(){
	_MF("Terran Tutorial",T10);
	_MF("T1) Wasteland",T11);
	_MF("T2) Backwater Station",T12);
	//_MF("T3) Desperate Alliance",T13);
	Broodwar->printf("mapfunc not found!");
	mapfunc=noaction;
}

void ExampleAIModule::onStart()
{
	Broodwar->sendText("Hello world! neoe");

	Broodwar->printf("The map is %s, a %d player map",
		Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());

	setMapFunc();

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



void ExampleAIModule::onUnitCreate(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay())
		Broodwar->printf("A %s [%x] created at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	else
	{
		/*if we are in a replay, then we will print out the build order
		(just of the buildings, not the units).*/
		if (unit->getType().isBuilding() && unit->getPlayer()->isNeutral()==false)
		{
			int seconds=Broodwar->getFrameCount()/24;
			int minutes=seconds/60;
			seconds%=60;
			Broodwar->printf("%.2d:%.2d: %s creates a %s",minutes,seconds,unit->getPlayer()->getName().c_str(),unit->getType().getName().c_str());
		}
	}
}
void ExampleAIModule::onUnitDestroy(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay())
		Broodwar->printf("A %s [%x] destroyed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}
void ExampleAIModule::onUnitMorph(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay())
		Broodwar->printf("A %s [%x] morphed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	else
	{
		/*if we are in a replay, then we will print out the build order
		(just of the buildings, not the units).*/
		if (unit->getType().isBuilding() && unit->getPlayer()->isNeutral()==false)
		{
			int seconds=Broodwar->getFrameCount()/24;
			int minutes=seconds/60;
			seconds%=60;
			Broodwar->printf("%.2d:%.2d: %s morphs a %s",minutes,seconds,unit->getPlayer()->getName().c_str(),unit->getType().getName().c_str());
		}
	}
}
void ExampleAIModule::onUnitShow(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay())
		Broodwar->printf("A [%s]%s(%d) [%x] spotted at (%d,%d)",
			unit->getPlayer()->getName().c_str(),
			unit->getType().getName().c_str(),
			unit->getType().getID(),
			unit,unit->getPosition().x(),unit->getPosition().y());
}
void ExampleAIModule::onUnitHide(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay())
		Broodwar->printf("A %s [%x] was last seen at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}
void dumpEnemy();
bool ExampleAIModule::onSendText(std::string text)
{
	if (text=="dumpEnemy") {dumpEnemy();return false;}
	if (text=="/show players")
	{
		showPlayers();
		return false;
	} else if (text=="/show forces")
	{
		showForces();
		return false;
	}  else
	{
		Broodwar->printf("You typed '%s'!",text.c_str());
	}
	return true;
}

void ExampleAIModule::drawStats()
{
	std::set<Unit*> myUnits = Broodwar->self()->getUnits();
	Broodwar->drawTextScreen(5,32,"I have %d units:",myUnits.size());
	std::map<UnitType, int> unitTypeCounts;
	for(std::set<Unit*>::iterator i=myUnits.begin();i!=myUnits.end();i++)
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

void ExampleAIModule::showPlayers()
{
	std::set<Player*> players=Broodwar->getPlayers();
	for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++)
	{
		Broodwar->printf("Player [%d]: %s is in force: %s",(*i)->getID(),(*i)->getName().c_str(), (*i)->getForce()->getName().c_str());
	}
}
void ExampleAIModule::showForces()
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