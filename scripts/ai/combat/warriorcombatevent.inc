include "include/dotempmods";
//include "include/attributes";
include "include/random";

function process_combat_event(ev)

	if (GetHp(me) < GetMaxHp(me)/2)
		ApplyHealing();
	endif

        if (ev.type == SYSEVENT_DAMAGED)
		if ((GetHp(me) * 100/GetMaxHp(me)) < flee_point)
			say("me0w!");
			EraseObjProperty(me,"#flees");
			flee(ev.source);
		endif
	endif

endfunction

function in_combat_event_loop(opponent)

	if (GetHp(me) < GetMaxHp(me)/2)
		ApplyHealing();
	endif

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

function ApplyHealing()

	if (!GetObjProperty(me,"#lasthealed"))
		SetObjProperty(me,"#lasthealed", ReadGameClock()-20);
	endif

	if ( GetObjProperty(me,"#lasthealed") < ReadGameClock())

		if (me.poisoned)
			CurePoison(me,0,0);
		else
			SetHp(me, GetHp(me) + (Random(GetMaxHp(me) - GetHp(me))+1));
		endif
		SetObjProperty(me,"#lasthealed", ReadGameClock()+20);

	endif

endfunction