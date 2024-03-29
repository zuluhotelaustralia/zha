include "include/client";
//include "include/attributes";

function sleepmode()

	foreach critter in ListMobilesNearLocation( me.x, me.y, me.z, 20 );
		if (!critter.isA(POLCLASS_NPC))
			return 0;
		endif
	endforeach

	var hiders := ListMobilesNearLocationEx( me.x, me.y, me.z, 5, LISTEX_FLAG_HIDDEN );
	foreach mobile in hiders
		var hiding := GetEffectiveSkill( mobile, SKILLID_HIDING );
		if( !mobile.npctemplate || mobile.script == "tamed")
			if( CheckSkill( me, SKILLID_DETECTINGHIDDEN, CInt(hiding), CInt(hiding*20) ) )
				mobile.hidden := 0;
				return 0;
			endif
		endif
	endforeach

	if (GetObjProperty(me, "killme"))
		ApplyRawDamage(me, GetHp(me)+1);
	endif

	EraseObjProperty(me, "#flees");

	DisableMainEvents();
	if ( (GetEffectiveSkill(me, 21) > 0 ) && (me.script != "merchant") )
		me.hidden := 1;
		EnableEvents(SYSEVENT_ENTEREDAREA, 4);
	else
		EnableEvents(SYSEVENT_ENTEREDAREA, 18);
	endif

	EnableEvents(SYSEVENT_ENGAGED);
	EnableEvents(SYSEVENT_DAMAGED);

	var ev;

	while (1)

		ev := os::wait_for_event( 120 );

		if (ev)

			case (ev.type)
				SYSEVENT_ENGAGED:
				SYSEVENT_DAMAGED:
				EVID_ALL_ATTACK_CMD:
					if (me.hidden)
						me.hidden := 0;
						say("*" + me.name + " springs from hiding!*");
					endif
					EnableMainEvents();
					return ev;
				SYSEVENT_ENTEREDAREA:
					if ( (!ev.source.dead) && !ev.source.concealed && !ev.source.hidden )
						if ( (GetHp(me) > (GetMaxHp(me)/2)) || (!me.hidden) )
							if ( (!ev.source.npctemplate) || (ev.source.script == "tamed") )
								if (me.hidden)
									me.hidden := 0;
									say("*" + me.name + " springs from hiding!*");
								endif
								EnableMainEvents();
								return ev;
							endif
						endif
					endif
			endcase

		endif

	endwhile

endfunction

function cleareventque()

	var ev;
	repeat
	until (! (ev := os::wait_for_event(0)) );

endfunction