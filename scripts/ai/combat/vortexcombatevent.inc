
include "include/random";
include "include/damages";
//include "include/attributes";

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

	var ev := array;
	ev.+type := SYSEVENT_DAMAGED;
	ev.+source := me;

	if (opponent)
		if (Distance(me,opponent) < 2)
			var opmagery := CInt(GetEffectiveSkill(opponent,SKILLID_MAGERY));
			if (opmagery)
				SendEvent(opponent, ev);
				//ApplyRawDamage(opponent, Random(CInt(opmagery/15)) ); // Seravy removed because BAD FOR SAFE ZONES
				ApplyTheDamage(opponent, me, Random(CInt(opmagery/15)));
			endif
		endif
	endif

endfunction

function in_combat_event_loop(opponent)

	spellattack(opponent);

endfunction

function prepare_for_fight( unused opponent)

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