/////////////////////////////////////////////////////
// customspells.inc -- Simplified gate and recall code
//                     for use with runebook.src
//
// Author(s): Racalac
// Revisions: 1.0, 11/13/99: Gate, Recall
//                           Mana, Reagent consumption
//                           Normal Spell Restrictions
//
// Notes: (rev1.0) - None
//
// Dependencies: (rev1.0) - client.inc
//                          objtype.inc
//                          spellrestrictions.inc
//
/////////////////////////////////////////////////////

use uo;
use os;

include "include/objtype";
//include "include/attributes";
include "include/spelldata";
include "include/areas";
include "include/client";
include "../pkg/std/housing/utility";

const RECALL_DELAY_MS	:= 2500;
const GATE_DELAY_MS	:= 4000;


function CustomRecall( caster, x, y, z, from_charge := 0 )

	var shp	:= GetHp(caster);
	var sx	:= caster.x;
	var sy	:= caster.y;
	var sz	:= caster.z;
	var smana	:= GetMana(caster);
	var circle	:= 4;
	var spelldelay := RECALL_DELAY_MS;

	caster.hidden := 0;
	PerformAction( caster, ANIM_CAST_DIR );
        SpeakPowerWords( caster, SPELLID_RECALL );

	if( !CanUseBook( caster ) )
		return 0;
	endif

	while( spelldelay >= 500 )
		sleepms( 500 );
		spelldelay := spelldelay - 500;
		if( GetHp(caster) < shp )
			SendSysMessage( caster, "You were interrupted during the casting" );
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE,LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
			return 0;
		endif

		if( caster.x != sx || caster.y != sy || caster.z != sz )
			SendSysMessage( caster, "You've lost your concentration!" );
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
			return 0;
		endif
	endwhile

	sleepms( spelldelay );

	if( GetHp(caster) < shp )
		SendSysMessage( caster, "You were interrupted during the casting" );
		PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
		PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
		return 0;
	endif

	if( caster.x != sx || caster.y != sy || caster.z != sz )
		SendSysMessage( caster, "You've lost your concentration!" );
		PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
		PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
		return 0;
	endif

	if( !from_charge )
        	if(!ConsumeReagents( caster, SPELLID_RECALL ))
		   	PrintTextAbovePrivate(caster,"*No reagents*", caster);
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE,LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
           		return 0;
        	endif

        	if(!ConsumeMana( caster, SPELLID_RECALL ))
		   	PrintTextAbovePrivate(caster,"*No mana*", caster);
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE,LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
           		return 0;
        	endif

	        if(!CheckSkill( caster, SKILLID_MAGERY, GetSpellDifficulty(SPELLID_RECALL), 50 ) )
        		SetMana(caster, smana);
			PrintTextAbovePrivate(caster,"*The spell fizzle*", caster);
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE,LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
           		return 0;
        	endif
        endif

	if( !can_cast( caster, circle ) )
		return 0;
	endif

	if( IsForbiddenAreaToRecallFrom( caster ) )
		SendSysMessage( caster, "You can't recall from there." );
		return 0;
	endif

	var tox := x;
	var toy := y;
	var toz := z;

	if( !Checkpoz(tox,toy) )
		SendSysMessage( caster, "The destination is a magic place with protection." );
		return 0;
	endif

	if( IsForbiddenAreaToRecallTo( caster, tox, toy ) )
		SendSysMessage( caster, "The destination is a magic place with protection." );
		return 0;
	endif

	var multi := GetStandingHeight( tox, toy, toz ).multi;
	if( multi )
		if( !IsOwner( caster, multi )  )
			if( !IsFriend( caster, multi ) )
				SendSysMessage( caster, "You can't recall to this house." );
				return;
			endif
		endif
	endif

	PlaySoundEffect( caster, SFX_SPELL_RECALL1 );
	sleepms(250);

	MoveCharacterToLocation( caster, tox, toy, toz );
	PlaySoundEffect( caster, SFX_SPELL_RECALL2 );

        return 1;

endfunction


function CustomGate( caster, x, y, z )

	var shp	:= GetHp(caster);
	var sx	:= caster.x;
	var sy	:= caster.y;
	var sz	:= caster.z;
	var smana	:= GetMana(caster);
	var circle	:= 7;
	var spelldelay := GATE_DELAY_MS;

	caster.hidden := 0;
	PerformAction( caster, ANIM_CAST_DIR );
        SpeakPowerWords( caster, SPELLID_GATE_TRAVEL );

	if( !CanUseBook( caster ) )
		return 0;
	endif
		if ( caster.x>=2555 && caster.y>=478 && caster.x <= 2615 && caster.y <= 3886 )
			SendSysMessage( caster, "Spell disabled in this area");
			return;
		endif

	while( spelldelay >= 500 )
		sleepms( 500 );
		spelldelay := spelldelay - 500;
		if( GetHp(caster) < shp )
			SendSysMessage( caster, "You were interrupted during the casting" );
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE,LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
			return 0;
		endif

		if( caster.x != sx || caster.y != sy || caster.z != sz )
			SendSysMessage( caster, "You've lost your concentration!" );
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE,LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
			return 0;
		endif
	endwhile

	sleepms( spelldelay );

	if( GetHp(caster) < shp )
		SendSysMessage( caster, "You were interrupted during the casting" );
		PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
		PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
		return 0;
	endif

	if( caster.x != sx || caster.y != sy || caster.z != sz )
		SendSysMessage( caster, "You've lost your concentration!" );
		PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
		PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
		return 0;
	endif

        if(!ConsumeReagents( caster, SPELLID_GATE_TRAVEL ))
	   	PrintTextAbovePrivate(caster,"*No reagents!*", caster);
		PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
		PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
        	return 0;
        endif

        if(!ConsumeMana( caster, SPELLID_GATE_TRAVEL ))
	   	PrintTextAbovePrivate(caster,"*No mana!*", caster);
		PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
		PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
        	return 0;
        endif

        if(!CheckSkill( caster, SKILLID_MAGERY, GetSpellDifficulty(SPELLID_GATE_TRAVEL), 50 ) )
	       	SetMana(caster, smana);
		PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
		PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
		SendSysMessage(caster, "The spell fizzles");
        	return 0;
        endif

	if( !can_cast(caster, circle) )
		return 0;
	endif

	if( IsForbiddenAreaToGateFrom( caster ) )
		SendSysMessage( caster, "You can't gate from there." );
		return 0;
	elseif( IsInArea( caster, AREAID_CRAFTMINE ) ) //safe zone
		SendSysMessage( caster, "You can't gate from there." );
		return 0;
	endif

	if( caster.multi )
		if( !IsOwner( caster, caster.multi )  )
			if( !IsFriend( caster, caster.multi ) )
				SendSysMessage( caster, "You can't gate from this house." );
				return;
			endif
		endif
	endif

	var tox := x;
	var toy := y;
	var toz := z;

	if( IsForbiddenAreaToGateTo( caster, tox, toy ) )
		SendSysMessage(caster, "The destination is a magic place with protection.");
		return 0;
	endif

	var multi := GetStandingHeight( tox, toy, toz ).multi;
		if( multi )
		if( !IsOwner( caster, multi )  )
			if( !IsFriend( caster, multi ) )
				SendSysMessage( caster, "You can't gate to this house." );
				return;
			endif
		endif
	endif

	set_critical(1);
	var pregate := CreateItemAtLocation( caster.x, caster.y, caster.z, 0x1af3, 1 );
	pregate.movable := 0;
	pregate.facing := 1;
	PlaySoundEffect( caster, SFX_SPELL_GATE_TRAVEL );
	sleepms(1500);

	var gate1 := CreateItemAtLocation( caster.x, caster.y, caster.z, UOBJ_BLUE_MOONGATE, 1 );
	gate1.movable := 0;
	gate1.facing := 1;
	DestroyItem(pregate);

	var gate2 := CreateItemAtLocation( tox, toy, toz, UOBJ_BLUE_MOONGATE, 1 );
	gate2.facing := 2;

	if (!gate1 || !gate2)
		DestroyItem(gate1);
		DestroyItem(gate2);
		return 0;
	endif

	SetObjProperty( gate1, "GateDestX", gate2.x );
	SetObjProperty( gate1, "GateDestY", gate2.y );
	SetObjProperty( gate1, "GateDestZ", gate2.z );
	SetObjProperty( gate1, "f", 1);
	SetObjProperty( gate1, "gateHasWarnings", 1);
	SetObjProperty( gate2, "GateDestX", gate1.x );
	SetObjProperty( gate2, "GateDestY", gate1.y );
	SetObjProperty( gate2, "GateDestZ", gate1.z );
	SetObjProperty( gate2, "f", 1);
	SetObjProperty( gate2, "gateHasWarnings", 1);

	Detach();

	set_critical(0);

	PlaySoundEffect( caster, SFX_SPELL_GATE_TRAVEL );

	sleep(30);

	set_critical(1);
	DestroyItem( gate1 );
	DestroyItem( gate2 );
	set_critical(0);

        return 1;

endfunction


function CanUseBook( me )

	var hand1 := GetEquipmentByLayer( me, LAYER_HAND1 );
	var hand2 := GetEquipmentByLayer( me, LAYER_HAND2 );

	if( hand1 || hand2 )
		var weapcfg := ReadConfigFile( ":*:itemdesc" );

		if( hand1 && weapcfg[hand1.objtype].BlocksCastingIfInHand != 0 )
			SendSysMessage( me, "Your hands must be free to use that." );
			PlayObjectCenteredEffect( me, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
			PlaySoundEffect( me, SFX_SPELL_FIZZLE );
			return 0;
		endif

		if( hand2 && weapcfg[hand2.objtype].BlocksCastingIfInHand != 0)
			SendSysMessage( me, "Your hand must be free to use that." );
			PlayObjectCenteredEffect( me, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
			PlaySoundEffect( me, SFX_SPELL_FIZZLE );
			return 0;
		endif
	endif

	return 1;

endfunction
