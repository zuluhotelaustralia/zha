//include "include/attributes";
function main_AI_loop()

    var ev;
    var wanders := 60;

    EnableMainEvents();

    look_around();

	var mydestx := 0;
	var mydesty := 0;
	var steps := 0;

    while (1)

	if (mydestx != 0)
		WalkTowardLocation(mydestx, mydesty);
		if ( (me.x == mydestx && me.y == mydesty) || ( steps > 10 ) )
			mydestx := 0;
		endif
	else
		wander();
		wanders := wanders +1;
	endif

	if (wanders > 60)
		wanders :=0;
		ev := sleepmode();
	else
		ev := os::wait_for_event( 1 );
	endif

	if (ev)
	repeat
	    case (ev.type)
                SYSEVENT_ENTEREDAREA:
                SYSEVENT_ENGAGED:
                SYSEVENT_DAMAGED:
			if ( (ev.source.script != "humuc") && (!ev.source.script["merchant"]) )
				Fight( ev.source);
			endif
		EVID_ALL_FOLLOW_CMD:
			mydestx := ev.x;
			mydesty := ev.y;
			steps := 0;
			drop_anchor();
			WalkTowardLocation(mydestx, mydesty);
	    endcase
        until (! (ev := os::wait_for_event(1)) );
	endif

   endwhile     

endfunction       

function look_around()

	var max_mana := -1;
	var targ;

	foreach critter in ListMobilesInLineOfSight( me, HALT_THRESHOLD )
		if( ( GetMana(critter) > max_mana ) && (!critter.script["humuc"]) && (!critter.script["merchant"]) )
			max_mana := GetMana(critter);
			targ := critter;
		endif
	endforeach

        if( targ )
		Fight( targ );
		return;
	endif

endfunction

function EnableMainEvents()

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED );
    EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA, HALT_THRESHOLD );

endfunction

function DisableMainEvents()

    DisableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA);

endfunction