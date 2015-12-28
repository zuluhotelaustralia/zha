//Barnabus 23/3/2003 - functions for war operations

//essentially a war consists of 3 items: 
//1 warball that is used by gms to configure the war and stores data,
//and 2 war stones.. one for each team that is linked to the warball. 
//A player can only be in 1 war at a time but there can be several 
//wars going at once.


use uo;
use os;

include "include/constants/propids";
include "include/utility";
include "include/stringarrays";
//include "include/itemutil";
include "include/classes";

//War Registration constants
const WAR_REG_NONE :=0;
const WAR_REG_BARD :=2; 
const WAR_REG_MAGE :=4; 
const WAR_REG_RANGER :=5; 
const WAR_REG_THIEF :=6; 
const WAR_REG_WARRIOR :=7; 
const WAR_REG_RACE:=8;


//basic getters and setters for war related global props
function SetWarTitle(title, ball)
	SetObjProperty(ball, "warTitle", title);
endfunction

function GetWarTitle(ball)
	return GetObjProperty(ball,"warTitle");
endfunction

function SetWarIntro(ball, intro)
	SetObjProperty(ball, "intro", intro);
endfunction

function GetWarIntro(ball)
	return GetObjProperty(ball, "intro");
endfunction

function SetAWOLTime(hours, ball)
	//stored in systime seconds
	SetObjProperty(ball, "AWOLTime", hours*3600);
endfunction

function GetAWOLTime(ball)
	//stored in systime seconds
	return GetObjProperty(ball, "AWOLTime");
endfunction

function GetWarCreator(ball)
	return  GetObjProperty(ball,"warCreatingStaff");
endfunction

function SetWarCreator(ball, staffname)
	SetObjProperty(ball, "warCreatingStaff", staffname);
endfunction
	
function SetWarTeamName(ball, index, tname)
	SetObjProperty(ball, "warTeamName"+index, tname);
	var myguild := GetWarGuild(ball, index);
	myguild.setprop( "guildabv", tname );
	myguild.setprop( "guildname", tname );
endfunction
	
function GetWarTeamName(ball, index)
	return GetObjProperty(ball, "warTeamName"+index);
endfunction

function SetWarStoneSer(ball, stoneserial, teamnumber)
	SetObjProperty(ball, "warStoneSerial"+teamnumber, stoneserial);
endfunction

function GetWarStoneSer(ball, teamnumber)
	return GetObjProperty(ball, "warStoneSerial"+teamnumber);
endfunction

function GetWarStone(ball, teamnumber)
	return SystemFindObjectBySerial(GetObjProperty(ball, "warStoneSerial"+teamnumber));
endfunction

function IsWar(ball)
	//returns true if war currently happening
	return GetObjProperty(ball, "WarHappening");
endfunction

function StartWar(ball)
	//returns true if war currently happening
	SetObjProperty(ball, "WarHappening", 1);
	AddAllPlayersToWarGuilds(ball);
	ResetAllPlayersAWOLTime(ball);
	CloseWarRegistration(ball);
	start_script("AWOLmanager", ball);
	BroadcastToWarParticipants(ball, "The War has been started! You are now a soldier in the " + GetWarTitle(ball));
	PageStaff(ball, "The war " + GetWarTitle(ball) + " has been started");
	PlayMessageHornsAll(ball);
endfunction

function EndWar(ball)
	//runs if war has been started
	if (! IsWar(ball))
		return;
	endif
	
	SetObjProperty(ball, "WarHappening", 0);

	FinaliseAlivePlayers(ball);
	BroadcastToWarParticipants(ball, "The " + GetWarTitle(ball) + " is over!");
	BroadcastToWarParticipants(ball, WinnerString(ball));
	PageStaff(ball, "The war " + GetWarTitle(ball) + " has ended");
	PlayMessageHornsAll(ball);
endfunction

function GetWarGuild(ball, index)
//each ball has 2 guilds that are set to war each other... this returns the guilds
	return (FindGuild(GetObjProperty(ball, "WarGuild" + index)));
endfunction

function IsWarRegistrationOpen(ball)
	//returns true if war registration allowed
	return  GetObjProperty(ball,"WarRegistrationOpen");
endfunction

function OpenWarRegistration(ball)
	SetObjProperty(ball, "WarRegistrationOpen", 1);
endfunction

function CloseWarRegistration(ball)
	SetObjProperty(ball, "WarRegistrationOpen", 0);
endfunction

function CreatePlayerDataStruct(player, warteam)
	//given required parameters this function returns an objtype struct
	var tempRecord:=struct;
	tempRecord.+playerserial:=player.serial;
	tempRecord.+team:=warteam;
	tempRecord.+kills:=0; 
	tempRecord.+battlepoints:=0; 
	tempRecord.+playersKilled:={};
	tempRecord.+killed:="alive";
	tempRecord.+AWOLTime:=0;
	tempRecord.+acctname:=player.acct.name;
	return tempRecord;
endfunction

function GetPlayerWData(ball, playerserial)

	var qPData:=GetObjProperty(ball, "PlayerData");
	if (!qPData)
		return -1; //error no data
	endif
	
	var i;	
	for (i:=1; i<=len(qPData); i:=i+1)
		if (qPData[i].playerserial==playerserial)
			return qPData[i];
		endif
	endfor
	return 0; //not found
endfunction

function SetPlayerWData(ball, playerstruct)
	//used to update an existing playerstruct record with this new struct
	var qPData:=GetObjProperty(ball,  "PlayerData");
	if (!qPData)
		return -1; //error no data
	endif
	
	var index:=  GetPlayerDataIndex(ball, playerstruct.playerserial);
	if (index<=0)
		return; //not found
	endif
	//update
	qPData[index]:=playerstruct;
	//save over existing prop
	SetObjProperty(ball, "PlayerData",qPData);
endfunction

function AddPlayerWData(ball, player, warteam)
	//used to create a new playerstruct record
	//checking for registration open and whether record already exists done elsewhere
	var qPData:=GetObjProperty(ball, "PlayerData");
	if (!qPData)
		//tempData:=array;
		qpData:={CreatePlayerDataStruct(player, warteam)};
	else
		qpData.append(CreatePlayerDataStruct(player, warteam));

	endif
	SetObjProperty(ball, "PlayerData",qPData);
endfunction


function RemovePlayerWData(ball, playerserial)
	var qpData:=GetObjProperty(ball,  "PlayerData");
	var tempArray := array;
	foreach q in qpData
		if (q.playerserial!=playerserial)
			tempArray.append(q);
		endif
	endforeach
	SetObjProperty(ball, "PlayerData",tempArray);	
endfunction


function GetPlayerDataIndex(ball, playerserial)

	//returns an index of player data that matches serial stored in array
	var qpData:=GetObjProperty(ball,  "PlayerData");
	if (!qpData)
		return 0; //error no data
	endif
	
	var i;	
	for (i:=1; i<=len(qpData); i:=i+1)
		if (qpData[i].playerserial==playerserial)
			return i;
		endif
	endfor
	return 0; //not found
endfunction

function ClearPlayerWData(ball)
	//clears all player data
	//also needs to go through each listed player and fix props
	SetObjProperty(ball, "PlayerData",{});
	EraseObjProperty(ball, "startedOnce");
endfunction

function ListWRegistedPlayerNames(ball)

endfunction

function GetRegisteredWarPlayers(ball, start, returnSize)
//returns a small array of player objects

	var playerserials:=GetAllRegisteredPlayerSerials(ball);
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


function IsPlayerRegistered(ball, playerserial)
	return GetPlayerDataIndex(ball, playerserial);
endfunction

//stone user sharing functions
function CheckCurrentWarUser(stone, who)
	//returns true if who is the current user
	if (who.serial == GetObjProperty(stone, "currentuser"))
		return 1;
	endif
	return 0;
endfunction

function GetCurrentWarUser(stone)
	return GetObjProperty(stone, "currentuser");
endfunction

function SetCurrentWarUser(stone, who)
	//sets user and time clicked
	SetObjProperty(stone, "#lastclicked", CInt(ReadGameClock())+60);
	SetObjProperty(stone, "currentuser", who.serial);
endfunction

function ClearCurrentWarUser(stone, who)
	//clears props - used when current user stops using stone
	EraseObjProperty(stone, "#lastclicked");
	EraseObjProperty(stone, "currentuser");
endfunction

function CheckCurrentWarUserTimeElapsed(stone, who)
	//returns true if time elapsed
	var clickMaxTime := CInt(GetObjProperty(stone, "#lastclicked"));
	if (clickMaxTime<ReadGameClock()) //that is current time is greater than reserved time
		return 1;
	endif
	return 0;
endfunction

function WarStoneUserElapsed(stone, who)
	//this ensures that someone cant just hog the stone indefinitely.
	//when a user clicks on the stone and it is not ReserveItem'ed their serial and the time is written to stone
	//when next person comes along.. they cant use stone until a minute has gone by.. if a minute has passed then
	//the current user is set to them....
	//when a player makes a selection in the player gumps it checks that they're the current player or else it will exit
	//with a message
	
	if (!CheckCurrentWarUser(stone, who))
		SendSysMessage(who, "Time elapsed for using stone. Another user is now using it. Please wait.", 3 ,34);
		return 1;
	endif
	return 0;
endfunction

//war stone getters and setters
function GetStoneTeamNumber(stone)
	return GetObjProperty(stone, "warTeamNumber");
endfunction

function SetStoneTeamNumber(stone, teamNumber)
	SetObjProperty(stone, "warTeamNumber", teamNumber);
endfunction

function GetWarBallSer(stone)
	return GetObjProperty(stone, "warballserial");
endfunction

function GetWarBall(stone)
	var ball := SystemFindObjectBySerial(GetObjProperty(stone, "warballserial"));
	return ball;
endfunction

function SetWarBallSer(stone, warballref)
	SetObjProperty(stone, "warballserial", warballref);
endfunction

//on player data storage

function SetOnPlayerData (player, ball, team)
//sets props on players
	SetObjProperty(player, "WarSerial", ball.serial);
	SetObjProperty(player, "WarTeam", team);
	SetOnPlayerRank(player, "Private");
endfunction

function GetOnPlayerTeam (player)
	return GetObjProperty(player, "WarTeam");
endfunction

function GetOnPlayerWarSerial (player)
	return GetObjProperty(player, "WarSerial");
endfunction

function SetOnPlayerRank(player, rankstr)
	SetObjProperty(player, "WarRank", rankstr);
endfunction

function GetOnPlayerRank(player)
	var rank := GetObjProperty(player, "WarRank");
	if (!rank)
		return "";
	else
		return rank;
	endif
endfunction

function EraseOnPlayerData(player)
	//cleans war props stored on players
	EraseObjProperty(player, "WarSerial");
	EraseObjProperty(player, "WarTeam");
	EraseObjProperty(player, "WarRank");
endfunction

//generic player handling

function RemoveAPlayerFromWar(player, ball)
	//this is done to remove a player completely from the war
	//including their struct data...
	if (IsWar(ball))
		ReturnPlayerToGuild(player);
	endif
	EraseOnPlayerData(player); //gets rid of cprops on player
	RemovePlayerWData(ball, player.serial); //removes struct data
	CheckEndOfWar(ball);
endfunction

function RemoveAllPlayersFromWar(ball)
	var aPlayer;
	foreach p in GetAllRegisteredPlayerSerials(ball)
		aPlayer := SystemFindObjectBySerial(p, SYSFIND_SEARCH_OFFLINE_MOBILES);
		RemoveAPlayerFromWar(aPlayer,ball);
	endforeach
endfunction

function EliminateAPlayerFromWar(player, ball)
	//similar to RemoveAPlayerFromWar... this function still retains the players data on the warball...
	//used for when a player is killed, goes AWOL, or at End Of War
	ReturnPlayerToGuild(player);
	EraseOnPlayerData(player);
	CheckEndOfWar(ball);
endfunction

function GetAllRegisteredPlayerSerials(ball)
	//returns an array of player serials
	var tempArr := array;
	var qpData:=GetObjProperty(ball,  "PlayerData");
	if (!qpData)
		return 0; //error no data
	endif
	
	var i;	
	for (i:=1; i<=len(qpData); i:=i+1)
		tempArr.append(qpData[i].playerserial);
	endfor
	return tempArr;
endfunction

function AwardKill(killer, killed)
	//ONLY CALLED WHEN KILLER AND KILLED BELONG TO SAME WAR BALL!
	//award kill to killer based on number of kills killed had
	
	var warball:=SystemFindObjectBySerial(GetOnPlayerWarSerial(killed));
	//broadcast
	var deathmsg := GetOnPlayerRank(killer) + " " + killer.name + " has slain " + GetOnPlayerRank(killed) + " " + killed.name +"!!!"; 
	BroadcastToWarParticipants(warball, deathmsg);
		
	var killerData := GetPlayerWData(warball, killer.serial);
	var killedData := GetPlayerWData(warball, killed.serial);
	
	//compare battlepoints
	var battlePointsGained := calculateBattlePointsEarned(killedData.battlepoints);
	killerData.kills:=killerData.kills+1;
	killerData.battlepoints:=killerData.battlepoints+battlePointsGained;
	killerData.playersKilled.append(killed.name);
	killerData.AWOLTime:=ReadGameClock()+GetAWOLTime(warball);
	killedData.killed:="killed";
	SetPlayerWData(warball, killerData);
	SetPlayerWData(warball, killedData);
	if (battlePointsGained)
		SendSysMessage(killer, "You have gained battlepoints", 3, 53);
		promoteKillerRank(killer, killerData.battlepoints,warball, killerData.team);
	endif
	//eliminate killed
	EliminateAPlayerFromWar(killed, warball);
endfunction

function calculateBattlePointsEarned(killedBPs)
//function designed to counter people cheating through killing dummy chars on other team
//to get battlepoints
	
/*
Rank1 : 0
Rank2:  1
Rank3:  3
Rank4:  6
Rank5: 10
Rank6: 15
Rank7: 21
Rank8: 28
Rank9: 36
Rank10: 45
Rank11: 55
Rank12: 66
*/

	if (killedBPs>=66)
		return 12;
	elseif(killedBPs>=55)
		return 11;
	elseif(killedBPs>=45)
		return 10;
	elseif(killedBPs>=36)
		return 9;
	elseif(killedBPs>=28)
		return 8;
	elseif(killedBPs>=21)
		return 7;
	elseif(killedBPs>=15)
		return 6;
	elseif(killedBPs>=10)
		return 5;
	elseif(killedBPs>=6)
		return 4;
	elseif(killedBPs>=3)
		return 3;
	elseif(killedBPs>=1)
		return 2;
	else
		return 1;
	endif
	return;
endfunction

function promoteKillerRank(killer, killerbps, warball, teamNum)
/*
Rank1 : 0
Rank2:  1
Rank3:  3
Rank4:  6
Rank5: 10
Rank6: 15
Rank7: 21
Rank8: 28
Rank9: 36
Rank10: 45
Rank11: 55
Rank12: 66
*/
	var rank;
	
	if (killerbps>=66)
		rank:= "Duke";
	elseif(killerbps>=55)
		rank:= "Lord";
	elseif(killerbps>=45)
		rank:= "General";
	elseif(killerbps>=36)
		rank:= "Colonel";
	elseif(killerbps>=28)
		rank:= "Lt Colonel";
	elseif(killerbps>=21)
		rank:= "Major";
	elseif(killerbps>=15)
		rank:= "Captain";
	elseif(killerbps>=10)
		rank:= "Leftenant";
	elseif(killerbps>=6)
		rank:= "Sgt Major";
	elseif(killerbps>=3)
		rank:= "Sargeant";
	elseif(killerbps>=1)
		rank:= "Corporal";
	else
		rank:= "Private";
	endif
	
	if (rank != GetOnPlayerRank(killer))
		killer.title_guild := GetWarTeamName(warball, teamNum) + " : " + rank;
		SendSysMessage(killer, "You have been promoted!", 3, 53);
		SetOnPlayerRank(killer, rank);
	endif
endfunction

//summary stats for team members 
function NumberMembersInTeamWhere(ball, teamNum, condition)
	//assumes each struct is unique elemet
	//condition is checked on element 6... killedStatus
	var qpData:=GetObjProperty(ball,  "PlayerData");
	var counter:=0;
	foreach q in qpData
		if (q.team==teamNum && q.killed == condition)
			counter:=counter+1;
		endif
	endforeach
	return counter;
endfunction


function MembersAliveInTeam(ball, teamNum)
	return NumberMembersInTeamWhere(ball, teamNum, "alive");
endfunction

//summary stats for number AWOL
function MembersAWOLInTeam(ball, teamNum)
	return NumberMembersInTeamWhere(ball, teamNum, "AWOL");
endfunction

//summary stats for number killed
function MembersKilledInTeam(ball, teamNum)
	return NumberMembersInTeamWhere(ball, teamNum, "killed");
endfunction

//returns players in team
function MembersInTeam(ball, teamNum)
	var qpData:=GetObjProperty(ball,  "PlayerData");
	var counter:=0;
	foreach q in qpData
		if (q.team==teamNum)
			counter:=counter+1;
		endif
	endforeach
	return counter;
endfunction


//returns highest bps scored by anyone
function GetMaxBPs(ball)
	var qpData:=GetObjProperty(ball,  "PlayerData");
	var mymax:=0;
	foreach q in qpData
		if (q.battlepoints>mymax)
			mymax:=q.battlepoints;
		endif
	endforeach
	return mymax;
endfunction

//returns players who have scored this many battlepoints
function GetPlayersWithBPs(ball, bps)
	var qpData:=GetObjProperty(ball,  "PlayerData");
	var playerArray :=array;
	var player;
	foreach q in qpData
		if (q.battlepoints==bps)
			var player := SystemFindObjectBySerial(q.playerserial, SYSFIND_SEARCH_OFFLINE_MOBILES);
			if (player)
				playerArray.append(player);
			endif
		endif
	endforeach
	return playerArray;
endfunction

//returns highest kills scored by anyone
function GetMaxKills(ball)
	var qpData:=GetObjProperty(ball,  "PlayerData");
	var mymax:=0;
	foreach q in qpData
		if (q.kills>mymax)
			mymax:=q.kills;
		endif
	endforeach
	return mymax;
endfunction

//returns players who have scored this many kills
function GetPlayersWithKills(ball, kills)
	var qpData:=GetObjProperty(ball,  "PlayerData");
	var playerArray :=array;
	var player;
	foreach q in qpData
		if (q.kills==kills)
			var player := SystemFindObjectBySerial(q.playerserial, SYSFIND_SEARCH_OFFLINE_MOBILES);
			if (player)
				playerArray.append(player);
			endif
		endif
	endforeach
	return playerArray;
endfunction

function GetTeamSumOfBattlepoints(ball, teamnum)
	var sumbp:=0;
	var qpData:=GetObjProperty(ball,  "PlayerData");
	foreach q in qpData
		if(q.team==teamnum)
			sumbp := sumbp + q.battlepoints;
		endif
	endforeach
	return sumbp;
endfunction

function TeamPoints(ball, teamnum)
	//calculates a teams points based on total battlepoints, total killed, and total awol
	var ourdeadtotal := MembersKilledInTeam(ball, teamNum);
	var ourawoltotal := MembersAWOLInTeam(ball, teamNum);
	var ourbps := GetTeamSumOfBattlepoints(ball,teamNum);
	return ((ourbps*3)-(ourawoltotal)-(ourdeadtotal*2));
endfunction

function CheckEndOfWar(ball)
	//end of war when one team has no members left
	if (IsWar(ball))
		if (MembersAliveInTeam(ball, 1)==0)
			BroadcastToWarParticipants(ball, GetWarTeamName(ball, 1) + " have all been eliminated!");
			WhichTeamWon(ball);
			EndWar(ball);
		endif
		if (MembersAliveInTeam(ball, 2)==0)
			BroadcastToWarParticipants(ball, GetWarTeamName(ball, 2) + " have all been eliminated!");
			WhichTeamWon(ball);
			EndWar(ball);
		endif
	endif
endfunction

//player guild handling

function AddPlayerToWarGuild(player, ball, teamnum)
	var oldguild := GetPlayerGuild(player);
	if (oldguild)
		SetObjProperty(player, "OldTitleGuild", player.title_guild);
		SetObjProperty(player, "OldGuildID", GetObjProperty(player, "guild_id"));
		oldguild.removemember(player);
		SendSysMessage(player, "You have temporarily left your guild to join the war! You will be automatically added back to it after your tour is over.", 3, 53);
	endif
	
	player.title_guild := GetWarTeamName(ball, teamNum) + " : Private";
	var warGuild := GetWarGuild(ball, teamNum);
	SetObjProperty(player, "guild_id", warGuild.guildid);
	warGuild.addmember(player);
endfunction

function GetPlayerGuild(player)
//returns players current guild based on guild_id cprop
//note that when players are in the war guild this will return the war guild
//otherwise it will return their initial guild if they have one
	return (FindGuild(GetObjProperty(player, "guild_id")));
endfunction

function ReturnPlayerToGuild(player)
	//get player team and ball... find current guild and delete
	var ball :=  SystemFindObjectBySerial(GetOnPlayerWarSerial (player));
	var teamnum :=  GetOnPlayerTeam (player);
	var warGuild := GetWarGuild(ball, teamNum);
	warGuild.removemember(player);

	var oldguildid := GetObjProperty(player, "OldGuildID");
	if (oldguildid)
		SetObjProperty(player, "guild_id", GetObjProperty(player,"OldGuildID"));
		EraseObjProperty(player,"OldGuildID");
		var oldguild := GetPlayerGuild(player);
		if (oldguild)
			oldguild.addmember(player);
			player.title_guild:=GetObjProperty(player, "OldTitleGuild");
			SendSysMessage(player, "You have rejoined your old guild!", 3, 53);		
		endif
		EraseObjProperty(player, "OldTitleGuild");
	else
		EraseObjProperty(player, "guild_id");
		player.title_guild:="";
	endif
endfunction

function AddAllPlayersToWarGuilds(ball)
	//this is called when the war starts... players are moved from their old guilds to the war guild
	//assumes each struct is unique elemet
	var qpData:=GetObjProperty(ball,  "PlayerData");
	var player;
	foreach q in qpData
		player := SystemFindObjectBySerial(q.playerserial, SYSFIND_SEARCH_OFFLINE_MOBILES);
		if (player)
			AddPlayerToWarGuild(player, ball, q.team);
		endif
	endforeach
endfunction

function ResetAllPlayersAWOLTime(ball)
//this is used at the start of a war so that the AWOL manager will give everyone 2 days from now to kill
//someone on other team before being ditched
	var qpData:=GetObjProperty(ball,  "PlayerData");
	foreach q in qpData
		q.AWOLTime:=ReadGameClock()+GetAWOLTime(ball); 
	endforeach
	SetObjProperty(ball, "PlayerData",qpData);	

endfunction

function SetPStructToAWOL(playerDataStruct, ball)
	//sets a player to AWOL, and eliminates them from war
	playerDataStruct.killed:="AWOL";
	SetPlayerWData(ball, playerDataStruct);
	var player := SystemFindObjectBySerial(playerDataStruct.playerserial, SYSFIND_SEARCH_OFFLINE_MOBILES);
	if (player)
		EliminateAPlayerFromWar(player, ball);
	endif
	SendSysMessage(player, "Because you have not killed an enemy soldier in the war for the last 2 days you have been declared AWOL!", 3,53);
endfunction

function SetPStructToSurvived(playerDataStruct, ball)
	//sets a player to 'survived the war' status
	playerDataStruct.kills:="Survived";
	SetPlayerWData(ball, playerDataStruct);
	var player := SystemFindObjectBySerial(playerDataStruct.playerserial, SYSFIND_SEARCH_OFFLINE_MOBILES);
	if (player)
		EliminateAPlayerFromWar(player, ball);
	endif
	SendSysMessage(player, "You have survived the war!!!!", 3,53);
endfunction

function FinaliseAlivePlayers(ball)
//used at the end of a war to finalise all records and players
//if you were alive at end of war then u are set to survived
	var qpData:=GetObjProperty(ball,  "PlayerData");
	foreach q in qpData
		if (q.killed == "alive")
			SetPStructToSurvived(q, ball);
		endif
	endforeach	
endfunction

function BroadcastToWarParticipants(ball, message)
	var playerserials:=GetAllRegisteredPlayerSerials(ball);
	var player;
	foreach p in playerserials
		player:=SystemFindObjectBySerial(CInt(p));
		if (player)
			SendSysMessage(player, message, 3, 53);
		endif
	endforeach
endfunction

function PageStaff(ball, message)
	var pages := GetGlobalProperty( PROPID_GLOBAL_STAFF_PAGES );
	if( !pages )
		pages := {};
	endif
	if (GetWarTitle(ball))
		ball.name := GetWarTitle(ball);
	else
		ball.name := "War";
	endif
	var num_pages := pages.size();
	pages[num_pages+1] := ball.serial;
	pages[num_pages+2] := message;
	SetGlobalProperty(PROPID_GLOBAL_STAFF_PAGES, pages);
endfunction

//Registration Condition functions
function GetWarRegistrationConditionString(ball, teamnum)
	//returns string corresponding to Registration Condition constant
	
	var tempstr;
	var regcondition := GetWarRegistrationCondition(ball, teamnum);
	case (regcondition)
		WAR_REG_BARD:
			tempstr:= "Bard";
		break;
		WAR_REG_MAGE:
			tempstr:= "Mage";
		break;
		WAR_REG_RANGER:
			tempstr:= "Ranger";
		break;
		WAR_REG_THIEF:
			tempstr:= "Thief";
		break;
		WAR_REG_WARRIOR:
			tempstr:= "Warrior";
		break;
		WAR_REG_RACE:
			return GetWarRaceRegistrationCondition(ball, teamnum);
		break;
		default:
			return "No registration conditions set";
	endcase
	return tempstr;
endfunction

function GetWarRegistrationCondition(ball, teamnum)
//returns int corresponding to registration condition constant corresponding to Registration Condition constant
	return GetObjProperty(ball, "RegistrationCondition"+teamnum);
endfunction

function SetWarRegistrationCondition(ball, val, teamnum)
	//Sets the stones registration condition (as constant)
	//WAR_REG_NONE :=0; WAR_REG_BARD :=2; WAR_REG_MAGE :=4; 
	//WAR_REG_RANGER :=5; WAR_REG_THIEF :=6; WAR_REG_WARRIOR :=7; WAR_REG_RACE:=8;

	SetObjProperty(ball, "RegistrationCondition"+teamnum, val);
endfunction

function CheckWarRegistrationCondition(who, ball, teamnum)
	//returns 1 if player can register 0 otherwise
	var regcondition := GetWarRegistrationCondition(ball, teamnum);
	var charclass;
	
	case (regcondition)
		WAR_REG_BARD:
			charclass := GetObjProperty( who, "IsBard" );
			if (charclass)
				return 1;
			endif
			return 0;
		break;
		WAR_REG_MAGE:
			charclass := GetObjProperty( who, "IsMage" );
			if (charclass)
				return 1;
			endif
			return 0;
		break;
		WAR_REG_RANGER:
			charclass := GetObjProperty( who, "IsRanger" );
			if (charclass)
				return 1;
			endif
			return 0;
		break;
		WAR_REG_THIEF:
			charclass := GetObjProperty( who, "IsThief");
			if (charclass)
				return 1;
			endif
			return 0;
		break;
		WAR_REG_WARRIOR:
			charclass := GetObjProperty( who, "IsWarrior" );
			if (charclass)
				return 1;
			endif
			return 0;	
		break;
		WAR_REG_RACE:
			return (CheckWarRaceRegistrationCondition(ball, who, teamnum));
		break;
		default:
			return 1; //none set
	endcase

endfunction

function CheckWarRaceRegistrationCondition(ball, who, teamnum)
//returns 1 if player the right race for the registration condition
	var warrace := GetObjProperty(ball, "RegistrationRace"+teamnum);
	if (who.title_race==warrace)
		return 1;
	endif
	return 0;	
endfunction

function SetWarRaceRegistrationCondition(race, ball, teamnum)
	//races allowed -- human, Elf, Dark-Elf, Goblin, Barbarian, Dwarf
	SetObjProperty(ball, "RegistrationRace"+teamnum, race);
endfunction

function GetWarRaceRegistrationCondition(ball, teamnum)
	return GetObjProperty(ball, "RegistrationRace" + teamnum);
endfunction

function CheckMinimumRegistrationCondition(player)
	//all wars are offlimits to crafters, classed players under level 3, 
	//and unclassed players with less than 2000 skill points
	if (GetObjProperty(player, "IsCrafter"))
		SendSysMessage(player, "Crafter cannot enlist in wars!", 3,53);
		return 0;
	endif
	if (FindClasseLevel(player) < 2)
		if (GetTotalSkillPoints(player)<1500)
			SendSysMessage(player, "Only characters level 2+ or with more than 1500 skillpoints can join wars!!!!", 3,53);
			return 0;
		endif
	endif

	return 1;
endfunction

function FindClasseLevel(player)
	//used to determine minimum registration condition
	var classlist := GetClasseIds();
	foreach key in classlist
		if (GetObjProperty(player, key))
			return GetObjProperty(player, key); //classed level
		endif
	endforeach
	return 0; //unclassed
endfunction

function GetTotalSkillPoints(player)
	var i:=0;
	var total:=0;
	for(i:=0; i<=SKILLID__HIGHEST; i:=i+1)
		total := total +  GetEffectiveSkillBaseValue(player, i);
	endfor
	return total/10;
endfunction

function CheckAccountNotOnOtherTeam(ball, teamNumToRegisterFor, accountname)
	var qpData:=GetObjProperty(ball,  "PlayerData");
	foreach q in qpData
		if (q.acctname==accountname && q.team!=teamNumToRegisterFor)
			return 0;
		endif
	endforeach
	return 1;
endfunction

function PlayMessageHornsAll(ball)
	var qpData:=GetObjProperty(ball,  "PlayerData");
	foreach q in qpData
		var player := SystemFindObjectBySerial(q.playerserial);
		if (player)
			PlaySoundEffect( player, 0x003e );
		endif
	endforeach
endfunction

function WarSummary(who, ball)
	GFInitGump();
	GFPage(1);
	GFResizePic(0, 0, BKGND_PAPER_EX, 400, 600);
		
	GFTextMid(10, 10, 400, 53, "Summary for " + GetWarTitle(ball));
	if (IsWar(ball))
		GFTextMid(10, 30, 400, 53, "This war is currently being fought!");
	endif
	
	GFTextMid(10, 50, 400, 53, "War Team " + GetWarTeamName(ball, 1) + ":-");
	GFTextMid(10, 75, 400, 10, "Number in team: " + MembersInTeam(ball, 1));
	GFTextMid(10, 100, 400, 10, "Number alive: " + MembersAliveInTeam(ball, 1));
	GFTextMid(10, 125, 400, 10, "Number killed: " + MembersKilledInTeam(ball, 1));
	GFTextMid(10, 150, 400, 10, "Number AWOL: " + MembersAWOLInTeam(ball, 1));
	GFTextMid(10, 175, 400, 10, "Team points: " + TeamPoints(ball, 1));
	
	GFTextMid(10, 210, 400, 53, "War Team " + GetWarTeamName(ball, 2) + ":-");
	GFTextMid(10, 235, 400, 10, "Number in team: " + MembersInTeam(ball, 2));
	GFTextMid(10, 260, 400, 10, "Number alive: " + MembersAliveInTeam(ball, 2));
	GFTextMid(10, 285, 400, 10, "Number killed: " + MembersKilledInTeam(ball, 2));
	GFTextMid(10, 310, 400, 10, "Number AWOL: " + MembersAWOLInTeam(ball, 2));
	GFTextMid(10, 335, 400, 10, "Team points: " + TeamPoints(ball, 2));
	

	var highestkills := GetMaxKills(ball);
	if (highestkills)
		GFTextMid(10, 375, 400, 53, "Highest kills: " + highestkills);
		var pstring;
		foreach p in GetPlayersWithKills(ball, highestkills)
			if(!pstring)
				pstring := p.name;
			else
				pstring := pstring + ", " + p.name;
			endif
		endforeach
		GFTextMid(10, 400, 400, 53, "Obtained by: " + pstring);
	
		var highestbps := GetMaxBPs(ball);
		pstring:="";
		GFTextMid(10, 425, 400, 53, "Highest battlepoints: " + highestbps);
		foreach p in GetPlayersWithBPs(ball, highestbps)
			if(!pstring)
				pstring := p.name;
			else
				pstring := pstring + ", " + p.name;
			endif
		endforeach
		GFTextMid(10, 450, 400, 53, "Obtained by: " + pstring);
	endif
	
	if (! IsWar(ball) && GetObjProperty(ball, "startedOnce"))
		GFTextMid(10, 480, 400, 53, WinnerString(ball));
	endif
	
	GFTextLine(90, 570, 5, "Exit");
	var exitButton := GFButtonID( 60, 575, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	
	var choice := GFSendGump(who);
	
	if(choice[exitButton.keyid])
		return;
	endif
endfunction

function WarIntroGump(ball, who)
	var intro:=GetWarIntro(ball);
	if (!intro)
		SendSysMessage(who, "No introduction has been set for this war");
		return;
	endif
	SendStringAsTipWindow( who, intro );
endfunction

function DeleteWarGuilds(ball)
	var guild1 := GetWarGuild(ball, 1);
	var guild2 := GetWarGuild(ball, 2);
	var i;
	var allenemies:=guild1.enemyguilds;

   for(i:=1;i<=allenemies.size();i:=i+1)
   	guild1.removeenemyguild(allenemies[i]);
   endfor

	allenemies:=guild2.enemyguilds;

  	for(i:=1;i<=allenemies.size();i:=i+1)
		guild2.removeenemyguild(allenemies[i]);
  	endfor

	RemoveAllPlayersFromWar(ball);
	
	DestroyGuild(guild1);
	DestroyGuild(guild2);
endfunction

function WhichTeamWon(ball)
//sets teamwon prop of winner
	if (TeamPoints(ball, 1) > TeamPoints(ball, 2))
		SetObjProperty(ball, "TeamWon", 1);
		return 1;
	elseif (TeamPoints(ball, 1) < TeamPoints(ball, 2))
		SetObjProperty(ball, "TeamWon", 2);
		return 2;
	else
		SetObjProperty(ball, "TeamWon", 0);
		return 0;
	endif
endfunction

function WinnerString(ball)
	//simple function to explain result to gms and players...
	var winner := GetObjProperty(ball, "TeamWon");
	if (winner>0)
		return GetWarTeamName(ball, GetObjProperty(ball, "TeamWon")) + " has won the war!!!!";
	else
		return "The war ended in a draw!";
	endif
endfunction
