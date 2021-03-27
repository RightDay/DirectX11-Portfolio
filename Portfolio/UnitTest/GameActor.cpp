#include "stdafx.h"
#include "GameActor.h"

void GameActor::minusHP(UINT num)
{
	if (hp <= 0) return;

	hp -= num;
}
