include "ai/main/npcinfo";
//include "include/attributes";
include "include/client";
include "include/gumps";
include "include/yesno";
include "include/random";

var waypointindex :=1;


function main_AI_loop()
	waypointsetup();
	var ev;
	var wanders := 60;
	var next_loot := ReadGameClock() + 20;

	var LOOTSTUFF := imalooter(me.npctemplate);

	if ( GetObjProperty(me,"summoned") ) LOOTSTUFF := 0; endif


	EnableMainEvents();

	look_around();
	
	var mywpt:=GetCurrentWayPoint();
	var mydestx := mywpt.x;
	var mydesty := mywpt.y;
	var steps := 0;
	while (1)
		look_around();
		WalkTowardLocation(mydestx, mydesty);
		//DestroyNearbyItems();
		if ( (me.x == mydestx && me.y == mydesty)	)
			if (!OneOrLessWaypoints())
				mywpt:=GetNextWayPoint();
				mydestx := mywpt.x;
				mydesty := mywpt.y;
				steps:=0;
			else
				sleep(1);
			endif
		else
			steps:=steps+1;
			//Broadcast(steps);
		endif
		if (steps>150)
			steps:=0;
			TeleToFirstWayPoint();
		endif

		ev := os::wait_for_event( 1 );

		if (ev)
			repeat
				case (ev.type)
					SYSEVENT_ENTEREDAREA:
						look_around();
					EVID_ALL_ATTACK_CMD:
						Fight( ev.source );
					SYSEVENT_ENGAGED:
					SYSEVENT_DAMAGED:
						if (ev.source)
							get_help( ev.source );
							Fight(ev.source);
						endif
					SYSEVENT_SPEECH:
						if (!ev.source.isA(POLCLASS_NPC))
		    				if (lower(ev.text) == "set waypoints" && ev.source.cmdlevel >=2)
								WayPointInfoGump(ev.source);
		    				endif
						endif
					EVID_ALL_FOLLOW_CMD:
						mydestx := ev.x;
						mydesty := ev.y;
						steps := 0;
						drop_anchor();
						WalkTowardLocation(mydestx, mydesty);
				endcase
			until (!(ev := os::wait_for_event(1)));
		endif

	endwhile

endfunction

function waypointsetup()
	if (! GetAllWaypoints())
		var mypos := makeWayPointStruct(me.x, me.y, me.z);
		SetObjProperty(me, "waypoints", {mypos});
	endif
endfunction

function look_around()

	foreach critter in ListMobilesInLineOfSight( me, HALT_THRESHOLD )
		var myfacing := me.facing;
		if ((!critter.npctemplate || critter.script == "tamed") && !critter.cmdlevel)
			case myfacing
			1: //south east
				if (critter.x > me.x && critter.y < me.y)
					IntruderAlert( critter );
				endif
			2: //south 
				if (critter.x > me.x)
					IntruderAlert( critter );
				endif
			3: //south west
				if (critter.x > me.x && critter.y > me.y)
					IntruderAlert( critter );
				endif
			4: //west 
				if (critter.y > me.y)
					IntruderAlert( critter );
				endif
			5: //north west
				if (critter.x < me.x && critter.y > me.y)
					IntruderAlert( critter );
				endif
			6: //north 
				if (critter.x < me.x)
					IntruderAlert( critter );
				endif
			7: //north east
				if (critter.x < me.x && critter.y < me.y)
					IntruderAlert( critter );
				endif
			8: //east 
				if (critter.y < me.y)
					IntruderAlert( critter );
				endif
			endcase
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
	
	foreach critter in ListMobilesInLineOfSight( me, HALT_THRESHOLD ) //hearing
		if(!critter.cmdlevel && RandomDiceStr("1d10") == 1)
			if( !critter.npctemplate || critter.script == "tamed")
				say ("Hmmm... what was that noise");
				sleep(1);
				WalkTowardLocation(critter.x, critter.y);
				look_around();
				break;
			endif
		endif
	endforeach
endfunction

function EnableMainEvents()

	DisableEvents( SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED );
	EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA + SYSEVENT_SPEECH, HALT_THRESHOLD );
endfunction

function DisableMainEvents()

	DisableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA);

endfunction

function get_help( opponent )

	var inev;
	var ev array;
	ev.+type := EVID_ALL_ATTACK_CMD;
	ev.+source := opponent;

	foreach critter in ListMobilesNearLocation( me.x, me.y, me.z, 6 )
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

function IntruderAlert(opponent)
	say ("To arms! Intruder!");
	get_help( opponent );
	Fight( opponent );
endfunction

function GetCurrentWayPoint()
	return GetWaypointByIndex(waypointindex);
endfunction

function GetNextWayPoint()
	var waypoints := GetAllWaypoints();
	waypointindex:=waypointindex+1;
	if (waypointindex>len(waypoints))
		waypointindex:=1;	
	endif
	return GetWaypointByIndex(waypointindex);
endfunction

function OneOrLessWaypoints()
	var waypoints := GetAllWaypoints();
	if (len(waypoints)<=1)
		waypointindex:=1;
	endif
endfunction

function makeWayPointStruct(x, y, z)
	//Broadcast("making struct: " + x + ",  " + y);
	var waypoint:=struct;
	waypoint.+x:=x;
	waypoint.+y:=y;
	waypoint.+z:=z;
	return waypoint;
endfunction

function GetAllWaypoints()
	return GetObjProperty(me, "waypoints");
endfunction

function SetWaypointByIndex(index, x, y, z)
	var waypoints:=array;
	waypoints:=GetObjProperty(me, "waypoints");
	waypoints[index].x:=x;
	waypoints[index].y:=y;
	waypoints[index].z:=z;
	SetObjProperty(me, "waypoints", waypoints);
endfunction

function GetWaypointByIndex(index)
	var waypoints:=array;
	waypoints:=GetAllWaypoints();
	return waypoints[index];
endfunction

function AddWaypoint(who)
	var waypoints:=array;
	waypoints := GetAllWaypoints();
	if (len(waypoints)>20)
		SendSysMessage(who, "Only 20 waypoints per NPC are permitted");
		return;
	endif
	
	SendSysMessage(who, "Select new waypoint location" ,3,53);
	var wp := TargetCoordinates(who);
	if (!wp)
		SendSysMessage(who, "Invalid location");
		return;
	endif
	
	waypoints:=GetObjProperty(me, "waypoints");
	var tempway:=makeWayPointStruct(wp.x, wp.y, wp.z);
	waypoints.append(tempway);
	SetObjProperty(me, "waypoints", waypoints);
endfunction

function WayPointInfoGump(who)
	GFInitGump();
	GFPage(0);
	GFResizePic(0, 0, BKGND_SCROLL, 400, 530);
	GFTextMid(100, 10, 200, 10, "Waypoints on NPC: " + me.name);
	
	
	var waypoints := GetAllWaypoints();
	var i;
	var y := 40;
	
	var hButton:=array;
	var tempstr;
	for(i:=1;i<=len(waypoints);i:=i+1)	
		GFTextLine(50, y, 5, i +": " + waypoints[i].x + ", " + waypoints[i].y + ", " + waypoints[i].z);
		hButton.append(GFButtonID(40, y+3, G_BLUE_SMALL_CIRCLE_1, G_BLUE_SMALL_CIRCLE_0, 1 ));
		y:=y+20;
	endfor
	
	//add new message button
	GFTextLine(50, 470, 53, "ADD NEW WAYPOINT" );
	var addnewbutton := GFButtonID(40, 475, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	
	var choice := GFSendGump(who);
	
	//return values
	if (choice[addnewbutton.keyid])
		AddWayPoint(who);
	endif
		
	var counter:=0;
	
	foreach b in hButton
		counter:=counter+1;
   	if (choice[b.keyid])
			if (YesNo(who, "Delete this waypoint?"))
				DeleteWaypoint(who, counter);
			endif
		endif
	endforeach
	RestartScript( me );
	return 0;
endfunction

function DeleteWaypoint(who, index)
	//Broadcast(index);
	var tempwaypoints:=array;
	var i:=1;
	foreach q in GetObjProperty(me, "waypoints")
		if (i!=index)
			tempwaypoints.append(q);
		endif
		i:=i+1;
	endforeach
	SetObjProperty(me, "waypoints", tempwaypoints);
	SendSysMessage(who, "Waypoint deleted!");
endfunction

function TeleToFirstWayPoint()
	var waypt1:=GetWaypointByIndex(1);
	if (waypt1)
		say("Kal Ort Por");
		PlaySoundEffect( me, SFX_210 );
		sleepms(250);
		MoveCharacterToLocation( me, waypt1.x, waypt1.y, waypt1.z, MOVECHAR_FORCELOCATION );
		PlaySoundEffect( me, SFX_SPELL_RECALL );
	endif
endfunction

//dont know if this is needed
function DestroyNearbyItems()
	foreach item in ListItemsNearLocation( me.x, me.y, me.z, 1 )
		if (item.objtype!=0x2006 && item.movable && !item.newbie) //not corpses
			PerformAction(me, 0x000c);
			PlaySoundEffect(me, 0x238);
			DestroyItem(item);
		endif
	endforeach
endfunction