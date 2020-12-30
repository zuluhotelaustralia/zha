use uo;
use os;

function GetRaceProtType(who)

	var race:= who.title_race;
	var racecfg:= ReadConfigFile ("::races");

	var elem:= FindConfigElem(racecfg, race);
	return elem.Prot;

endfunction

function GetRaceProtAmount(who)

	var race:=who.title_race;
	var racecfg:= ReadConfigFile ("::races");
	if (!racecfg)
		broadcast(racecfg);
	endif

	var elem:= FindConfigElem(racecfg, race);
	if (!elem)
		return 0;
	endif

	return elem.PerLevel;

endfunction

function GetRaceProtBonus (who)

	var perlvl:= GetRaceProtAmount(who);
	return CInt(GetClasseLevel(who) * perlvl);

endfunction

function SetRaceProt(who)

	if (GetObjProperty(who, GetRaceProtType(who)) < GetRaceProtBonus(who))
		SetObjProperty (who, GetRaceProtType(who), GetRaceProtBonus(who));
	endif
	return;

endfunction

function GetRaces()

	return {"Human",
		"Elf",
		"Dark-Elf",
		"Barbarian",
		"Goblin",
		"Dwarf",
		"Vampire",
		"Demon"		};

endfunction

function IsProhibitedClasseForRace(classeid, race)

	case (race)
		"Human":	return 0;
		"Elf":		if (classeid==CLASSEID_DEATHKNIGHT || classeid==CLASSEID_NECROMANCER)
					return 1;
				endif
		"Demon":
		"Vampire":
		"Dark-Elf":	if (classeid==CLASSEID_PALADIN || classeid==CLASSEID_PRIEST)
					return 1;
				endif
		"Dwarf":	if (classeid==CLASSEID_MAGE || classeid==CLASSEID_NECROMANCER)
					return 1;
				endif
		"Barbarian":	if (classeid==CLASSEID_MAGE || classeid==CLASSEID_NINJA)
					return 1;
				endif
		"Goblin":	if (classeid==CLASSEID_MONK || classeid==CLASSEID_PALADIN)
					return 1;
				endif
	endcase

	return 0;

endfunction

function GetHTMLColor(who)

	case (who.title_race)
		"Human":	return "#FFCC99";
		"Elf":		return "#009966";
		"Dark-Elf":	return "#000000";
		"Dwarf":	return "#999900";
		"Barbarian":	return "#993300";
		"Goblin":	return "#336666";
		"Vampire":	return "#999999";
		"Demon":	return "#CC0000";
		default:	return "#000000";
	endcase

endfunction