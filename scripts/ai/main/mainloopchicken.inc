const GFX_UNSHORN_SHEEP := 0xcf;
const GFX_SHORN_SHEEP := 0xdf;

//include "include/attributes";
function main_AI_loop()

    var ev;
    var wanders := 60;
    var nextlay := CInt(ReadGameClock() + 3600);
    var nextbark := ReadGameClock() + 30;
    var next_wander := 0;

    EnableMainEvents();

	var mydestx := 0;
	var mydesty := 0;
	var steps := 0;

    while (1)

	if (ReadGameClock() > next_wander)

		if (mydestx != 0)
			WalkTowardLocation(mydestx, mydesty);
			if ( (me.x == mydestx && me.y == mydesty) || ( steps > 10 ) )
				mydestx := 0;
			endif
		else
			wander();
			wanders := wanders +1;
		endif

		next_wander := ReadGameClock()+5;
		if (wanders > 60)
			wanders :=0;
			ev := sleepmode();
	    	endif
	else
		ev := os::wait_for_event( 5 );
	endif

	if (ev)
        repeat
	    case (ev.type)
                SYSEVENT_DAMAGED:
                SYSEVENT_ENGAGED:
		    RunLikeHell( ev.source );
		EVID_ALL_FOLLOW_CMD:
			mydestx := ev.x;
			mydesty := ev.y;
			steps := 0;
			drop_anchor();
			WalkTowardLocation(mydestx, mydesty);
            endcase
        until (! (ev := os::wait_for_event(5)) );
	endif

	if (ReadGameClock() > nextlay)
		nextlay := ReadGameClock()+ 52000 + CInt(3800*Random(2));
		CreateItemAtLocation(me.x, me.y, me.z,"egg",1);
	endif

	if ( ReadGameClock() > nextbark )
		nextbark := ReadGameClock() + ( (Random(5)+1) * 60 );
		PlaySoundEffect(me,0x70);
	endif

   endwhile     

endfunction       

function RunLikeHell(opponent)

	var ev, sleepdelay;
	while ( (opponent) && not
	    (opponent.dead || opponent.hidden || opponent.concealed) &&
	    (Distance(me,opponent) < 10) )
	        sleepdelay := 300 - (CInt(GetDexterity(me)) * 1.5);
  		if (sleepdelay < 0) sleepdelay := 0; endif
	    RunAwayFrom( opponent );
		sleepms(sleepdelay);
        repeat
        until (! (ev := os::wait_for_event(1)) );

	endwhile

endfunction

function look_around()

	return;

endfunction

function EnableMainEvents()

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED + SYSEVENT_ENTEREDAREA);
    EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, HALT_THRESHOLD );

endfunction

function DisableMainEvents()

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA);

endfunction