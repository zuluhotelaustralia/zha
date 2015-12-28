include "ai/main/npcinfo";
//include "include/attributes";
include "include/client";

function main_AI_loop()

	var ev;
	var wanders := 60;
	var next_loot := ReadGameClock() + 20;

	var LOOTSTUFF := imalooter(me.npctemplate);

	if ( GetObjProperty(me,"summoned") ) LOOTSTUFF := 0; endif


	EnableMainEvents();

	look_around();

	var mydestx := 0;
	var mydesty := 0;
	var steps := 0;

	while (1)
		//recursively look for orcs to res
		while (lookForOrcToRes())
		endwhile
		
		var hiders := ListMobilesNearLocationEx( me.x, me.y, me.z, HALT_THRESHOLD, LISTEX_FLAG_HIDDEN );
		if( len(hiders) )
			foreach mobile in hiders
				var hiding := GetEffectiveSkill( mobile, SKILLID_HIDING );
				if( !mobile.npctemplate || mobile.script == "tamed")
					if( CheckSkill( me, SKILLID_DETECTINGHIDDEN, CInt(hiding), CInt(hiding*20) ) )
						mobile.hidden := 0;
						Fight( mobile );
						break;
					endif
				endif
			endforeach
		endif

		if (LOOTSTUFF)
			if (ReadGameClock() > next_loot)
				grabloot();
				next_loot := ReadGameClock() + 10;
			endif
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

		if (wanders > 60)
			wanders :=0;
			ev := sleepmode();
		else
			ev := os::wait_for_event( 2 );
		endif

		if (ev)
			repeat
				case (ev.type)
					SYSEVENT_ENTEREDAREA:
						if ( (!ev.source.npctemplate) || (ev.source.script == "tamed") )
							Fight( ev.source );
						endif
					EVID_ALL_ATTACK_CMD:
						Fight( ev.source );
					SYSEVENT_ENGAGED:
					SYSEVENT_DAMAGED:
						if (ev.source)
							get_help( ev.source );
							Fight(ev.source);
						endif
					EVID_ALL_FOLLOW_CMD:
						mydestx := ev.x;
						mydesty := ev.y;
						steps := 0;
						drop_anchor();
						WalkTowardLocation(mydestx, mydesty);
				endcase
			until (!(ev := os::wait_for_event(2)));
		endif

	endwhile

endfunction

function lookForOrcToRes()
		foreach corpse in ListItemsNearLocationOfType( me.x, me.y, me.z, HALT_THRESHOLD, 0x2006 )
		if (CheckLineOfSight( me, corpse ))
			if (! GetObjProperty(corpse, "triedToResButFailed"))
				SetObjProperty(corpse, "triedToResButFailed",1);

				if (GetObjProperty(corpse, "Type") == "Orc")
					var gettocorpsesteps :=0;
					
					while (Distance( me, corpse )&&gettocorpsesteps<40)
						RunTowardLocation(corpse.x, corpse.y);
						gettocorpsesteps:=gettocorpsesteps+1;
						sleepms(250);
					endwhile

					var npctemplate := GetObjProperty(corpse, "npctemplate");
					if(npctemplate)
						Say ("An Corp Orcus");
						PlaySoundEffect(me, SFX_SPELL_RESURRECTION);
						PlaySoundEffect(corpse, SFX_SPELL_RESURRECTION);
						var newmonster := CreateNpcFromTemplate( npctemplate, corpse.x, corpse.y, corpse.z);
						foreach item in EnumerateItemsInContainer( corpse )
							MoveItemToContainer( item, corpse.backpack );
						endforeach
						DestroyItem(corpse);
						SetObjProperty(newmonster, "noloot", 1);
						EraseObjProperty(newmonster, "triedToResButFailed");
						return 1;
					endif
				endif			
			endif
		endif
	endforeach
	return 0;
endfunction

function look_around()
	lookForOrcToRes();
	
	foreach critter in ListMobilesInLineOfSight( me, HALT_THRESHOLD )
		if (!critter.npctemplate || critter.script == "tamed")
			Fight( critter );
		endif
	endforeach

	var hiders := ListMobilesNearLocationEx( me.x, me.y, me.z, HALT_THRESHOLD, LISTEX_FLAG_HIDDEN );
	if( len(hiders) )
		foreach mobile in hiders
			var hiding := GetEffectiveSkill( mobile, SKILLID_HIDING );
			if( !mobile.npctemplate || mobile.script == "tamed")
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

function get_help( opponent )

	var inev;
	var ev array;
	ev.+type := EVID_ALL_ATTACK_CMD;
	ev.+source := opponent;

	foreach critter in ListMobilesNearLocation( me.x, me.y, me.z, 4 )
		if ( (critter.npctemplate) && (critter.script != "tamed") && (!critter.warmode) &&
		     (critter.serial != me.serial) )
			SendEvent(critter, ev);
		endif
		inev := wait_for_event(0);
		if (inev.type == EVID_ALL_ATTACK_CMD)
			return;
		endif
	endforeach

endfunction