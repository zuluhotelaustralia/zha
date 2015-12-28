use uo;
use os;
use util;
use basic;

include "include/dismount";
//include "include/attributes";
include "include/client";
include "include/spelldata";
include "include/classes";
include "include/random";
include "include/areas";

function AddToPersistedMods( byref allmods, byref mmod )

	set_critical(1);
	var i;
	for( i := 1; i <= len(allmods); i := i + 1 )
		if ( allmods[i][1] == mmod[1] )
			return 0;
		endif
	endfor

	allmods.append( mmod );
	set_critical(0);

	return i;

endfunction


function AddToPoisonMod( byref currentmod, byref mmod )

	set_critical(1);
	if( mmod[1] > currentmod[1] )
		currentmod[1] := mmod[1];
	endif

	if( mmod[2] > currentmod[2] )
		currentmod[2] := mmod[2];
	endif
	set_critical(0);

endfunction


function AddToStatMods( byref allmods, byref mmod )

	set_critical(1);
	var i;
	for( i := 1; i <= len(allmods); i := i + 1 )
		if ( allmods[i][1] == mmod[1] )
			return 0;
		endif
	endfor

	allmods.append( mmod );
	set_critical(0);

	return i;

endfunction


function BaseRegenRate( who )

	var baseregen := GetObjProperty(who,"baseregen");

	if (!baseregen)
		return 100;
	endif

	return baseregen;

endfunction


function CanMod( who, stat, persistedprop := 0 )

	var modarray := "#mods";
	if( persistedprop )
		modarray := "mods";
	endif

	var currentmods := GetObjProperty( who, modarray );
	if (!currentmods)
		return 1;
	endif

	foreach mmod in currentmods
		if ( mmod[1] == stat )
			return 0;
		endif
	endforeach
	
	return 1;

endfunction


function CurePoison( who, sound := 0, msg := 0 )

	set_critical( 1 );

	var currentmod := GetObjProperty( who, "poison" );
	if( !currentmod )
		set_critical( 0 );
		return;
	endif

	EraseObjProperty(who, "poison_level");
	EraseObjProperty( who, "poison" );
	SetHpRegenRate(who , ( BaseRegenRate(who)) * 12);
	who.SetPoisoned( 0 );

	set_critical( 0 );

	if( sound )
		PlaySoundEffectPrivate( who, sound, who );
	endif

	if( msg )
		PrintTextAbovePrivate( who, msg, who );
	endif

endfunction


//for name, graphic and color changes!
function DoPersistedMod( who, prop, newvalue, oldvalue, duration )

	var parms := {};
	parms[1] := who;
	parms[2] := prop;
	parms[3] := newvalue;
	parms[4] := oldvalue;
	parms[5] := duration;

	start_script( ":summoning:processpersistedmod", parms );
	sleepms(250);

endfunction


//for nonpersistedmods
function DoTempMod( who, stat, amt, duration )

	var parms := {};
	parms[1] := who;
	parms[2] := stat;
	parms[3] := amt;
	parms[4] := duration;

	start_script( ":summoning:processtempmod", parms );
	sleepms(250);

endfunction


function GetModAmount( who, buff:=1, skill:=SKILLID_MAGERY)

	var magery := GetEffectiveSkill( who, skill );
	if (GetObjProperty(who, "UsingBardInstrument")==1) //bard instrument cast
		magery := GetEffectiveSkill( who, SKILLID_ENTICEMENT );
	endif
	

	var amount := RandomInt(10) + CInt(magery/10) + 5;
	var priestlevel:=GetObjProperty(who, CLASSEID_PRIEST);
	if (priestlevel && buff)  // Buff bonus for priests wearing Healing items
		var healinc:=GetObjProperty(who, "HealingIncrease");
		if (healinc)
			healinc := CDbl(healinc) / 20;
			amount:=amount * (1 + healinc);
		endif
	endif
	amount := ModifyWithMagicEfficiency( who, amount );
	case (GetClass(who))
		CLASSEID_MAGE:		amount:=amount * (1+(CDbl(GetClasseLevel(who)) * BONUS_PER_LEVEL));
		CLASSEID_PRIEST:	amount:=amount * (1+(CDbl(GetClasseLevel(who)) * LARGE_BONUS_PER_LEVEL));
		CLASSEID_WARRIOR:	amount:=amount / (1+(CDbl(GetClasseLevel(who)) * BONUS_PER_LEVEL));
	endcase
	if( !amount )
		amount := 1;
	endif

	return CInt(amount);

endfunction


function GetModDuration( who, skill:=SKILLID_MAGERY )

	var magery := GetEffectiveSkill( who, skill);
	if (GetObjProperty(who, "UsingBardInstrument")==1) //bard instrument cast
		magery := GetEffectiveSkill( who, SKILLID_ENTICEMENT );
	endif

	var duration := magery * 4;

	duration := ModifyWithMagicEfficiency( who, duration );
	var class:= GetClass(who);
	var level:= GetClasseLevel(who);
	case (class)
		CLASSEID_MAGE:
		CLASSEID_DRUID:
		CLASSEID_NECROMANCER:	duration:=duration * (1 + level*BONUS_PER_LEVEL);
		CLASSEID_PRIEST:	duration:=duration * (1 + level*LARGE_BONUS_PER_LEVEL);		
	endcase
	if( !duration )
		duration := 1;
	endif

	return CInt(duration);

endfunction


function GetPoisonRegenMod( plvl )

	case (plvl)
		0: return 100;
		1: return -500;
		2: return -750;
		3: return -1500;
		4: return -2500;
		5: return -3500;
		6: return -5000;
	endcase

/*	
		0: return 0;
		1: return -500;
		2: return -1000;
		3: return -2000;
		4: return -4000;
		5: return -8000;
		6: return -16000;
*/

endfunction


function PoisonLevel( who )

	var currentmod := GetObjProperty( who, "poison" );
	if( !currentmod )
		currentmod := {};
	endif

	if( !len(currentmod) )
		return 0;
	else
		return CInt( currentmod[1] );
	endif

endfunction


function PoisonProtected( cast_on, plvl )

	if ( IsInArea(cast_on, AREAID_CRAFTMINE ) )
		return IMMUNED;
	endif
	if (IsInSafeArea(cast_on) && !GetObjProperty(cast_on, "#duel")) // Is in safe area and not dueling (Seravy)
		return IMMUNED;
	endif


	if(GetObjProperty(cast_on, "newbie")==1) //newbie character poison protection
		PrintTextAbovePrivate( cast_on, "Your young status grants you immunity from the poison!", cast_on );
		return IMMUNED;
	endif

	var prot := "PoisonImmunity";
	var protserial := GetObjProperty( cast_on, "PoisonImmunity" );
	var protlvl := GetObjProperty( cast_on, "PermPoisonImmunity" );
	if( !protlvl )
		protlvl := 0;
	endif

	var cmi := 0 - protlvl;
	var item;
	var charges;
	var cursed;

	if( protserial )
		foreach thing in ListEquippedItems( cast_on )
			if ( thing.serial == protserial )
				item := thing;
				charges := GetObjProperty( item, "PoisonImmunity" );
				cursed := GetObjProperty( item, "Cursed" );
				if (!charges)
					EraseObjProperty( cast_on, "PoisonImmunity" );
					EraseObjProperty( item, "PoisonImmunity" );
					PrintTextAbovePrivate(item, item.desc + " is out of charges!", cast_on );
					CheckForAnotherProtItem( cast_on, item, "PoisonImmunity" );
				endif
				break;
			endif
		endforeach
	endif

	if( charges >= plvl )
		charges := charges - plvl;
		SetObjProperty( item, "PoisonImmunity", charges );
		if( !cursed )
			PrintTextAbovePrivate( cast_on, "Your " + item.desc + " protected you from the poison!", cast_on );
			return IMMUNED;
		else
			return CURSED;
		endif
	
	elseif( cmi >= plvl )
		return CURSED;
	
	elseif( protlvl >= plvl )
		PrintTextAbovePrivate( cast_on, "Your poison immunity protect you from the poison", cast_on );
		return IMMUNED;
	endif
	
	return 0;


endfunction


function RandomHairColor()

	return Random(46)+1102;

endfunction


function SetPoison( who, plvl, msg := 0 )

	if( !who.isA(POLCLASS_NPC) )
		plvl := plvl - 1;
	endif

	if( plvl < 1 )
		plvl := 1;
	elseif( plvl > 6)
		plvl := 6;
	endif

	var parms := {};
	parms[1] := who;
	parms[2] := plvl;
	parms[3] := CStr( msg );

	start_script( ":summoning:processpoisonmod", parms );
	sleepms(250);

endfunction


function UndoTheMod( who, prop, amt )

	var 	msg,
		sound;
	
	var 	oldvalue := amt;

	case (prop)
		"cstr":
		"str" :		SetStrengthMod(who, GetStrengthMod(who) - amt);
	
		"cdex":
		"dex" :		SetDexterityMod(who, GetDexterityMod(who) - amt);
	
		"cint":
		"int" :		SetIntelligenceMod(who, GetIntelligenceMod(who) - amt);
	
		"car":
		"ar"  :		who.ar_mod := who.ar_mod - amt;
				msg := "Armor is now " + who.ar;
				sound := 0x01e3;
			
		"p"   :		who.setParalyzed( 0 );
		"call":
		"all" :		SetStrengthMod(who, GetStrengthMod(who) - amt);
				SetDexterityMod(who, GetDexterityMod(who) - amt);
				SetIntelligenceMod(who, GetIntelligenceMod(who) - amt);
			
		"cpoly":
		"poly":		who.ar_mod := who.ar_mod - CInt( amt / 3 );
				msg := "Armor is now " + who.ar;
				sound := 0x01e3;
				SetStrengthMod(who, GetStrengthMod(who) - amt);
				SetDexterityMod(who, GetDexterityMod(who) - amt);
				SetIntelligenceMod(who, GetIntelligenceMod(who) - amt);
			
        	"name"    : 	who.name := oldvalue;
				msg := "Your name is now " + who.name;
				sound := 0x01e3;
			
		"graphic" : 	who.graphic := oldvalue;
	
		"color"   : 	who.color := oldvalue;
	
		"spiritspeak":	if (!amt)
					RevokePrivilege( who, "hearghosts" );
                                        SendSysMessage(who, "You are now no longer detecting Brainwaves.");
                                        var casterLoc := {who.x, who.y, who.z};
                                        MoveCharacterTolocation(who, 1, 1, 1, MOVECHAR_FORCELOCATION);
                                        MoveCharacterTolocation(who, casterLoc[1], casterLoc[2], casterLoc[3], MOVECHAR_FORCELOCATION );
				endif

		"seehidden":	if (!amt)
					RevokePrivilege( who, "seehidden" );
				endif
			
        	"title_prefix": who.title_prefix := oldvalue;
        
        	"title_suffix": who.title_suffix := oldvalue;
        
		"gender":	who.gender := oldvalue;
	
		"hair":
			DestroyItem( GetEquipmentByLayer( who, LAYER_HAIR ));
			if (oldvalue)
				var newhair := CreateItemAtLocation(5288, 1176, 0, oldvalue, 1);
				newhair.color := GetObjProperty(who,"haircolor");
				EquipItem(who,newhair);
			endif
		"beard":
			DestroyItem(GetEquipmentByLayer(who,LAYER_BEARD));
			if (oldvalue)
				var newhair := CreateItemAtLocation(5288, 1176, 0, oldvalue, 1);
				newhair.color := GetObjProperty(who,"haircolor");
				EquipItem(who,newhair);
			endif
		"poison":
			SetHpRegenRate(who , ( BaseRegenRate(who)) * 12);
			who.SetPoisoned( 0 );
	endcase

	if( sound )
		PlaySoundEffectPrivate(who, sound, who);
	endif

	if( msg )
		PrintTextAbovePrivate(who,msg,who);
	endif

endfunction


function WipeMods( who )

	set_critical( 1 );

	var had_mods := 0;
	var modarray := GetObjProperty( who, "#mods" );
	if( !modarray )
		modarray := {};
	else
		had_mods := 1;
	endif

	foreach mmod in modarray
		UndoTheMod( who, mmod[1], mmod[2] );
	endforeach
	EraseObjProperty( who, "#mods" );


	var persitedmodarray := GetObjProperty( who, "mods" );
	var newmods := {};
	if( !persitedmodarray )
		persitedmodarray := {};
	else
		had_mods := 1;
	endif

	foreach mmod in persitedmodarray
		if( mmod[1] != "poison" )
			UndoTheMod( who, mmod[1], mmod[2] );
		else
			newmods.append( mmod );
		endif
	endforeach

	if( len(newmods) )
		SetObjProperty( who, "mods", newmods );
	else
		EraseObjProperty( who, "mods" );
	endif

	set_critical(0);

	return had_mods;

endfunction


function GetMod(who, mod) // By Seravy

  set_critical( 1 );
  var modValue := 0;
  var modarray := GetObjProperty( who, "#mods" );
  if( !modarray )
    modarray := {};
  endif
  foreach mmod in modarray
    if (mod == mmod[1])
      modValue := {mmod[2], mmod[3]};
    endif
  endforeach

  if (!modValue)
    var permmodarray := GetObjProperty( who, "mods" );
    if( !permmodarray )
      permmodarray := {};
    endif
    foreach permmod in permmodarray
      if (mod == permmod[1])
        modValue := {permmod[2], permmod[3]};
      endif
    endforeach
  endif
  set_critical( 0 );

  return modValue;

endfunction



function WipeMod( who, mod ) // By Seravy

  set_critical(1);

  var wipedTheMod := 0;
  var modarray := GetObjProperty( who, "#mods" );
  if( !modarray )
    modarray := {};
  endif

  var newmodarray := {};
  foreach tempmod in modarray
    if( mod == tempmod[1])
      UndoTheMod( who, tempmod[1], tempmod[2]);
      wipedTheMod := 1;
    else
      newmodarray.append(tempmod);
    endif
  endforeach

  if(len(newmodarray))
    SetObjProperty(who, "#mods", newmodarray);
  else
    EraseObjProperty( who, "#mods" );
  endif

  // Persited Mods
  var persitedmodarray := GetObjProperty( who, "mods" );
  if( !persitedmodarray )
    persitedmodarray := {};
  endif

  var newpermmodarray := {};
  foreach tempmod in persitedmodarray
    if( tempmod[1] == mod)
      UndoTheMod(who, tempmod[1], tempmod[2]);
      wipedTheMod := 1;
    else
      newpermmodarray.append(tempmod);
    endif
  endforeach

  if(len(newpermmodarray))
    SetObjProperty(who, "mods", newpermmodarray);
  else
    EraseObjProperty( who, "mods" );
  endif

  set_critical(0);

  return wipedTheMod;
endfunction
