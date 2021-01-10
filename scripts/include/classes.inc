use uo;

include "include/spelldata";
//include "include/attributes";
include "include/itemutil";
include "include/client";
include "include/classitems";
include "include/class_bonuses";
include "include/races";

const CLASSEID_BARD       := "IsBard";
const CLASSEID_CRAFTER    := "IsCrafter";
const CLASSEID_MAGE       := "IsMage";
const CLASSEID_RANGER     := "IsRanger";
const CLASSEID_THIEF      := "IsThief";
const CLASSEID_WARRIOR    := "IsWarrior";
const CLASSEID_DRUID    := "IsDruid";
const CLASSEID_DEATHKNIGHT    := "IsDeathknight";
const CLASSEID_NINJA    := "IsNinja";
const CLASSEID_PALADIN    := "IsPaladin";
const CLASSEID_PRIEST    := "IsPriest";
const CLASSEID_MONK    := "IsMonk";
const CLASSEID_NECROMANCER    := "IsNecromancer";
const CLASSEID_POWERPLAYER  := "IsPowerPlayer";

// COMMENTED OUT UNFOR NOW UNLESS LEVELS IS TO BE INCREASED
/*const LEVEL1 := 360;
const LEVEL2 := 460;
const LEVEL3 := 560;
const LEVEL4 := 660;
const LEVEL5 := 760;
const LEVEL6 := 860;
const LEVEL7 := 960;
const LEVEL8 := 1060;


const PERCENT1 := 60;
const PERCENT2 := 65;
const PERCENT3 := 70;
const PERCENT4 := 75;
const PERCENT5 := 80;
const PERCENT6 := 85;
const PERCENT7 := 90;
const PERCENT8 := 95;*/


const LEVEL1 := 600;
const LEVEL2 := 720;
const LEVEL3 := 840;
const LEVEL4 := 960;
const LEVEL5 := 1080;
const LEVEL6 := 1200;
const LEVEL7 := 1201;
const LEVEL8 := 1202;


const PERCENT1 := 60;
const PERCENT2 := 65;
const PERCENT3 := 75;
const PERCENT4 := 85;
const PERCENT5 := 95;
const PERCENT6 := 100;
const PERCENT7 := 110;
const PERCENT8 := 111;

const EACH_MUST_REPRESENT := 7.5;
const REPRESENT_LEVEL_MOD := 1;
const AVERAGE_SKILL       := 75;
const AVERAGE_LEVEL_MOD   := 15;
const PPSKILLNUMBER 		  := 52;

var counter:=0;

function ClasseBonus( who, classeid )

	var level := CDbl(GetObjProperty( who, classeid ) );

	if( level )
		return( 1 + BONUS_PER_LEVEL * level );
	else
		return( 1 );
	endif

endfunction


function ClasseBonusByLevel( level )

	return CDbl( 1.0 + ( BONUS_PER_LEVEL * CDbl(level) ) );

endfunction

function ClasseSmallBonusByLevel( level )

	return CDbl( 1.0 + ( SMALL_BONUS_PER_LEVEL * CDbl(level) ) );

endfunction

function ClasseLargeBonusByLevel( level )

	return CDbl( 1.0 + ( LARGE_BONUS_PER_LEVEL * CDbl(level) ) );

endfunction


function ClasseBonusBySkillId( who, skillid )

	return( ClasseBonusByLevel( IsSpecialisedIn( who, skillid ) ) );

endfunction


function EnumerateRestrictedItemTypesFromClasse( classeid )

	var fract_array := {};
	var restricted_types := {};

	case( classeid )

		CLASSEID_BARD:		fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					break;

		CLASSEID_CRAFTER:	fract_array.append( GetChainmailArmorGraphics() );
					fract_array.append( GetPlatemailArmorGraphics() );
					break;

		CLASSEID_MAGE:		fract_array.append( GetLeatherArmorGraphics() );
					fract_array.append( GetStuddedLeatherArmorGraphics() );
					fract_array.append( GetBoneArmorGraphics() );
					fract_array.append( GetRingmailArmorGraphics() );
					fract_array.append( GetChainmailArmorGraphics() );
					fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					break;

		CLASSEID_NECROMANCER:	fract_array.append( GetStuddedLeatherArmorGraphics() );
					fract_array.append( GetRingmailArmorGraphics() );
					fract_array.append( GetChainmailArmorGraphics() );
					fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					break;

		CLASSEID_RANGER:	fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					break;

		CLASSEID_THIEF:		fract_array.append( GetBoneArmorGraphics() );
					fract_array.append( GetRingmailArmorGraphics() );
					fract_array.append( GetChainmailArmorGraphics() );
					fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					break;

		CLASSEID_NINJA:		fract_array.append( GetBoneArmorGraphics() );
					fract_array.append( GetRingmailArmorGraphics() );
					fract_array.append( GetChainmailArmorGraphics() );
					fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					break;

		CLASSEID_DRUID:		fract_array.append( GetBoneArmorGraphics() );
					fract_array.append( GetRingmailArmorGraphics() );
					fract_array.append( GetChainmailArmorGraphics() );
					fract_array.append( GetPlatemailArmorGraphics() );
					break;

		CLASSEID_MONK:		fract_array.append( GetLeatherArmorGraphics() );
					fract_array.append( GetStuddedLeatherArmorGraphics() );
					fract_array.append( GetBoneArmorGraphics() );
					fract_array.append( GetRingmailArmorGraphics() );
					fract_array.append( GetChainmailArmorGraphics() );
					fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					break;

		CLASSEID_PALADIN:	fract_array.append( GetLeatherArmorGraphics() );
					fract_array.append( GetStuddedLeatherArmorGraphics() );
					fract_array.append( GetBoneArmorGraphics() );
					break;

		CLASSEID_DEATHKNIGHT:	fract_array.append( GetLeatherArmorGraphics() );
					fract_array.append( GetStuddedLeatherArmorGraphics() );
					break;

		CLASSEID_PRIEST:	fract_array.append( GetPlatemailArmorGraphics() );

	endcase


	foreach type in fract_array
		foreach graphic in type
			restricted_types.append( graphic );
		endforeach
	endforeach

	return restricted_types;

endfunction


function GetClasseIdForSkill( skillid )

	var classArray := {};
	foreach id in GetClasseIds()
		if( skillid in GetClasseSkills( id ) )
			classArray.append(id);
		endif
	endforeach

	if (classArray.size() > 0)
		return classArray;
	else
		return {0};
	endif

endfunction


function GetClasseIds()

	return { CLASSEID_BARD,
		 CLASSEID_CRAFTER,
		 CLASSEID_MAGE,
		 CLASSEID_RANGER,
		 CLASSEID_THIEF,
		 CLASSEID_WARRIOR/*,
		 CLASSEID_DRUID,
		CLASSEID_DEATHKNIGHT,
		CLASSEID_NINJA,
		CLASSEID_NECROMANCER,
		CLASSEID_POWERPLAYER,
		CLASSEID_MONK,
		CLASSEID_PALADIN,
		CLASSEID_PRIEST*/};

endfunction


function GetClasseName( classeid )

        case( classeid )

                CLASSEID_BARD:		return( "Bard" );
                CLASSEID_CRAFTER:	return( "Crafter" );
                CLASSEID_MAGE:		return( "Mage" );
                CLASSEID_RANGER:	return( "Ranger" );
                CLASSEID_THIEF:		return( "Thief" );
                CLASSEID_WARRIOR:	return( "Warrior" );
                CLASSEID_DRUID:		return( "Druid" );
		CLASSEID_DEATHKNIGHT:	return( "Deathknight" );
		CLASSEID_NINJA:		return( "Ninja" );
		CLASSEID_POWERPLAYER:	return( "Power Player" );
		CLASSEID_NECROMANCER:	return ("Necromancer");
		CLASSEID_MONK:		return( "Monk" );
		CLASSEID_PALADIN:	return( "Paladin" );
		CLASSEID_PRIEST:	return ("Priest");
                default:		return( "INVALID" );

        endcase

endfunction


function GetClasseSkills( classeid )

	case( classeid )

		CLASSEID_BARD:		return { SKILLID_BEGGING,
						 SKILLID_CARTOGRAPHY,
						 SKILLID_ENTICEMENT,
						 SKILLID_HERDING,
						 SKILLID_MUSICIANSHIP,
						 SKILLID_PEACEMAKING,
						 SKILLID_PROVOCATION,
						 SKILLID_TASTEID	};

		CLASSEID_CRAFTER:	return { SKILLID_ARMSLORE,
						 SKILLID_BLACKSMITHY,
						 SKILLID_BOWCRAFT,
						 SKILLID_CARPENTRY,
						 SKILLID_LUMBERJACKING,
						 SKILLID_MINING,
						 SKILLID_TAILORING,
						 SKILLID_TINKERING	};

		CLASSEID_MAGE:		return { SKILLID_ALCHEMY,
						 SKILLID_EVALINT,
						 SKILLID_INSCRIPTION,
						 SKILLID_ITEMID,
						 SKILLID_MAGERY,
						 SKILLID_MEDITATION,
						 SKILLID_MAGICRESISTANCE,
						 SKILLID_SPIRITSPEAK	};

		CLASSEID_RANGER:	return { SKILLID_ANIMALLORE,
						 SKILLID_TAMING,
						 SKILLID_ARCHERY,
						 SKILLID_NATURELORE,
						 SKILLID_COOKING,
						 SKILLID_FISHING,
						 SKILLID_TRACKING,
						 SKILLID_VETERINARY	};

		CLASSEID_THIEF:		return { SKILLID_DETECTINGHIDDEN,
						 SKILLID_HIDING,
						 SKILLID_LOCKPICKING,
						 SKILLID_POISONING,
						 SKILLID_REMOVETRAP,
						 SKILLID_SNOOPING,
						 SKILLID_STEALING,
						 SKILLID_STEALTH	};

		CLASSEID_WARRIOR:	return { SKILLID_ANATOMY,
						 SKILLID_FENCING,
						 SKILLID_HEALING,
						 SKILLID_MACEFIGHTING,
						 SKILLID_PARRY,
						 SKILLID_SWORDSMANSHIP,
						 SKILLID_TACTICS,
						 SKILLID_WRESTLING	};

		CLASSEID_DRUID:    	return { SKILLID_ALCHEMY,
						 SKILLID_ANIMALLORE,
						 SKILLID_HERDING,
						 SKILLID_MAGERY,
						 SKILLID_NATURELORE,
						 SKILLID_TAMING,
						 SKILLID_VETERINARY,
						 SKILLID_MEDITATION	};

		CLASSEID_NINJA:		return { SKILLID_STEALTH,
						 SKILLID_HIDING,
						 SKILLID_SWORDSMANSHIP,
						 SKILLID_ARCHERY,
						 SKILLID_FENCING,
						 SKILLID_TACTICS,
						 SKILLID_ALCHEMY,
						 SKILLID_POISONING	};

		CLASSEID_DEATHKNIGHT:	return { SKILLID_ANATOMY,
						 SKILLID_NECROMANCY,
						 SKILLID_HEALING,
						 SKILLID_MACEFIGHTING,
						 SKILLID_PARRY,
						 SKILLID_SWORDSMANSHIP,
						 SKILLID_TACTICS,
						 SKILLID_MEDITATION	};

		CLASSEID_NECROMANCER: 	return { SKILLID_NECROMANCY,
						 SKILLID_MAGERY,
						 SKILLID_MEDITATION,
						 SKILLID_MAGICRESISTANCE,
						 SKILLID_EVALINT,
						 SKILLID_SPIRITSPEAK,
						 SKILLID_FORENSICS,
						 SKILLID_ALCHEMY	};

		CLASSEID_MONK:		return { SKILLID_HIDING,
						 SKILLID_STEALTH,
						 SKILLID_WRESTLING,
						 SKILLID_MACEFIGHTING,
						 SKILLID_PSIONICS,
						 SKILLID_MEDITATION,
						 SKILLID_TACTICS,
						 SKILLID_PARRY		};

		CLASSEID_PALADIN:	return { SKILLID_SWORDSMANSHIP,
						 SKILLID_FENCING,
						 SKILLID_HEALING,
						 SKILLID_ANATOMY,
						 SKILLID_PARRY,
						 SKILLID_TACTICS,
						 SKILLID_PIETY,
						 SKILLID_MEDITATION	};

		CLASSEID_PRIEST: 	return { SKILLID_PIETY,
						 SKILLID_MEDITATION,
						 SKILLID_MAGERY,
						 SKILLID_SPIRITSPEAK,
						 SKILLID_MACEFIGHTING,
						 SKILLID_HEALING,
						 SKILLID_PARRY,
						 SKILLID_MAGICRESISTANCE};

		CLASSEID_POWERPLAYER:
					return { SKILLID_BEGGING,
						 SKILLID_CARTOGRAPHY,
						 SKILLID_ENTICEMENT,
						 SKILLID_HERDING,
						 SKILLID_MUSICIANSHIP,
						 SKILLID_PEACEMAKING,
						 SKILLID_PROVOCATION,
						 SKILLID_TASTEID,
						 SKILLID_ARMSLORE,
						 SKILLID_BLACKSMITHY,
						 SKILLID_BOWCRAFT,
						 SKILLID_CARPENTRY,
						 SKILLID_LUMBERJACKING,
						 SKILLID_MINING,
						 SKILLID_TAILORING,
						 SKILLID_TINKERING,
						 SKILLID_ALCHEMY,
						 SKILLID_EVALINT,
						 SKILLID_INSCRIPTION,
						 SKILLID_ITEMID,
						 SKILLID_MAGERY,
						 SKILLID_MEDITATION,
						 SKILLID_MAGICRESISTANCE,
						 SKILLID_SPIRITSPEAK,
						 SKILLID_ANIMALLORE,
						 SKILLID_TAMING,
						 SKILLID_ARCHERY,
						 SKILLID_CAMPING,
						 SKILLID_COOKING,
						 SKILLID_FISHING,
						 SKILLID_TRACKING,
						 SKILLID_VETERINARY,
						 SKILLID_DETECTINGHIDDEN,
						 SKILLID_HIDING,
						 SKILLID_LOCKPICKING,
						 SKILLID_POISONING,
						 SKILLID_REMOVETRAP,
						 SKILLID_SNOOPING,
						 SKILLID_STEALING,
						 SKILLID_STEALTH,
						 SKILLID_ANATOMY,
						 SKILLID_FENCING,
						 SKILLID_HEALING,
						 SKILLID_MACEFIGHTING,
						 SKILLID_PARRY,
						 SKILLID_SWORDSMANSHIP,
						 SKILLID_TACTICS,
						 SKILLID_WRESTLING,
						 SKILLID_FORENSICS,
						 SKILLID_NATURELORE,
						 SKILLID_NECROMANCY,
						 SKILLID_PIETY,
						 SKILLID_PSIONICS	};


		default:		return {};

	endcase

endfunction


function GetGeneralSkills()

	return { SKILLID_BEGGING,
		 SKILLID_ARMSLORE,
		 SKILLID_MAGICRESISTANCE,
		 SKILLID_ARCHERY,
		 SKILLID_HIDING,
		 SKILLID_TACTICS	};

endfunction


function GetMaxCircle( who )

	foreach classeid in GetClasseIds()
		if( GetObjProperty( who, classeid ) )
			return GetMaxCircleForClasse( classeid );
		endif
	endforeach

	return 0;

endfunction


function GetMaxCircleForClasse( classeid )

	case( classeid )

		CLASSEID_BARD:		return 10;

		CLASSEID_CRAFTER:	return 6;

		CLASSEID_MAGE:		return 0;

		CLASSEID_NECROMANCER:	return 0;

		CLASSEID_RANGER:	return 12;

		CLASSEID_THIEF:		return 8;

		CLASSEID_NINJA:		return 8;

		CLASSEID_WARRIOR:	return 4;

		CLASSEID_DRUID:		return 0;

		CLASSEID_DEATHKNIGHT: 	return 12;

		CLASSEID_POWERPLAYER: 	return 0;

		CLASSEID_MONK:		return 0;

		CLASSEID_PALADIN: 	return 12;

		CLASSEID_PRIEST: 	return 0;

		default:		return 0;

	endcase

endfunction


function GetSkillPointsMultiplier( who, skillid )

	return ClasseBonusByLevel( IsSpecialisedIn( who, skillid ) );

endfunction


function GetStatPointsMultiplier( who, stat )

	var level   := HaveStatAffinity( who, stat );
	var mult    := 1.0;
	if( level )
		mult  := ClasseBonusByLevel( level );
	else
		level := HaveStatDifficulty( who, stat );
		if( level )
			mult := CDbl( 1.0 / ClasseBonusByLevel( level ) );
		endif
	endif

	return mult;

endfunction


function HaveInvalidSkillEnchantmentForClasse( unused item, unused classeid )

         // Removed Class skill requirement for equipping skill items 12/6/4 Archy

	/* var ret	:= 0;
	var skillnum	:= GetObjProperty( item, "skilladv" );

	if( (skillnum) || (skillnum == 0) && !(skillnum in GetGeneralSkills()) )
		if( !(skillnum in GetClasseSkills( classeid )) )
			ret := 1;
		endif
	endif

	return ret; */

        return 0;

endfunction


function HaveRestrictedModEnchantmentForClasse( unused who, unused item, unused classeid )

	var ret := 0;
	/* removed for new ES system
	case( classeid )

		CLASSEID_BARD:		break;

		CLASSEID_CRAFTER:	break;

		CLASSEID_MAGE:		break;

		CLASSEID_NECROMANCER:	break;

		CLASSEID_RANGER:	break;

		CLASSEID_THIEF:		break;

		CLASSEID_POWERPLAYER: 	break;

		CLASSEID_DRUID:		break;

		CLASSEID_NINJA:		break;

		CLASSEID_DEATHKNIGHT:	break;

		CLASSEID_WARRIOR:	if( GetObjProperty( item, "MagicImmunity" ) )
						ret := 1;
					elseif( GetObjProperty( item, "SpellReflect" ) )
						ret := 1;
					elseif( GetObjProperty( item, "PermMagicImmunity" ) )
						ret := 1;
					elseif( GetObjProperty( item, "Permmr" ) )
						ret := 1;
					endif
					break;

		CLASSEID_MONK:		break;

		CLASSEID_PRIEST:	break;

		CLASSEID_PALADIN:	break;
	endcase
	*/
	return ret;

endfunction


function HaveStatAffinity( who, stat )

	var classeids;

	case( stat )
		"str":	classeids := {  CLASSEID_CRAFTER,
					CLASSEID_WARRIOR,
					CLASSEID_PALADIN,
					CLASSEID_DEATHKNIGHT,
					CLASSEID_MONK 	};	break;

		"dex":	classeids := {  CLASSEID_BARD,
					CLASSEID_THIEF,
					CLASSEID_NINJA,
					CLASSEID_MONK  };	break;

		"int":	classeids := {  CLASSEID_BARD,
					CLASSEID_DRUID,
					CLASSEID_MAGE,
					CLASSEID_NECROMANCER,
					CLASSEID_PRIEST,
					CLASSEID_MONK	};	break;

		default:	classeids := {};
	endcase

	foreach id in classeids
		var level := CInt( GetObjProperty( who, id ) );
		if( level )
			return level;
		endif
	endforeach

	return 0;

endfunction


function HaveStatDifficulty( who, stat )

	var classeids, level;

	case( stat )
		"str":
			classeids := {
				CLASSEID_BARD,
				CLASSEID_MAGE,
				CLASSEID_NECROMANCER	}; break;
		"dex":
			classeids := {
				CLASSEID_PALADIN,
				CLASSEID_DEATHKNIGHT,
				CLASSEID_PRIEST		};	break;
		"int":
			classeids := {
				CLASSEID_CRAFTER,
				CLASSEID_WARRIOR	};	break;
		default:
			classeids := {			};
	endcase

	foreach id in classeids
		level := CInt( GetObjProperty( who, id ) );
		if( level )
			return level;
		endif
	endforeach

	return 0;

endfunction


function IsSpecialisedIn( who, skillid )

  foreach classeid in GetClasseIdForSkill( skillid )
	if( classeid )
		if(GetObjProperty(who, classeid))
			return CInt(GetObjProperty(who, classeid));
		endif
	endif
  endforeach

endfunction


function IsBard( who )
	return GetObjProperty(who, CLASSEID_BARD );
endfunction
function IsCrafter( who )
	return GetObjProperty(who, CLASSEID_CRAFTER );
endfunction
function IsMage( who )
	return GetObjProperty(who, CLASSEID_MAGE );
endfunction
function IsRanger( who )
	return GetObjProperty(who, CLASSEID_RANGER );
endfunction
function IsThief( who )
	return GetObjProperty(who, CLASSEID_THIEF );
endfunction
function IsWarrior( who )
	return GetObjProperty(who, CLASSEID_WARRIOR );
endfunction
function IsDruid( who )
	return GetObjProperty(who, CLASSEID_DRUID );
endfunction
function IsPowerPlayer(who)
	return GetObjProperty(who, CLASSEID_POWERPLAYER );
endfunction
function IsNinja( who )
	return GetObjProperty(who, CLASSEID_NINJA );
endfunction
function IsDeathknight( who )
	return GetObjProperty(who, CLASSEID_DEATHKNIGHT );
endfunction
function IsNecromancer( who )
	return GetObjProperty(who, CLASSEID_NECROMANCER );
endfunction
function IsMonk( who )
	return GetObjProperty(who, CLASSEID_MONK );
endfunction
function IsPaladin( who )
	return GetObjProperty(who, CLASSEID_PALADIN );
endfunction
function IsPriest( who )
	return GetObjProperty(who, CLASSEID_PRIEST );
endfunction


function IsFromThatClasse( who, classe_skills )

	var classe	:= 0;
	var total	:= 0;
	var number := len( classe_skills );


	for i := 0 to SKILLID__HIGHEST
		var amount := GetEffectiveSkill( who, i );
		total := total + amount;
		if( i in classe_skills )
			classe := classe + amount;
		endif
	endfor

	/*Does a fast check on Powerplayers because their levels are determined slightly differently*/

	if (number == PPSKILLNUMBER)
		if(classe>=5880)
			return 4;
		elseif(classe>=5145)
			return 3;
		elseif(classe>=4410)
			return 2;
		elseif(classe>=3675)
			return 1;
		endif
		return 0;
	endif

	if( classe < AVERAGE_SKILL * number )
		return 0;
	elseif( classe < CInt(total * number * EACH_MUST_REPRESENT * 0.01) )
		return 0;

	else
		var level     := 1;
		var represent := EACH_MUST_REPRESENT + REPRESENT_LEVEL_MOD;
		var percent   := CInt( total * number * represent * 0.01 );
		var average_t := CInt( (AVERAGE_SKILL + AVERAGE_LEVEL_MOD) * number );
		while( (classe >= average_t) && (classe >= percent) )
			level     := level + 1;
			represent := CDbl( represent + REPRESENT_LEVEL_MOD );
			percent   := CInt( total * number * represent * 0.01 );
			average_t := CInt( average_t + AVERAGE_LEVEL_MOD * number );
		endwhile

		return level;
	endif

endfunction

function IsProhibitedClasseItem(who, item)
	var item_level, player_level;

	foreach CLASSEID in GetClasseIds()
		item_level := CInt(GetObjProperty(item, CLASSEID));
		if (item_level)
			player_level := CInt(GetObjProperty(who, CLASSEID));
			if (item_level>0)
				return (player_level < item_level);       // If the item level is positive
			else					          // player level >= item level to wear.

				return (player_level >= abs(item_level)); // If the item level is negative
			endif						  // the player level must be less than the
		endif							  // absolute value of that to wear it.
	endforeach
endfunction

function IsProhibitedByClasse( who, item )

	// *** NOTE *** Item mod restrictions commented out for ES system
	var ret := 0;

	if( GetObjProperty( who, CLASSEID_BARD	) )
		if( HaveInvalidSkillEnchantmentForClasse( item, CLASSEID_BARD ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_BARD ) )
			ret := 1;
		endif

	elseif( GetObjProperty( who, CLASSEID_CRAFTER ) )
		if( HaveInvalidSkillEnchantmentForClasse( item, CLASSEID_CRAFTER ) )
			ret := 1;
		//elseif( GetObjProperty( item, "str" ) )
		//	ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_CRAFTER ) )
			ret := 1;
		endif

	elseif( GetObjProperty( who, CLASSEID_MAGE ) )
		if( HaveInvalidSkillEnchantmentForClasse( item, CLASSEID_MAGE ) )
			ret := 1;
		//elseif( GetObjProperty( item, "str" ) )
		//	ret := 1;
		//elseif( GetObjProperty( item, "ArBonus" ) )
		//	ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_MAGE ) )
			ret := 1;
		endif

	elseif( GetObjProperty( who, CLASSEID_RANGER ) )
		if( HaveInvalidSkillEnchantmentForClasse( item, CLASSEID_RANGER ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_RANGER ) )
			ret := 1;
		endif

	elseif( GetObjProperty( who, CLASSEID_THIEF ) )
		if( HaveInvalidSkillEnchantmentForClasse( item, CLASSEID_THIEF ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_THIEF ) )
			ret := 1;
		endif

	elseif( GetObjProperty( who, CLASSEID_WARRIOR ) )
		if( HaveInvalidSkillEnchantmentForClasse( item, CLASSEID_WARRIOR ) )
			ret := 1;
		//elseif( GetObjProperty( item, "int" ) )
		//	ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_WARRIOR ) )
			ret := 1;
		elseif( HaveRestrictedModEnchantmentForClasse( who, item, CLASSEID_WARRIOR ) )
			ret := 1;
		endif

	elseif( GetObjProperty( who, CLASSEID_DRUID ) )
		if( HaveInvalidSkillEnchantmentForClasse( item, CLASSEID_DRUID ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_DRUID ) )
			ret := 1;
		endif

	elseif( GetObjProperty( who, CLASSEID_NINJA ) )
		if( HaveInvalidSkillEnchantmentForClasse( item, CLASSEID_NINJA ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_NINJA ) )
			ret := 1;
		endif

	elseif( GetObjProperty( who, CLASSEID_DEATHKNIGHT ) )
		if( HaveInvalidSkillEnchantmentForClasse( item, CLASSEID_DEATHKNIGHT ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_DEATHKNIGHT ) )
			ret := 1;
		endif
		if (item.isa(POLCLASS_WEAPON))
			if (item.attribute == "Archery")
				ret:=1;
			endif
		endif
	elseif( GetObjProperty( who, CLASSEID_NECROMANCER ) )
		if( HaveInvalidSkillEnchantmentForClasse( item, CLASSEID_NECROMANCER ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_NECROMANCER ) )
			ret := 1;
		endif

	elseif( GetObjProperty( who, CLASSEID_MONK ) )
		if( HaveInvalidSkillEnchantmentForClasse( item, CLASSEID_MONK ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_MONK ) )
			ret := 1;
		elseif (item.isa(POLCLASS_WEAPON) && !(item.graphic in VALID_MONK_WEAPS) )
			ret := 1;
		endif

	elseif( GetObjProperty( who, CLASSEID_PALADIN ) )
		if( HaveInvalidSkillEnchantmentForClasse( item, CLASSEID_PALADIN ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_PALADIN ) )
			ret := 1;
		elseif (item.isa(POLCLASS_WEAPON) )
			if ( !(item.graphic in VALID_PALADIN_WEAPS) )
				ret := 1;
			endif
			case (item.hitscript)
				"slayerscript":		if (GetObjProperty(item, "SlayType") == "Human");
								ret:=1;
							endif
				"piercing":
				"poisonhit":
				"lifedrainscript":
				"staminadrainscript":
				"blindinghit":
				"voidscript":		ret:=1;
			endcase
		endif

	elseif( GetObjProperty( who, CLASSEID_PRIEST ) )
		if( HaveInvalidSkillEnchantmentForClasse( item, CLASSEID_PRIEST ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_PRIEST) )
			ret := 1;
		endif
		if (item.isa(POLCLASS_WEAPON))
			if (item.attribute != "Macefighting")
				ret:=1;
			endif
		endif
	endif
	return ret;

endfunction


function AssignClasse( character )

	var t;

	foreach classe in GetClasseIds()
		t := IsFromThatClasse( character, GetClasseSkills( classe ) );
		if( t )
			SetObjProperty( character, classe, t );
		endif
	endforeach

endfunction

function GetClass(who)
	//ripped and modified from arch :) ... returns current class
	foreach key in GetClasseIds()
		if (GetObjProperty(who, key))
			return key;
		endif
	endforeach
	return 0; //no class found

endfunction

// New version of IsFromThatClasse - returns CLASSEID if classed, 0 otherwise
function SetClasse (who)

        var percentage:=0;
        var classtotal:=0;
        var playerclass:=0;
        var classlist := GetClasseIds();
        var skilltotal := GetSkillTotal(who);
        var tempclasstotal;
        // get class
        foreach class in classlist
          if (class != CLASSEID_POWERPLAYER)
            tempclasstotal:=GetClasseSkillTotal(who, class);
            if (tempclasstotal > classtotal && !IsProhibitedClasseForRace(class, who.title_race))
               playerclass:= class;
               classtotal:= tempclasstotal;
            endif
            //broadcast(class+" checked");
          endif
        endforeach
        percentage:=GetPercentage(classtotal, skilltotal);
        // get level
        var level:=0;
	if (skilltotal >= 2500)
		level:= CInt((skilltotal-2000) / 500);
		if (level > 8)
			level:=8;
		endif
           	playerclass:=CLASSEID_POWERPLAYER;
        else
             if (classtotal >= LEVEL8 && percentage >=PERCENT8)
                    level:=8;
             elseif (classtotal >= LEVEL7 && percentage >=PERCENT7)
                    level:=7;
             elseif (classtotal >= LEVEL6 && percentage >=PERCENT6)
                    level:=6;
             elseif (classtotal >= LEVEL5 && percentage >=PERCENT5)
                    level:=5;
             elseif (classtotal >= LEVEL4 && percentage >=PERCENT4)
                    level:=4;
             elseif (classtotal >= LEVEL3 && percentage >=PERCENT3)
                    level:=3;
             elseif (classtotal >= LEVEL2 && percentage >=PERCENT2)
                    level:=2;
             elseif (classtotal >= LEVEL1 && percentage >=PERCENT1)
                    level:=1;
             endif
        endif

        // apply race bonus
       	SetRaceProt(who);

        // set class
	foreach class in classlist
		EraseObjProperty(who, class);
	endforeach
        if (level)
           SetObjProperty(who, playerclass, level);
           return playerclass;
        endif
        return 0;

endfunction

function GetSkillTotal (who)
         var total:=0;
         for i:= 0 to SKILLID__HIGHEST
             total:=total + CDbl(GetEffectiveSkillBaseValue(who, i)) / 10;
         endfor
         return total;
endfunction

// returns the total skills in given class for that player
function GetClasseSkillTotal (who, class)

         var classtotal:=0;
         var skills:=GetClasseSkills(class);
         for i:= 0 to SKILLID__HIGHEST
             if (i in skills)
                   classtotal:=classtotal+ CDbl(GetEffectiveSkillBaseValue(who, i)) /10;
             endif

         endfor

        // broadcast(class+": "+classtotal);
        return classtotal;

endfunction

// Returns the % of class skills to total skills for class
function GetClasseSkillPercentage(who, class)

         var percentage:=0;
         percentage:= (CDbl(GetClasseSkillTotal(who, class)) / CDbl(GetSkillTotal(who))) * 100;
          //        broadcast("SHOULDN'T SEE THIS");
         return percentage;

endfunction

// simpler version of GetClasseSkillPercentage to save on cpu time
function GetPercentage(classtotal, skilltotal)
         return (CDbl(classtotal) / CDbl(skilltotal)) * 100;
endfunction

// Returns class level (regardless of class)
function GetClasseLevel(who)

	var class:= GetClass(who);
	if (class)
		return GetObjProperty(who, class);
	else
		return 0;
	endif

endfunction

function GetAvailableClasseCmds(who)

	var class:=GetClass(who);
	var level:=GetClasseLevel(who);
	var cmdarray:= array;

	case (class)

		CLASSEID_MONK:		cmdarray.append(".stunmode");
					cmdarray.append(".disarmmode");
		CLASSEID_DEATHKNIGHT:	if (level >=GET_STEED_AT_LEVEL )
						cmdarray.append(".summonsteed");
					endif
					if (level >=GET_RISE_AT_LEVEL )
						cmdarray.append(".rise");
					endif
		CLASSEID_PALADIN:	if (level >=GET_STEED_AT_LEVEL )
						cmdarray.append(".summonsteed");
					endif
					if (level >=GET_RISE_AT_LEVEL )
						cmdarray.append(".rise");
					endif
		CLASSEID_NINJA:		if (level >=GET_ESCAPE_AT_LEVEL )
						cmdarray.append(".escape");
					endif
		CLASSEID_NECROMANCER:	if (level >=GET_POSSESS_UNDEAD_AT_LEVEL)
						cmdarray.append(".possessundead");
					endif
	endcase

	return cmdarray;

endfunction

function GetElementalAffinityByClasse(class, dmgtype)

	var affinity:=0;
	case (dmgtype)
		FIRE:	case(class)
				CLASSEID_DRUID:		affinity:=LARGE_BONUS_PER_LEVEL;
				CLASSEID_MAGE:
				CLASSEID_NECROMANCER:	affinity:=BONUS_PER_LEVEL;
				CLASSEID_NINJA:
				CLASSEID_WARRIOR:	affinity:=PENALTY_PER_LEVEL;
			endcase

		AIR:	case(class)
				CLASSEID_DRUID:		affinity:=LARGE_BONUS_PER_LEVEL;
				CLASSEID_NECROMANCER:
				CLASSEID_MAGE:		affinity:=BONUS_PER_LEVEL;
				CLASSEID_NINJA:
				CLASSEID_WARRIOR:	affinity:=PENALTY_PER_LEVEL;
			endcase

		EARTH:	case(class)
				CLASSEID_DRUID:		affinity:=LARGE_BONUS_PER_LEVEL;
				CLASSEID_MAGE:
				CLASSEID_NECROMANCER:	affinity:=BONUS_PER_LEVEL;
				CLASSEID_NINJA:
				CLASSEID_WARRIOR:	affinity:=PENALTY_PER_LEVEL;
			endcase

		WATER:	case(class)
				CLASSEID_DRUID:		affinity:=LARGE_BONUS_PER_LEVEL;
				CLASSEID_NECROMANCER:
				CLASSEID_MAGE:		affinity:=BONUS_PER_LEVEL;
				CLASSEID_NINJA:
				CLASSEID_WARRIOR:	affinity:=PENALTY_PER_LEVEL;
			endcase

		NECRO:	case(class)
				CLASSEID_NECROMANCER:	affinity:=LARGE_BONUS_PER_LEVEL;
				CLASSEID_DEATHKNIGHT:
				CLASSEID_MAGE:		affinity:=BONUS_PER_LEVEL;
				CLASSEID_WARRIOR:
				CLASSEID_PRIEST:
				CLASSEID_DRUID:
				CLASSEID_NINJA:
				CLASSEID_PALADIN:	affinity:=PENALTY_PER_LEVEL;
			endcase

		HOLY:	case(class)
				CLASSEID_PRIEST:	affinity:=LARGE_BONUS_PER_LEVEL;
				CLASSEID_PALADIN:
				CLASSEID_MAGE:		affinity:=BONUS_PER_LEVEL;
				CLASSEID_NECROMANCER:
				CLASSEID_DEATHKNIGHT:
				CLASSEID_DRUID:
				CLASSEID_NINJA:
				CLASSEID_WARRIOR:	affinity:=PENALTY_PER_LEVEL;
			endcase

		ACID:	case(class)
				CLASSEID_DRUID:		affinity:=LARGE_BONUS_PER_LEVEL;
				CLASSEID_MAGE:		affinity:=BONUS_PER_LEVEL;
				CLASSEID_NINJA:
				CLASSEID_WARRIOR:	affinity:=PENALTY_PER_LEVEL;
			endcase
	endcase

	return affinity;

endfunction

function GetResistanceMultiplier (who, dmgtype)

	var class := GetClass(who);
	var level := GetClasseLevel(who);

	var mult := GetElementalAffinityByClasse(class, dmgtype);
	return level * mult;

endfunction

function db(who, text)

	if (GetObjProperty(who, "debugmode"))
		SendSysMessage(who, text);
	endif

endfunction

function GetsNoArmorPenalty(who)

	var class:=GetClass(who);
	case (class)
		CLASSEID_PRIEST:
		CLASSEID_NECROMANCER:
		CLASSEID_DRUID:
		CLASSEID_RANGER:
		CLASSEID_PALADIN:
		CLASSEID_DEATHKNIGHT:	return 1;
		default:		return 0;
	endcase

endfunction