use util;
include "include/gumps";
include "../pkg/opt/queststone/queststone";
//this ai will respond to a set of keywords with responses....
//these are stored in a 2d array...
//each word index must be unique

function main_AI_loop()
	
    var ev;
    //var wanders := 60;
    //var next_wander := ReadGameClock();
    //var next_news := ReadGameClock();
    EnableMainEvents();
	bpsetup();
	var mydestx := 0;
	var mydesty := 0;
	var steps := 0;

	SetAnchor( CInt(me.x), CInt(me.y), 4, 50 );

    while (1)

	ev := os::wait_for_event( 2 );
	if (ev)
        repeat
            case (ev.type)
					SYSEVENT_ENTEREDAREA:
						if (!ev.source.isA(POLCLASS_NPC))
							GiveReport(ev.source);
						endif          
					SYSEVENT_SPEECH:
		    			if (!ev.source.isA(POLCLASS_NPC))
		    				if (lower(ev.text) == "set attackers" && ev.source.cmdlevel >=2)
								BountyPointInfoGump(ev.source);
		    				endif
		    				if (lower(ev.text) == "report")
								GiveReport(ev.source);	
							endif
		    			endif
	
               SYSEVENT_ENGAGED:
 	            SYSEVENT_DAMAGED:
                   Fight( ev.source );
		   			 ReturnHome();
					EVID_ALL_FOLLOW_CMD:
            endcase
        until (! (ev := os::wait_for_event(2)) );
	endif

   endwhile     

endfunction       

function bpsetup()
	var bountypoints := GetAllBountyPoints();
	if (!bountypoints || bountypoints==error || !bountypoints[1] || bountypoints[1]==error)
		SetObjProperty(me, "bountypoints", {});
	endif
	var mserials := GetObjProperty (me, "currentbountyserials");
	if (!mserials || mserials==error || !mserials[1] || mserials[1]==error)
		SetObjProperty(me, "currentbountyserials", {});
	endif
endfunction

function look_around()
	return;
endfunction

function EnableMainEvents()

    DisableEvents( SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED );
    EnableEvents( SYSEVENT_SPEECH + SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA, HALT_THRESHOLD );
    EnableEvents( SYSEVENT_SPEECH, 3 );
    DisableEvents(SYSEVENT_ITEM_GIVEN);

endfunction

function DisableMainEvents()
    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_ENGAGED + SYSEVENT_DAMAGED );
    DisableEvents(SYSEVENT_ITEM_GIVEN);
endfunction

function makeBountyPointStruct(npctemplate, x, y, z, num_monsters)
	var bountypoint:=struct;
	bountypoint.+npctemplate:=npctemplate;
	bountypoint.+x:=x;
	bountypoint.+y:=y;
	bountypoint.+z:=z;
	bountypoint.+num_monsters:=num_monsters;
	return bountypoint;
endfunction

function GetAllBountyPoints()
	return GetObjProperty(me, "bountypoints");
endfunction

function SetBountPointByIndex(index, bpstruct)
	var bountypoints:=array;
	bountypoints:=GetObjProperty(me, "bountypoints");
	bountypoints[index]=bpstruct;
	SetObjProperty(me, "bountypoints", bountypoints);
endfunction

function GetBountyPointByIndex(index)
	var bountypoints:=array;
	bountypoints:=GetAllBountyPoints();
	return bountypoints[index];
endfunction

function AddBountyPoint(who)
	var quest := GetObjProperty (me, "AssocQuestStoneSer");
	if (!quest)
		SendSysMessage(who, "Choose a quest stone to associate with outpost guard first!");
		return 0;
	endif
	if (quest)
		var stone:= SystemFindObjectBySerial(quest);
		if (!stone)
			SendSysMessage(who, "Invalid quest stone");
			return 0;
		endif
	endif
	var bountypoints:=array;
	bountypoints := GetAllBountyPoints();
	if (len(bountypoints)>20)
		SendSysMessage(who, "Only 20 attacker points per NPC are permitted");
		return;
	endif
	
	var bp:=enterBountyPoint(who);	
	bountypoints:=GetObjProperty(me, "bountypoints");
	bountypoints.append(bp);
	SetObjProperty(me, "bountypoints", bountypoints);
endfunction

function BountyPointInfoGump(who)
	GFInitGump();
	GFPage(0);
	GFResizePic(0, 0, BKGND_SCROLL, 600, 530);
	GFTextMid(100, 10, 200, 10, "Attacker spawns for outpost guards " + me.name);
	
	
	var bountypoints := GetAllBountyPoints();
	
	var i;
	var y := 40;
	
	var hButton:=array;
	var tempstr;
	if (bountypoints)
		for(i:=1;i<=len(bountypoints);i:=i+1)	
			var tempstr:= bountypoints[i].npctemplate + " " + bountypoints[i].x + ", " +	bountypoints[i].y + ", " + bountypoints[i].z +". No. " + bountypoints[i].num_monsters;
			GFTextLine(50, y, 5, i +": " + tempstr);
			hButton.append(GFButtonID(40, y+3, G_BLUE_SMALL_CIRCLE_1, G_BLUE_SMALL_CIRCLE_0, 1 ));
			y:=y+20;
		endfor
	endif
	//add new message button
	GFTextLine(50, 470, 53, "ADD NEW ATTACKER SPAWN POINT" );
	var addnewbutton := GFButtonID(40, 475, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );

	//associate quest
	GFTextLine(50, 490, 53, "ASSOCIATE QUEST STONE [DO THIS FIRST!!!]" );
	var assocqueststone := GFButtonID(40, 495, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );

	//
	GFTextLine(50, 510, 53, "SPECIFY TIME INTERVAL BETWEEN SPAWNINGS" );
	var specifytime:= GFButtonID(40, 515, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );

	
	var choice := GFSendGump(who);
	
	//return values
	if (choice[addnewbutton.keyid])
		AddBountyPoint(who);
	elseif(choice[assocqueststone.keyid])
		associateQuestStone(who);
	elseif(choice[specifytime.keyid])
		SpecifyTimeInterval(who);
	endif
		
	var counter:=0;
	
	foreach b in hButton
		counter:=counter+1;
   	if (choice[b.keyid])
			if (YesNo(who, "Delete this attacker spawn point?"))
				DeleteBountyPoint(who, counter);
			endif
		endif
	endforeach
	//RestartScript( me );
	return 0;
endfunction

function DeleteBountyPoint(who, index)
	//Broadcast(index);
	var tempbountypoints:=array;
	var i:=1;
	foreach q in GetObjProperty(me, "bountypoints")
		if (i!=index)
			tempbountypoints.append(q);
		endif
		i:=i+1;
	endforeach
	SetObjProperty(me, "bountypoints", tempbountypoints);
	SendSysMessage(who, "Attacker spawn point deleted!");
endfunction

function enterBountyPoint(who)
	var mtemp := SendTextEntryGump( who, "Enter Monster Template. Assumes valid template");
	if (! mtemp)
		return;
	endif
	SendSysMessage( who, "Select position");
	var bp := TargetCoordinates(who);
	if (!bp)
		SendSysMessage(who, "Invalid location");
		return;
	endif
	var timetemp := CInt(SendTextEntryGump( who, "Number to spawn:"));
	if (! timetemp)
		return;
	endif
	var bpstruct:=makeBountyPointStruct(mtemp, bp.x, bp.y, bp.z, timetemp);
	return bpstruct;
endfunction

function GiveReport(who)
	var bountypoints := GetAllBountyPoints();
	if (!bountypoints || bountypoints==error || !bountypoints[1] || bountypoints[1]==error)
		return;
	endif

	if (!QuestCheck(who))
		return;
	endif
	
	if (!HasAmbushTimeElapsed())
		foreach mob in ListMobilesNearLocation(me.x, me.y, me.z, 20)
			if (IsHostile(mob))
				say("To arms! To arms! We are under attack!");
				return;
			endif
		endforeach
		say("Its all quiet at the moment, my lord.");
		return;
	endif
	
	RemoveCurrentMonsters();
	CreateNewMonsters(who);
	say("My lord, we are under attack!");	
endfunction


function ReturnHome()

	var myhome := GetObjProperty(me,"myhome");
	MoveObjectToLocation(me,myhome[1],myhome[2],myhome[3], _DEFAULT_REALM,MOVEOBJECT_FORCELOCATION); //AutoConversion

endfunction

function associateQuestStone(who)
	SendSysMessage( who, "Select Queststone." );
	var qstone := Target( who );
	
   if( !qstone || qstone.objtype != 41762) // queststone objtype
   	SendSysMessage( who, "canceled." );
     	return;
   endif

	SetObjProperty (me, "AssocQuestStoneSer", qstone.serial);
	SendSysMessage( who, "Outpost watcher associated with queststone." );
endfunction

function SpecifyTimeInterval(who)
	SendSysMessage(who, "This is the time period in minutes between spawns.");
	var timetemp := SendTextEntryGump( who, "Enter time interval (mins)");
	if (! timetemp)
		return;
	endif
	SetObjProperty(me, "TimeInterval", CInt(timetemp)*60);
endfunction

function HasAmbushTimeElapsed()
	//returns true if time elapsed
	var lastwalkedTime := CInt(GetObjProperty(me, "#lastspawned"));
	if (ReadGameClock()>lastwalkedTime) //that is current time is greater than reserved time
		return 1;
	endif
	return 0;
endfunction

function SetCurrentAmbushTime()
	SetObjProperty(me, "#lastspawned", ReadGameClock()+CInt(GetObjProperty(me, "TimeInterval")));
endfunction


function QuestCheck(who)
	var quest := GetObjProperty (me, "AssocQuestStoneSer");
	if (quest)
		var stone:= SystemFindObjectBySerial(quest);
		if (!stone)
			return 0;
		endif
		if (!IsQuestDeployed(stone) || !IsRegistered(stone, who.serial))
			return 0;			
		endif
	endif
	return 1;
endfunction

function RemoveCurrentMonsters()
	var stone_serial := Cint(GetObjProperty (me, "AssocQuestStoneSer"));
	var currentmonsterserials := GetObjProperty(me, "currentbountyserials");
	foreach ms in currentmonsterserials	
		//kill it if it exists
		var monst:=GetMonsterFromSerial(CInt(ms));
		if (monst)
			RemoveObjective(stone_serial, CInt(ms));
			RevokePrivilege( monst, "invul" );
			SetObjProperty( monst, "guardkill", 1);
			ApplyRawDamage( monst, CInt(GetHp(monst)+3) );
		endif
	endforeach
	//clear serials
	SetObjProperty(me, "currentbountyserials", {});
endfunction

function GetMonsterFromSerial(serial)
	if (!serial || serial == error)
		return 0;
	endif
	var monster := SystemFindObjectBySerial(serial);
	if (!monster || monster == error)
		return 0;
	endif

	return monster;
endfunction

function CreateNewMonsters(who)
	var mserials:=array;
	var stone_serial := Cint(GetObjProperty (me, "AssocQuestStoneSer"));
		if (!len(GetAllBountyPoints()))
		return 0;
	endif
	var rand_num:= RandomDiceRoll( "1d"+CStr(len(GetAllBountyPoints()) ));
	var all_bps:=GetAllBountyPoints();
	var rand_bp:=all_bps[rand_num];
	var i:=1;
	//spawn monster of that type at location
	while (i<=rand_bp.num_monsters)
		var t_monst:=SpawnMonster(rand_bp, stone_serial, who);
		if (t_monst)
			mserials.append(t_monst.serial);
		endif
		i:=i+1;
	endwhile;
	SetObjProperty(me, "currentbountyserials", mserials);
	SetCurrentAmbushTime();
endfunction

function SpawnMonster(attack_point, stone_serial, who)
	if (!attack_point.npctemplate || !stone_serial)
		return 0;
	endif
	var monst:=CreateNpcFromTemplate( attack_point.npctemplate ,CInt(attack_point.x),CInt(attack_point.y),CInt(attack_point.z));
	if (!monst)
		monst:=CreateNpcFromTemplate( attack_point.npctemplate,CInt(me.x),CInt(me.y),CInt(me.z));
		MoveObjectToLocation(monst,attack_point.x,attack_point.y,attack_point.z, _DEFAULT_REALM,MOVEOBJECT_FORCELOCATION); //AutoConversion
	endif
	if (!monst)
		return 0;
	endif	
	var monstanchor:=struct;
	monstanchor.+x:=who.x;
	monstanchor.+y:=who.y;
	monstanchor.+range:=1;
	monstanchor.+psub:=50;
	SetObjProperty(monst, "Anchor", monstanchor);
	
	AddMonsterObjective(stone_serial, monst, DetermineNPCTemplateQPs(attack_point.npctemplate));
	SetObjProperty(monst, "guardignore", 1);
	SetObjProperty(monst, "RemoveFromQuestAfterDeath", 1);
	monst.title_guild := "Raider";
	//RestartScript(monst);
	var ev := struct;
	ev.+ type:= SYSEVENT_DAMAGED;
	ev.+ source:=who;
	SendEvent( monst, ev );
	return monst;
endfunction