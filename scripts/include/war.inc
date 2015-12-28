//Barnabus 23/3/2003 - functions for war operations

//these are functions for the war system that allow for communication between the package and other packages...

use uo;
use os;

include "../pkg/opt/war/war";

function IsWarActive(player)
	//checks whether the war that a player is a member of is actually running
	var warball := SystemFindObjectBySerial(GetObjProperty(player, "WarSerial"));
	if (warball)
		return IsWar(warball);
	endif
	return 0;
endfunction

function GetWarStoneForPlayer(player)
	var warball := SystemFindObjectBySerial(GetObjProperty(player, "WarSerial"));
	//Broadcast("warball: " + warball + " WarSerial on player: " + GetObjProperty(player, "WarSerial"));
	if (warball)
		var warteam :=  GetOnPlayerTeam (player);
		var warstone := GetWarStone(warball, warteam);
		//Broadcast("warteam: " + warteam + "   warstone: " + warstone);
		if (warstone)
			return warstone;
		endif
	endif
	return 0;
endfunction

function SameWarOnOppositeTeams(killer_serial, ghost)
	// returns true if ghost and killer are in the same war but on different teams
	var killer := SystemFindObjectBySerial(killer_serial);
	if(GetObjProperty(killer, "WarSerial") == GetObjProperty(ghost, "WarSerial"))
		if(GetObjProperty(killer, "WarTeam") != GetObjProperty(ghost, "WarTeam"))
			return 1;
		endif
	endif
	return 0;
endfunction

function DoWarKill(killer_serial, ghost)
	//this should do the following
	//increment killers battlepoints // rank
	//eliminate ghost from war
	var killer := SystemFindObjectBySerial(killer_serial);
	SendSysMessage(killer, "You killed someone from the other team!", 3, 53);
	SendSysMessage(ghost, "You were killed by someone from the other team!", 3, 53);
	AwardKill(killer, ghost);
endfunction