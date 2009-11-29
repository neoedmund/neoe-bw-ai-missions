#pragma once
#include <BWAPI.h>
#include "utils.h"

class MissionsAIModule : public BWAPI::AIModule
{
public:
	MapHandler* mh;
	void setMapHandler();

	 void onStart();
	 void onFrame();
	 bool onSendText(std::string text);
	 void onUnitCreate(BWAPI::Unit* unit);
	 void onUnitDestroy(BWAPI::Unit* unit);
	 void onUnitMorph(BWAPI::Unit* unit);
	 void onUnitShow(BWAPI::Unit* unit);
	 void onUnitHide(BWAPI::Unit* unit);
	 void onEnd(bool isWinner){};
	 void onPlayerLeft(BWAPI::Player* player){};
	 void onNukeDetect(BWAPI::Position target){if (!BW->isReplay())mh->onNukeDetect(target);};
	 void onUnitRenegade(BWAPI::Unit* unit){if (!BW->isReplay())mh->onUnitRenegade(unit);};

	void drawStats(); 
	void showPlayers();
	void showForces();	

};

