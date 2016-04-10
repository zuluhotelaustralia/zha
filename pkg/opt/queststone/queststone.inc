//Barnabus 23/3/2003 - functions for quest stone operations

use uo;
use os;

include "include/constants/propids";
include "include/utility";
include "include/stringarrays";
include "include/itemutil";
include "include/classes";
include "include/yesno";
include "include/debug";


//update removed qCompletedObjectives prop and replaced with qPObjComp+stoneserial prop

//RewardQuestItems constants
const REWARDQUESTITEMS_NO :=0; //noone can buy quest items yet
const REWARDQUESTITEMS_ALL :=1; // all players can buy quest items with quest points
const REWARDQUESTITEMS_COMPLETED :=2; //completed players only can buy quest items with quest points

//Finalise Option constants
const FINALOPTION_NONE :=0;
const FINALOPTION_PERSCOMPLNALLOBJS :=1; 
const FINALOPTION_PERSCOMPLN1OBJ :=2;
const FINALOPTION_PERSMAXQP :=3;
const FINALOPTION_ALLCOMPLNALLOBJS :=4;
const FINALOPTION_ALLCOMPLN1OBJ :=5;
const FINALOPTION_ALLMAXQP :=6;

//Registration Condition constants
const REGCONDITION_NONE :=0;
const REGCONDITION_NEWBIE :=1; 
const REGCONDITION_BARD :=2; 
const REGCONDITION_CRAFTER :=3; 
const REGCONDITION_MAGE :=4; 
const REGCONDITION_RANGER :=5; 
const REGCONDITION_THIEF :=6; 
const REGCONDITION_WARRIOR :=7; 
const REGCONDITION_RACE:=8;
const REGCONDITION_LEVELONLY:=9;
const REGCONDITION_POWERPLAYER:=10;



function ChangeQuestTitle(stone, title)
	SetObjProperty(stone, "title", title);
	stone.name := "Quest Stone: " + title;
endfunction

function ChangeQuestIntro(stone, intro)
	SetObjProperty(stone, "intro", intro);
endfunction

function ChangeQuestFinal(stone, final)
	SetObjProperty(stone, "final", final);
endfunction

function GetQuestFinal(stone)
	return GetObjProperty(stone, "final");
endfunction


function GetCreator(stone)
	return GetObjProperty(stone, "ownerserial");
endfunction

function SetCreator(stone, ownerserial)
	SetObjProperty(stone, "ownerserial", ownerserial);
endfunction


function SetKillerQuestPointPenalty(stone, val)
	SetObjProperty(stone, "KillerQuestPointPenalty", val);
endfunction

function GetKillerQuestPointPenalty(stone)
	return GetObjProperty(stone, "KillerQuestPointPenalty");
endfunction

function SetLootingAllowed(stone, val)
	SetObjProperty(stone, "LootingAllowed", val);
endfunction

function GetLootingAllowed(stone)
	return GetObjProperty(stone, "LootingAllowed");
endfunction

function SetAutoTeleDeath(stone, val)
If(Getobjproperty(stone, "AutoTelePoint"))
   EnableAutoTelePoint(stone, stone.x, stone.y, stone.z);
endif
	SetObjProperty(stone, "AutoTeleDeath", val);
endfunction

function GetAutoTeleDeath(stone)
	return GetObjProperty(stone, "AutoTeleDeath");
endfunction


function RegisterPlayer(who, stone)
	if (!IsRegistered(stone, who.serial))
		if (AddQuest(who, stone.serial))
			AddPlayerData(stone, who);
			return 1;
		else
			SendSysMessage(who, "You are only permitted to register for a maximum of 5 quests.", 3, 53);
			return 0;
		endif
	else
		SendSysMessage(who, "You are already registered for this quest.", 3, 53);
		return 0;
	endif
endfunction

function AddPlayerData(stone, player)
	//stores player data in parrallel arrays
	var pserials:=GetObjProperty(stone, "playerserials");
	var qpoints:=GetObjProperty(stone, "playerquestpoints");
	var psize:=pserials.size();
	if (!pserials[1])
		//Broadcast("nothing in pserial[1]"); 
		SetObjProperty(stone, "playerserials",{player.serial});
		SetObjProperty(stone, "playerquestpoints",{0}); //0 points is starting default
		//SetObjProperty(player, "Quester", stone.serial);//done in AddQuest
		return;
	endif
	pserials[psize+1]:=player.serial;
	qpoints[psize+1]:=0;
	SetObjProperty(stone, "playerserials",pserials);
	SetObjProperty(stone, "playerquestpoints",qpoints);
	//sets queststone serial in "Quester" prop on player so can track looting protection and killing penalty
	// killing protection is // killing a player -100 qps // cant loot people on same quest
	//SetObjProperty(player, "Quester", stone.serial);//updated: now done in AddQuest
endfunction

function RemovePlayerData(stone, player_serial)
	//removes player data in parrallel arrays
	//this will need to be changed to delete any props on players if they are on there
	//still retains players completed array data
	var pserials:=GetObjProperty(stone, "playerserials");
	var qpoints:=GetObjProperty(stone, "playerquestpoints");
	var tempPSerials:=array;
	var tempQPs:=array;
	var i:=1;
	
	foreach p in pserials
		if (p!=player_serial)
			tempPSerials.append(p);
			tempQPs.append(qpoints[i]);
		endif
		i:=i+1;
	endforeach
 
        var player := SystemFindObjectBySerial( player_serial , SYSFIND_SEARCH_OFFLINE_MOBILES);
        var questarr := GetObjProperty(player, "Quester");
        var tempq := array;
        
	foreach q in questarr
		var stone := SystemFindObjectBySerial(q);
		if (q != cint(stone.serial))
                   tempq.append(q);
		endif
	endforeach
 
        SetObjProperty(player, "Quester", tempq);
	SetObjProperty(stone, "playerserials",tempPSerials);
	SetObjProperty(stone, "playerquestpoints",tempQPs);
	//clean player held data

	EraseObjProperty(player, "qPObjComp"+stone.serial); //remove personal objectives completed prop
	RemoveQuest(player, stone.serial);
endfunction


function ClearPlayerData(stone)
	var pserials:=GetObjProperty(stone, "playerserials");
	var player;
	foreach p in pserials
		player := SystemFindObjectBySerial(p, SYSFIND_SEARCH_OFFLINE_MOBILES);
		EraseObjProperty(player, "qPObjComp"+stone.serial); //remove personal objectives completed prop
		RemoveQuest(player, stone.serial); //remove quest stone prop
	endforeach		
	
	EraseObjProperty(stone, "playerserials");
	EraseObjProperty(stone, "playerquestpoints");
	EraseObjProperty(stone, "CompletedPlayers");
endfunction


function ListRegistedPlayerNames(stone)
//simply returns names of registered players
	var playerserials:=GetObjProperty(stone, "playerserials");
	var playernames:=array;
	var player;
	foreach p in playerserials
		player:=SystemFindObjectBySerial(p);
		playernames.append(player.name);
		//Broadcast("player name: " + player.name);
	endforeach
	return playernames;
endfunction

function IsRegistered(stone, playerserial)
	//returns whether a player serial is already listed in registered list
	//0 =not registered, >0 indicates index number
	var playerserials:=GetObjProperty(stone, "playerserials");
	var result :=0;
	var index:=1;
	foreach p in playerserials
		if (p==playerserial)
			result:=index;
			break;
		endif
		index:=index+1;
	endforeach
	return result;
endfunction

function AddBannedPlayer(stone, player_serial)
	//adds a player to the banned list
	var qBannedPlayers:=GetObjProperty(stone, "BannedPlayers"); //list of banned player serials

	if (!qBannedPlayers[1])
		//Broadcast("nothing in pserial[1]"); 
		SetObjProperty(stone, "BannedPlayers",{player_serial});
		return;
	endif
	
	qBannedPlayers.append(player_serial);
	
	SetObjProperty(stone, "BannedPlayers",qBannedPlayers);
	return 1;
endfunction

function RemoveBannedPlayer(stone, player_serial)
	//remove a player from the banned list
	var qBannedPlayers:=GetObjProperty(stone, "BannedPlayers"); //list of banned player serials
	var tempBanned := array;
	var result :=0;
	foreach q in qBannedPlayers
		if (q!=player_serial)
			tempBanned.append(q);
		else
			result:=1; //found banned player
		endif
	endforeach
	
	SetObjProperty(stone, "BannedPlayers",tempBanned);
	return result;
endfunction

function AddQuestSpawnPoint(who, stone, spawn)
	var spawnmonsters:= GetObjProperty(spawn, "PointData");
	//check spawn for suitability
	if (spawnmonsters[1]!="NPC"&&spawnmonsters[1]!="Group")
		PrintTextAbove(stone, "Spawnpoint type not supported");
		return 0; //spawn type invalid
	endif
	var objective := 0;
	var qSpawnPoints:=GetObjProperty(stone, "SpawnPoints");
	var qObjectives:=GetObjProperty(stone, "Objectives");

	if (!qSpawnPoints[1])
		//Broadcast("nothing in pserial[1]"); 
		SetObjProperty(stone, "SpawnPoints",{spawn.serial});
		SetObjProperty(spawn, "QuestStoneSerial", stone.serial); //0 points is starting default
		PrintTextAbove(spawn, "Spawnpoint added to quest");
                RunSpawnEditGump(who, spawn, stone);
		return;
	endif
	
	if (IsSpawnInQuest(stone, spawn.serial))
		PrintTextAbove(stone, "Spawnpoint already listed for quest");
		return 0;
	endif
	
	qSpawnPoints.append(spawn.serial);
	
	SetObjProperty(spawn, "QuestStoneSerial", stone.serial);
	SetObjProperty(stone, "SpawnPoints",qSpawnPoints);
	PrintTextAbove(stone, "Spawnpoint added to quest");
 
        RunSpawnEditGump(who, spawn, stone);
	return 1;
endfunction

function RemoveQuestSpawnPoint(stone, spawn)
	var qSpawnPoints:=GetObjProperty(stone, "SpawnPoints");
  var activespawns := GetObjProperty(stone, "SpawnActive");
        
	var tempQSPoints:=array;
 
	foreach q in qSpawnPoints
		if (q!=spawn.serial)
			tempQSPoints.append(q);
		endif
	endforeach
        activespawns.erase( cint(spawn.serial) );
	SetObjProperty(stone, "SpawnPoints",tempQSPoints);
	SetObjProperty(stone, "SpawnActive",activespawns);
	EraseObjProperty(spawn, "QuestStoneSerial");
	PrintTextAbove(spawn, "Spawnpoint removed from quest");
endfunction

function deleteAllQuestSpawnPointsRefs(stone)
	var qSpawnPoints:=GetObjProperty(stone, "SpawnPoints");
	var spawn;
	var result :=0;
	var index:=1;
	foreach q in qSpawnPoints
		spawn := SystemFindObjectBySerial(q);
		RemoveQuestSpawnPoint(stone, spawn);
	endforeach
	
	EraseObjProperty(stone, "SpawnPoints");
endfunction

function IsSpawnInQuest(stone, spawnserial)
	//returns whether a spawn serial is already listed in registered list
	//0 =not registered, >0 indicates index number
	var qSpawnPoints:=GetObjProperty(stone, "SpawnPoints");	
	//Broadcast("spawnserial : " +spawnserial);
	spawnserial := CInt(spawnserial);
	var result :=0;
	var index:=1;
	foreach q in qSpawnPoints
		q := CInt(q);
		//Broadcast("index : " + index + ": " + q);
		if (q==spawnserial)
			result:=index;
			break;
		endif
		index:=index+1;
	endforeach
	return result;
endfunction

function QuestAwardNPCDeath(queststone_serial, corpse_serial, player_serial, spawn_serial, points)
	//Broadcast("awarding death");
	var stone := SystemFindObjectBySerial(queststone_serial);
        var corpse := SystemFindObjectBySerial(corpse_serial);
	if (!IsQuestDeployed(stone))
		//Broadcast("stone not deployed");
		return 0; //if quest has not been deployed then no questpoints can be obtained.
	endif
 
        if (!player_serial)
	        player_serial :=GetNPCKiller(corpse_serial);
        endif
        
	if (!IsRegistered(stone, player_serial))
		debugmessage("not registered");
		return 0;
	endif	
        //Check To See If Spawn Is Active For This Quest
        If(!IsSpawnActive(stone, spawn_serial))
        debugmessage("spawninactive");
           return 0;
        endif
	//completed players cant get more questpoints
	if (IsCompleted(stone, player_serial))
			return 0;
	endif
debugmessage("stone: "+queststone_serial+" player: "+player_serial+" points: "+points);
	AwardQuestPoints(queststone_serial, player_serial, points);
endfunction

function DetermineNPCDeathAward(corpse_serial)
	//Broadcast("mobile serial " + corpse_serial);
	//awarded quest points currently based on npc str, int or magic item level whatever higher
	var corpse := SystemFindObjectBySerial(corpse_serial);
	var npctemplate := GetObjProperty(corpse,"npctemplate");
	//Broadcast("corpse : " + corpse.name);
	return DetermineNPCTemplateQPs(npctemplate);
endfunction

function DetermineNPCTemplateQPs(npctemplate)
	var questPoints:=0;
	var npc_cfg	:= ReadConfigFile( "::npcdesc" ); //find monsters desc info	
	var magicitemlevel	:= npc_cfg[npctemplate].magicitemlevel; // magic item level
	var monsterInt := npc_cfg[npctemplate].int;
	var monsterStr := npc_cfg[npctemplate].str;
	
	//Broadcast("monster item level : " + magicitemlevel);
	//Broadcast("monster int : " + monsterInt);
	//Broadcast("monster str : " + monsterStr);
	
	//points awarded is greater of magicitemlevel*75, int/5, and str/5
	questPoints := magicitemlevel*75; //somewhere between 75 (lvl 1) and 450 (lvl 6)
		
	if (questPoints<monsterInt/5) //somewhere between 7 for an orc, and 400 for a balron
		questPoints :=monsterInt/5;
	endif
	
	if(questPoints<monsterStr/5) //about 200 for a balron
		questPoints :=monsterStr/5;
	endif
	//Broadcast("quest points awarded: " + questPoints);
	return questPoints;

endfunction

function GetNPCKiller(corpse_serial)
	//returns killers serial
	var corpse := SystemFindObjectBySerial(corpse_serial);
	var killer:=getobjproperty(corpse,PROPID_MOBILE_KILLED_BY_SERIAL);
	if (!killer)
		return 0;
	endif
	return killer;
endfunction

function AwardQuestPoints(queststone_serial, player_serial, questPoints)
	var stone := SystemFindObjectBySerial(queststone_serial);
	var playerIndex :=IsRegistered(stone, player_serial);
	if (playerIndex)
		var qpoints:=GetObjProperty(stone, "playerquestpoints");
		qpoints[playerIndex]:= qpoints[playerIndex] + questPoints;
		SetObjProperty(stone, "playerquestpoints",qpoints);
		var player := SystemFindObjectBySerial(player_serial);
		if (questPoints>0)
			SendSysMessage(player, "You have been awarded quest points.", 3, 53);
		elseif(questPoints<0)
			SendSysMessage(player, "You have lost quest points.", 3, 53);
		endif
		CheckFinaliseQuest(stone, player);
	endif
endfunction

function GetPlayersQuestPoints(queststone_serial, player_serial)
	var stone := SystemFindObjectBySerial(queststone_serial);
	var playerIndex :=IsRegistered(stone, player_serial);
	var qpoints;
	if (playerIndex)
		qpoints:=GetObjProperty(stone, "playerquestpoints");
	endif
	if(qpoints)
		return qpoints[playerIndex];
	endif
	return 0;
endfunction

function AddNewObjective(queststone_serial, objective, type, questpoints)
	//has a parrallel array of objectives, completion status, and messages
	var stone := SystemFindObjectBySerial(queststone_serial);
	
	var qObjectives:=GetObjProperty(stone, "Objectives");
	var qObjComplete:=GetObjProperty(stone, "ObjCompleteStatus"); //parrallel array for objective data. 0 if incomplete, 1 if complete.
	var qObjCompleteMessages := GetObjProperty(stone, "ObjCompletedMessages"); //parrallel array of objective completed strings
	
	if (!qObjectives[1])
		//Broadcast("nothing in pserial[1]"); 
		SetObjProperty(stone, "Objectives",{objective.serial});
		SetObjProperty(stone, "ObjCompleteStatus",{0});
		SetObjProperty(stone, "ObjCompletedMessages", {""});
		SetObjProperty(objective, "QuestStoneSerial", stone.serial); //0 points is starting default
		SetObjProperty(objective, "QuestPoints", questpoints);
		SetObjProperty(objective, "QuestObjectiveType", type);
		PrintTextAbove(stone, "Objective added to quest");
		return;
	endif
	
	if (IsObjectiveRegistered(stone, objective.serial))
		PrintTextAbove (stone, "That objective has already been registered.");
		return 0;
	endif
	
	qObjectives.append(objective.serial);
	qObjComplete.append(0); //default incomplete
	qObjCompleteMessages.append(""); //adds a placeholding null string in position that can be set later
	
	SetObjProperty(stone, "Objectives",qObjectives);
	SetObjProperty(stone, "ObjCompleteStatus",qObjComplete); 
	SetObjProperty(stone, "ObjCompletedMessages", qObjCompleteMessages);
	SetObjProperty(objective, "QuestStoneSerial", stone.serial); 
	SetObjProperty(objective, "QuestPoints", questpoints);
	SetObjProperty(objective, "QuestObjectiveType", type);
	PrintTextAbove(stone, "Objective added to quest");
	return 1;
endfunction

function	deleteAllOneOffObjectives(stone)
//destroys or kills all one off objectives
	var objective;
	var qObjectives:=GetObjProperty(stone, "Objectives");

	foreach q in qObjectives
		objective := SystemFindObjectBySerial(q);
		if (objective.isa(POLCLASS_MOBILE))
			//kill it
				SetObjProperty( objective, "guardkill", 1);
				ApplyRawDamage( objective, CInt(GetHp(objective)+3) );
		else
			//destroy it
			DestroyItem( objective );
		endif
	endforeach	
endfunction
	
function	deletePersonalObjectives(stone)
//destroys all personal objectives
	var objective;
	var qObjectives:=GetObjProperty(stone, "PersonalObjectives");

	foreach q in qObjectives
		objective := SystemFindObjectBySerial(q);
		if (objective.isa(POLCLASS_MOBILE))
			//kill it
				SetObjProperty( objective, "guardkill", 1);
				ApplyRawDamage( objective, CInt(GetHp(objective)+3) );
		else
			//destroy it
			DestroyItem( objective );
		endif
	endforeach

endfunction

function AddNewPersonalObjective(stone_serial, objective, objectivetype, questpoints)
//personal objectives are stored in a separate array because one-off objectives completion status is stored
//in a parrallel array, while personal objective completion status is stored on the player
//this is used when objective data is stored on the objective... like for location objetives...
//not object type objectives.
	var stone := SystemFindObjectBySerial(stone_serial);

	var qObjectives:=GetObjProperty(stone, "PersonalObjectives");
	var qObjCompleteMessages:= GetObjProperty(stone, "PersonalObjMessages"); //parrallel array of objective completed strings

	if (!qObjectives[1])
		SetObjProperty(stone, "PersonalObjectives",{objective.serial});
		SetObjProperty(stone, "PersonalObjMessages", {""});
		SetObjProperty(objective, "QuestStoneSerial", stone.serial);
		SetObjProperty(objective, "QuestPoints", questpoints);
		SetObjProperty(objective, "QuestObjectiveType", objectivetype);
		PrintTextAbove(stone, "Objective added to quest");
		return;
	endif
	
	if (IsPersonalObjectiveRegistered(stone, objective.serial))
		PrintTextAbove (stone, "That objective has already been registered.");
		return 0;
	endif
	
	qObjectives.append(objective.serial);
	qObjCompleteMessages.append(""); //place holder message string
	
	SetObjProperty(stone, "PersonalObjectives",qObjectives);
	SetObjProperty(stone, "PersonalObjMessages", qObjCompleteMessages);
	SetObjProperty(objective, "QuestStoneSerial", stone.serial); 
	SetObjProperty(objective, "QuestPoints", questpoints);
	SetObjProperty(objective, "QuestObjectiveType", objectivetype);
	PrintTextAbove(stone, "Objective added to quest");
	return 1;
endfunction


function RemoveObjective(queststone_serial, objective_serial)
	//Broadcast("stone serial " + queststone_serial);
	//Broadcast("objective_serial " + objective_serial);
	var objective := SystemFindObjectBySerial(objective_serial);
	var stone := SystemFindObjectBySerial(queststone_serial);
	var tempObj := array;
	var tempObjCompMessage := array;
	var tempObjCompStatus := array;
	var qObjectives:=GetObjProperty(stone, "Objectives");
	var qObjCompleteMessage := GetObjProperty(stone, "ObjCompletedMessages"); //parrallel array of objective completed strings
	var qObjCompleteStatus := GetObjProperty(stone, "ObjCompleteStatus"); 
	var i:=1;
	foreach q in qObjectives
		if (q!=objective_serial)
			tempObj.append(q);
			tempObjCompMessage.append(qObjCompleteMessage[i]);
			tempObjCompStatus.append(qObjCompleteStatus[i]);
		endif
		i:=i+1;
	endforeach

	EraseObjProperty(objective, "QuestPoints");
	EraseObjProperty(objective, "QuestObjectiveType");
	EraseObjProperty(objective, "QuestStoneSerial"); 
	SetObjProperty(stone, "Objectives", tempObj);
	SetObjProperty(stone, "ObjCompletedMessages", tempObjCompMessage);
	SetObjProperty(stone, "ObjCompleteStatus", tempObjCompStatus);
	//PrintTextAbove(stone, "Objective removed from quest");
	return 1;
endfunction

function RemovePersonalObjective(stone, objective_serial)
	//this only removes the personal objective from the stone
	//doesnt clean up data on players
	var objective := SystemFindObjectBySerial(objective_serial);
	var qObjectives:=GetObjProperty(stone, "PersonalObjectives");
	var qObjCompleteMessages:= GetObjProperty(stone, "PersonalObjMessages");
	var tempObj := array;
	var tempCompMessages := array;
	var i:=1;
		
	foreach q in qObjectives
		if (q!=objective_serial)
			tempObj.append(q);
			tempCompMessages.append(qObjCompleteMessages[i]);
		endif
		i:=i+1;
	endforeach
	EraseObjProperty(objective, "QuestPoints");
	EraseObjProperty(objective, "QuestObjectiveType");
	EraseObjProperty(objective, "QuestStoneSerial"); 
	SetObjProperty(stone, "PersonalObjectives", tempObj);
	SetObjProperty(stone, "PersonalObjMessages", tempCompMessages);
	
	PrintTextAbove(stone, "Objective removed from quest");
endfunction

function IsObjectiveRegistered(stone, objective_serial)
	//returns whether an objective serial is already listed in registered list
	//0 =not registered, >0 indicates index number
	var qObjectives:=GetObjProperty(stone, "Objectives");
	//Broadcast("objserial : " +objective_serial);
	objective_serial := CInt(objective_serial);
	var result :=0;
	var index:=1;
	foreach q in qObjectives
		q := CInt(q);
		//Broadcast("index : " + index + ": " + q);
		if (q==objective_serial)
			result:=index;
			break;
		endif
		index:=index+1;
	endforeach
	return result;
endfunction

function GetOneOffCompletedMessage(stone, objective_serial)
	var objIndex := IsObjectiveRegistered(stone, objective_serial);
	if (objIndex)
		var qObjCompleteMessages := GetObjProperty(stone, "ObjCompletedMessages");
		//Broadcast("index is: " + objIndex);
		//Broadcast("in get function: " + qObjCompleteMessages[objIndex]);
		return  qObjCompleteMessages[objIndex];
	endif
	return 0;
endfunction

function SetOneOffCompletedMessage(stone, objective_serial, completedMessage)
	var qObjectives:=GetObjProperty(stone, "Objectives");
	var result :=0;
	var index:=1;
	foreach q in qObjectives
		//q := CInt(q);
		//Broadcast("objective : " + index + ": " + q);
		if (q==objective_serial)
			result:=index;
			//Broadcast("found at : " + result);
			break;
		endif
		index:=index+1;
	endforeach
	var qObjCompleteMessages := GetObjProperty(stone, "ObjCompletedMessages");
	qObjCompleteMessages[result]:=completedMessage; 	//set to message
	SetObjProperty(stone, "ObjCompletedMessages", qObjCompleteMessages);
endfunction

function GetPersonalCompletedMessage(stone, objective_serial)

	var objIndex := IsPersonalObjectiveRegistered(stone, objective_serial);
	if (objIndex)
		var qObjCompleteMessages:= GetObjProperty(stone, "PersonalObjMessages");
		return  qObjCompleteMessages[objIndex];
	endif
	return 0;
endfunction

function SetPersonalCompletedMessage(stone, objective_serial, completedMessage)
	var objIndex := IsPersonalObjectiveRegistered(stone, objective_serial);
	if (objIndex)
		var qObjCompleteMessages:= GetObjProperty(stone, "PersonalObjMessages");
		qObjCompleteMessages[objIndex]:=completedMessage;
		SetObjProperty(stone, "PersonalObjMessages", qObjCompleteMessages);
	endif
endfunction


function IsPersonalObjectiveRegistered(stone, objective_serial)
	//returns whether a personal objective serial is already listed in personal registered list
	//0 =not registered, >0 indicates index number
	var qObjectives:=GetObjProperty(stone, "PersonalObjectives");
	//Broadcast("objserial : " +objective_serial);
	objective_serial := CInt(objective_serial);
	var result :=0;
	var index:=1;
	foreach q in qObjectives
		if (q==objective_serial)
			result:=index;
			break;
		endif
		index:=index+1;
	endforeach
	return result;
endfunction


function AddItemObjective(stone_serial, item, questpoints)
	//SetObjProperty(item, "QuestStoneSerial", stone_serial);
	AddNewObjective(stone_serial, item, "item", questpoints);
endfunction

function AddMonsterObjective(stone_serial, monster, questpoints)
	AddNewObjective(stone_serial,  monster, "monster", questpoints);
endfunction

function ReturnQuestItemObjective(stone_serial, player_serial, item_serial)
	//should do one-off quest items and personal obj type items
	var stone := SystemFindObjectBySerial(stone_serial);
	
	if (!IsRegistered(stone, player_serial))
		return 0;
	endif
	
	//completed players cant completed objectives
	if (IsCompleted(stone, player_serial))
			return 0;
	endif

	if (!IsQuestDeployed(stone))
		return 0; //if quest has not been deployed then no objectives can be completed.
	endif
	
	var item := SystemFindObjectBySerial(item_serial);
	var item_points := GetObjProperty(item, "QuestPoints");
	
	if (IsObjectiveRegistered(stone, item_serial))
		if (!IsObjectiveCompleted(stone, item_serial))
			SetObjectiveComplete(stone, item_serial);
			//BroadcastOneOffCompletedMessage(stone, item_serial);
			AwardQuestPoints(stone.serial, player_serial, item_points);
			return 1;
		endif
	endif
	return 0;
endfunction

function ChangeObjectiveQuestPoints(item, questpoints)
	SetObjProperty(item, "QuestPoints", questpoints);
endfunction


function ReturnQuestMonsterObjective(stone_serial, player_serial, monster_serial, spawn_serial, questpoints, dontComplete:=0)
//this is called when a monster is killed (death.src) from global scripts in /include/queststone.inc
	//Broadcast("stone_serial " + stone_serial + " player_serial " + player_serial + " monster_serial " + monster_serial + " questpoints " + questpoints);
	var stone := SystemFindObjectBySerial(stone_serial);
	
	if (!stone || stone==error)
		Broadcast("cant find stone error");
		return 0;
	endif
	
	if (!IsRegistered(stone, player_serial))
		//Broadcast("not reged");
		return 0;
	endif
	
	//completed players cant complete objectives
	if (IsCompleted(stone, player_serial))
			//Broadcast("completed quest");
			return 0;
	endif
	
	if (!IsQuestDeployed(stone))
	//	Broadcast("not deployed");
		return 0; //if quest has not been deployed then no objectives can be completed.
	endif
	
	if (!IsObjectiveRegistered(stone, monster_serial))
		//Broadcast("objective not registered");
		return;//objective not found
	endif
	if (!IsObjectiveCompleted(stone, monster_serial))
	//	Broadcast("objective not completed");
	//	Broadcast("stoneserial: " +stone.serial);
	//	Broadcast("objectivserial: " + monster_serial);
		if (! dontComplete)
			SetObjectiveComplete(stone, monster_serial);
		endif
		//BroadcastOneOffCompletedMessage(stone, monster_serial);
		AwardQuestPoints(stone_serial, player_serial, questpoints);
		return;
	endif
	Broadcast("erm?");
	Broadcast("stoneserial: " +stone.serial);
	Broadcast("objectivserial: " + monster_serial);
	Broadcast("stone.name: " + stone.name);

endfunction

function CreateOneOffLocationObjective(stone, loc, questpoints)
	var questlocation := CreateItemAtLocation( loc.x, loc.y, loc.z, "questlocationtile");
	var questtitle:=GetQuestTitle(stone.serial);
	if (questtitle)
		questlocation.name := "a quest location for " + questtitle;
	else
		questlocation.name := "a quest location";
	endif
	
	if (questlocation)
		AddNewObjective(stone.serial, questlocation, "OneOffLocation", questpoints);
	endif
endfunction

function CreatePersonalLocationObjective(stone, loc, questpoints)
	var questlocation := CreateItemAtLocation( loc.x, loc.y, loc.z, "questlocationtile");
	questlocation.name := "Location for " + GetQuestTitle(stone.serial);
	if (questlocation)
		AddNewPersonalObjective(stone.serial, questlocation, "PersonalLocation", questpoints);
	endif
endfunction


function ReturnQuestLocationObjective(stone_serial, player_serial, location_serial, questpoints, questobjtype)
	//this is called when a questlocationtile is walked on
	
	var stone := SystemFindObjectBySerial(stone_serial);
	
	//unregistered cant completed objectives
	if (!IsRegistered(stone, player_serial))
		return 0;
	endif
	
	//completed players cant completed objectives
	if (IsCompleted(stone, player_serial))
			return 0;
	endif
		
	if (!IsQuestDeployed(stone))
		return; //if quest has not been deployed then no objectives can be completed.
	endif
	
	if (questobjtype == "OneOffLocation")
		if (!IsObjectiveRegistered(stone, location_serial))
			return 0;//objective not found
		endif

		if (!IsObjectiveCompleted(stone, location_serial))
			SetObjectiveComplete(stone, location_serial);
			BroadcastOneOffCompletedMessage(stone, location_serial);
			AwardQuestPoints(stone_serial, player_serial, questpoints);
			return 1;
		endif
	elseif (questobjtype == "PersonalLocation")
		if (!IsPersonalObjectiveRegistered(stone, location_serial))
			return 0;//objective not registered
		endif
		var player := SystemFindObjectBySerial(player_serial, SYSFIND_SEARCH_OFFLINE_MOBILES);
		if (!IsPersonalObjectiveCompleted(player, location_serial, stone_serial))
			SetPersonalObjectiveComplete(player, location_serial, stone_serial);
			BroadcastPersonalCompletedMessage(stone, location_serial, player);	
			AwardQuestPoints(stone_serial, player_serial, questpoints);
			return 1;	
		endif
	endif
endfunction

function IsPersonalObjectiveCompleted(who, objective_serial, stone_serial)
//personal objectives data are stored on the player in prop qCompletedObjectives
	//Broadcast("IsPersonalObjectiveCompleted");
	var qObjectives:=GetObjProperty(who, "qPObjComp"+stone_serial);
	if (!qObjectives)
		return 0; //not complete because cant find any complete objectives
	endif
	var result:=0;
	var index:=1;
	foreach q in qObjectives
		if (q==objective_serial)
			result:=index;
			break;
		endif
		index:=index+1;
	endforeach
	//Broadcast("result " +result);	
	return result;
	
	//returns the index of the objective if in the completed list
	//else returns 0
endfunction



function IsObjectiveCompleted(stone, item_serial)
//this checks one-off objectives to see if they are complete
	item_serial:=CInt(item_serial);
	var qObjectives:=GetObjProperty(stone, "Objectives");
	//Broadcast(qObjectives);
	//Broadcast("item_serial : " +item_serial);
	//objective_serial := CInt(objective_serial);
	var result :=0;
	var index:=1;
	foreach q in qObjectives
		//q := CInt(q);
		//Broadcast("index : " + index + ": " + q);
		if (q==item_serial)
			result:=index;
			break;
		endif
		index:=index+1;
	endforeach
	var qObjComplete:=GetObjProperty(stone, "ObjCompleteStatus");
	//Broadcast(	qObjComplete[index] );
	return qObjComplete[index];
endfunction

function SetPersonalObjectiveComplete(player, location_serial, stone_serial)
//adds completed objective to array on player
	var qObjectives:=GetObjProperty(player, "qPObjComp"+stone_serial);
	if (!qObjectives[1])
		SetObjProperty(player, "qPObjComp"+stone_serial,{location_serial});
		return;
	endif
	qObjectives.append(location_serial);
	SetObjProperty(player, "qPObjComp"+stone_serial,qObjectives);
endfunction

function SetObjectiveComplete(stone, item_serial)
	var qObjectives:=GetObjProperty(stone, "Objectives");
	//Broadcast("objserial : " +objective_serial);
	//objective_serial := CInt(objective_serial);
	var result :=0;
	var index:=1;
	foreach q in qObjectives
		//q := CInt(q);
		//Broadcast("objective : " + index + ": " + q);
		if (q==item_serial)
			result:=index;
			//Broadcast("found at : " + result);
			break;
		endif
		index:=index+1;
	endforeach
	var qObjComplete:=GetObjProperty(stone, "ObjCompleteStatus");	
	qObjComplete[result]:=1; 	//set to complete
	SetObjProperty(stone, "ObjCompleteStatus", qObjComplete);
endfunction

function ListObjectives(stone)
	//LIST OF ONE OFF OBJECTIVES.. returns the objective object
	var qObjectives:=GetObjProperty(stone, "Objectives");
	var qObjectiveObjs := array;

	foreach q in qObjectives
		qObjectiveObjs.append(SystemFindObjectBySerial(q));
	endforeach
	return qObjectiveObjs;

endfunction

function ListPersonalObjectives(stone)
	//LIST OF PERSONAL OBJECTIVES.. returns the objective object
	var qObjectives:=GetObjProperty(stone, "PersonalObjectives");
	var qObjectiveObjs := array;

	foreach q in qObjectives
		qObjectiveObjs.append(SystemFindObjectBySerial(q));
	endforeach
	return qObjectiveObjs;

endfunction



//Simple Getters
function GetQuestTitle(stone_serial)
	var stone := SystemFindObjectBySerial(stone_serial);
	var title:= GetObjProperty(stone, "title");
	if (!title || title==error)
		return "To be named";
	endif
	return title;
endfunction

function GetIntro(stone_serial)
	var stone := SystemFindObjectBySerial(stone_serial);
	return (GetObjProperty(stone, "intro"));
endfunction

function GetRegisteredPlayers(stone_serial, start, returnSize)
	//simply returns a small number of names of registered players

	var stone := SystemFindObjectBySerial(stone_serial);
	var playerserials:=GetObjProperty(stone, "playerserials");
	var players:=array;
	var player;
	var i;
	var maxPlayers:=playerserials.size();
	
	if (!playerserials[1])
		return;
	endif
		
	for(i:=start;i<=maxPlayers&&players.size()<returnSize;i:=i+1)
		player:=SystemFindObjectBySerial(CInt(playerserials[i]), SYSFIND_SEARCH_OFFLINE_MOBILES);
		players.append(player);
	endfor
	return players;
endfunction

function IsBanned(stone, player_serial)
	var qBannedPlayers:=GetObjProperty(stone, "BannedPlayers");
	//Broadcast("objserial : " +objective_serial);
	var result :=0;
	var index:=1;
	foreach q in qBannedPlayers
		if (q==player_serial)
			result:=index;
			break;
		endif
		index:=index+1;
	endforeach
	return result;
endfunction

function ListBannedPlayerNames(stone)
//simply returns names of banned players
	var qBannedPlayers:=GetObjProperty(stone, "BannedPlayers");
	var playernames:=array;
	var player;
	foreach p in qBannedPlayers
		if (IsBanned(stone, p))
			player:=SystemFindObjectBySerial(p, SYSFIND_SEARCH_OFFLINE_MOBILES);
			playernames.append(player.name);
		endif
	endforeach
	return playernames;
endfunction

function ListCompletedPlayerNames(stone)
//simply returns names of completed players
	var qCompletedPlayers:=GetObjProperty(stone, "CompletedPlayers");
	var playernames:=array;
	var player;
	foreach p in qCompletedPlayers
			player:=SystemFindObjectBySerial(p, SYSFIND_SEARCH_OFFLINE_MOBILES);
			playernames.append(player.name);
	endforeach
	return playernames;
endfunction

function GetOneOffObjectives(stone_serial, start, returnSize)
		//simply returns a small number of objectives serials

	var stone := SystemFindObjectBySerial(stone_serial);
	var qObjectiveSerials:=GetObjProperty(stone, "Objectives");
	var tempObjectiveserials := array;
	var tempObj;
	var i;
	var maxObjs:=qObjectiveSerials.size();
	
	if (!qObjectiveSerials[1])
		return;
	endif
		
	for(i:=start;i<=maxObjs&&tempObjectiveserials.size()<returnSize;i:=i+1)
		//tempObj:=SystemFindObjectBySerial(CInt(qObjectiveSerials[i]));
		//tempObjectiveObjs.append(tempObj);
		tempObjectiveserials.append(qObjectiveSerials[i]);
	endfor
	return tempObjectiveserials;
endfunction

function GetPersonalObjectives(stone_serial, start, returnSize)
	//simply returns a small number of objectives serials

	var stone := SystemFindObjectBySerial(stone_serial);
	var qObjectiveSerials:=GetObjProperty(stone, "PersonalObjectives");
	var tempObjectivesers := array;
	var i;
	var maxSers:=qObjectiveSerials.size();
	
	if (!qObjectiveSerials[1])
		return;
	endif
		
	for(i:=start;i<=maxSers&&tempObjectivesers.size()<returnSize;i:=i+1)
		tempObjectivesers.append(qObjectiveSerials[i]);
	endfor
	return tempObjectivesers;

endfunction

function GetSpawns(stone_serial, start, returnSize)
	//simply returns a small number of the total number of quest spawns referred to

	var stone := SystemFindObjectBySerial(stone_serial);
	var qSpawnRefs:=GetObjProperty(stone, "SpawnPoints");
	var tempSpawns := array;
	var temp;
	var i;
	var maxQS:=qSpawnRefs.size();
	
	if (!qSpawnRefs[1])
		return;
	endif
		
	for(i:=start;i<=maxQS&&tempSpawns.size()<returnSize;i:=i+1)
		temp:=SystemFindObjectBySerial(CInt(qSpawnRefs[i]));
		if( !temp.serial)
			RemoveDeadSpawn(CInt(qSpawnRefs[i]), stone_serial);
		endif
		tempSpawns.append(temp);
	endfor
	return tempSpawns;

endfunction

function GetStoneStorageArea(stone)
	var storage:= FindStorageArea("Merchant Storage");

	var newpack:=find_or_create_item(storage, "temp "+stone.serial,0xe75);

	if (!storage)
	//	SendSysMessage(who,"unable to find merchant storage");
		return;
	endif

	if (!newpack)
	//	SendSysMessage(who,"unable to create temporary backpack");
		return;
	endif
	return newpack;
endfunction

function GetNumberOfQuestRewardItems(stone)
	var backpackitems:=EnumerateItemsInContainer(GetStoneStorageArea(stone));
	var counter:=0;
	foreach item in backpackitems
		if (GetObjProperty(item, "QuestBuyValue"))
			counter:=counter+1;
		endif
	endforeach
	return counter;
endfunction

function DestroyStorageArea(stone)
	var backpackitems:=EnumerateItemsInContainer(GetStoneStorageArea(stone));
	foreach item in backpackitems
		DestroyItem(item);
	endforeach
	
	var storage:= FindStorageArea("Merchant Storage");
	DestroyRootItemInStorageArea(storage, "temp " + stone.serial);
endfunction

function OpenStoneStorageArea(stone, who)
	var stonepack:=GetStoneStorageArea(stone);
	SendOpenSpecialContainer(who, stonepack);
endfunction

function addQuestRewardItem(item,stone,questbuyvalue)
	SetObjProperty(item, "QuestBuyValue",questbuyvalue);
	SetObjProperty(item, "QuestStoneSerial", stone.serial);
	SetObjProperty(item, "QuestNote", "Reward from " + GetQuestTitle(stone.serial));
	MoveItemToContainer( item, GetStoneStorageArea(stone));
	item.movable:=0;
endfunction

function removeQuestRewardItem(item,stone,who)
	EraseObjProperty(item, "QuestBuyValue");
	EraseObjProperty(item, "QuestStoneSerial");
	EraseObjProperty(item, "QuestNote");
	EraseObjProperty(item, "NumberOfTimesRewarded");
	item.movable:=1;
	MoveItemToContainer( item, who.backpack);
endfunction

function changeQuestRewardItemBuyValue(item, questbuyvalue)
	SetObjProperty(item, "QuestBuyValue",questbuyvalue);
endfunction

function BuyQuestRewardItem(item,stone,who)
//function for player to 'buy' a reward... require >= player quest points
//creates a copy of the item they want in bankbox
	var requiredQuestPoints := GetObjProperty(item, "QuestBuyValue");
	var playerQuestPoints := GetPlayersQuestPoints(stone.serial, who.serial);
	if (!requiredQuestPoints)
		return 0;
	endif
	if (playerQuestPoints>=requiredQuestPoints)
		item.movable:=1;
		var bankbox := FindBankBox( who );
		if( bankbox )
			AwardQuestPoints(stone.serial, who.serial, -requiredQuestPoints);
			IncrementNumberTimesRewarded(item);
			var dupe := CreateItemInContainer( bankbox, item.objtype, item.amount );
			DupeItem( item, dupe );
			if( item.usescript )
				dupe.usescript := item.usescript;
			endif
			SetObjProperty(dupe,"QuestNote", "given to " + who.serial + " - " + who.name + "from queststone " + stone.serial);
			EraseObjProperty(dupe, "NumberOfTimesRewarded");
			//MoveItemToContainer( dupe, bankbox);
			return 1;
		else
			SendSysMessage(who, "Cant find bankbox :(");
			return 0;
		endif
	endif
	return 0;
endfunction

function QuestBroadcast(stone, sending, receiving, txtcolor, message)
	
case(receiving)
		1:
			Broadcast( cstr(sending) + ": " + cstr(message), 3 , txtcolor);
		3:
			var pserials:=GetObjProperty(stone, "playerserials");
			foreach p in pserials
				var player := SystemFindObjectBySerial(p);
				if (player)
					SendSysMessage(player, cstr(sending) + ": " + cstr(message), 3 , txtcolor);
				endif
			endforeach
			//and send to queststone owner
			var owner := SystemFindObjectBySerial(GetCreator(stone));
			SendSysMessage(owner, cstr(sending) + ": " + cstr(message),3, txtcolor);
		2:
			foreach p in EnumerateOnlineCharacters()
				if (p.cmdlevel >= 1)
					SendSysMessage(p, cstr(sending) + ": " + cstr(message), 3 , txtcolor);
				endif
			endforeach
			return ;
Endcase
	//and send to queststone owner
	var owner := SystemFindObjectBySerial(GetCreator(stone));
	SendSysMessage(owner, message,3, 53);
endfunction


function BroadcastOneOffCompletedMessage(stone, objective_serial)
	var message:= GetOneOffCompletedMessage(stone, objective_serial);
	if (!message||message=="")
		return;
	endif
	QuestBroadcast(stone, , , , message);
endfunction

function BroadcastPersonalCompletedMessage(stone, objective_serial, who)
	var message:=GetPersonalCompletedMessage(stone, objective_serial);
	if (!message||message=="")
		return;
	endif
	SendSysMessage(who, message,3, 53);
endfunction

function IsRegistrationOpen(queststone)
	//returns 1 if players are allowed to register yet
	return (GetObjProperty(queststone, "RegistrationOpen")==1);
endfunction

function IsQuestDeployed(queststone)
var dep := cint(GetObjProperty(queststone, "QuestDeployed"));
//broadcast(dep);
	//returns 1 if questpoints should be awarded and objectives set to completed...
	return dep;
endfunction

function GetRewardQuestItems(queststone)
	//returns 1 if items can be bought from queststone by all players
	//returns 2 if items can only be bought by players in 'completed quest' list
	return GetObjProperty(queststone, "RewardQuestItems");
endfunction

function SetRegistrationOpen(queststone, val)
	SetObjProperty(queststone, "RegistrationOpen", CInt(val));
endfunction

function SetQuestDeployed(queststone, val)
	SetObjProperty(queststone, "QuestDeployed", CInt(val));
endfunction

function SetRewardQuestItems(queststone, val)
//These constants are used:
//REWARDQUESTITEMS_NO :=0; //noone can buy quest items yet
//REWARDQUESTITEMS_ALL :=1; // all players can buy quest items with quest points
//REWARDQUESTITEMS_COMPLETED :=2; //completed players only can buy quest items with quest points
	
	SetObjProperty(queststone, "RewardQuestItems", CInt(val));
endfunction

function MakeBookContentsArray(contentsString)
	//Broadcast(contentsString);
	//converts a single string into an array of string with max individual characters !> 22
	if (!contentsString)
		return {""};
	endif

	var wordsArray:= SplitWords(contentsString);//now broken into an array of words
	//Broadcast("wordsArray: ", wordsArray);
	var tempContentArray:=array;
	
	//var i:=1;
	var tempString:="";
	
	foreach word in wordsArray
		//Broadcast("word: " + word + "   size: " + len(word));
		if (len(tempString)+len(word)<30)
			tempString := tempString + " " + word;
			//Broadcast(tempString);
		else
			tempContentArray.append(tempString);
			//Broadcast("tempString: " + tempString);
			tempString:=word;
			//i:=i+1; //next line of content
		endif		
	endforeach
	
	//last word
	tempContentArray.append(tempString);
		
	return tempContentArray;
endfunction

//finalisation options
function GetFinaliseOption(queststone)
// FINALOPTION_NONE
// FINALOPTION_PERSCOMPLNALLOBJS
// FINALOPTION_PERSCOMPLN1OBJ
// FINALOPTION_PERSMAXQP
// FINALOPTION_ALLCOMPLNALLOBJS
// FINALOPTION_ALLCOMPLN1OBJ
// FINALOPTION_ALLMAXQP
	return GetObjProperty(queststone, "FinaliseOption");
endfunction

function SetFinaliseOption(queststone, val)
//uses these constants
// FINALOPTION_NONE
// FINALOPTION_PERSCOMPLNALLOBJS
// FINALOPTION_PERSCOMPLN1OBJ
// FINALOPTION_PERSMAXQP
// FINALOPTION_ALLCOMPLNALLOBJS
// FINALOPTION_ALLCOMPLN1OBJ
// FINALOPTION_ALLMAXQP

	SetObjProperty(queststone, "FinaliseOption", CInt(val));
endfunction

function GetFinalMaxQP(queststone)
	return GetObjProperty(queststone, "FinalMaxQP");
endfunction

function SetFinalMaxQP(queststone, val)
	//only used when quest finalisation based on questpoints
	//FINALOPTION_PERSMAXQP or FINALOPTION_MAXQP
	SetObjProperty(queststone, "FinalMaxQP", CInt(val));
endfunction

function GetFinalObjectiveSerial(queststone)
	return GetObjProperty(queststone, "FinaliseObjectiveSerial");
endfunction

function SetFinalObjectiveSerial(queststone, val)
	//only used when there is a single final objective chosen e.g. for options
	//FINALOPTION_PERSCOMPLN1OBJ and FINALOPTION_COMPLN1OBJ
	SetObjProperty(queststone, "FinaliseObjectiveSerial", CInt(val));
endfunction

function GetFinalOptionString(stone)
	var foption:=GetFinaliseOption(stone);
	case (foption)
		FINALOPTION_PERSCOMPLNALLOBJS:
			return "Personal completion when all personal objectives met";
		break;
		FINALOPTION_PERSCOMPLN1OBJ:
			return "Personal completion when a single personal objective met";
		break;
		FINALOPTION_PERSMAXQP:
			return "Personal completion when a quest point limit met = " + GetFinalMaxQP(stone);
		break;
		FINALOPTION_ALLCOMPLNALLOBJS:
			return "Completion for all when all one-off objectives met";
		break;
		FINALOPTION_ALLCOMPLN1OBJ:
			return "Completion for all when a single one-off objective met";
		break;
		FINALOPTION_ALLMAXQP:
			return "Completion for all when a quest point limit met by one player";
		break;
		default:
			return "Dont autofinalise... Ill stop the quest manually";
	endcase	
endfunction

function CheckFinaliseQuest(queststone, player)
// FINALOPTION_PERSCOMPLNALLOBJS //met when all personal objectives were completed
// FINALOPTION_PERSCOMPLN1OBJ // if a single personal objective completed
// FINALOPTION_PERSMAXQP //if player has greater than quest point max
// FINALOPTION_ALLCOMPLNALLOBJS // if all one off objectives were completed
// FINALOPTION_ALLCOMPLN1OBJ // if a single one off objective completed
// FINALOPTION_ALLMAXQP //if player has greater than quest point max then all players complete

//for PERSONAL FINALISE OPTIONS -- single player finalises...
												//player is added to completed list
												
//for ALL FINALISE OPTIONS -- 		//all players are added to completed list
												//registration closed
	if (IsCompleted(queststone, player.serial))
		return; //if player has already completed then dont check that finalise conditions met
	endif
	var finaliseoption := GetFinaliseOption(queststone);
	
	case (finaliseoption)
		FINALOPTION_PERSCOMPLNALLOBJS:
			if (IsAllPersonalObjectivesComplete(queststone, player))
				DoPrivateFinal(queststone, player);
			endif
		break;
		FINALOPTION_PERSCOMPLN1OBJ:
			if(IsFinalPersonalObjectiveComplete(queststone, player))
				DoPrivateFinal(queststone, player);
			endif
		break;
		FINALOPTION_PERSMAXQP:
			if (IsOverQPMax(queststone, player.serial))
				DoPrivateFinal(queststone, player);
			endif
		break;
		FINALOPTION_ALLCOMPLNALLOBJS:
			if (IsAllOneOffObjectivesComplete(queststone))
				DoPublicFinal(queststone, player);
			endif
		break;
		FINALOPTION_ALLCOMPLN1OBJ:
			if(IsFinalObjectiveComplete(queststone))
				DoPublicFinal(queststone, player);
			endif
		break;
		FINALOPTION_ALLMAXQP:
			if (IsOverQPMax(queststone, player.serial))
				DoPublicFinal(queststone, player);
			endif
		break;
		default:
		//no finaliseoption set
			return 0;
	endcase

	
endfunction

function DoPrivateFinal(stone, player)
	AddCompletedPlayer(stone, player.serial);
	SendSysMessage(player, "Well done you have completed the quest!",3 ,53);
	SendFinalPersonalMessage(stone, player);
	PlaySoundEffect( player, 0x003e );

	var tox := GetObjProperty (stone, "tox");
    	var toy := GetObjProperty (stone, "toy");
    	var toz := GetObjProperty (stone, "toz");
if( tox && toy && toz)
	MoveObjectToLocation( player, tox, toy, toz, _DEFAULT_REALM); //AutoConversion
endif
	
endfunction

function DoPublicFinal(stone, player)
	QuestBroadcast(stone, , , , "The quest has been completed. The final objective was achieved by " + player.name);	
	QuestBroadcast(stone, , , , "The quest and all protections will officially end in 2 minutes");
	sleep(60);
	QuestBroadcast(stone, , , , "The quest and all protections will officially end in 1 minute");
	sleep(60);
	CompleteAllPlayers(stone);
	SetRegistrationOpen(stone, 0);
	QuestBroadcast(stone, , , , "The quest has been completed. The final objective was achieved by " + player.name);
	SendFinalPublicMessage(stone);
	SendSoundEffectAll(stone, 0x003e);
endfunction

function SendSoundEffectAll(stone, sound)
	var pserials:=GetObjProperty(stone, "playerserials");
	foreach p in pserials
		var player := SystemFindObjectBySerial(p);
		if (player)
			PlaySoundEffect( player, sound );
		endif
	endforeach
endfunction

function SendFinalPersonalMessage(stone, player)
	sleep(5); //a bit of delay so that objective met message and final message are separate
	var finalmessage := GetQuestFinal(stone);
	if (finalmessage)
		SendSysMessage(player, finalmessage, 3, 53);
	endif
endfunction

function SendFinalPublicMessage(stone)
	sleep(5); //a bit of delay so that objective met message and final message are separate
	var finalmessage := GetQuestFinal(stone);
	if (finalmessage)
		QuestBroadcast(stone, , , , finalmessage);
	endif
endfunction


function CompleteAllPlayers(queststone)
	//sets all players to completed
	var pserials:=GetObjProperty(queststone, "playerserials");
	foreach p in pserials
		AddCompletedPlayer(queststone, p);
	endforeach
endfunction

function IsOverQPMax(queststone, playerserial)
	var qpmax:= GetFinalMaxQP(queststone);
	var playerqp := GetPlayersQuestPoints(queststone.serial, playerserial);
	if (playerqp>=qpmax)
		return 1;
	endif
	return 0;
endfunction

function IsAllOneOffObjectivesComplete(queststone)
	var objectives:=GetObjProperty(queststone, "Objectives");
	foreach obj in objectives
		if (!IsObjectiveCompleted(queststone, obj))
			return 0;
		endif			
	endforeach
	return 1;
endfunction

function IsAllPersonalObjectivesComplete(queststone, player)
	var persObjectives:=GetObjProperty(queststone, "PersonalObjectives");
	foreach obj in persObjectives
		if (!IsPersonalObjectiveCompleted(player, obj, queststone.serial))
			return 0;
		endif			
	endforeach
	return 1;
endfunction

function IsFinalObjectiveComplete(queststone)
//used to check whether a queststones final one off  has been completed yet
// for finalise option FINALOPTION_ALLCOMPLN1OBJ
	var final_serial := GetFinalObjectiveSerial(queststone);
	return IsObjectiveCompleted(queststone, final_serial);
endfunction

function IsFinalPersonalObjectiveComplete(queststone, player)
//used to check whether a queststones personal objective has been completed yet
// for finalise option FINALOPTION_PERSCOMPLN1OBJ
	var final_serial := GetFinalObjectiveSerial(queststone);
	//Broadcast ("final_serial: " + final_serial);
	//var res := IsPersonalObjectiveCompleted(player, final_serial);
	//Broadcast ("completed yet?: " + res);
	return IsPersonalObjectiveCompleted(player, final_serial, queststone.serial);
endfunction

//completed functions
function IsCompleted(stone, playerserial)
	//returns whether a player serial is already listed in completed list
	//0 =not completed, >0 indicates index number
	var qCompletedPlayers:=GetObjProperty(stone, "CompletedPlayers");
	var result :=0;
	var index:=1;
	foreach p in qCompletedPlayers
		if (p==playerserial)
			result:=index;
			break;
		endif
		index:=index+1;
	endforeach
	return result;
endfunction

function AddCompletedPlayer(stone, player_serial)
	//adds a player to the banned list
	var qCompletedPlayers:=GetObjProperty(stone, "CompletedPlayers"); //list of banned player serials

	if (!qCompletedPlayers[1])
		//Broadcast("nothing in pserial[1]"); 
		SetObjProperty(stone, "CompletedPlayers",{player_serial});
		return;
	endif
	
	qCompletedPlayers.append(player_serial);
	var player:=SystemFindObjectBySerial(player_serial);
	SetObjProperty(stone, "CompletedPlayers",qCompletedPlayers);
	return 1;
endfunction

function RemoveCompletedPlayer(stone, player_serial)
	//remove a player from the completed list
	var qCompletedPlayers:=GetObjProperty(stone, "CompletedPlayers"); //list of completed player serials
	var tempCompleted := array;
	var result :=0;
	foreach q in qCompletedPlayers
		if (q!=player_serial)
			tempCompleted.append(q);
		else
			result:=1; //found completed player
		endif
	endforeach
	
	SetObjProperty(stone, "CompletedPlayers",tempCompleted);
	return result;
endfunction

//Registration Condition functions
function GetRegistrationConditionString(stone)
	//returns string corresponding to Registration Condition constant
	var tempstr;
	var regcondition := GetRegistrationCondition(stone);
	case (regcondition)
		REGCONDITION_NEWBIE:
			return "Newbie";
		break;
		REGCONDITION_BARD:
			tempstr:= "Bard";
		break;
		REGCONDITION_CRAFTER:
			tempstr:= "Crafter";
		break;
		REGCONDITION_MAGE:
			tempstr:= "Mage";
		break;
		REGCONDITION_RANGER:
			tempstr:= "Ranger";
		break;
		REGCONDITION_THIEF:
			tempstr:= "Thief";
		break;
		REGCONDITION_WARRIOR:
			tempstr:= "Warrior";
		break;
		REGCONDITION_RACE:
			return GetObjProperty(stone, "RegistrationRace");
		break;
		REGCONDITION_POWERPLAYER:
			tempstr:= "Power Player";
		break;
		REGCONDITION_LEVELONLY:
			tempstr:= "any class";
		break;
		default:
			return "No registration conditions set";
	endcase
	return tempstr + " level " + GetRegistrationConditionLevel(stone);
endfunction

function GetRegistrationCondition(stone)
//returns int corresponding to registration condition constant corresponding to Registration Condition constant
	return GetObjProperty(stone, "RegistrationCondition");
endfunction

function SetRegistrationCondition(stone, val)
//Sets the stones registration condition (as constant)
	SetObjProperty(stone, "RegistrationCondition", val);
endfunction

function GetRegistrationConditionLevel(stone)
//returns int corresponding to registration condition level
//used when registration condition is a class to specift level...
	return CInt(GetObjProperty(stone, "RegistrationConditionLevel"));
endfunction

function SetRegistrationConditionLevel(stone, val)
//sets registration condition level (e.g. if reg condition is Mage this might be 4 .. corresponding to level 4)
//used when registration condition is a class to specift level...
	SetObjProperty(stone, "RegistrationConditionLevel", val);
endfunction

function CheckRegistrationCondition(who, stone)
	//returns 1 if player can register 0 otherwise

	var regcondition := GetRegistrationCondition(stone);
	var requiredLevel := GetRegistrationConditionLevel(stone);
	//Broadcast("regcondition " + regcondition + "     requiredLevel " +  requiredLevel);
	var charLevel;
	case (regcondition)
		REGCONDITION_NEWBIE:
			if (GetObjProperty(who, "newbie")==1)
				return 1;	
			else
				return 0;
			endif
		break;
		REGCONDITION_BARD:
			charLevel := GetObjProperty( who, "IsBard" );
			if (charLevel>=requiredLevel)
				return 1;
			endif
			return 0;
		break;
		REGCONDITION_CRAFTER:
			charLevel := GetObjProperty( who, "IsCrafter" );
			if (charLevel>=requiredLevel)
				return 1;
			endif
			return 0;	
		break;
		REGCONDITION_MAGE:
			charLevel := GetObjProperty( who, "IsMage" );
			if (charLevel>=requiredLevel)
				return 1;
			endif
			return 0;
		break;
		REGCONDITION_RANGER:
			charLevel := GetObjProperty( who, "IsRanger" );
			if (charLevel>=requiredLevel)
				return 1;
			endif
			return 0;		
		break;
		REGCONDITION_THIEF:
			charLevel := GetObjProperty( who, "IsThief");
			if (charLevel>=requiredLevel)
				return 1;
			endif
			return 0;	
		break;
		REGCONDITION_WARRIOR:
			charLevel := GetObjProperty( who, "IsWarrior" );
			if (charLevel>=requiredLevel)
				return 1;
			endif
			return 0;		
		break;
		REGCONDITION_POWERPLAYER:
			charLevel := GetObjProperty( who, "IsPowerPlayer" );
			if (charLevel>=requiredLevel)
				return 1;
			endif
			return 0;		
			break;
		REGCONDITION_RACE:
			return (CheckRaceRegistrationCondition(stone, who));
		break;
		REGCONDITION_LEVELONLY:
			charLevel := CInt(FindAnyClassLevel(who));
			//Broadcast("charLevel: " + charLevel);
			//Broadcast("requiredLevel: " + requiredLevel);

//			Broadcast("" + (charLevel>=requiredLevel));
			if (charLevel>=requiredLevel)
				return 1;
			endif
			return 0;
		break;	
		default:
			return 1; //none set
	endcase

endfunction

function CheckRaceRegistrationCondition(stone, who)
//returns 1 if player the right race for the registration condition
	var stonerace := GetObjProperty(stone, "RegistrationRace");
	if (who.title_race==stonerace)
		return 1;
	endif
	return 0;	
endfunction

function SetRaceRegistrationCondition(stone, race)
	//races allowed -- human, Elf, Dark-Elf, Goblin, Barbarian, Dwarf
	SetObjProperty(stone, "RegistrationRace", race);
endfunction

//stone user sharing functions
function CheckCurrentUser(stone, who)
	//returns true if who is the current user
	if (who.serial == GetObjProperty(stone, "currentuser"))
		return 1;
	endif
	return 0;
endfunction

function GetCurrentUser(stone)
	return GetObjProperty(stone, "currentuser");
endfunction

function SetCurrentUser(stone, who)
	//sets user and time clicked
	SetObjProperty(stone, "#lastclicked", CInt(ReadGameClock())+60);
	SetObjProperty(stone, "currentuser", who.serial);
endfunction

function ClearCurrentUser(stone, who)
	//clears props - used when current user stops using stone
	EraseObjProperty(stone, "#lastclicked");
	EraseObjProperty(stone, "currentuser");
endfunction

function CheckCurrentUserTimeElapsed(stone, who)
	//returns true if time elapsed
	var clickMaxTime := CInt(GetObjProperty(stone, "#lastclicked"));
	if (clickMaxTime<ReadGameClock()) //that is current time is greater than reserved time
		return 1;
	endif
	return 0;
endfunction

//personal object type objectives
function CreatePersonalObjTypeObjective(stone, objtype, objdesc, questpoints)
	//check whether obj type already in objtypedata array of structs
	//if so return
	//otherwise add to that and add new personal objective
	AddNewPersonalObjectTypeObjective(stone, objtype, objdesc, questpoints);
endfunction

function AddNewPersonalObjectTypeObjective(stone, object_type, objdesc, questpoints)
//personal objectives are stored in a separate array because one-off objectives completion status is stored
//in a parrallel array, while personal objective completion status is stored on the player
//this is used for adding a new object type objective cos data cant be stored on objective

	var qObjectives:=GetObjProperty(stone, "PersonalObjectives");
	var qObjCompleteMessages:= GetObjProperty(stone, "PersonalObjMessages"); //parrallel array of objective completed strings

	if (!qObjectives[1])
		SetObjProperty(stone, "PersonalObjectives",{object_type});
		SetObjProperty(stone, "PersonalObjMessages", {""});
		AddNewObjectTypeData(stone, object_type, objdesc, questpoints);
		PrintTextAbove(stone, "Objective added to quest");
		return;
	endif
	
	if (IsPersonalObjectiveRegistered(stone, object_type))
		PrintTextAbove (stone, "That objective has already been registered.");
		return 0;
	endif
	
	qObjectives.append(object_type);
	qObjCompleteMessages.append(""); //place holder message string
	AddNewObjectTypeData(stone, object_type, objdesc, questpoints);
	SetObjProperty(stone, "PersonalObjectives",qObjectives);
	SetObjProperty(stone, "PersonalObjMessages", qObjCompleteMessages);
	PrintTextAbove(stone, "Objective added to quest");
	return 1;
endfunction

function RemovePersonalObjectTypeObjective(stone, object_type)
	//this only removes the personal objective from the stone
	//doesnt clean up data on players
	var qObjectives:=GetObjProperty(stone, "PersonalObjectives");
	var qObjCompleteMessages:= GetObjProperty(stone, "PersonalObjMessages");
	var tempObj := array;
	var tempCompMessages := array;
	var i:=1;
		
	foreach q in qObjectives
		if (q!=object_type)
			tempObj.append(q);
			tempCompMessages.append(qObjCompleteMessages[i]);
		endif
		i:=i+1;
	endforeach
	SetObjProperty(stone, "PersonalObjectives", tempObj);
	SetObjProperty(stone, "PersonalObjMessages", tempCompMessages);
	RemoveObjectTypeData(stone, object_type);
	PrintTextAbove(stone, "Objective removed from quest");
endfunction

function AddNewObjectTypeData(stone, obj_type, obj_type_name, questpoints, specificname:="", spawnorigin:=0, amount:=1)
//unlike other objectives quest specific data cannot be stored on the item for object_types... instead it is stored
//in an array of stucts with this structure...
	var tempRecord:=CreateObjTypeStruct(obj_type, questpoints, obj_type_name, specificname, spawnorigin, amount);
	//Broadcast("tempRecord: " + tempRecord);
	var objectTypeData:=GetObjProperty(stone, "ObjectTypeData");
	if (!objectTypeData[1])
		SetObjProperty(stone, "ObjectTypeData",{tempRecord});
		return;
	endif
	objectTypeData.append(tempRecord);
	SetObjProperty(stone, "ObjectTypeData",objectTypeData);
endfunction

function CreateObjTypeStruct(obj_type, questpoints, obj_type_name, specificname, spawnorigin, amount)
	//given required parameters this function returns an objtype struct
	var tempRecord:=struct;
	tempRecord.+objtype:=obj_type;
	tempRecord.+questpoints:=questpoints;
	tempRecord.+objtypename:=obj_type_name; //this is default name of object so i dont have to look it up in cfg file
	tempRecord.+specificname:=specificname; //this is for when the gm wants to specify a specific object name
	tempRecord.+spawnorigin:=spawnorigin; //for when the gm wants to specify from which spawnpoint the item must come from
	tempRecord.+amount:=amount;
	return tempRecord;
endfunction

function GetObjTypeData(stone, obj_type)
	//Broadcast("GET TYPE DATA - objtype " + obj_type);
	//nst numElemsInData:=6;
	//returns a struct of objtype data stored in array
	var qObjectTypeData:=GetObjProperty(stone, "ObjectTypeData");
	if (!qObjectTypeData)
		return 0; //error no data
	endif
	
	var i;	
	for (i:=1; i<=len(qObjectTypeData); i:=i+1)
		if (qObjectTypeData[i].objtype==obj_type)
			return qObjectTypeData[i];
		endif
	endfor
	return 0; //not found
endfunction

function GetObjTypeIndex(stone, obj_type)
	//Broadcast("GET TYPE DATA - objtype " + obj_type);
	//nst numElemsInData:=6;
	//returns a struct of objtype data stored in array
	var qObjectTypeData:=GetObjProperty(stone, "ObjectTypeData");
	if (!qObjectTypeData)
		return -1; //error no data
	endif
	
	var i;	
	for (i:=1; i<=len(qObjectTypeData); i:=i+1)
		if (qObjectTypeData[i].objtype==obj_type)
			return i;
		endif
	endfor
	return 0; //not found
endfunction

function SetObjTypeData(stone, objtypestruct)
	//used to update an existing ObjTypeData record with this new struct
	//struct obj_type is used to look up existing struct to replace (elem[1])
	var qObjectTypeData:=GetObjProperty(stone, "ObjectTypeData");
	if (!qObjectTypeData)
		return -1; //error no data
	endif
	
	var index:=  GetObjTypeIndex(stone, objtypestruct.objtype);
	if (index<=0)
		return; //not found
	endif
	//update
	qObjectTypeData[index]:=objtypestruct;
	//save over existing prop
	SetObjProperty(stone, "ObjectTypeData",qObjectTypeData);
endfunction



function RemoveObjectTypeData(stone, obj_type)
	var qObjectTypeData:=GetObjProperty(stone, "ObjectTypeData");
	var tempArray := array;
	foreach q in qObjectTypeData
		//Broadcast("looking at: " + qObjectTypeData[i][1]);
		if (q.objtype!=obj_type)
			tempArray.append(q);
		endif
	endforeach
	SetObjProperty(stone, "ObjectTypeData",tempArray);	
endfunction

function ReturnQuestObjectTypeObjective(stone_serial, player, item)
	//should do one-off quest items and personal obj type items
	var stone := SystemFindObjectBySerial(stone_serial);
	
	if (!IsRegistered(stone, player.serial))
		return 0;
	endif
	
	//completed players cant completed objectives
	if (IsCompleted(stone, player.serial))
			return 0;
	endif

	if (!IsQuestDeployed(stone))
		return 0; //if quest has not been deployed then no objectives can be completed.
	endif
	
	var objtype := item.objtype;
	
	//get stored data
	
	if (IsPersonalObjectiveRegistered(stone, objtype))
		if (!IsPersonalObjectiveCompleted(player, objtype, stone.serial)||MultipleObjTypeReturnsAllowed(stone))
			var objtypedata := GetObjTypeData(stone, objtype);
			if (objtypedata)
				//check additional conditions
							
				if (objtypedata.specificname) //check for specific name condition
					if (!(item.desc==objtypedata.specificname || item.name==objtypedata.specificname))
						//doesnt meet specific name condition;
						return 0;
					endif
				endif
				if (objtypedata.spawnorigin) //check for spawnpoint condition
					if (GetObjProperty(item,"SpawnPoint")!=objtypedata.spawnorigin)
						//doesnt meet  spawnpoint condition
						return 0;
					endif
				endif
				//amount is always set so dont need to check for
				if (item.amount<objtypedata.amount)
					//doesnt meet amount restriction
					return 0;
				endif
				//destroy amount of item equal to amount required
				if (item.amount>objtypedata.amount)
					//just reduce the item stack
					//item.amount := item.amount-objtypedata.amount;
					SubtractAmount( item, objtypedata.amount);
				else
					//equal to amount required so destroy the item
					DestroyItem( item );
				endif
				//grants points, send message, set completed
				SetPersonalObjectiveComplete(player, objtype, stone.serial);
				BroadcastPersonalCompletedMessage(stone, objtype, player);	
				AwardQuestPoints(stone.serial, player.serial, objtypedata.questpoints);
				return 1;
			endif
		else
			SendSysMessage(player, "You have already returned an item of this type", 3, 53);
			return;
		endif
	endif
	return 0;
endfunction

function GetNumberTimesRewarded(item)
	var tempnumtimes:= GetObjProperty(item, "NumberOfTimesRewarded");
	if (!tempnumtimes)
		return 0;
	endif
	return GetObjProperty(item, "NumberOfTimesRewarded");
endfunction

function IncrementNumberTimesRewarded(item)
	var timeawarded:= GetObjProperty(item, "NumberOfTimesRewarded");
	if (timeawarded)
		SetObjProperty(item, "NumberOfTimesRewarded", timeawarded+1);
	else
		SetObjProperty(item, "NumberOfTimesRewarded", 1);
	endif
endfunction

function SetPrerequisiteQuest(stone, prereqQuestSerial)
	SetObjProperty(stone, "PreviousQuestRequired", prereqQuestSerial);
endfunction

function GetPrerequisiteQuest(stone)
	return GetObjProperty(stone, "PreviousQuestRequired");
endfunction

function CheckQuestPrequisite(stone, player_serial)
//returns true if no former quest stone or player has completed the former quest stone
	var formerQuestSer := GetPrerequisiteQuest(stone);
	if (!formerQuestSer)
		return 1; //no former quest so check is successful
	endif
	var formerQuest := SystemFindObjectBySerial(formerQuestSer);
	if (IsCompleted(formerQuest, player_serial))
			return 1;
	endif
	return 0;
endfunction

//Quest Array handling
//basically players can now be in up to 5 quests so their Quester prop needs to be an array
//and handled properly

//Add quest to player
function AddQuest(player, quest)
//returns 1 if successful, -1 if couldnt add cos of greater than 5 allowed
	var tempqarray := array;
	var oldqarray:=GetObjProperty(player, "Quester");
	if (!oldqarray)
		//Broadcast("no old array");
		SetObjProperty(player, "Quester", {quest});
		return 1;
	endif
	
	if (len(oldqarray)>=5)
		//Broadcast("greater than 5");
		return 0;
	endif
	
	//handle case where player has old Quester prop which wasnt an array
	if (TypeOf(oldqarray) != "Array")
		//Broadcast("not type of array");
		tempqarray.append(oldqarray);
		tempqarray.append(quest);
		SetObjProperty(player, "Quester", tempqarray);
	else
		//Broadcast("an array");
		oldqarray.append(quest);
		SetObjProperty(player, "Quester", oldqarray);
	endif
	//Broadcast(GetObjProperty(player, "Quester"));
	return 1;
endfunction



//Remove quest
function RemoveQuest(player, quest)
//removes quest if found... returns 1
	var oldqarray:=GetObjProperty(player, "Quester");
	//handle case where player has old quester prop which wasnt an array
	if (TypeOf(oldqarray) != "Array")
		//Broadcast("not type of array");
		if (oldqarray==quest)
			EraseObjProperty(player, "Quester");
		endif
		return 1;
	endif
	//now for array handling
	var tempqarray :=array;
	foreach q in oldqarray
		if (q!=quest)
			tempqarray.append(q);
		endif
	endforeach
	if (len(tempqarray)<1)
		EraseObjProperty(player, "Quester");
	else
		SetObjProperty(player, "Quester", tempqarray);
	endif
	return 1;
endfunction



//Find quest in quest array
function GetQuest(player, quest)
//returns index number of array in Quester prop if on player (queststone serial)
	var oldqarray:=GetObjProperty(player, "Quester");
	if (!oldqarray)
		return 0;
	endif
	//handle case where not array
	if (TypeOf(oldqarray) != "Array")
		//Broadcast("not type of array");
		if (oldqarray==quest)
			return 1;
		else
			return 0;
		endif
	endif
	
	//now for array handling
	foreach q in oldqarray
		if (q==quest)
			return 1;
		endif
	endforeach
	return 0;

endfunction


//Find quests that two players share - for pk protection
function FindSharedQuests(player1, player2)
	var temparray:=array; //returns array of shared quests
	var oldqarray1:=GetObjProperty(player1, "Quester");
	var oldqarray2:=GetObjProperty(player2, "Quester");
	//Broadcast("player 1 quests: " + oldqarray1);
	//Broadcast("player 2 quests: " + oldqarray2);

	if (!oldqarray1 || !oldqarray2)
		return 0;
	endif
	if (TypeOf(oldqarray1) == "Array" && TypeOf(oldqarray2) == "Array")
		//Broadcast("both are arrays");
		foreach q in oldqarray1
			//Broadcast(q);
			if (GetQuest(player2, CInt(q)))
				//GetQuest(player, quest)
				temparray.append(q);
			endif
		endforeach
		return temparray;
	endif
	if (TypeOf(oldqarray2) == "Array")
		if (GetQuest(player2, oldqarray1))
			temparray.append(oldqarray1);
			return temparray;
		endif
	endif
	if (TypeOf(oldqarray1) == "Array")
		if (GetQuest(player1, oldqarray2))
			temparray.append(oldqarray2);
			return temparray;
		endif
	endif
	//remaining case is both arent arrays
	if (oldqarray1==oldqarray2)
		temparray.append(oldqarray2);
		return temparray;
	endif
endfunction

function myProgressGump(who, stone)
	
	GFInitGump();
	GFPage(1);
	GFResizePic(0, 0, BKGND_SCROLL, 550, 200);
	
	var questtitle:=GetQuestTitle(stone.serial);
	GFTextMid(10, 20, 280, 10, "Quest: " + questtitle);
	
	if (IsRegistered(stone, who.serial))
		GFTextLine(40, 50, 5, "You currently have " + GetPlayersQuestPoints(stone.serial, who.serial) + " questpoints");
		var persobjectives:=GetObjProperty(stone, "PersonalObjectives");
		var objectives:=GetObjProperty(stone, "Objectives");
		var counter:=0;
		if (persobjectives)
			foreach obj in persobjectives
				if (IsPersonalObjectiveCompleted(who, obj, stone.serial))
					counter := counter+1;
				endif			
			endforeach
			GFTextLine(40, 80, 5, "You have completed " + counter + " of " + persobjectives.size() + " personal objectives.");
		endif
		counter:=0;
		if (objectives)
			foreach obj in objectives
				if (IsObjectiveCompleted(stone, obj))
					counter := counter+1;
				endif			
			endforeach
			GFTextLine(40, 110, 5, "" + counter + " of the " + objectives.size() + " one-off objectives for this quest have been completed.");
		endif
		if (IsCompleted(stone, who.serial))
			GFTextLine(40, 140, 5, "You have completed this quest.");		
		endif
		
	else
		GFTextLine(40, 80, 5, "You are not registered for this quest.");	
	endif
	
	GFTextLine(55, 170, 5, "Close");

	

	var closeButton := GFButtonID( 40, 175, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	

	var choice := GFSendGump(who);
	//if (StoneUserElapsed(stone, who))
	//	return;
	//endif

	if (choice[closeButton.keyid])
		return;
	endif


endfunction

function FindAnyClassLevel(who)
	var level;
	foreach key in GetClasseIds()
 	  if (GetObjProperty(who, key))
  	      level := GetObjProperty(who, key);
   	endif
	endforeach
	if (!level||level==error)
		return 0;
	endif

	return level;
	
endfunction

function MultipleObjTypeReturnsAllowed(stone)
//returns true if stone set to allow the same objtype (for personal objectives) to be returned multiple times by one player
//default is a single player can only return an objtype objective once.
	var multiobjtypeoption := GetObjProperty(stone,  "MultipleObjTypeReturnsAllowed");
	if (!multiobjtypeoption || multiobjtypeoption==error)
		return 0;
	endif
	return 1;
endfunction

function ToggleMultipleObjTypeReturns(stone)
//sets toggles MultipleObjTypeReturnsAllowed option on stone
	var multiobjtypeoption := GetObjProperty(stone,  "MultipleObjTypeReturnsAllowed");
	//Broadcast (multiobjtypeoption);
	if (!multiobjtypeoption || multiobjtypeoption==error)
		SetObjProperty(stone,  "MultipleObjTypeReturnsAllowed", 1);
		return;
	endif
	SetObjProperty(stone,  "MultipleObjTypeReturnsAllowed", 0);
endfunction

function MultipleObjTypeReturnsString(stone)
	var multiobjtypeoption := GetObjProperty(stone,  "MultipleObjTypeReturnsAllowed");
	if (!multiobjtypeoption || multiobjtypeoption==error)
		return "Item type can be returned once only by a single player";
	endif
	return "Item type can be returned multiple times by a single player";
endfunction

//web page functionality
function	setQuestURL(who, stone)
	var webURL := GetObjProperty(stone, "WebPageURL");
	if (webURL)
		if (! YesNo(who, "Current quest webpage is " + webURL + ". Do you want to change this?"));
			SendSysMessage(who, "Web page was left as " + webURL);
			return;
		endif
	endif
	
	SendSysMessage(who, "Enter quest webpage URL. e.g forum.gamiix.com/viewtopic.php?t=17 ... Hit escape to delete webpage.", 3, 53);
	var newweb :=CStr(RequestInput( who, who.backpack, "URL:" ));
	if (newweb=="0")
		SendSysMessage(who, "Quest web page deleted.", 3, 53);
		EraseObjProperty(stone, "WebPageURL");
		return;
	endif
	SetObjProperty(stone, "WebPageURL", newweb);
	SendSysMessage(who, "Quest web page is now '" + newweb + "'. Test via Playergump option in main menu.", 3, 53);
endfunction

//quest clean up function so that npcs that register themselves at creation can clean off dead npcs... initially for use with
//musicians and allowing them to be npcwipe-tolerant with customspawnpoints.
function CleanUpDeadPersonalObjectives(stone)
	var obj;
	foreach pos in GetObjProperty(stone, "PersonalObjectives")
		obj:=SystemFindObjectBySerial(pos);
		if (!obj||obj==error)
			RemovePersonalObjective(stone, pos);
		endif
	endforeach
endfunction

function RunSpawnEditGump(who, spawn, stone)
var sro;
var sr;
var io := "No.";
if(GetObjProperty(spawn, "SetQuestPointReward" ))
   sr := GetObjProperty(spawn, "SetQuestPointReward" );
else
   sr := "N/A.";
endif
var qObjectives := GetObjProperty(stone, "SpawnActive");

If( !qObjectives )
   qObjectives := dictionary{cint(spawn.serial) -> 0};
endif

if (qObjectives[cint(spawn.serial)] == 1)
   io := "Yes.";
elseif (qObjectives[cint(spawn.serial)] == 0)
   io := "No.";
else
    qObjectives.append(cint(spawn.serial));
    qObjectives[cint(spawn.serial)] := 0;
    io := "No.";
endif

if(GetObjProperty(spawn, "SetQuestPointReward" ))
   sro := "On.";
else
   sro := "Off.";
endif
        GFInitGump();
	GFPage(1);
	GFResizePic(0, 0, BKGND_PAPER_EX, 230, 400);

	GFTextLine(20, 20, 230, spawn.desc);
        GFTextLine(20, 40, 66,"("+spawn.x+","+spawn.y+","+spawn.z+")");
	GFTextLine(30, 120, 5, "Goto Spawn");
	GFTextLine(30, 150, 5, "Open Spawn Gump");
	GFTextLine(30, 180, 5, "Remove Spawn");
	GFTextLine(30, 210, 5, "Delete Spawn");
	GFTextLine(30, 260, 5, "Reward Points: "+io);
	GFTextLine(30, 290, 5, "Is Set Reward: "+sro);
	GFTextLine(30, 320, 5, "Set Reward: "+sr );

	GFTextLine(30, 370, 5, "Exit Spawn Changes");


	//GFTextMid(10, 120, 280, 10, "Current colour: " + CStr(dye.color));

	var gotospawnbutton := GFButtonID( 10, 125, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var openspawnbutton := GFButtonID( 10, 155, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var removespawnbutton  := GFButtonID( 10, 185, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var deletespawnbutton  := GFButtonID( 10, 215, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var isobjbutton  := GFButtonID( 10, 265, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var issetrwdbutton  := GFButtonID( 10, 295, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var setrwdamtbutton  := GFButtonID( 10, 325, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var exitPlayerGumpbutton  := GFButtonID( 10, 375, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );

	var choice := GFSendGump(who);
	if (StoneUserElapsed(stone, who))
		return;
	endif

	if (choice[gotospawnbutton.keyid])
		MoveObjectToLocation(who,spawn.x, spawn.y, spawn.z, _DEFAULT_REALM); //AutoConversion
                RunSpawnEditGump(who, spawn, stone);
	elseif (choice[openspawnbutton.keyid])
                Start_Script(":spawnpoint:spawnpoint", array{ who, spawn });
                RunSpawnEditGump(who, spawn, stone);
	elseif (choice[removespawnbutton.keyid])
//Remove Spawn Point From Quest.
           If(YesNo(who, "Are You Sure You Want To Remove This Spawn From Quest?"))
              RemoveQuestSpawnPoint(stone, spawn);
           else
              SendSysMessage(who, "Canceled.");
           endif
           Return;
	elseif (choice[deletespawnbutton.keyid])
//Destroy The Spawn Point
           If(YesNo(who, "Are You Sure You Want To Destroy This Spawn?"))
              RemoveQuestSpawnPoint(stone, spawn);
	      DestroyItem(spawn);
           else
              SendSysMessage(who, "Canceled.");
           endif
           Return;
	elseif (choice[isobjbutton.keyid])
//Enable/Disable objective status
                 If( qObjectives[cint(spawn.serial)] == 1 )
                    qObjectives[cint(spawn.serial)] := 0;
	            SetObjProperty(stone, "SpawnActive",qObjectives);
                 else
                    qObjectives[cint(spawn.serial)] := 1;
	            SetObjProperty(stone, "SpawnActive",qObjectives);
                    PrintTextAbove(spawn, "Objective Armed");
                 endif
                 RunSpawnEditGump(who, spawn, stone);
	elseif (choice[issetrwdbutton.keyid])
//Enable/Disable Static Points For completing spawn.
                If( GetObjProperty(spawn, "SetQuestPointReward" ))
                   EraseObjProperty(spawn, "SetQuestPointReward");
                   SendSysMessage(who, "Static Reward Points Disabled");
                else
                 var reward := CInt(SendTextEntryGump(who, "How many quest points are rewarded? [integer]"));
                 SendSysMessage(who, "Objective Set With Reward Of: "+reward);
                 SetObjProperty(spawn, "SetQuestPointReward", reward);
                endif
                RunSpawnEditGump(who, spawn, stone);
	elseif (choice[setrwdamtbutton.keyid])
 //change amount of point awarded for completing spawn.
               If( !GetObjProperty(spawn, "SetQuestPointReward" ))
                   SendSysMessage(who, "Static Reward Points Are Disabled. Please Enable.", 3, 38);
                   SendSysMessage(who, "Canceled.");
                else
		   var reward := CInt(SendTextEntryGump(who, "How many quest points are rewarded? [integer]"));
                   SendSysMessage(who, "Objective Set With Reward Of: "+reward, 3 , 30);
                   SetObjProperty(spawn, "SetQuestPointReward", reward);
                Endif
                RunSpawnEditGump(who, spawn, stone);
	elseif (choice[exitPlayerGumpbutton.keyid])
		return;
	endif
endfunction

Function IsSpawnActive(stone, spawn)
var glenn := Getobjproperty(Stone, "SpawnActive");
If(glenn[cint(spawn)] == 1 )
return 1;
endif
Return 0;

Endfunction

Function EnableAutoTelePoint(Stone, ox, oy, oz)
   var coords := struct{ x, y , z };
   coords.x := ox;
   coords.y := oy;
   coords.z := oz;
   SetObjProperty(Stone, "AutoTelePoint", coords);
endfunction

Function RemoveDeadSpawn(spawn_serial, stone_serial)
	var stone := SystemFindObjectBySerial(stone_serial);
	
	var qSpawnPoints:=GetObjProperty(stone, "SpawnPoints");
        
	var tempQSPoints:=array;
 
	foreach q in qSpawnPoints
		if (q!=spawn_serial)
			tempQSPoints.append(q);
		endif
	endforeach
	SetObjProperty(stone, "SpawnPoints",tempQSPoints);
EndFunction