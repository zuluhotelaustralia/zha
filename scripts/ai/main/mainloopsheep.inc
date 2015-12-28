const GFX_UNSHORN_SHEEP := 0xcf;
const GFX_SHORN_SHEEP := 0xdf;

//include "include/attributes";
function main_AI_loop()

    var ev;
    var wanders := 60;
    var next_wander := 0;
    var next_regrow := ReadGameClock()+CInt(Random(1800));
    var nextbark := ReadGameClock() + 30;

    CheckSheepAmount();
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
        until (! (ev := os::wait_for_event(1)) );
	endif

	if (ReadGameClock() > next_regrow)
		next_regrow := ReadGameClock()+CInt(1800+Random(300));
		if ( me.graphic == GFX_SHORN_SHEEP )
			me.graphic := GFX_UNSHORN_SHEEP;
		endif
		CheckSheepAmount();
	endif

	if ( ReadGameClock() > nextbark )
		nextbark := ReadGameClock() + ( (Random(5)+1) * 60 );
		case (Random(2))
		0: PlaySoundEffect(me,0xd7);
		1: PlaySoundEffect(me,0xd8);
		endcase
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
	    case (ev.type)
                SYSEVENT_DAMAGED:
		    if ( Random(3) == 1 )
                    	Fight( ev.source );
		    	return;
                    endif
            endcase
        until (! (ev := os::wait_for_event(1)) );

	endwhile

endfunction

function look_around()

	return;

endfunction

function EnableMainEvents()

    DisableEvents( SYSEVENT_ENTEREDAREA + SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED);
    EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, HALT_THRESHOLD );

endfunction

function DisableMainEvents()

    DisableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED);

endfunction

function CheckSheepAmount()

	var deathchance := -4;
	foreach npc in ListMobilesNearLocation(me.x, me.y, me.z, 4)
		if (npc.npctemplate == "sheep")
			deathchance := deathchance + 5;
		endif
	endforeach

	if ( Random(500) < deathchance )		
		Damage( me, GetMaxHp(me) + 3 );
	endif

endfunction