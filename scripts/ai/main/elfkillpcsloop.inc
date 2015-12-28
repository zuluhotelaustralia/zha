include "ai/main/npcinfo";
//include "include/attributes";
include "include/client";
include "../pkg/opt/queststone/queststone";
include "include/traitorcheck";

const DIST_BEFORE_RECALL := 30;
const DIST_TO_QUESTSTONE := 30;
const SECS_BETWEEN_WARNINGS := 120;

function elf_AI_loop()

	var ev;
	var wanders := 60;
	var next_loot := ReadGameClock() + 20;
	var race := GetRace();
	var enemy:= GetEnemyRace();
	var LOOTSTUFF := imalooter(me.npctemplate);
	var spawnpoint := 0;
	var spawnpointserial := GetObjProperty(me, "SpawnPoint");
	if (spawnpointserial)
		spawnpoint := SystemFindObjectBySerial(spawnpointserial);
	endif
	var isleader:= GetObjProperty(me, "leader");
	var lastwarning := 0;

	if ( GetObjProperty(me,"summoned") ) LOOTSTUFF := 0; endif
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
				if( (!mobile.npctemplate && mobile.title_race!=race)|| mobile.script == "tamed" || GetObjProperty(mobile, enemy))
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
			if (spawnpoint && Distance(mydestx, spawnpoint) > DIST_BEFORE_RECALL)
				RecallToSpawnPoint(spawnpoint);
			else
				WalkTowardLocation(mydestx, mydesty);
				if ( (me.x == mydestx && me.y == mydesty) || ( steps > 10 ) )
					mydestx := 0;
				endif
			endif
		else
			wander();
			if (spawnpoint && Distance(me, spawnpoint) > DIST_BEFORE_RECALL)
				RecallToSpawnPoint(spawnpoint);
			endif
			wanders := wanders + 1;
		endif

		if (wanders > 60)
			wanders :=0;
			if (spawnpoint)
				RecallToSpawnPoint(spawnpoint);
			endif
			ev := sleepmode();
		else
			ev := os::wait_for_event( 2 );
		endif

		if (ev)
			repeat
				case (ev.type)
					SYSEVENT_ENTEREDAREA:
						if (ev.source.title_race==race)
							if (!Random(10))
								say("Welcome, "+race+"!");
							endif
						elseif (ev.source.title_race==enemy || GetObjProperty(ev.source, enemy))
							if (!Random(4))
								say("Prepare to die, "+enemy+"!");
							endif
							get_help(ev.source);
							fight(ev.source);
						elseif (!ev.source.npctemplate)
							get_help(ev.source);
							fight(ev.source);
						endif
					EVID_ALL_ATTACK_CMD:
						Fight( ev.source );
					SYSEVENT_ENGAGED:																	if(GetGlobalProperty("sendwarnings") && !ev.source.npctemplate && isleader)
							foreach item in ListItemsNearLocation(me.x, me.y, me.z, DIST_TO_QUESTSTONE)
								if(item.objtype==0xa322) // queststone
									// send warning message to questers
									lastwarning := ReadGameClock();
									if (lastwarning > GetObjProperty(me, "LastWarning")) 
										QuestBroadcast(item.serial, , , , me.name+": We are being attacked by "+ev.source.name+"!");
										SetObjProperty(me, "LastWarning", lastwarning +SECS_BETWEEN_WARNINGS);
									endif
								endif
							endforeach
						endif
					SYSEVENT_DAMAGED:
						if (ev.source)
							if (ev.source.title_race==race)
								say("Traitor! Kill the heretic!");
								start_script(":spells:settraitor", ev.source);
							endif
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

function look_around()

	foreach critter in ListMobilesInLineOfSight( me, HALT_THRESHOLD )
		if ((!critter.npctemplate && critter.title_race != GetRace()) || critter.script == "tamed" || GetObjProperty(critter, GetEnemyRace()))
			if(critter)
				Fight( critter );
			endif
		endif
	endforeach

	var hiders := ListMobilesNearLocationEx( me.x, me.y, me.z, HALT_THRESHOLD, LISTEX_FLAG_HIDDEN );
	if( len(hiders) )
		foreach mobile in hiders
			var hiding := GetEffectiveSkill( mobile, SKILLID_HIDING );
			if( (!mobile.npctemplate && mobile.title_race!=GetRace()) || mobile.script == "tamed" || GetObjProperty(mobile, GetEnemyRace()))
				say("Someone is hidden here!");
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

function GetRace()

	if (GetObjProperty(me, "Elf"))
		return "Elf";
	else 
		return "Dark-Elf";
	endif

endfunction

function GetEnemyRace()

	if (GetObjProperty(me, "Elf"))
		return "Dark-Elf";
	else 
		return "Elf";
	endif

endfunction

function RecallToSpawnPoint(spawnpoint)
	say("Kal Ort Por");
	PerformAction(me, 16);
	sleepms(2500);
	PlaySoundEffect(me, SFX_SPELL_RECALL1);
	sleepms(250);
	MoveCharacterToLocation(me, spawnpoint.x, spawnpoint.y, spawnpoint.z);
	PlaySoundEffect(me, SFX_SPELL_RECALL2);
	return;
endfunction	

