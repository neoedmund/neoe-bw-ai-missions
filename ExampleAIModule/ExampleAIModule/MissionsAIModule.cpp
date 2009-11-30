#include "MissionsAIModule.h"
#include "utils.h"
#include <BWAPI.h>

using namespace BWAPI;

void setMapHandler();

static bool spotlog=false;
static bool stopai=false;
static Unit* lastSelect=NULL;
static Order lastOrder=Orders::None;
void dumpSelect(){
	for each(Unit* u in BW->getSelectedUnits()){
		if (u==lastSelect && u->getOrder()==lastOrder){break;}
		BW->printf("select [%s]%s(%d) [%x] at (%d,%d) %s",
			u->getPlayer()->getName().c_str(),
			u->getType().getName().c_str(),
			u->getType().getID(),
			u,u->getPosition().x(),u->getPosition().y(),
			u->getOrder().getName().c_str()
			);
		lastSelect=u;
		lastOrder=u->getOrder();
		break;
	}
}

void MissionsAIModule::onFrame()
{
	if (BW->isReplay())return;
	if (BW->isPaused())return;
	if (!stopai)mh->onFrame();
	dumpSelect();	
}


#define _MF(a,b) if (strcmp(BW->mapName().c_str(),a)==0) {mh = new b;return;}

void MissionsAIModule::setMapHandler(){
	_MF("Terran Tutorial",T10);
	_MF("T1) Wasteland",T11);
	_MF("T2) Backwater Station",T12);
	_MF("T3) Desperate Alliance",T13);
	_MF("Untitled Scenario",T14);
	_MF("T5) The Antigan Revolt",T15);
	BW->printf("no special handler for this map, use general one.");
	mh=new T00;
}

void MissionsAIModule::onStart()
{
	BW->sendText("Hello world! neoe");

	BW->printf("The map is %s, a %d player map",
		BW->mapName().c_str(),BW->getStartLocations().size());

	setMapHandler();

	// Enable some cheat flags
	BW->enableFlag(Flag::UserInput);
	// Uncomment to enable complete map information
	//BW->enableFlag(Flag::CompleteMapInformation);

	if (BW->isReplay())
	{
		BW->printf("The following players are in this replay:");
		for(std::set<Player*>::iterator p=BW->getPlayers().begin();p!=BW->getPlayers().end();p++)
		{
			if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
			{
				BW->printf("%s, playing as a %s",(*p)->getName().c_str(),(*p)->getRace().getName().c_str());
			}
		}
	}
	else
	{
		BW->printf("The match up is %s v %s",
			BW->self()->getRace().getName().c_str(),
			BW->enemy()->getRace().getName().c_str());
	}

}



void MissionsAIModule::onUnitCreate(BWAPI::Unit* unit)
{
	if (!BW->isReplay())
		BW->printf("A %s [%x] created at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	else
	{
		mh->onUnitCreate(unit);
	}
}
void MissionsAIModule::onUnitDestroy(BWAPI::Unit* unit)
{
	if (!BW->isReplay())mh->onUnitDestroy(unit);
}
void MissionsAIModule::onUnitMorph(BWAPI::Unit* unit)
{
	if (!BW->isReplay())mh->onUnitMorph(unit);
}
void MissionsAIModule::onUnitShow(BWAPI::Unit* unit)
{
	if (!BW->isReplay()){
		if (spotlog) BW->printf("A [%s]%s(%d) [%x] spot at (%d,%d)",
			unit->getPlayer()->getName().c_str(),
			unit->getType().getName().c_str(),
			unit->getType().getID(),
			unit,unit->getPosition().x(),unit->getPosition().y());
		mh->onUnitShow(unit);
	}
}
void MissionsAIModule::onUnitHide(BWAPI::Unit* unit)
{
	if (!BW->isReplay())mh->onUnitHide(unit);
}
void dumpEnemy();
bool MissionsAIModule::onSendText(std::string text)
{
	if (text=="dumpEnemy") {dumpEnemy();return false;}
	else if (text=="dumpData"){Util1::exportStaticData();}
	else if (text=="spotlog"){spotlog=!spotlog;}
	else if (text=="stopai"){stopai=!stopai;}
	else if (text=="/show players")
	{
		showPlayers();
		return false;
	} else if (text=="/show forces")
	{
		showForces();
		return false;
	}  
	if (mh->onSendText(text))return true;
	BW->printf("You typed '%s'!",text.c_str());
	return false;
}

void MissionsAIModule::drawStats()
{
	US myUnits = MYUNITS;
	BW->drawTextScreen(5,32,"I have %d units:",myUnits.size());
	std::map<UnitType, int> unitTypeCounts;
	for(US::const_iterator i=myUnits.begin();i!=myUnits.end();i++)
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
		BW->drawTextScreen(5,16*line,"- %d %ss",(*i).second, (*i).first.getName().c_str());
		line++;
	}
}

void MissionsAIModule::showPlayers()
{
	std::set<Player*> players=BW->getPlayers();
	for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++)
	{
		BW->printf("Player [%d]: %s is in force: %s",(*i)->getID(),(*i)->getName().c_str(), (*i)->getForce()->getName().c_str());
	}
}
void MissionsAIModule::showForces()
{
	std::set<Force*> forces=BW->getForces();
	for(std::set<Force*>::iterator i=forces.begin();i!=forces.end();i++)
	{
		std::set<Player*> players=(*i)->getPlayers();
		BW->printf("Force %s has the following players:",(*i)->getName().c_str());
		for(std::set<Player*>::iterator j=players.begin();j!=players.end();j++)
		{
			BW->printf("  - Player [%d]: %s",(*j)->getID(),(*j)->getName().c_str());
		}
	}
}