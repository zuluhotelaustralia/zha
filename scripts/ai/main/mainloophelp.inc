include "ai/main/npcinfo";

function main_AI_loop(next_wander := 0)

    var ev;
    var wanders := 60;

	var configfile := ReadConfigFile("::npcdesc");

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

		next_wander := ReadGameClock()+3;
		if (wanders > 60)
			wanders :=0;
			ev := sleepmode();
    		endif
	else
		ev := os::wait_for_event( 3 );
	endif

	if (ev)
        repeat
            case (ev.type)
		SYSEVENT_ENTEREDAREA:
			if ( (ev.source.npctemplate) && (ev.source.script != "tamed") )
                		if (!configfile[ev.source.npctemplate].guardignore)
					say("Ah ha! " + ev.source.name);
                        		fight(ev.source);
                		endif
			endif
                SYSEVENT_SPEECH:
		    if (!ev.source.isA(POLCLASS_NPC))
		    	wanders :=0;
		    	next_wander := ReadGameClock()+30;
		    	check_speech(ev.text, ev.source);
		    endif
                SYSEVENT_ENGAGED:
                SYSEVENT_DAMAGED:
		    EraseObjProperty(ev.source,"#quest");
		    EraseObjProperty(ev.source,"#ignoreit");
                    fight( ev.source );
		SYSEVENT_ITEM_GIVEN:
		    next_wander := ReadGameClock()+60;
		    TakeItem(ev.source,ev.item);
		EVID_ALL_FOLLOW_CMD:
			mydestx := ev.x;
			mydesty := ev.y;
			steps := 0;
			drop_anchor();
			WalkTowardLocation(mydestx, mydesty);
            endcase
        until (! (ev := os::wait_for_event(3)) );
	endif

   endwhile     

endfunction       

function look_around()

	return;

endfunction

function EnableMainEvents()

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED );
    EnableEvents( SYSEVENT_ENTEREDAREA + SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, HALT_THRESHOLD );
    EnableEvents(SYSEVENT_SPEECH, 2 );
    EnableEvents(SYSEVENT_ITEM_GIVEN);

endfunction

function DisableMainEvents()

    DisableEvents( SYSEVENT_ENTEREDAREA + SYSEVENT_SPEECH + SYSEVENT_ENGAGED + SYSEVENT_DAMAGED );
    DisableEvents(SYSEVENT_ITEM_GIVEN);

endfunction