include "include/dotempmods";
//include "include/attributes";
include "include/random";

function process_combat_event(ev)

	DoPoison(ev.source);

        if (ev.type == SYSEVENT_DAMAGED)
		if ((GetHp(me) * 100/GetMaxHp(me)) < flee_point)
			EraseObjProperty(me,"#flees");
			flee(ev.source);
		endif
        endif

endfunction

function in_combat_event_loop(opponent)

	DoPoison(opponent);

endfunction

function post_combat()

	DisableCombatEvents();
	EnableMainEvents();

	SetWarMode( 0 );
	SetOpponent( 0 );

	sleep(1);
	look_around();

endfunction

function prepare_for_fight(opponent)

	DisableMainEvents();
	EnableCombatEvents();

endfunction

function DoPoison(you)

	if (!GetObjProperty(me,"#lastbreath"))
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 10);
	endif

	if (GetObjProperty(me,"#lastbreath") > ReadGameClock() )
		return;
	endif

	SetObjProperty(me,"#lastbreath",ReadGameClock() + 10);

	if ( (Distance(me,you)<2) && (Random(4)==1) )

		var plvl := Random(GetMaxHp(me)/40)+1;
		SetPoison(you,plvl,"You have been poisoned by " + me.name);

	endif

endfunction