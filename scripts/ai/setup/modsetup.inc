use uo;
use os;
use cfgfile;
include "include/attributes";

var modnpc := Self();

if( modnpc.isA(POLCLASS_NPC) )
	var strmod := GetObjProperty( modnpc, "BaseStrmod" );
	if (strmod)
		AlterStrengthMod(modnpc, GetBaseStrength(modnpc) + strmod);
		//EraseObjProperty(modnpc, "BaseStrmod");
	endif
endif


if( modnpc.isA(POLCLASS_NPC) )
	var intmod := GetObjProperty( modnpc, "BaseIntmod" );
	if (intmod)
		AlterIntelligenceMod(modnpc, GetBaseIntelligence(modnpc) + intmod);
		//EraseObjProperty(modnpc, "BaseIntmod");
	endif
endif


if( modnpc.isA(POLCLASS_NPC) )
	var dexmod := GetObjProperty( modnpc, "BaseDexmod" );
	if (dexmod)
		AlterDexterityMod(modnpc, dexmod);
		//EraseObjProperty( modnpc, "BaseDexmod" );
	endif
endif

//RecalcVitals(modnpc);

if( GetObjProperty( modnpc, "BaseHpRegen" ) && GetHpRegenRate(modnpc) == 100 )
	var base_hp_regen := GetObjProperty( modnpc, "BaseHpRegen" );
	SetObjProperty( modnpc, "baseregen", base_hp_regen );
	SetHpRegenRate(modnpc , ( base_hp_regen) * 12);
endif


if( GetObjProperty( modnpc, "BaseManaRegen" ) && GetManaRegenRate(modnpc) == 100 )
	var base_mana_regen := GetObjProperty( modnpc, "BaseManaRegen" );
	SetObjProperty( modnpc, "basemanaregen", base_mana_regen );
	SetManaRegenRate(modnpc , ( base_mana_regen) * 12);
endif


if( GetObjProperty( modnpc, "BaseStaminaRegen" ) && GetStaminaRegenRate(modnpc) == 100 )
	var base_stamina_regen := GetObjProperty( modnpc, "BaseStaminaRegen" );
	SetObjProperty( modnpc, "basestaminaregen", base_stamina_regen );
	SetStaminaRegenRate(modnpc , ( base_stamina_regen) * 12);
endif

if( 1 )
	var equip_cfg := ReadConfigFile(":*:equip");
	if( equip_cfg )
		var equip_elem := FindConfigElem(equip_cfg, modnpc.npctemplate);
		if( equip_elem )
			EquipFromTemplate(modnpc, modnpc.npctemplate);
		endif
	endif
endif


if( !GetObjProperty( modnpc, "Karma" ) )
	var npc_cfg	:= ReadConfigFile( "::npcdesc" );
	var stat_tot	:= GetStrength(modnpc) + GetDexterity(modnpc) + GetIntelligence(modnpc);
	var ar	:= modnpc.ar + 3;
	var base	:= CInt(stat_tot * ar / 12);
	if( !base )
		base	:= 1;
	endif

	var multiply	:= 1;
	case( modnpc.script )
		"spellkillpcs":
		"firebreather":	multiply := 1.2;
				break;
		default:
	endcase

	base := CInt(base * multiply);
	
	var bonus := 0;
	foreach item in ListEquippedItems( modnpc )
		if( item.skillid )
			case( item.hitscript )
				":combat:mainhit":		break;
				":combat:banishscript":		bonus := 250;	break;
				":combat:poisonhit":		bonus := 100 + (GetObjProperty( item, "Poisonlvl" ) * 150);	break;
				":combat:piercingscript":	bonus := 500;
				":combat:voidscript":		bonus := 1000;
				":combat:lifedrainscript":	bonus := 250;
				":combat:manadrainscript":	bonus := 200;
				":combat:spellstrikescript":	bonus := 100 + (GetObjProperty( item, "EffectCircle" ) * 50);	break;
				default:
			endcase
		endif
	endforeach

	if( GetObjProperty( modnpc, "BaseHpRegen" ) )
		bonus := bonus + GetObjProperty( modnpc, "BaseHpRegen" );
	endif

	if( GetObjProperty( modnpc, "BaseManaRegen" ) )
		bonus := bonus + GetObjProperty( modnpc, "BaseManaRegen" );
	endif

	if( GetObjProperty( modnpc, "BaseStaminaRegen" ) )
		bonus := bonus + GetObjProperty( modnpc, "BaseStaminaRegen" );
	endif

	var possible_prots := { "PermPoisonImmunity",
			 	  "PermMagicImmunity",
			 	  "Permmr",
			 	  "FireProtection",
			 	  "WaterProtection",
			 	  "AirProtection",
			 	  "EarthProtection",
			 	  "FreeAction",
			 	  "NecroProtection",
			 	  "HolyProtection",
			 	  "PhysicalProtection",
			 	  "HealingIncrease" };
			 	 
	foreach prot in possible_prots
		if( GetObjProperty( modnpc, prot ) )
			bonus := bonus + (GetObjProperty( modnpc, prot ) * 100);
		endif
	endforeach

	base := base + bonus;
	if( base > 15000 )
		base := 15000;
	endif

	var karma := CInt( base * (Random(21)+90) / 100 );
	var fame  := CInt( base * (Random(21)+90) / 100 * 0.875 );

	case( npc_cfg[modnpc.npctemplate].alignment )
		"evil":		karma := 0 - karma;	break;
		"good":		break;
	
		default:	karma := 0 - karma;	break;
	endcase

	SetObjProperty( modnpc, "Karma", karma );
	SetObjProperty( modnpc, "Fame", fame  );
endif


if( !GetObjProperty( modnpc, "AiSerial" ) || GetObjProperty( modnpc, "AiSerial" ) != modnpc.serial )
	SetObjProperty( modnpc, "AiSerial", modnpc.serial );
endif


if( GetProperty( "Anchor" ) )
	var anchor := GetProperty( "Anchor" );
	SetAnchor( CInt(anchor.x), CInt(anchor.y), CInt(anchor.range), CInt(anchor.psub) );
endif         

// Sno changed the following on 17/1/03. Stops NPCs from fully regenerating when their script is restarted.

var dont_restart_vitals := GetObjProperty(modnpc, "dont_restart_vitals");

if (!dont_restart_vitals)
	SetHp(modnpc, GetMaxHp(modnpc));
	SetMana(modnpc, GetMaxMana(modnpc));
	SetStamina(modnpc, GetMaxStamina(modnpc));
else
	SetObjProperty(modnpc, "dont_restart_vitals", 0); // So next time it's restarted manually they DO reset
endif