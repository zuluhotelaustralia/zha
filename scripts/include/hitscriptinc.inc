// HitScript Include

use uo;

include "include/client";
//include "include/attributes";
include "include/dotempmods";
include "include/classes";
include "include/damages";
include "include/bitwise";
include "include/random";
include "include/myutil";
include "../pkg/foundations/hooks/zhaArchery";

var cfgfile := ReadConfigFile(":*:itemdesc");
var weaponElem;

var lastWeaponChecked := 0;
var haveUnequipped   := 0; // The serial of the object last unequipped, or 0 if the weapon has been
			   // changed.

function RecalcDmg( attacker, defender, weapon, armor, byref basedamage, piercing := 0 )

	weaponElem := cfgfile[GetObjtype(weapon)];

	if( !GetObjProperty( weapon, "Astral" ) )
		return RecalcPhysicalDmg( attacker, defender, weapon, armor, basedamage, piercing );
	else
		return RecalcAstralDmg( attacker, defender, weapon, armor, basedamage, piercing );
	endif

endfunction


function RecalcPhysicalDmg( attacker, defender, weapon, armor, byref basedamage, piercing := 0 )
	var ninjalevel := GetObjProperty( attacker , CLASSEID_NINJA );
	if (ninjalevel>=6)
		if (ApplyAmbushEffect(attacker, defender))
			piercing := 1;
		endif
	endif


	var rawdamage;
	var absorbed;
	var ar := CInt(armor.ar) + CInt(defender.ar_mod);
	var anatomy := GetEffectiveSkill( attacker, SKILLID_ANATOMY );

	if( defender.isA(POLCLASS_NPC) )
		if( !attacker.npctemplate )
			basedamage := basedamage * 1.5;
		endif
	endif

	basedamage := ModByDist( attacker, defender, weapon, basedamage );
	basedamage := basedamage * ( 1 + anatomy * 0.002 );

	if( !piercing )
		rawdamage := GetShieldAbsorption( attacker, defender, weapon, basedamage );
		absorbed  := ar * (Random(51) + 50) * 0.01;
	else
		rawdamage := basedamage;
		absorbed  := 0;
	endif

	rawdamage := rawdamage - absorbed;
	rawdamage := rawdamage * 0.5;
	rawdamage := ModByProt( defender, rawdamage );

	if( rawdamage < 0 )
		rawdamage := 0;
	endif

	MaceHit( attacker, defender, weapon, armor, absorbed );
//	CheckForTheSkillGain( attacker , defender , weapon );

	return CInt( rawdamage );

endfunction


function RecalcAstralDmg( attacker, defender, weapon, armor, byref basedamage, piercing := 0 )

	var rawdamage;
	var absorbed := 0;

	var ar := CInt( GetObjProperty( armor, "Astral" ) ) * 25 * CInt( armor.ar );

	basedamage := basedamage
		* ((GetEffectiveSkill( attacker, SKILLID_SPIRITSPEAK ) + 50.0 + (GetIntelligence(attacker) * 0.2)) * 0.01)
		/ ((GetEffectiveSkill( attacker, SKILLID_TACTICS ) + 50.0 + (GetStrength(attacker) * 0.2)) * 0.01);

	AwardSkillPoints( attacker, SKILLID_EVALINT, 30 );
	AwardSkillPoints( attacker, SKILLID_SPIRITSPEAK, 30 );

	if( defender.isA(POLCLASS_NPC) )
		basedamage := basedamage * ClasseBonus( attacker, CLASSEID_MAGE );
	else
		var level := GetObjProperty( attacker, CLASSEID_MAGE );
		if( level )
			level := level - 2;
			if( level >= 1 )
				basedamage := basedamage * ClasseBonusByLevel( level );
			endif
		endif
	endif

	if( GetObjProperty( attacker, CLASSEID_WARRIOR ) )
		if( !GetObjProperty( attacker, CLASSEID_MAGE ) )
			basedamage := basedamage / ClasseBonus( attacker, CLASSEID_WARRIOR );
		endif
	endif

	if( GetObjProperty( defender, CLASSEID_WARRIOR ) )
		if( !GetObjProperty( defender, CLASSEID_MAGE ) )
			basedamage := basedamage * ClasseBonus( defender, CLASSEID_WARRIOR );
		endif
	endif

	basedamage := basedamage * ( 1 + GetEffectiveSkill( attacker, SKILLID_EVALINT ) * 0.002 );
	rawdamage  := basedamage;

	if( !piercing )

		var meditation := GetEffectiveSkill( defender, SKILLID_MEDITATION );
		meditation := meditation * ClasseBonus( defender, CLASSEID_MAGE );

		if( (Random(100) + 1) <= CInt( meditation * 0.5 ) )

			var resist := GetEffectiveSkill( defender, SKILLID_MAGICRESISTANCE );
			var defint := GetIntelligence(defender);

			AwardSkillPoints( defender, SKILLID_MEDITATION, 30 );
			AwardSkillPoints( defender, SKILLID_MAGICRESISTANCE, 30 );
			PerformAction(  defender, ANIM_CAST_AREA );
			SendSysMessage( defender, "You successfully block a part of the astral attack." );

			if( GetObjProperty( defender, CLASSEID_MAGE ) )
				rawdamage := rawdamage - (resist + defint) * 0.25 * ClasseBonus( defender, CLASSEID_MAGE );
			elseif( GetObjProperty( defender, CLASSEID_WARRIOR ) )
				rawdamage := rawdamage - (resist + defint) * 0.25 / ClasseBonus( defender, CLASSEID_WARRIOR );
			endif

			if( rawdamage < 0 )
				rawdamage := 0;
			endif

		endif

		if( ar )
			absorbed := CInt( ar * (Random(51) + 50) / 100 );
		endif

	endif

	rawdamage := rawdamage - absorbed;
	rawdamage := rawdamage * 0.5;
	if( rawdamage < 0 )
		rawdamage := 0;
	endif

//	CheckForTheSkillGain( attacker , defender , weapon );

	return CInt(rawdamage);

endfunction

function ModByDist( attacker, defender, weapon, basedamage )

	// Sno changed this 23/1/03. For some reason it didn't like weapon.Attribute - it returned "<uninitialized object>".
	// So I've re-read itemdesc.cfg, and done it that way... it seems a bit unnecessary - is this because of a core bug?
	
	if( lower(weaponElem.Attribute) == lower(ATTRIBUTEID_ARCHERY) )
		/*
		basedamage := basedamage
			* ((GetDexterity(attacker) + 60.0) * 0.01)
			/ ((GetEffectiveSkill( attacker, SKILLID_TACTICS ) + 50.0 + (GetStrength(attacker) / 5.0)) * 0.01);
		*/
		
		//basedamage := basedamage * ((GetDexterity(attacker) + 30.0) * 0.01) * 0.5;
			
		//Broadcast("ModByDist: basedamage " +  basedamage);

		var rlevel := GetObjProperty( attacker, CLASSEID_RANGER );
		if (rlevel)
			basedamage := basedamage * ((1+ClasseBonus( attacker, CLASSEID_RANGER ))/2);	
		else
			rlevel :=0; //in case is error									
		endif
		
		var dist := Distance( attacker, defender );
		if( (dist <= 1) || (dist > 10 + (rlevel*RANGER_LEVEL_ARCHERY_RANGE_BONUS)) ) // a bit more long rang for rangers
			basedamage := basedamage * 0.25;
		endif
		
		
	else
		if( defender.isA(POLCLASS_NPC) )
			//Broadcast("Before: "+basedamage);
		endif
		/*
			basedamage := basedamage * ClasseBonus( attacker, CLASSEID_WARRIOR );
			if (IsWarrior(attacker))
				broadcast("After2: "+basedamage);
			endif
		else
			var level := GetObjProperty( attacker, CLASSEID_WARRIOR );
			if( level )
				level := CInt(level/2) + 3;
				if( level >= 1 )
					basedamage := basedamage * ClasseSmallBonusByLevel( level );
				endif

			endif
		endif
		*/
		if (IsWarrior(attacker))
			basedamage := WarriorMeleeBonuses(basedamage, attacker, defender);
			//broadcast("After warrior: "+basedamage);
		endif
		
		if (IsNinja(attacker))
			basedamage := basedamage + BackStabDamageBonus(GetClasseLevel(attacker), attacker, defender);
		endif
	
		if(IsPaladin(attacker))
			basedamage := PaladinHitscriptBonuses(basedamage, attacker, defender);
			basedamage := PaladinKarmaBonusDamage(basedamage, attacker, defender);
			//broadcast("After paladin: "+basedamage);
		endif
		
		if (IsPriest(attacker))
			// tactics compensator
			
			basedamage := basedamage * ((GetEffectiveSkill(attacker, SKILLID_MACEFIGHTING) + GetClasseLevel(attacker)*10) *0.01);
			//basedamage := PriestHitscriptBonuses(basedamage, attacker, GetClasseLevel(attacker));
			//broadcast("After priest: "+basedamage);
		endif

		if(IsDeathknight(attacker))
			basedamage := DeathKnightHitscriptBonuses(basedamage, attacker, defender);
			//broadcast("After deathknight: "+basedamage);
		endif

		if (IsMonk(attacker))
			if (attacker.weapon.graphic in VALID_MONK_WEAPS)
				basedamage := basedamage + (MONK_BONUS_DAMAGE_WITH_STAFF * GetClasseLevel(attacker));
				//broadcast("After monk: "+basedamage);
			endif
		endif
					
	endif

	if( GetObjProperty( attacker , CLASSEID_MAGE ) )
		if( !GetObjProperty( attacker , CLASSEID_WARRIOR ) )
			basedamage := basedamage / ClasseBonus( attacker , CLASSEID_MAGE );
		endif
	endif

	if( GetObjProperty( defender , CLASSEID_MAGE ) )
		if( !GetObjProperty( defender , CLASSEID_WARRIOR ) )
			basedamage := basedamage * ClasseBonus( defender , CLASSEID_MAGE );
		endif
	endif

	
	return basedamage;

endfunction


function MaceHit( attacker, defender, weapon, armor, absorbed )

	var archance := 2;
	var weapchance := 2;

	if( lower(weaponElem.Attribute) == lower(ATTRIBUTEID_MACEFIGHTING) )

		var loss := Random(2) + 1;
		if(loss > GetStamina(defender) )
			SetStamina(defender, 0);
		else
			SetStamina(defender, GetStamina(defender) - loss);
		endif

		if( armor.hp )
			if( Random(100) < 5 )
				armor.hp := armor.hp - 1;
			endif
		endif

	elseif( Random(100) < archance )

		if( armor.hp )
			armor.hp := armor.hp - 1;
		endif

	endif

	if( Random(100) < weapchance )
		if( weapon.hp )
			weapon.hp := weapon.hp - 1;
		endif
	endif

	if( armor.quality )
		if( (armor.hp > armor.maxhp) || (armor.hp < 1) )
			DestroyEquipment( defender, armor );
		endif
	endif

	if( weapon.quality )
		if( (weapon.hp > weapon.maxhp) || (weapon.hp < 1) )
			DestroyEquipment( attacker, weapon );
		endif
	endif

endfunction


function ModByProt( defender, basedamage )

	var physicalprot := GetObjProperty( defender, "PhysicalProtection" );
	if( !physicalprot )
		return basedamage;
	else
		basedamage := basedamage * ( 1 - physicalprot * 0.05 );
		return basedamage;
	endif

endfunction


function GetShieldAbsorption( attacker, defender, weapon, basedamage )
	var mlevel := CInt(GetObjProperty(defender, CLASSEID_MONK));
	var shield := GetEquipmentByLayer( defender, LAYER_HAND2 );
	var shieldar := CInt(shield.ar);
	if (mlevel)
		shieldar:= GetEffectiveSkill(defender, SKILLID_BATTLE_DEFENSE ) / 3;
	endif
	if( shieldar || mlevel>0)
		var parry_skill := GetEffectiveSkill( defender, SKILLID_BATTLE_DEFENSE );
		parry_skill := CInt( parry_skill * ClasseBonus( attacker, CLASSEID_WARRIOR ) );

		if( (Random(100) + 1) <= CInt(parry_skill / 2) )
			var absorbed;

			PerformAction( defender, ANIM_ATTACK_XBOW );
			SendSysMessage( defender, "You successfully parry the attack." );
			if (mlevel)
				AwardSkillPoints( defender, SKILLID_PARRY, 20 );
			endif
			if( lower(weaponElem.Attribute) == lower(ATTRIBUTEID_ARCHERY) )
				absorbed := shieldar;
				if (mlevel>=5)
					if(defender.weapon.hitscript == "wrestlinghitscript")
						var weaponType := cfgfile[attacker.weapon.objtype];
						if (weaponType) 
							sleepms((100-weaponType.Speed)/10);
							DoBowAttack(defender, attacker, weaponType, SKILLID_BATTLE_DEFENSE);
						endif
					endif
				endif
			else
				absorbed := CInt(shieldar / 2);
			endif

			basedamage := basedamage - absorbed;
			if( basedamage < 0 )
				basedamage := 0;
			endif
			if (! mlevel>0)
				MaceHit( attacker, defender, weapon, shield, absorbed );
			endif
		endif
	endif
	return basedamage;

endfunction


function DealDamage( attacker, defender, weapon, armor, basedamage, rawdamage )

	if( !GetObjProperty( weapon, "Astral" ) )

		var poisoned := GetObjProperty( weapon, "SkillPoisoned" );
		var reactive := GetObjProperty( defender, "ReactiveArmor" );
		if (GetObjProperty(attacker, "NoReactiveArmour"))
			reactive := 0;
		endif
		var onhit    := GetObjProperty( armor, "OnHitScript" );
			//Check if the item was poisonned by someone
		if( poisoned )

			var plvl := CInt( poisoned / 3 );
			poisoned  := poisoned - 1;
			if( poisoned )
				SetObjProperty( weapon, "SkillPoisoned", poisoned );
			else
				EraseObjProperty( weapon, "SkillPoisoned" );
			endif

			SetPoison( defender, plvl, "You have been poisoned by " + attacker.name + "." );

		endif

		// Altered by Sno 23/1/03 - makes warriors take less damage from physical... is this right?
		//altered 30/6/04 -- paladins and deathknights take a lil less dmg
		var class:=GetClass(defender);
		var level:=GetClasseLevel(defender);
		if (class==CLASSEID_WARRIOR)
			level:=level+3;
		endif
		if (class==CLASSEID_PALADIN || class==CLASSEID_DEATHKNIGHT)
			level:=level-1;
		endif
		if (level < 0)
			level:=0;
		endif
		if( attacker.isA(POLCLASS_NPC) )
			case (class)
				CLASSEID_WARRIOR:	
				CLASSEID_PALADIN:
				CLASSEID_DEATHKNIGHT:	rawdamage := CInt( rawdamage / ClasseSmallBonusByLevel( level ));
			endcase
		else
			case (class)
				CLASSEID_WARRIOR:
				CLASSEID_PALADIN:
				CLASSEID_DEATHKNIGHT:	rawdamage := CInt( rawdamage / ClasseSmallBonusByLevel( level ));
							basedamage := CInt( basedamage / ClasseSmallBonusByLevel( level ) );
			endcase
		endif

		//Check if reactive armor spell is on
		if( reactive )
			start_script( ":combat:reactivearmoronhit", { attacker, defender, basedamage, rawdamage, reactive} );
		endif

		//Check if the armor have an on hit script and launch it
		if( onhit )
			start_script( onhit, {attacker, defender, weapon, armor, basedamage, rawdamage} );
		else
			ApplyTheDamage( defender, attacker, rawdamage );
		endif

	else

		ApplyTheAstralDamage( defender, attacker, rawdamage );

	endif

	checkBowUnequip(attacker, weapon);

endfunction


function DestroyEquipment( who, item )

	var cursed := GetObjProperty( item, "Cursed" );

	if( cursed )
		SetObjProperty( item, "Cursed", 3 );
	endif

	if( who.backpack )
		MoveItemToContainer( item, who.backpack );
	endif

	if( who.isA(POLCLASS_NPC) )
		if( item.skillid )
			PrintTextAbove( who, who.name + " seem exhausted and can barely hit anyones anymore!" );
		else
			PrintTextAbove( who, who.name + "'s defences has been penetrated making it easy to hit vital parts!" );
		endif
	endif

	DestroyItem( item );

endfunction

function checkBowUnequip(who, weapon)
	if (who.isA(POLCLASS_NPC))
		return;
	endif

	set_critical(1);
	
	if (weapon.serial != lastWeaponChecked)
		lastWeaponChecked := weapon.serial;
		haveUnequipped := 0;
	endif
	
	set_critical(0);
	
	sleepms(50);

	if (weaponElem.Attribute == "Archery")
		var ammoType   := weaponElem.ProjectileType;
		var ammo       := EnumerateItemsInContainerOfObjtype(who.backpack, ammoType);
		var weaponIsEquipped := weapon in ListEquippedItems(who);
		
		if (!ammo.size() && weaponIsEquipped && (haveUnequipped != weapon.serial))
			if (MoveItemToContainer(weapon, who.backpack))
				PrintTextAbovePrivate(who, "Having run out of ammunition, you put away your weapon.", who);
				haveUnequipped := 1;
			endif
		endif

	endif
endfunction

function BackStabDamageBonus(ninjalevel, attacker, defender)
	var backstab:=0;
	
	if ((ninjalevel && attacker.facing == defender.facing) && (attacker.x!=defender.x || attacker.y!=defender.y))
		var myfacing := attacker.facing;
		
		case (myfacing) //Conversion
		1: //south east
			if (defender.x > attacker.x && defender.y < attacker.y)
				backstab:=1;
			endif
		2: //south 
			if (defender.x > attacker.x)
				backstab:=1;
			endif
		3: //south west
			if (defender.x > attacker.x && defender.y > attacker.y)
				backstab:=1;
			endif
		4: //west 
			if (defender.y > attacker.y)
				backstab:=1;
			endif
		5: //north west
			if (defender.x < attacker.x && defender.y > attacker.y)
				backstab:=1;
			endif
		6: //north 
			if (defender.x < attacker.x)
				backstab:=1;
			endif
		7: //north east
			if (defender.x < attacker.x && defender.y < attacker.y)
				backstab:=1;
			endif
		8: //east 
			if (defender.y < attacker.y)
				backstab:=1;
			endif
		endcase
	endif
	
	if (backstab)
		PrintTextAbove(attacker, "*Backstab!*",3,38);
		PlaySoundEffect(defender, 0x01cb);
		return ninjalevel * NINJA_BACKSTAB_BONUS_DAMAGE;
	endif
	return 0;
endfunction

function ApplyAmbushEffect(attacker, defender)

	var timeambush := GetObjProperty(attacker, "attackInitiatedWhileHidden");
	//Broadcast(timeambush + " " + ReadGameClock());
	if (!timeambush || timeambush ==error)
		return 0;
	endif	
	if (timeambush>(ReadGameClock()-5))
		PrintTextAbove(attacker, "*Ambush!* " ,3,38);
		EraseObjProperty(attacker, "attackInitiatedWhileHidden");
		return 1;
	endif
			
	return 0;
endfunction

function PaladinKarmaBonusDamage(basedamage, attacker, defender)
	//Damage = BaseDamage * (1 + ((PaladinKarma - DefenderKarma) / 20000)) 
	//MUST BE MOUNTED BY SPECIAL STEED
	if (! GetObjProperty(GetEquipmentByLayer( attacker, LAYER_MOUNT ), "specialsteed"))
		return basedamage;
	endif	

	var attacker_karma:=CInt(GetObjProperty( attacker, "Karma"));
	var defender_karma:=CInt(GetObjProperty( defender, "Karma"));
	
	var extradmg:= (basedamage * (1.0 + (CDbl(attacker_karma - defender_karma) / PALADIN_KARMA_DAMAGE_MOD_DENOMINATOR)))-basedamage;
	//broadcast("Karma bonus: "+extradmg);
	return CInt(basedamage + extradmg);
endfunction

function PaladinHitscriptBonuses(basedamage, attacker, defender)
	//Bonus weapon damage per level with Dragonslayer/Silver/Holy (cumulative with other damage bonuses) 
    var slaytype;
    if (attacker.weapon.hitscript == "slayerscript")
	if (defender.isa(POLCLASS_NPC))
          slaytype:=GetObjProperty(attacker.weapon, "SlayType");
          case (slaytype)
 	       "Dragonkin":
               "Undead":
               "Daemon": basedamage:=basedamage* ClasseBonus(attacker, CLASSEID_PALADIN);
          endcase
        else
          slaytype:=GetObjProperty(attacker.weapon, "SlayType");
          case (slaytype)
 	       "Dragonkin":
               "Undead":
               "Daemon": basedamage:=basedamage * ClasseSmallBonusByLevel( GetClasseLevel(attacker));
          endcase    
        endif    
    endif
    return basedamage;
endfunction

function PriestHitscriptBonuses(basedamage, attacker, clevel)
	//Bonus weapon damage per level with Silver/Holy (cumulative with other damage bonuses) 
    if (attacker.weapon.hitscript == "slayerscript")
          var slaytype:=GetObjProperty(attacker.weapon, "SlayType");
          case (slaytype)
               "Undead":
               "Daemon": basedamage:=basedamage+(clevel * PRIEST_SPECIAL_WEAPON_BONUS_DAMAGE);
          endcase
    endif
    return basedamage;
endfunction

function DeathKnightHitscriptBonuses(basedamage, attacker, defender)
	//Bonus to weapon damage per level with Bloody/Leech's/Vampiric
	if (defender.isa(POLCLASS_NPC))
    		case (attacker.weapon.hitscript)
 			"lifedrainscript":
      			"manadrainscript":
       			"staminadrainscript": basedamage:=basedamage* ClasseBonus(attacker, CLASSEID_DEATHKNIGHT);
	 	endcase
	 else
    		case (attacker.weapon.hitscript)
 			"lifedrainscript":
      			"manadrainscript":
       			"staminadrainscript": basedamage:=basedamage * ClasseSmallBonusByLevel( GetClasseLevel(attacker));
	 	endcase	
	 endif 	
    return basedamage;
endfunction

function WarriorMeleeBonuses(basedamage, attacker, defender)
	if (defender.isa(POLCLASS_NPC))
		basedamage:= basedamage * ClasseBonus (attacker, CLASSEID_WARRIOR);
	else
		basedamage:= basedamage * ClasseSmallBonusByLevel( GetClasseLevel(attacker));
	endif
	return basedamage;
endfunction