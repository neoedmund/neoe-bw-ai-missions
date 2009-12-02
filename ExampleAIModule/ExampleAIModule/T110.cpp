#include <BWAPI.h>
#include "utils.h"


using namespace BWAPI;

void T110::builds(){
	Util1::buildEnough(UnitTypes::Terran_Engineering_Bay,1);
		Util1::buildEnough(UnitTypes::Terran_Missile_Turret,1);
		Util1::buildEnough(UnitTypes::Terran_Academy,1);
			
		Util1::buildEnough(UnitTypes::Terran_Factory,4);
		Util1::buildEnough(UnitTypes::Terran_Starport,4);
		Util1::upgarade(UpgradeTypes::Terran_Ship_Plating);
		Util1::upgarade(UpgradeTypes::Terran_Ship_Weapons);
		Util1::upgarade(UpgradeTypes::Terran_Vehicle_Plating);
		Util1::upgarade(UpgradeTypes::Terran_Vehicle_Weapons);		
		
}
void T110::trains(){
	//what army is handy in this mission?
		int rd =rand() % 60;
		if (rd<10)		Util1::trainEnough(UnitTypes::Terran_Wraith,3);
		else if (rd<20)Util1::trainEnough(UnitTypes::Terran_Vulture,100);
		else if (rd<30)Util1::trainEnough(UnitTypes::Terran_Science_Vessel,3);
		else if (rd<40)	Util1::trainEnough(UnitTypes::Terran_Battlecruiser,10);
		else if (rd<50)	Util1::trainEnough(UnitTypes::Terran_Goliath,30);
		else Util1::trainEnough(UnitTypes::Terran_Ghost,3);
}
