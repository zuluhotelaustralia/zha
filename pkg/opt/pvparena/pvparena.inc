include "include/client";
include "include/areas";
include "include/res";
include "include/dotempmods";
include "include/bitwise";

// Constants

const PVPARENA_ACTIVE		:= "#pvparenaactive";
const PVPARENA_TEAM 		:= "#pvparenateam";
const PVPARENA_NOTSETUP		:= "pvparenanotsetup";
const PVPARENA_PLAYERNOTSETUP	:= "#pvparenaplayernotsetup";
const PVPARENA_CANCELSETUP	:= "#pvparenacancelsetup";
const PVPARENA_PLAYERBUSY	:= "#pvparenaplayerbusy";
const PVPARENA_MEMBERS		:= "pvparenamembers";
const PVPARENA_BANLIST		:= "pvparenabanlist";
const PVPARENA_WASNTCRIMINAL	:= "pvparenawasntcriminal";

const PVPARENA_CANTJOIN_NOTPLAYER	:=	1;
const PVPARENA_CANTJOIN_STAFF		:=	2;
const PVPARENA_CANTJOIN_TEAM		:=	3; // Already in a team.
const PVPARENA_CANTJOIN_BUSY		:=	4;
const PVPARENA_CANTJOIN_BANNED		:=	5;
const PVPARENA_CANTJOIN_OUTOFAREA	:=	6; // Not in spectator area.
const PVPARENA_CANTJOIN_HASITEMS	:=	7;

						//bitwise
const PVPARENA_OPTION_DETECT		:=	1; // Detect every 30 secs
const PVPARENA_OPTION_PREBLESS		:=	2; // Allow prebless
//const PVPARENA_OPTION_CURSE		:=	4; // Allow cursing

// Functions

function IsInSpectatorArea(who, stone)
	var coords := Unpack(GetObjProperty(stone, "SpectatorArea"));
	return IsInBox(who, coords[1], coords[2], coords[3], coords[4]);
endfunction

function IsInBattleArea(who, stone)
	var coords := Unpack(GetObjProperty(stone, "BattleArea"));
	return IsInBox(who, coords[1], coords[2], coords[3], coords[4]);
endfunction

function IsInBase1(who, stone)
	var coords := Unpack(GetObjProperty(stone, "Base1"));
	return IsInBox(who, coords[1], coords[2], coords[3], coords[4]);
endfunction

function IsInBase2(who, stone)
	var coords := Unpack(GetObjProperty(stone, "Base2"));
	return IsInBox(who, coords[1], coords[2], coords[3], coords[4]);
endfunction

// Check if a player is setup yet
function isSetup(who, setupstone)
	var setup1 := (!GetObjProperty(who, PVPARENA_PLAYERNOTSETUP));
	var serials := Unpack(GetObjProperty(setupstone, "serials"));
	return (setup1 && (who.serial in serials));
endfunction

// Send all the members of both teams a message
function messagePlayers(players, message, font := _DEFAULT_TEXT_FONT, color := _DEFAULT_TEXT_COLOR)
	foreach player in players
		SendSysMessage(player, message, font, color);
	endforeach
endfunction

function destroyItems(who)
	foreach item in ListEquippedItems(who)
		if (item.isA(POLCLASS_EQUIPMENT))
			DestroyItem(item);
		endif
	endforeach
	
	foreach item in EnumerateItemsInContainer(who.backpack)
		DestroyItem(item);
	endforeach
endfunction

// This function is for cleaning up afterwards.
function cleanup(stone, fightactive := 1)
	var exitlocation := Unpack(GetObjProperty(stone, "BattleOverPoint"));
	var serials := Unpack(GetObjProperty(stone, "serials")), member;

	foreach serial in serials
		member := SystemFindObjectBySerial(serial);
		if (!member)
			member := SystemFindObjectBySerial(serial, SYSFIND_SEARCH_OFFLINE_MOBILES);
		endif
		if (!member)
			continue;
		endif
		
		EraseObjProperty(member, "dontAffectKarmaFame");
		EraseObjProperty(member, PVPARENA_TEAM);
		if (GetObjProperty(member, PVPARENA_PLAYERNOTSETUP))
			SetObjProperty(member, PVPARENA_CANCELSETUP, 1);
		endif
		
		if (fightactive)
			killPlayer(member); 
			
			if (GetObjProperty(member, PVPARENA_WASNTCRIMINAL))
				member.SetCriminal(0);
				EraseObjProperty(member, PVPARENA_WASNTCRIMINAL);
			endif
			
			MoveCharacterToLocation( member, exitlocation[1], exitlocation[2], exitlocation[3], MOVECHAR_FORCELOCATION );
			resPlayer(member); // Res them and destroy their items.
			fixStats(member);
		endif
	endforeach
	EraseObjProperty(stone, "serials");
	
	if (fightactive)
		var battlearea := Unpack(GetObjProperty(stone, "BattleArea"));
		var base1      := Unpack(GetObjProperty(stone, "Base1"));
		var base2      := Unpack(GetObjProperty(stone, "Base2"));
		cleanArea(battlearea[1], battlearea[2], battlearea[3], battlearea[4]);
		cleanArea(base1[1], base1[2], base1[3], base1[4]);
		cleanArea(base2[1], base2[2], base2[3], base2[4]);
	endif
	
	var setupstone1 := SystemFindObjectBySerial(GetObjProperty(stone, "readystone1"));
	var setupstone2 := SystemFindObjectBySerial(GetObjProperty(stone, "readystone2"));
	EraseObjProperty(setupstone1, "serials");
	EraseObjProperty(setupstone2, "serials");
	
	EraseObjProperty(stone, PVPARENA_MEMBERS);
	EraseObjProperty(stone, PVPARENA_ACTIVE);
	EraseObjProperty(stone, "team1size");
	EraseObjProperty(stone, "team2size");
endfunction

// This function determines whether or not a player can join the battle.
function cantJoin(who, stone)
	if (!who.isA(POLCLASS_MOBILE) || who.isA(POLCLASS_NPC))
		return PVPARENA_CANTJOIN_NOTPLAYER;
	elseif (who.cmdlevel)
		return PVPARENA_CANTJOIN_STAFF;
	elseif (GetObjProperty(who, PVPARENA_TEAM))
		return PVPARENA_CANTJOIN_TEAM;
	elseif (GetObjProperty(who, PVPARENA_PLAYERBUSY))
		return PVPARENA_CANTJOIN_BUSY;
	elseif (!IsInSpectatorArea(who, stone))
		return PVPARENA_CANTJOIN_OUTOFAREA;
	elseif (hasItems(who))
		SendSysMessage(who, "Sorry, but you may not enter the fight with any items - including runebooks, newbied items, clothing, etc.", 3, 38);
		return PVPARENA_CANTJOIN_HASITEMS;
	endif
	
	return 0;
endfunction

// For keeping track of items made in the arena
function ArenaCreateItemInBackpack(who, item, amount := 1, rename := 1)
	var newitem := CreateItemInBackpack(who, item, amount);
	SetObjProperty(newitem, "Arena", 1);
	if (rename)
		newitem.name := "Stolen from the PVP arena!";
	endif
	return newitem;
endfunction

function ArenaCreateItemInContainer(container, item, amount := 1, rename := 1)
	var newitem := CreateItemInContainer(container, item, amount);
	SetObjProperty(newitem, "Arena", 1);
	if (rename)
		newitem.name := "Stolen from the PVP arena!";
	endif
	return newitem;
endfunction

function cleanArea(x1, x2, y1, y2)
	var z1 := GetWorldHeight(x1, y1);
	var z2 := GetWorldHeight(x2, y2);
	foreach item in ListObjectsInBox(x1, y1, z1, x2, y2, z2)
		if (item.isA(POLCLASS_CORPSE) || GetObjProperty(item, "Arena"))
			DestroyItem(item);
		elseif (item.isA(POLCLASS_NPC))
			SetObjProperty(item, "guardkill", 1);
			ApplyRawDamage(item, GetMaxHP(item));
		endif
	endforeach
endfunction

function hasItems(who)
	var baseweight := 13;
	if (who.dead)
		baseweight := baseweight + 1; // death shroud +1
	endif
	
	if (EnumerateItemsInContainer(who.backpack).size())
		return 1;
	else
		foreach item in ListEquippedItems(who)
			if (item.isA(POLCLASS_EQUIPMENT))
				return 1;
			endif
		endforeach
	endif
	
	return (who.weight>baseweight);
endfunction

// Kill their mount, kill them, return the fame they lost on death.
function killPlayer(who)
	if (!who.dead && !who.cmdlevel)
		SetObjProperty(who, "nocorpse", 1);
		SetObjProperty(who, "dontAffectKarmaFame", 1);
		var mount := GetEquipmentByLayer(who, LAYER_MOUNT );
		if (mount)
			DestroyItem(mount);
		endif
		
		ApplyRawDamage(who, GetHP(who)+10);
	endif
endfunction

function resPlayer(who)
	if (who.cmdlevel)
		return;
	endif
	
	if (who.dead)
		ResurrectMobile(who);
	endif
	destroyItems(who);
endfunction

// Remove mods (bless, strength, etc.) and then refresh their hp/mana/stamina
function fixStats(who, wipethemods := 1)
	if (wipethemods)
		WipeMods(who);
	endif
	
	SetHP(who, GetMaxHP(who));
	SetMana(who, GetMaxMana(who));
	SetStamina(who, GetMaxStamina(who));
endfunction

function recordSerial(stone, item)
	var serials := Unpack(GetObjProperty(stone, "itemserials"));
	if (!(item.serial in serials))
		serials.append(item.serial);
	endif
	
	if (item.isA(POLCLASS_CONTAINER))
		foreach subitem in EnumerateItemsInContainer(item)
			if (!(subitem.serial in serials))
				serials.append(subitem.serial);
			endif
		endforeach
	endif
	
	SetObjProperty(stone, "itemserials", Pack(serials));
endfunction