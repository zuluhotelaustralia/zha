include "include/spelldata";
//include "include/attributes";
include "include/random";

function process_combat_event(ev)

	spellattack(ev.source);
        if (ev.type == SYSEVENT_DAMAGED)
		if ((GetHp(me) * 100/GetMaxHp(me)) < flee_point)
			EraseObjProperty(me,"#flees");
			flee(ev.source);
		endif
        endif

endfunction

function spellattack(opponent);

        cast_meteor(me);

endfunction

function in_combat_event_loop(opponent)

	spellattack(opponent);
        
endfunction

function prepare_for_fight(opponent)

	DisableMainEvents();
	EnableCombatEvents();

endfunction

function post_combat()

	DisableCombatEvents();
	EnableMainEvents();

	SetWarMode( 0 );
	SetOpponent( 0 );

	sleep(1);
	look_around();

endfunction


function cast_meteor( caster )

	var nextbreath := GetObjProperty(me,"#lastbreath");

	if ( (nextbreath) && (nextbreath > ReadGameClock() ) )
		return;
	endif

	SetObjProperty(me,"#lastbreath",ReadGameClock() + CInt(Random(10)+10) );

	var targets := ListMobilesNearLocation(me.x,me.y,me.z, 4);
	PerformAction(me,0x0c);

	var dmg := CInt( GetHp(me) / 15 ) + RandomDiceStr("1d15");
	
	foreach mobile in targets
		if ( (mobile) && (!mobile.npctemplate || mobile.script == "tamed") && (!mobile.cmdlevel))
			PlayMovingEffect(caster, mobile,MOVING_EFFECT_FIREBALL, 10,1,1);
			ApplyElementalDamageNoResist( mobile, caster, dmg, FIRE );
		endif
	endforeach

endfunction