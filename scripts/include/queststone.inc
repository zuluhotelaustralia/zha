//Barnabus 23/3/2003 - functions for queststone communication between packages or with globals

use uo;
//include "include/constants/propids";
include "../pkg/opt/queststone/queststone";

function IsAutoTeleQuest(player)
	var questarr := GetObjProperty(player, "Quester");
	foreach q in questarr
		var stone := SystemFindObjectBySerial(q);
		if (stone&&IsQuestDeployed(stone)&&GetAutoTeleDeath(stone))
			return stone;
		endif
	endforeach
	return 0;
endfunction

function QuestNPCDeath(mobile_serial, queststone_serial)
	QuestAwardNPCDeath(queststone_serial, mobile_serial);	
endfunction

function MonsterObjectiveMet(stone_serial, player_serial, monster_serial, questpoints, dontComplete:=0)
	//Broadcast("stone_serial " + stone_serial +  ",player_serial  " + player_serial + " ,monster_serial " +monster_serial + ", questpoints " + questpoints);
	ReturnQuestMonsterObjective(stone_serial, player_serial, monster_serial, questpoints, dontComplete);
endfunction

function QuestStoneKillProtection(killer_serial, ghost)
	//players who kill other players on same quest take a quest point penalty
	var killer:= SystemFindObjectBySerial(killer_serial);

	var sharedquests := FindSharedQuests(ghost, killer);
	var stone;
	if  (sharedquests)
		foreach q in sharedquests
			stone := SystemFindObjectBySerial(q);
			SendSysMessage(killer, "You have slain a fellow Quester!", 3, 53);
			var questpenalty := CInt(GetKillerQuestPointPenalty(stone));
			AwardQuestPoints(stone.serial, killer_serial, -questpenalty);		
		endforeach
	endif
	return 0;
endfunction		

function LootingCorpseOfFellowQuester(player, corpse)
	/*	HANDLED IN antiloot.src exemptions
	if (GetObjProperty(corpse, "whose")==player.serial)
		return 0;
	endif
	*/
	var corpseplayer := SystemFindObjectBySerial(GetObjProperty(corpse, "whose"), SYSFIND_SEARCH_OFFLINE_MOBILES);
	var sharedquests := FindSharedQuests(player, corpseplayer);
	var stone;
	//Broadcast("shared quests: " + sharedquests);
	if  (sharedquests)
		foreach q in sharedquests
			stone := SystemFindObjectBySerial(q);
			//Broadcast("deployed? : " + IsQuestDeployed(stone));
			//Broadcast("looting? : " + GetLootingAllowed(stone));
			if (stone && IsQuestDeployed(stone) && !GetLootingAllowed(stone))
				//Broadcast("return " + 1);
				return 1;
			endif
		endforeach
	endif
	//Broadcast("return " + 0);
	return 0;
endfunction
		
