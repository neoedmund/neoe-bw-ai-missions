#pragma once
#include <BWAPI.h>
using namespace BWAPI;
class DropMission
{
public:
	DropMission(Position point, size_t dropShipCnt);
	~DropMission(void);
	void run();
private:
	int status;
	Position dropPoint;
	size_t shipCnt;
};
