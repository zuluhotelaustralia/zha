

include "include/random";
//include "include/attributes";

function process_combat_event(ev)

	spellattack(ev.source);

	case (ev.type)
		SYSEVENT_DAMAGED:
			if ( (GetHp(me) < (GetStrength(me)/2)) && !(GetObjProperty( me, "Type" ) == "Undead" ) )
				CastSpell(me,me,"gheal");
				if( me.poisoned )
					CastSpell(me,me,"cure");
				endif
			endif
                        if ((GetHp(me) * 100/GetMaxHp(me)) < flee_point)
				EraseObjProperty(me,"#flees");
                                flee(ev.source);
                        endif
	endcase

endfunction

function spellattack(opponent)

	if (!GetObjProperty(me,"#lastbreath"))
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);
	endif

	if (GetObjProperty(me,"#lastbreath") > ReadGameClock() )
		return;
	endif

	if (Random(2)==1)
		sleep(1);
		cast_offensive_spell(me,opponent);
	endif

	SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);

	foreach myfriend in ListMobilesNearLocation(me.x, me.y, me.z, HALT_THRESHOLD)
		if (myfriend.npctemplate && myfriend.script != "tamed" )
			healerstuff(myfriend);
		endif
	endforeach

endfunction

function in_combat_event_loop(opponent)

	spellattack(opponent);

endfunction      

function prepare_for_fight(opponent)

	if( !GetObjProperty( me, "mr" ) )
		CastSpell( me, me, "reflect" );
	endif

	DisableMainEvents();
	EnableCombatEvents();

endfunction

function post_combat()

	DisableCombatEvents();
	EnableMainEvents();

	SetWarMode( 0 );
	SetOpponent( 0 );

	num_casts := 2;

	sleep(1);
	look_around();

endfunction


function healerstuff(mobile)

if (!mobile.npctemplate || mobile.npctemplate["guard"] || mobile.script == "tamed")
        return;
endif
  
	if( (mobile.poisoned) && (CheckLineOfSight(me,mobile)) )
		TurnToward(mobile);
		CastSpell(me,mobile,"cure");
	elseif( (GetHp(mobile) < (GetMaxHp(mobile)/2)) && (CheckLineOfSight(me,mobile))  )
		TurnToward(mobile);
		CastSpell(me,mobile,"gheal");
	endif
   
endfunction