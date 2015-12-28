////////////////////////////////////////////////////////////////
//
//	Author: Archaean
//	email: lex@lateralsystems.net
//	
//	Generic functions for artifacts
//	
////////////////////////////////////////////////////////////////

use uo;
use os;
use util;


include "include/client";
include "include/yesno";
include "include/random";
include "include/itemutil";
include "include/attributes";
include "include/cmdlevel_check";
include "include/classes";

// Class-restricted artifact constants
const	CONTROL_STATDIVIDER	:=	2.8; // User's prime stat is divided by this and added to control chance
const	CONTROL_LEVELMULTIPLIER	:=	10;  // User's class level is multiplied by this and added to control chance
// Example: Level 4 mage with 130 INT = (130/2.8) + (4*10) = 86% chance for control

// PP artifact constants
const	CONTROL_SKILLDIVIDER	:=	40;  // Used for PP artifacts... Control chance = Total skills / CONTROL_SKILLDIVIDER
// Example: PP with 3640 total skill points = 3640 / 40 = 91% chance for control

const	CONTROL_CHANCE_MAXIMUM	:=	99;

// Colors for powers (currently the same ones used in random loot except str, int, dex)
const	NECRO_COLOR	:=	1170;
const	EARTH_COLOR	:=	343;
const	FIRE_COLOR	:=	240;
const	AIR_COLOR	:=	1001;
const	WATER_COLOR	:=	206;
const	PHYS_COLOR	:=	1160;
const	HEAL_COLOR	:=	1182;
const	MOONSTONE_COLOR	:=	611;
const	SPELL_COLOR	:=	1161;
const	STR_COLOR	:=	1172;
const	INT_COLOR	:=	1171;
const	DEX_COLOR	:=	1159;
const	RANDSKILL_COLOR	:=	1298;

// Returns 1 if control successful, or 0 if not
function CheckControl(user, artifact)

	var charclass:=0, artclass:=0, primestat, controlchance, i;
	var classlist := GetClasseIds();

	// Check artifact is in backpack
	if (artifact.container != user.backpack )
		SendSysMessage(user, "The artifact must be in your backpack");
		return 0;
	endif

	// On first use apply race/class limits
	if (!GetObjProperty(artifact, "RaceSpecific"))
		if (GetObjProperty(artifact, "QuestNote"))
			MatchArtifactToQuestWinner(artifact);
		else
			AssignRandomClassRace(artifact);
		endif
	endif

	// Get character's class and artifact's class
	charclass := GetClass(user);
	artclass := GetClass(artifact);

	// Check user is correct class
	if (artclass != charclass || GetObjProperty(artifact, "RaceSpecific") != user.title_race)
		SendSysMessage(user, "You have no idea how to use this mighty artifact");
		return 0;
	endif

	artifact.movable := 0;
	// Calculate control chance
	if (artclass == "IsPowerPlayer")
		var total :=0, i:=0;
		for(i:=0; i<=SKILLID__HIGHEST; i:=i+1)
			total := total + GetEffectiveSkillBaseValue(user, i);
		endfor
		controlchance := CInt(total / CONTROL_SKILLDIVIDER);
	else  // Class Artifact
		var charlevel := GetObjProperty(user, artclass);
		var primestat := GetPrimeStat(user, artclass);
		controlchance := CInt((primestat/CONTROL_STATDIVIDER) + (charlevel*CONTROL_LEVELMULTIPLIER));
	endif

	if (controlchance > CONTROL_CHANCE_MAXIMUM)
		controlchance := CONTROL_CHANCE_MAXIMUM;
	endif

	SendSysMessage(user, "Control chance: "+CStr(controlchance)+"%");
	SendSysMessage(user, "You attempt to control the artifact's power...");
	
	PlaySoundEffect(user, 0x222);
	sleep(2);
	if (Random(100)+1 > controlchance)
		SendSysMessage(user, "The artifact's power is too great to contain!");
		sleep(2);
		RetributiveStrike(artifact, user);
		return 0;
	endif
	SendSysMessage(user, "You bring the artifact under your control!");
	return 1;
	
endfunction


function RetributiveStrike (artifact, user, victim := 0)
	
	if (victim)
		if (YesNo(user, "Are you sure?") && !getCommandLevel(victim))
			// Kill victim
			ApplyDamage(victim, 9999);
			PlaySoundEffect(victim, 0x11E);
			PlayObjectCenteredEffect (victim, 0x36b0, 1, 0);
		else
			SendSysMessage(user, "Cancelled");
			return;
		endif
	endif
	// Kill user
	ApplyDamage(user, 9999);
	PlaySoundEffect(user, 0x11E);
	PlayObjectCenteredEffect (user, 0x36b0, 1, 0);
	DestroyItem(artifact);	

endfunction


function GetPrimeStat(user, artclass)

	case (artclass)
		CLASSEID_BARD: 
			return CInt((GetBaseIntelligence(user) + GetBaseDexterity(user)) / 2);
		CLASSEID_CRAFTER:
			return GetBaseStrength(user);
		CLASSEID_MAGE:
			return GetBaseIntelligence(user);
		CLASSEID_RANGER:
			return GetBaseDexterity(user);
		CLASSEID_THIEF:
			return GetBaseDexterity(user);
		CLASSEID_WARRIOR:
			return GetBaseStrength(user);
	endcase

endfunction

function MatchArtifactToQuestWinner(artifact)

	var artrace;
	var notetext:= GetObjProperty(artifact, "QuestNote");
	var notearray := SplitWords(notetext);
	var playerserial := CInt(notearray[3]);
	
	var player := SystemFindObjectBySerial(playerserial);
	
	if (!player)
		player := SystemFindObjectBySerial(playerserial, SYSFIND_SEARCH_OFFLINE_MOBILES);
		if (!player)
			AssignRandomClassRace(artifact);
			return;
		endif
	endif
	SetObjProperty(artifact, "RaceSpecific", player.title_race);
	SetObjProperty(artifact, GetClass(player), 1);
	SetObjProperty(artifact, "IDed", 1);
	case (player.title_race)
		"Dwarf":	artrace := "a Dwarven";
		"Elf":		artrace := "an Elven";
		"Dark-Elf":	artrace := "a Dark-Elven";
		"human":	artrace := "a Human";
		"Barbarian":	artrace := "a Barbarian";
		"Goblin":	artrace := "a Goblin";
	endcase
	artifact.name := artrace + " " + GetClasseName(GetClass(artifact)) + "'s " + GetObjProperty(artifact, "IsGMItem");
	artifact.color := 1298;	
	EraseObjProperty(artifact, "EnchantColor");
	return;

endfunction

function AssignRandomClassRace(artifact)
	
	var artrace, raceSpec, artclass;

	var i := Random(8);
	case (i)
		0:	raceSpec := "Vampire";
		1:	raceSpec := "Dwarf";
		2:	raceSpec := "Barbarian";
		3:	raceSpec := "Demon";
		4:	raceSpec := "Elf";
		5:	raceSpec := "Dark-Elf";
		6:	raceSpec := "Human";
		7:	raceSpec := "Goblin";
	endcase
	SetObjProperty(artifact, "RaceSpecific", raceSpec);
	case (raceSpec)
		"Dwarf":	artrace := "a Dwarven";
		"Elf":		artrace := "an Elven";
		"Dark-Elf":	artrace := "a Dark-Elven";
		"human":	artrace := "a Human";
		"Barbarian":	artrace := "a Barbarian";
		"Goblin":	artrace := "a Goblin";
		"Vampire":	artrace := "a Vampiric";
		"Demon":	artrace := "a Demonic";
	endcase

	// Set artifact's classe
	var classes:=GetClasseIds();
	i:= Random(len(classes)) + 1;
	artclass:= classes[i];
	if (IsProhibitedClasseForRace(artclass, raceSpec))
		AssignRandomClassRace(artifact);
	endif
	SetObjProperty(artifact, artclass, 1);

	artifact.name := artrace + " " + GetClasseName(GetClass(artifact)) + "'s " + GetObjProperty(artifact, "IsGMItem");
	SetObjProperty(artifact, "IDed", 1);
	artifact.color := 1298;
	EraseObjProperty(artifact, "EnchantColor");
	return;

endfunction