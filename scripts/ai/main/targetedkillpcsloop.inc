include "ai/main/npcinfo";
//include "include/attributes";
include "include/client";

function main_AI_loop()

	var ev;
	var wanders := 0;


	EnableMainEvents();

	look_around();

	var mydestx := 0;
	var mydesty := 0;
	var steps := 0;

	while (1)

		var hiders := ListMobilesNearLocationEx( me.x, me.y, me.z, HALT_THRESHOLD, LISTEX_FLAG_HIDDEN );
		if( len(hiders) )
			foreach mobile in hiders
				var hiding := GetEffectiveSkill( mobile, SKILLID_HIDING );
				if( mobile.serial == targetserial)
					if( CheckSkill( me, SKILLID_DETECTINGHIDDEN, CInt(hiding), CInt(hiding*20) ) )
						mobile.hidden := 0;
						Fight( mobile );
						break;
					endif
				endif
			endforeach
		endif


		if (mydestx != 0)
			WalkTowardLocation(mydestx, mydesty);
			if ( (me.x == mydestx && me.y == mydesty) || ( steps > 10 ) )
				mydestx := 0;
			endif
		else
			wander();
			wanders := wanders + 1;
		endif

		if (wanders > 6)
			
			KillMobile(me);
		else
			ev := os::wait_for_event( 2 );
		endif

		if (ev)
			repeat
				case (ev.type)
					SYSEVENT_ENTEREDAREA:
						if (ev.source.serial==targetserial )
							Fight( ev.source );
						endif
					SYSEVENT_ENGAGED:
					SYSEVENT_DAMAGED:
						if (ev.source.serial==targetserial)
							Fight(ev.source);
						endif
				endcase
			until (!(ev := os::wait_for_event(2)));
		endif

	endwhile

endfunction

function look_around()

	foreach critter in ListMobilesInLineOfSight( me, HALT_THRESHOLD )
		if (critter.serial == targetserial)
			Fight( critter );
		endif
	endforeach

	var hiders := ListMobilesNearLocationEx( me.x, me.y, me.z, HALT_THRESHOLD, LISTEX_FLAG_HIDDEN );
	if( len(hiders) )
		foreach mobile in hiders
			var hiding := GetEffectiveSkill( mobile, SKILLID_HIDING );
			if( mobile.serial==targetserial)
				if( CheckSkill( me, SKILLID_DETECTINGHIDDEN, CInt(hiding), CInt(hiding*20) ) )
					mobile.hidden := 0;
					Fight( mobile );
					break;
				endif
			endif
		endforeach
	endif

endfunction

function EnableMainEvents()

	DisableEvents( SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED );
	EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA, HALT_THRESHOLD );

endfunction

function DisableMainEvents()

	DisableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA);

endfunction

