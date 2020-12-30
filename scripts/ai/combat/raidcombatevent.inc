function process_combat_event( unused ev)

endfunction

function in_combat_event_loop2( unused opponent, loops)

	if ( loops > 100 )
		SetWarMode(0);
		return;
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

function prepare_for_fight( unused opponent)

	DisableMainEvents();
	EnableCombatEvents();

endfunction