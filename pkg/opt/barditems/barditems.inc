//Barnabus 1/5/2003 - functions for special bard items

use uo;
use os;

include "ai/main/npcinfo"; 

function CheckMaxMusicianShip(who)
	//returns true if who has 130+ musicianship
	if (CInt(GetEffectiveSkill(who,SKILLID_MUSICIANSHIP)>=130))
		return 1;
	endif
	return 0;
endfunction

function BardLevelCheck(who, item)
	var playerBLevel := GetObjProperty(who, "IsBard");
	
	if (! playerBLevel)
		return -1;
	endif
	
	var levelReq := GetObjProperty(item, "IsBard");
	
	if (playerBLevel<levelReq)
		return 0;
	endif

	return 1; //player is >= bard level restriction
endfunction

function IsHostileOrNotGuild(mob, who)
	if (IsHostile(mob)) // a hostile npc
		return 1;
	endif
	if (! mob.acct) //an npc
		return 0;
	endif
	if (GetObjProperty(mob, "guild_id")!=GetObjProperty(who, "guild_id")) //player not in same guild
		return 1;
	endif
	return 0;	//player in same guild
endfunction

function IsGuildMember(mob, who)
	if (! mob.acct)
		return 0;
	endif
	if (GetObjProperty(mob, "guild_id")==GetObjProperty(who, "guild_id"))
		return 1;
	endif
	return 0;	
endfunction

