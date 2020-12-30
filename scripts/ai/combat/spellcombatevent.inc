
include "include/random";
//include "include/attributes";

function process_combat_event(ev)

	spellattack(ev.source);

	case (ev.type)
        	SYSEVENT_DAMAGED:
			if ( ( GetHp(me) < (GetStrength(me)/2)) && !(GetObjProperty( me, "Type" ) == "Undead" ) )
				CastSpell(me,me,"gheal");
				sleepms(500);
				if( me.poisoned )
					CastSpell(me,me,"cure");
					sleepms(500);
					CastSpell(me,ev.source,"poison");
				endif
			endif
                        if ((GetHp(me) * 100/GetMaxHp(me)) < flee_point)
				EraseObjProperty(me,"#flees");
                                flee(ev.source);
                        endif
			break;
        	default: break;
	endcase

endfunction

function spellattack(opponent)

	if (!GetObjProperty(me,"#lastbreath"))
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 2);
	endif

	if (GetObjProperty(me,"#lastbreath") > ReadGameClock() )
		return;
	endif

	if (Random(3) != 1)
		sleep(1);
		cast_offensive_spell(me,opponent);
	endif

	SetObjProperty(me,"#lastbreath",ReadGameClock() + 2);

endfunction

function in_combat_event_loop(opponent)

	spellattack(opponent);

endfunction

function prepare_for_fight(unused opponent)
	if( !MagicResistant())
		CastSpell( me, me, "reflect" );
	endif

	DisableMainEvents();
	EnableCombatEvents();

endfunction

function MagicResistant()
	if( GetObjProperty( me, "mr") )
		return 1;
	endif
	if (GetObjProperty(me,  "PermMagicImmunity") > 3)
		return 1;
	endif
	return 0;
endfunction

function post_combat()

	if( GetHp(me) < (GetStrength(me)/2) && !(GetObjProperty( me, "Type" ) == "Undead" ) )
		CastSpell(me,me,"gheal");
		sleepms(500);
	endif

	DisableCombatEvents();
	EnableMainEvents();

	SetWarMode( 0 );
	SetOpponent( 0 );

	summons := 1;

	sleep(1);
	look_around();

endfunction