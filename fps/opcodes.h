
#pragma once

class MY_OP
{
public:
	enum
	{
		S_CLIENT_INFO = 1,
		S_JOIN_GAME,				// reply join game request
		S_REMOVE_PLAYER,
		S_PLAYERS_SNAP2,
		S_ADD_PROJECTILE,
		S_REMOVE_PROJECTILE,
		S_BULLET_HIT,
		S_HIT_SCAN,
		S_PLAYER_STATUS,
		S_PLAYERS_STATS,
		//S_HIT_FEEDBACK,
		S_DAMAGE_FEEDBACK,
	};

	enum
	{
		C_JOIN_GAME = 1,			// request join game
		C_INPUT,
	};
};

