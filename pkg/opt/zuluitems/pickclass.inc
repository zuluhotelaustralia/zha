use uo;

include "include/gumps";
include "include/classes";
include "include/attributes";
include "include/client";

function sendclassgump(who, class);

	if (!class)
		class := "Home";
	endif

	var race := who.title_race;

	if(!race)
		SendSysMessage(who, "You need to pick a race first.");
		return;
	endif
	dropallskills(who);
	/* array{
	bard 1
	crafter 2
	mage 3
	ranger 4
	thief 5
	warrior 6
	druid 7
	deathknight 8
	ninja 9
	paladin 10
	priest 11
	monk 12
	necromancer 13
	}*/
	
	var excludelist := array{1,1,1,1,1,1,1,1,1,1,1,1,1};
	
	case (race)
		"Elf": excludelist[8] := 0; excludelist[13] := 0; break;
		"Dark-Elf": excludelist[10] := 0; excludelist[11] := 0; break;
		"Goblin": excludelist[10] := 0; excludelist[12] := 0; break;
		"Barbarian": excludelist[3] := 0; excludelist[9] := 0; break;
		"Dwarf": excludelist[3] := 0; excludelist[13] := 0; break;
		"Demon": excludelist[10] := 0; excludelist[11] := 0; break;
		"Vampire": excludelist[10] := 0; excludelist[11] := 0; break;
		"Human":  break;
	endcase
	
	GFInitGump( 40 , 40 );
	
	GFNoClose();
	GFNoDispose();
	GFNoMove();
	GFResizePic(0, 0, BKGND_STONESLAB, 400, 500);
	GFCheckerTrans(3, 3, 394, 20); // Top trans
	GFCheckerTrans(3, 26, 175, 443); // Left Trans
	GFCheckerTrans(181, 26 , 216, 443); // Right Trans
	GFCheckerTrans(3, 472, 394, 25); // Bottom Trans
	GFTextMid(10, 1, 380, 5, "Please Select A Class");
	var pageline := 50;
	var bardButton,crafterButton,mageButton,rangerButton,thiefButton,warriorButton,druidButton,dkButton,ninjaButton,paladinButton,priestButton,monkButton,necromancerButton,ppButton;
	//Left Menu
	GFTextLine(35, pageline, 38+(25*excludelist[1]), "Bard");
	if (excludelist[1] != 0) bardButton := GFButtonID( 20, pageline+3, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );endif
	pageline := pageline+30;
	
	GFTextLine(35, pageline, 38+(25*excludelist[2]), "Crafter");
	if (excludelist[2] != 0) crafterButton := GFButtonID( 20, pageline+3, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );endif
	pageline := pageline+30;
	
	GFTextLine(35, pageline, 38+(25*excludelist[3]), "Mage");
	if (excludelist[3] != 0) mageButton := GFButtonID( 20, pageline+3, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );endif
	pageline := pageline+30;
	
	GFTextLine(35, pageline, 38+(25*excludelist[4]), "Ranger");
	if (excludelist[4] != 0) rangerButton := GFButtonID( 20, pageline+3, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );endif
	pageline := pageline+30;
	
	GFTextLine(35, pageline, 38+(25*excludelist[5]), "Thief");
	if (excludelist[5] != 0) thiefButton := GFButtonID( 20, pageline+3, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );endif
	pageline := pageline+30;
	
	GFTextLine(35, pageline, 38+(25*excludelist[6]), "Warrior");
	if (excludelist[6] != 0) warriorButton := GFButtonID( 20, pageline+3, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );endif
	pageline := pageline+30;
	
	GFTextLine(35, pageline, 38+(25*excludelist[7]), "Druid");
	if (excludelist[7] != 0) druidButton := GFButtonID( 20, pageline+3, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );endif
	pageline := pageline+30;
	
	GFTextLine(35, pageline, 38+(25*excludelist[8]), "Deathknight");
	if (excludelist[8] != 0) dkButton := GFButtonID( 20, pageline+3, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );endif
	pageline := pageline+30;
	
	GFTextLine(35, pageline, 38+(25*excludelist[9]), "Ninja");
	if (excludelist[9] != 0) ninjaButton := GFButtonID( 20, pageline+3, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );endif
	pageline := pageline+30;
	
	GFTextLine(35, pageline, 38+(25*excludelist[10]), "Paladin");
	if (excludelist[10] != 0) paladinButton := GFButtonID( 20, pageline+3, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );endif
	pageline := pageline+30;
	
	GFTextLine(35, pageline, 38+(25*excludelist[11]), "Priest");
	if (excludelist[11] != 0) priestButton := GFButtonID( 20, pageline+3, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );endif
	pageline := pageline+30;
	
	GFTextLine(35, pageline, 38+(25*excludelist[12]), "Monk");
	if (excludelist[12] != 0) monkButton := GFButtonID( 20, pageline+3, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );endif
	pageline := pageline+30;
	
	GFTextLine(35, pageline, 38+(25*excludelist[13]), "Necromancer");
	if (excludelist[13] != 0) necromancerButton := GFButtonID( 20, pageline+3, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );endif
	pageline := pageline+30;
	
	GFTextLine(35, pageline, 63, "Power Player");
	ppButton := GFButtonID( 20, pageline+3, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	
	//Bottom Menu
	GFTextLine(315, 475, 63, "Confirm");
	var confirmButton  := GFButtonID( 300, 478, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	
	//Right Text
	var textline := 30;
	if ( class == "Home")
	GFTextMid(210, textline, 185, 53, "Use this page to select");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "your class.");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Click a button on the");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "left and read the info.");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "When you've decided");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "click confirm down the bottom.");
	elseif ( class == "Bard" )
		//Right Text - Bard
	textline := 30;
	GFTextMid(210, textline, 185, 5, "Bard Class");
	textline := textline +27;
	GFTextMid(210, textline, 185, 5, "Skills:");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Begging");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Cartography");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Enticement");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Herding");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Musicianship");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Peacemaking");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Provocation");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Taste Identification");
	elseif ( class == "Crafter" )
		//Right Text - Crafter
	textline := 30;
	GFTextMid(210, textline, 185, 5, "Crafter Class");
	textline := textline +27;
	GFTextMid(210, textline, 185, 5, "Skills:");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Arms lore");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Blacksmithy");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Bowcraft and Fletching");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Carpentry");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Lumberjacking");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Mining");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Tailoring");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Tinkering");
	elseif ( class == "Mage" )
		//Right Text - Mage
	textline := 30;
	GFTextMid(210, textline, 185, 5, "Mage Class");
	textline := textline +27;
	GFTextMid(210, textline, 185, 5, "Skills:");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Alchemy");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Evaluate Intelligence");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Inscription");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Item Identification");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Magery");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Meditation");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Magic Resistance");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Spirit Speak");
	elseif ( class == "Ranger" )
		//Right Text - Ranger
	textline := 30;
	GFTextMid(210, textline, 185, 5, "Ranger Class");
	textline := textline +27;
	GFTextMid(210, textline, 185, 5, "Skills:");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Animal Lore");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Animal Taming");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Archery");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Nature Lore");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Cooking");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Fishing");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Tracking");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Veterinary");
	elseif ( class == "Thief" )
		//Right Text - Thief
	textline := 30;
	GFTextMid(210, textline, 185, 5, "Thief Class");
	textline := textline +27;
	GFTextMid(210, textline, 185, 5, "Skills:");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Detect Hidden");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Hiding");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Lockpicking");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Poisoning");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Remove Trap");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Snooping");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Stealing");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Stealth");
	elseif ( class == "Warrior" )
		//Right Text - Warrior
	textline := 30;
	GFTextMid(210, textline, 185, 5, "Warrior Class");
	textline := textline +27;
	GFTextMid(210, textline, 185, 5, "Skills:");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Anatomy");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Fencing");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Healing");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Macefighting");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Parry");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Swordsmanship");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Tactics");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Wrestling");
	elseif ( class == "Druid" )
		//Right Text - Druid
	textline := 30;
	GFTextMid(210, textline, 185, 5, "Druid Class");
	textline := textline +27;
	GFTextMid(210, textline, 185, 5, "Skills:");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Alchemy");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Animal Lore");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Herding");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Magery");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Nature Lore");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Animal Taming");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Veterinary");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Meditation");
	elseif ( class == "DK" )
		//Right Text - Death Knight
	textline := 30;
	GFTextMid(210, textline, 185, 5, "Deathknight Class");
	textline := textline +27;
	GFTextMid(210, textline, 185, 5, "Skills:");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Anatomy");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Necromancy");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Healing");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Macefighting");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Parry");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Swordsmanship");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Tactics");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Meditation");
	elseif ( class == "Ninja" )
		//Right Text - Ninja
	textline := 30;
	GFTextMid(210, textline, 185, 5, "Ninja Class");
	textline := textline +27;
	GFTextMid(210, textline, 185, 5, "Skills:");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Alchemy");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Archery");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Fencing");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Hiding");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Poisoning");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Stealth");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Swordsmanship");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Tactics");
	elseif ( class == "Paladin" )
		//Right Text - Paladin
	textline := 30;
	GFTextMid(210, textline, 185, 5, "Paladin Class");
	textline := textline +27;
	GFTextMid(210, textline, 185, 5, "Skills:");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Anatomy");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Fencing");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Healing");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Meditation");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Parry");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Piety");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Swordsmanship");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Tactics");
	elseif ( class == "Priest" )
		//Right Text - Priest
	textline := 30;
	GFTextMid(210, textline, 185, 5, "Priest Class");
	textline := textline +27;
	GFTextMid(210, textline, 185, 5, "Skills:");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Healing");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Macefighting");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Magery");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Magic Resistance");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Meditation");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Parry");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Piety");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Spirit Speak");
	elseif ( class == "Monk" )
		//Right Text - Monk
	GFTextMid(210, textline, 185, 5, "Monk Class");
	textline := textline +27;
	GFTextMid(210, textline, 185, 5, "Skills:");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Hiding");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Macefighting");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Meditation");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Parry");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Psionics");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Stealth");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Tactics");
	textline := textline +17;
	GFTextMid(210, textline, 185, 53, "Wrestling");
	elseif ( class == "Necromancer" )
		//Right Text - Necromancer
		textline := 30;
		GFTextMid(210, textline, 185, 5, "Necromancer Class");
		textline := textline +27;
		GFTextMid(210, textline, 185, 5, "Skills:");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "Alchemy");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "Evaluate Inteligence");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "Forensics");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "Magery");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "Magic Resistance");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "Meditation");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "Necromancy");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "Spirit Speak");
	elseif ( class == "PP" )
		//Right Text - Necromancer
		textline := 30;
		GFTextMid(210, textline, 185, 5, "Power Player");
		textline := textline +27;
		GFTextMid(210, textline, 185, 5, "Skills:");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "All.");
		textline := textline +27;
		GFTextMid(210, textline, 185, 53, "Power players aren't a class");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "like other classes.");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "Their advantage is that");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "they don't receive any disadvantaged.");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "Their disadvantage however is that");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "they don't receive any advantages.");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "They make up for this by having");
		textline := textline +17;
		GFTextMid(210, textline, 185, 53, "access to all the skills.");
	endif
	var choice := GFSendGump(who);
	if (choice[confirmButton.keyid])
		buffskills(who, class);
	elseif (choice[bardButton.keyid])
		sendclassgump(who, "Bard");
	elseif (choice[crafterButton.keyid])
		sendclassgump(who, "Crafter");
	elseif (choice[mageButton.keyid])
		sendclassgump(who, "Mage");
	elseif (choice[rangerButton.keyid])
		sendclassgump(who, "Ranger");
	elseif (choice[thiefButton.keyid])
		sendclassgump(who, "Thief");
	elseif (choice[warriorButton.keyid])
		sendclassgump(who, "Warrior");
	elseif (choice[druidButton.keyid])
		sendclassgump(who, "Druid");
	elseif (choice[dkButton.keyid])
		sendclassgump(who, "DK");
	elseif (choice[ninjaButton.keyid])
		sendclassgump(who, "Ninja");
	elseif (choice[paladinButton.keyid])
		sendclassgump(who, "Paladin");
	elseif (choice[priestButton.keyid])
		sendclassgump(who, "Priest");
	elseif (choice[monkButton.keyid])
		sendclassgump(who, "Monk");
	elseif (choice[necromancerButton.keyid])
		sendclassgump(who, "Necromancer");
	elseif (choice[ppButton.keyid])
		sendclassgump(who, "PP");
	endif
	
	

endfunction

function buffskills(who, class)

	if (class == "Home")
		SendSysMessage(who, "You need to actually pick a class.");
		sendclassgump(who, "Home");
	endif
	
	var skillarray := array{};
	
	case (class)
		"Bard": skillarray := GetClasseSkills("IsBard");break;
		"Crafter": skillarray := GetClasseSkills("IsCrafter");break;
		"Mage": skillarray := GetClasseSkills("IsMage");break;
		"Ranger": skillarray := GetClasseSkills("IsRanger");break;
		"Thief": skillarray := GetClasseSkills("IsThief");break;
		"Warrior": skillarray := GetClasseSkills("IsWarrior");break;
		"Druid": skillarray := GetClasseSkills("IsDruid");break;
		"DK": skillarray := GetClasseSkills("IsDeathknight");break;
		"Ninja": skillarray := GetClasseSkills("IsNinja");break;
		"Paladin": skillarray := GetClasseSkills("IsPaladin");break;
		"Priest": skillarray := GetClasseSkills("IsPriest");break;
		"Monk": skillarray := GetClasseSkills("IsMonk");break;
		"Necromancer": skillarray := GetClasseSkills("IsNecromancer");break;
		"PP": skillarray := GetClasseSkills("IsPowerPlayer");break;
	endcase
	var skill_amount := 75;
	if (class == "PP")
		skill_amount := 40;
	endif
	for skillid := 0 to SKILLID__HIGHEST
            if (skillid in skillarray)
               SetBaseSkill(who, skillid, skill_amount);
            endif
  endfor
  SetClasse(who);
	
endfunction

function dropallskills(who)
	
	var skill_amount := 0;
	
	for skillid := 0 to SKILLID__HIGHEST
            if (skillid in skillarray)
               SetBaseSkill(who, skillid, skill_amount);
            endif
  endfor
endfunction
  