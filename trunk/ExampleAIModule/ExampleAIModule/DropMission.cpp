#include "DropMission.h"
#include <BWAPI.h>
using namespace BWAPI;
// 1get enough ship, 2get enough army, 3set gather spot, 4army goto spot,5ship goto spot,
	// 6 guards goto spot, 7.army upload to ship, 8 guard attach drop spot, 9 unload all to drop spot
	// 10. ship return to gather spot, 11 ship return to center, 12 goto status 0

DropMission::DropMission(Position point, size_t dropShipCnt){
	status=0;
	dropPoint=point;
	shipCnt = dropShipCnt;
}

DropMission::~DropMission(void)
{
}
void DropMission::run(){

}