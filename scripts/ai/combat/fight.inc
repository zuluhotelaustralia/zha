use uo;
use os;

include "include/random";
include "include/attributes";
include "ai/main/npcinfo";
include "include/cmdlevel_check";

function Fight( opponent )

	if ((opponent.cmdlevel > 0) || (opponent.serial == me.serial))
        	SetWarMode(0);
        	opponent := 0;
		return;
	endif

	if (opponent.npctemplate == "humuc")
        	SetWarMode(0);
        	opponent := 0;
		return;
	endif

	var oldprio := set_priority(50);

	SetOpponent( opponent );
    	prepare_for_fight( opponent );
	TurnToward( opponent );

	if ( GetObjProperty( me, "keywordbattlecry") )
		say( GetObjProperty( me, "keywordbattlecry") );
	endif

	var loops := 0;
	var steps := 0;
    	var ev;
	var waittime := 0;
	var randomruns := 0;
	var oldx := opponent.x;
	var oldy := opponent.y;
	var oldd := Distance(me,opponent);
	var cd;

	var wasmoving:=1;

    while ( (opponent) && not
	    (opponent.dead || opponent.hidden || opponent.concealed) &&
	    (Distance(me,opponent) < 20) )

	cd := CloseDistance( opponent );
	case (cd)
	0:
		//Closedistance returned 0, which means the mob needs to move
		wasmoving := 1;
		loops := loops + 1;
		steps := steps + 1;
		waittime := 0;
		if ( ( oldx >= (opponent.x-1) && (oldx <= opponent.x+1)) &&
		     ( oldy >= (opponent.y-1) && (oldy <= opponent.y+1)) && (oldd > 1) )
			//our opponent hasn't moved much but we did, so:
			if ( (steps > 4) && (oldd <= Distance(me,opponent)) )
				//after 4 steps we're not closing in, so lets do something else:
				randomruns := randomruns + 1;
				if ( runrandom(opponent, randomruns) )
					PeaceMade();
					return;
				endif
				oldd := Distance(me,opponent);
				steps := 0;
				if ( randomruns > 2 )
					//screw it, we can't get to him
					//look around for others to kill
					look_around();
				endif
			else
				//if we're closing in, lets reset the distance and steps
				if ( Distance(me,opponent) < oldd )
					steps := 0;
					oldd := Distance(me, opponent);
				endif
			endif
		else
			//we moved, but so did our opponent, so lets chase him longer:
			oldx := opponent.x;
			oldy := opponent.y;
			steps := 0;
			if ( loops > 20 )
				//screw it, we can't catch him
				//look for others to kill
				look_around();
			endif
		endif
	1:
		//Closedistance returned 1, which means the mob doesn't need to move
		//unless he can't see his opponent
		if (wasmoving) TurnToward(opponent); endif
		wasmoving := 0;

		steps := 0;
		randomruns := 0;
		if ( CheckLineOfSight(me,opponent) )
			loops := 0;
			waittime := 1;
			EraseObjProperty(me,"#flees");
		else
			loops := loops +1;
			waittime := 1;
			RunTowardLocation(me.x+(Random(3)-1), me.y+(Random(3)-1));
		endif
	2:
		//closedistance returned 2, which means the mob was peacemade:
		PeaceMade();
		return;
	3:
		//closedistance returned 3, which means the mob is running away
		loops := 0;//closedistance maintains its own runaway counter
		steps := 0;
		randomruns := 0;
		waittime := 0;
		wasmoving := 1;
	4:
		//closedistance returned 4, which means the opponent went onto a multi
		// edit by harlz to stop them wandering around and remove the recall onto boats which was being abused
	// if (opponent.multi.isA(POLCLASS_BOAT))
	//		DoTeleport(me,opponent);
  //   return;
	//	else

			DoRecall(me);
      return;

   // endif
	endcase

	in_combat_event_loop(opponent);

	ev := wait_for_event( waittime );
	case (ev.type)
		SYSEVENT_DAMAGED:
		SYSEVENT_ENGAGED:
			if (ev.source)
			  process_combat_event(ev);
			  if (Random(6)==1)
				if ((!ev.source.npctemplate) || (ev.source.script == "tamed") )
					opponent := ev.source;
			  		SetOpponent( opponent );
					TurnToward( opponent );
				endif
			  endif
			endif
                EVID_PEACEMADE:
			PeaceMade();
			return;
	endcase

    endwhile

    post_combat();

endfunction

function EnableCombatEvents()

    EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + EVID_PEACEMADE );

endfunction

function DisableCombatEvents()

    DisableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + EVID_PEACEMADE );

endfunction

function flee(opponent)

	EraseObjProperty(me,"killme");

	if ( (me.script == "immobile") || (me.script == "immobilespell") || (me.frozen) || (me.paralyzed) )
		sleep(1);
		return;
	endif

	var numflees := GetObjProperty(me,"#flees");

	if (numflees)
		if ( numflees > 10 )
			// MoveObjectToLocation(me,5288,1176,0, _DEFAULT_REALM,MOVEOBJECT_FORCELOCATION); //AutoConversion
			RevokePrivilege( me, "invul" );
			SetObjProperty(me,"guardkill",1);
			me.kill();
		else
			numflees := numflees + 1;
		endif
	else
		numflees := 1;
	endif

	SetObjProperty(me,"#flees",numflees);

	var runs := 0;
	var ev, sleepdelay;

	while ( (Distance(me, opponent) < 10) && (runs < 25) )
	        sleepdelay := 300 - (CInt(GetDexterity(me)) * 1.5);
  		if (sleepdelay < 0) sleepdelay := 0; endif
		WalkAwayFrom(opponent);
		sleepms(sleepdelay);

		in_combat_event_loop(opponent);
		runs := runs +1;
		ev := wait_for_event(0);
		if (ev.type == SYSEVENT_DAMAGED)
			process_combat_event(ev);
		endif
	endwhile

	SetObjProperty(me, "dont_restart_vitals", 1); // So vitals are not reset to maximum!
	RestartScript( me );

endfunction

function runrandom(opponent, numruns := 2)

	if ( (me.script == "immobile") || (me.script == "immobilespell") )
		sleep(1);
		return 0;
	endif

	var stepstotake := (numruns)+4;

	var tox := me.x;
	var toy := me.y;

	var highx := tox - opponent.x;
	if ( highx < 0 ) highx := 0 - highx; endif
	var highy := toy - opponent.y;
	if ( highy < 0 ) highy := 0 - highy; endif

	case ( highy > highx )
	0:	//run along the x access
		case ( Random(2) )
		0:	//to the right
			toy := (me.y - stepstotake);
			if (Random(2)) tox := opponent.x; endif //to his x
		1:	//to the left
			toy := (me.y + stepstotake);
			if (Random(2)) tox := opponent.x; endif //to his x
		endcase
	1:
		//run along the y access
		case( Random(2) )
		0:	//to the right
			tox := (me.x - stepstotake);
			if (Random(2)) toy := opponent.y; endif //to his y
		1:	//to the left
			tox := (me.x + stepstotake);
			if (Random(2)) toy := opponent.y; endif //to his y
		endcase
	endcase

	var runs := 0;
	var ev;
	while ( (Distance(me, opponent) > 1) && (runs <= stepstotake) )
		//WalkTowardLocation( tox, toy );
		RunTowardLocation( tox, toy);
		in_combat_event_loop(opponent);
		runs := runs +1;
		ev := wait_for_event(0);
		if (ev)
			case (ev.type)
			EVID_PEACEMADE: return 1;
			SYSEVENT_DAMAGED:
				process_combat_event(ev);
				if (ev.source.serial == opponent.serial)
					flee(opponent);
				endif
			endcase
		endif
		if ( (me.x == tox) && (me.y == toy) )
			return 0;
		endif
	endwhile

	return 0;

endfunction

function PeaceMade()

	SetWarMode( 0 );
	SetOpponent( 0 );
	sleep(1);
	DisableCombatEvents();
	EnableMainEvents();
	// EraseObjProperty(me,"#fighttime");

endfunction

function GoToSpawnPoint(spawnpoint)
	say("Kal Ort Por");
	PerformAction(me, 16);
	sleepms(2500);
	PlaySoundEffect(me, SFX_SPELL_RECALL1);
	sleepms(250);
	if (spawnpoint.isA(POLCLASS_ITEM))
		MoveObjectToLocation(me, spawnpoint.x, spawnpoint.y, spawnpoint.z, _DEFAULT_REALM); //AutoConversion
	else
		MoveObjectToLocation(me, spawnpoint[1], spawnpoint[2], spawnpoint[3], _DEFAULT_REALM); //AutoConversion
	endif
	PlaySoundEffect(me, SFX_SPELL_RECALL2);
	DisableCombatEvents();
	EnableMainEvents();
	return;
endfunction

function DoRecall(me)
var spawnpoint := 0;
	var spawnpointserial := GetObjProperty(me, "SpawnPoint");
	if (GetObjProperty(me,"championSpawn"))
		spawnpointserial := GetObjProperty(me,"championSpawn");
	endif
	if (spawnpointserial)
		spawnpoint := SystemFindObjectBySerial(spawnpointserial);
	elseif (GetObjProperty(me,"createdxyz"))
		spawnpoint := GetObjProperty(me,"createdxyz");
	endif
//Recalling only for healers with spawnpoints
		if(spawnpoint != error && spawnpoint != 0)
			GoToSpawnPoint(spawnpoint);
// harlz - a check if the npc has no spawnpoint set by the end of this function he'll just die without leaving a body
		elseif (((spawnpoint == error) || (spawnpoint == 0)) && (me.script !="tamed") && (me.npctemplate))
	   RevokePrivilege( me, "invul" );
	   SetObjProperty( me, "guardkill", 1);
	   ApplyRawDamage( me, GetMaxHp(me)+3 );
	   return;
    endif
endfunction

function DoTeleport(me,opponent)
	say("Rel Por");
	sleep(1);
	PlaySoundEffect(me, 0x1ff);
	MoveObjectToLocation(me, opponent.x, opponent.y, opponent.z, _DEFAULT_REALM); //AutoConversion
endfunction