include "include/random";
//include "include/attributes";

function process_combat_event(ev)

   if (ev.type == SYSEVENT_DAMAGED)
		if ((GetHp(me) * 100/GetMaxHp(me)) < flee_point)
			EraseObjProperty(me,"#flees");
			flee(ev.source);
		endif
   endif

endfunction

function in_combat_event_loop(opponent)

	ThrowPotion(opponent);

endfunction

function post_combat()

	DisableCombatEvents();
	EnableMainEvents();

	SetWarMode( 0 );
	SetOpponent( 0 );

	sleep(1);
	look_around();

endfunction

function prepare_for_fight(unused opponent)

	DisableMainEvents();
	EnableCombatEvents();

endfunction

function ThrowPotion(you)

	if (!you)
		return;
	endif

	if (!GetObjProperty(me,"#lastbreath"))
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);
	endif

	if (GetObjProperty(me,"#lastbreath") > ReadGameClock() )
		return;
	endif

	SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);

	if ( (Distance(me,you)>2) && Random(2) )

		var parms := {};
		parms[1] := me;
		parms[2] := you;

		start_script( ":alchemy:explosion", parms );

	endif

endfunction