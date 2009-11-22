#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>

static bool analyzed;
static bool analysis_just_finished;
static BWTA::Region* home;
static BWTA::Region* enemy_base;
DWORD WINAPI AnalyzeThread();

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
	void drawStats(); //not part of BWAPI::AIModule
	void showPlayers();
	void showForces();
	std::map<BWAPI::Unit*,BWAPI::UnitType> buildings;
private :
	void sampleCode();

};
