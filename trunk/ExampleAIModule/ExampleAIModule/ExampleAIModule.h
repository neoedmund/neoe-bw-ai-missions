#pragma once
#include <BWAPI.h>

class ExampleAIModule : public BWAPI::AIModule
{
public:
	virtual void onStart();
	virtual void onFrame();
	virtual bool onSendText(std::string text);
	virtual void onUnitCreate(BWAPI::Unit* unit);
	virtual void onUnitDestroy(BWAPI::Unit* unit);
	virtual void onUnitMorph(BWAPI::Unit* unit);
	virtual void onUnitShow(BWAPI::Unit* unit);
	virtual void onUnitHide(BWAPI::Unit* unit);

	void drawStats(); 
	void showPlayers();
	void showForces();

	
	virtual void onEnd(bool isWinner){};
	virtual void onPlayerLeft(BWAPI::Player* player){};
	virtual void onNukeDetect(BWAPI::Position target){};
	virtual void onUnitRenegade(BWAPI::Unit* unit){};

	void (*mapfunc) ();
	void setMapFunc();

};

void T10();
void T11();
void T12();
void T13();