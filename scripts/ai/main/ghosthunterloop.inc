include "ai/main/npcinfo";
//include "include/attributes";
include "include/client";
//include "include/spelldata";
const DIST_BEFORE_RECALL := 30;
var teleportrange;
function main_AI_loop()

	var ev;
	var wanders := 60;
	var next_loot := ReadGameClock() + 20;
	var spawnpoint := 0;
	var spawnpointserial := GetObjProperty(me, "SpawnPoint");
	if (spawnpointserial)
		spawnpoint := SystemFindObjectBySerial(spawnpointserial);
	endif	

	teleportrange:=GetGlobalProperty ("ghosthunterrange");
	if (!teleportrange)
		teleportrange:=20;
	endif
	var LOOTSTUFF := imalooter(me.npctemplate);

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
				if( !mobile.npctemplate || mobile.script == "tamed")
					if( CheckSkill( me, SKILLID_DETECTINGHIDDEN, CInt(hiding), CInt(hiding*20) ) )
						mobile.hidden := 0;
						Fight( mobile );
						break;
					endif
				endif
			endforeach
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
					EVID_ALL_ATTACK_CMD:
						if (ev.source)
							BanishGhost(ev);
						endif
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

function look_around()

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
	EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA, HALT_THRESHOLD);

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
		     (critter.serial != me.serial))
			SendEvent(critter, ev);
		endif
		inev := wait_for_event(0);
		if (inev.type == EVID_ALL_ATTACK_CMD)
			return;
		endif
	endforeach

endfunction

function BanishGhost (ev)

	var ghost:= ev.source;
	TeleToGhost(ghost);

	var dx := ev.x;
	if (!dx)
		dx:=1475;
	endif
	var dy := ev.y;
	if (!dy)
		dy:=1645;
	endif
	var dz:= ev.z;
	if (!dz)
		dz := GetWorldHeight(dx, dy);
	endif
	var range:=ev.range;
	Detach();

	PlaySoundEffect(me, SFX_SPELL_RECALL2);
	sleepms(1000);
	Say("Begone, "+ghost.name+"! I banish thee!");

	var banishtime:=GetObjProperty(me, "banishtime");
	for i:= 1 to banishtime
		if (Distance(me, ghost) > 10)
			TeleToGhost(ghost);
			PlaySoundEffect(me, SFX_SPELL_TELEPORT);
		endif
		if (i%2==0)
			PerformAction(me, 17);
			PlaySoundEffect(me, 0x24b);  // spirit speak			
		endif
		sleepms(1000);
		if (!ghost.dead && me) 
			sleep(2);
			Say("Kal Ort Por");
			PerformAction(me, 16);
			sleepms(2000);
			if (me)
				PlaySoundEffect(me, SFX_SPELL_RECALL1);
				MoveCharacterToLocation(me, 5274, 1162, 0);
				ApplyRawDamage(me, GetHp(me)+3);
			endif
		endif	

		if (!me)
			break;
		endif
	endfor
	if (me)
		PrintTextAbove(ghost, "*Banished*");
		sleepms(2000);
		PlaySoundEffect(ghost, SFX_SPELL_RECALL1);
		sleepms(500);
		MoveCharacterToLocation(ghost, dx, dy, dz);
		PlaySoundEffect(ghost, SFX_SPELL_RECALL2);
		sleep(5);
		if (me)
			Say("Kal Ort Por");
			PerformAction(me, 16);
			sleepms(2000);
			if (me)
				PlaySoundEffect(me, SFX_SPELL_RECALL1);
				MoveCharacterToLocation(me, 5274, 1162, 0);
				ApplyRawDamage(me, GetHp(me)+3);
			endif
		endif
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

function TeleToGhost(ghost)

	var x, y, z, j:=0;
	Repeat
		j:=j+1;
		x:= RandomInt(4)+1;
		y:= RandomInt(4)+1;
		if (RandomInt(2))
			x := x * -1;
		endif
		if (RandomInt(2))
			y := y * -1;
		endif
		x := x + ghost.x;
		y := y + ghost.y;
		z := GetWorldHeight(x, y);
	Until (MoveCharacterToLocation(me, x, y, z, MOVECHAR_FORCELOCATION) || j>20);
	
	return;
endfunction

