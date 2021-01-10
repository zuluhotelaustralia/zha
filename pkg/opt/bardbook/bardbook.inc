use uo;
use os;
use math;
use basic;
use cfgfile;
use party;

include "include/areas";
include "include/attributes";
include "include/objtype";
include "include/client";
include "include/itemutil";
include "include/pol95";

function GetScriptBard( songid )

	var songcfg := ReadConfigFile( "songs" );
	var script := songcfg[songid].script;

	return script;

endfunction

function TryToCastBard( caster, song )

	//Variables declaration and initialisation
	var conf	 := ReadConfigFile( "songs" );
	var elem	 := FindConfigElem( conf, song );
	var circ	 := GetConfigInt( elem, "Circle" );
	var spelldelay := (circ*2)*1000;
	var statcost	 := circ*12;
	var difficulty := 100+(circ*10);
	var value	 := difficulty*4;
	var powerwords	:= GetConfigString( elem, "PowerWords");
	var failsound	:= GetConfigInt( elem, "SFXF");
	var passsound	:= GetConfigInt( elem, "SFXS");
	var skillused	:= GetConfigInt( elem, "SkillUse");
	var instrument	:= GetConfigInt( elem, "Instrument");

	var sx := caster.x;
	var sy := caster.y;
	var sz := caster.z;

	var scroll := GetObjProperty( caster, "SongScroll" );
	if(scroll>0)
		difficulty := difficulty + 10;
		spelldelay := spelldelay + 2000;
		statcost := statcost +20;
	endif

	//Check backpack for the right instrument
	if( !FindObjtypeInContainer( caster.backpack, instrument ) )
		SendSysMessage( caster, "You do not have the required magical instrument!" );
		EraseObjProperty( caster, "#Casting" );
		return 0;
	endif

	if( !caster.cmdlevel )
		if( IsInAntiMagicArea( caster ) )
				SendSysMessage( caster, "You cannot use magic songs here." );
				EraseObjProperty( caster, "#Casting" );
				return 0;
		endif
	endif

	//Speak the power words
	PrintTextAbove(caster, powerwords );

	//Delay and interruption management
	var shp := GetHp(caster);

	while( spelldelay >= 500 )
		sleepms( 500 );
		spelldelay := spelldelay - 500;
		if( GetHp(caster) < shp )
			SendSysMessage( caster, "Your song has been interrupted!" );
			PlaySoundEffect(caster, failsound);
			EraseObjProperty( caster, "#Casting" );
			return 0;
		endif

	endwhile

	sleepms( spelldelay );

	//Consume the mana
	var manacheck;
	If(GetMana(caster)<statcost)
		manacheck := 0;
	else
		SetMana(caster, GetMana(caster) - statcost);
		manacheck := 1;
	endif
	if(manacheck < 1)
		PlaySoundEffect(caster, failsound);
		SendSysMessage( caster, "You need more mana for this magic song!" );
		EraseObjProperty( caster, "#Casting" );
		return 0;
	endif


	if( caster.x != sx || caster.y != sy || caster.z != sz )
		SendSysMessage( caster, "You lose your concentration." );
		PlaySoundEffect(caster, failsound);
		EraseObjProperty( caster, "#Casting" );
		return 0;
	endif

	//Do the skill check and return mana if failure occur
       	if( !CheckSkill( caster, skillused, difficulty, value ) )
		SetMana(caster, GetMana(caster) + statcost);
		if( GetMana(caster) > GetMaxMana(caster) )
			SetMana(caster, GetMaxMana(caster));
		endif
		PlaySoundEffect( caster, failsound );
		SendSysMessage( caster, "You fail the magic song." );
		EraseObjProperty( caster, "#Casting" );
		return 0;
	endif

	PlaySoundEffect( caster, passsound );

	if(circ > 2)
		//Speak the power words 2
		var powerwords2	:= GetConfigString( elem, "PowerWords2");
		PrintTextAbove(caster, powerwords2 );
	endif

	return 1;

endfunction


function Send_AttackBard( ttarget, source )

	var ev := array;
	ev.+type;
	ev.+source;

	ev.type := SYSEVENT_ENGAGED;
	ev.source := source;
	SendEvent( ttarget, ev );

	ev.type := 0x0800; //spell attack
	ev.spell := 4;
	SendEvent(ttarget, ev);

endfunction


function CanTargetAreaBard( caster )

	var cx := caster.x;
	var cy := caster.y;
	var shp := GetHp(caster);

	var cast_loc := TargetCoordinates( caster );
	if( !cast_loc.x )
		SendSysMessage( caster, "Error in targeting");
		EraseObjProperty( caster, "#Casting" );
		return 0;
	endif

	if( !caster.cmdlevel )
		if ( caster.x>=2555 && caster.y>=478 && caster.x <= 2615 && caster.y <= 3886 )
			SendSysMessage( caster, "Song disabled in this area");
			EraseObjProperty( caster, "#Casting" );
			return 0;
		endif
	endif

	if( (cx != caster.x) || (cy != caster.y) || (shp > GetHp(caster)) )
		SendSysMessage( caster, "You lose your concentration." );
		EraseObjProperty( caster, "#Casting" );
		return 0;
	endif

	if( !CheckLosAt( caster, cast_loc.x, cast_loc.y, cast_loc.z ) )
        	SendSysMessage( caster, "That is not in your line of sight." );
        	EraseObjProperty( caster, "#Casting" );
		return 0;
	endif

	return cast_loc;

endfunction


function CanTargetSpellBard( caster )

	var cx := caster.x;
	var cy := caster.y;
	var shp := GetHp(caster);

	var cast_on := Target95( caster, TGTOPT_CHECK_LOS );
	if(!cast_on  || !GetStrength(cast_on) || cast_on.hidden)
		return 0;
	endif

	if( !caster.cmdlevel )
		if ( caster.x>=2555 && caster.y>=478 && caster.x <= 2615 && caster.y <= 3886 )
			SendSysMessage( caster, "Spell disabled in this area");
			return 0;
		endif
	endif

	if ( (cx != caster.x) || (cy != caster.y) || (shp > GetHp(caster)) )
		SendSysMessage( caster, "You lose your concentration." );
		return 0;
	endif

   if (caster.acct && cast_on.acct)//both players
	//party notification here
	 if (caster.party != cast_on.party)
	 	SendPartyMsg(cast_on.party, cast_on, "" + caster.name + " cast a spell on me!");
	 endif
  endif

	return cast_on;

endfunction


function SummonCreatureBard( caster, npctemplate, cast_loc, sfx := 0x5A8, delay := 2 )

	var parms := {};

	parms[1] := caster;
	parms[2] := npctemplate;
	parms[3] := cast_loc;
	parms[4] := sfx;
	parms[5] := delay;

	start_script( ":bardbook:herosummon", parms );

endfunction


function IsProtectedBard( caster, cast_on, circle )

	var protserial := GetObjProperty( cast_on, "MagicImmunity" );
	var protlvl	 := Cint(GetObjProperty( cast_on, "PermMagicImmunity" ));
	if( !protlvl )
		protlvl := 0;
	endif

	var cmi := 0 - protlvl;
	var item;
	var charges;
	var cursed;

	if( protserial )
		foreach thing in ListEquippedItems( cast_on )
			if( thing.serial == protserial )
				item := thing;
				charges := CInt( GetObjProperty( item, "MagicImmunity" ) );
				cursed := GetObjProperty( item, "Cursed" );
				if( !charges )
					EraseObjProperty( cast_on, "MagicImmunity" );
					EraseObjProperty( item, "MagicImmunity" );
					PrintTextAbovePrivate(item, CStr(item.desc + " is out of charges!"), cast_on );
					CheckForAnotherProtItemBard( cast_on, item, "MagicImmunity" );
				endif
				break;
			endif
		endforeach
	endif

	if( charges >= circle )
		charges := charges - circle;
		SetObjProperty( item, "MagicImmunity", charges );
		if( !cursed )
			PrintTextAbovePrivate( cast_on, CStr("Your " + item.desc + " protected you from the spell!"), cast_on );
			return 1;
		else
			return 2;
		endif

	elseif( cmi >= circle )
		return CURSED;

	elseif( protlvl >= circle )
		PrintTextAbovePrivate( cast_on, CStr(caster.name + "'s spell is absorbed by your magical protection!"), cast_on );
		PrintTextAbovePrivate( cast_on, CStr("The spell dissipates upon contact with " + cast_on.name + "'s magical barrier!"), caster );
		return 1;
	endif

	return 0;

endfunction


function CheckForAnotherProtItemBard( who, unused item, prot )

	var prots := { "PoisonImmunity",
			 "MagicImmunity",
			 "SpellReflect" };

	if( !(prot in prots) )
		return 0;
	endif

	EraseObjProperty( who, prot );
	foreach thing in ListEquippedItems( who )
		if( GetObjProperty( thing, prot ) )
			SetObjProperty( who, prot, thing.serial );
			break;
		endif
	endforeach

	return 1;

endfunction


function DoTempModBard( who, stat, amt, duration )

	var parms := {};
	parms[1] := who;
	parms[2] := stat;
	parms[3] := amt;
	parms[4] := duration;

	start_script( ":summoning:processtempmod", parms );
	sleepms(250);

endfunction