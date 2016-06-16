//---------------------------------------------------------------------------

#include <game/Game.h>
#include <application/Starter.h>

#include <vld.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	implement_link(PlayerObject);
	implement_link(SimpleGroundObject);

	static Entrance start(Game::start);
}

//---------------------------------------------------------------------------
