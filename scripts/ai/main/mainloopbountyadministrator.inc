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
							if (!Random(4))
								say ("Say 'bounty' to me to find out about the current bounty!");
     						endif				
						endif          
					SYSEVENT_SPEECH:
		    			if (!ev.source.isA(POLCLASS_NPC))
		    				if (lower(ev.text) == "set bounty" && ev.source.cmdlevel >=2)
								BountyPointInfoGump(ev.source);
		    				endif
		    				if (lower(ev.text) == "bounty")
								TellBounty(ev.source);	
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

function makeBountyPointStruct(npctemplate, x, y, z, pos_description)
	var bountypoint:=struct;
	bountypoint.+npctemplate:=npctemplate;
	bountypoint.+x:=x;
	bountypoint.+y:=y;
	bountypoint.+z:=z;
	bountypoint.+pos_description:=pos_description;
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
		SendSysMessage(who, "Choose a quest stone to associate with bounty administrator first!");
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
		SendSysMessage(who, "Only 20 bountypoints per NPC are permitted");
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
	GFTextMid(100, 10, 200, 10, "Bounty spawn locations for bounty administrator " + me.name);
	
	
	var bountypoints := GetAllBountyPoints();
	
	var i;
	var y := 40;
	
	var hButton:=array;
	var tempstr;
	if (bountypoints)
		for(i:=1;i<=len(bountypoints);i:=i+1)	
			var tempstr:= bountypoints[i].npctemplate + " " + bountypoints[i].x + ", " +	bountypoints[i].y + ", " + bountypoints[i].z +". Last seen: " + bountypoints[i].pos_description;
			GFTextLine(50, y, 5, i +": " + tempstr);
			hButton.append(GFButtonID(40, y+3, G_BLUE_SMALL_CIRCLE_1, G_BLUE_SMALL_CIRCLE_0, 1 ));
			y:=y+20;
		endfor
	endif
	//add new message button
	GFTextLine(50, 470, 53, "ADD NEW BOUNTY SPAWN POINT" );
	var addnewbutton := GFButtonID(40, 475, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );

	//associate quest
	GFTextLine(50, 490, 53, "ASSOCIATE QUEST STONE [DO THIS FIRST!!!]" );
	var assocqueststone := GFButtonID(40, 495, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );

	//
	GFTextLine(50, 510, 53, "GO TO CURRENT BOUNTY MONSTER" );
	var gotomonster:= GFButtonID(40, 515, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );

	
	var choice := GFSendGump(who);
	
	//return values
	if (choice[addnewbutton.keyid])
		AddBountyPoint(who);
	elseif(choice[assocqueststone.keyid])
		associateQuestStone(who);
	elseif(choice[gotomonster.keyid])
		GoToCurrentBounty(who);
	endif
		
	var counter:=0;
	
	foreach b in hButton
		counter:=counter+1;
   	if (choice[b.keyid])
			if (YesNo(who, "Delete this bounty spawn point?"))
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
	SendSysMessage(who, "Bounty spawn point deleted!");
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
	SendSysMessage(who, "Players told 'They were last seen ' ... plus what you enter now");
	var timetemp := SendTextEntryGump( who, "Where were they last seen:");
	if (! timetemp)
		return;
	endif
	var bpstruct:=makeBountyPointStruct(mtemp, bp.x, bp.y, bp.z, timetemp);
	return bpstruct;
endfunction

function TellBounty(who)
	if (!QuestCheck(who))
		return;
	endif
	var monster := GetCurrentBountyMonster();
	if (!monster)
		monster := SetNewBountyMonster();
	endif
	if (!monster)
		say("Dang! Aint nothing at the moment");
		return;
	endif
	var index := GetObjProperty(monster, "bountyindex");
	var bp := GetBountyPointByIndex(index);
	say("For crimes against humanity " + monster.name +" is to be put to death!");
	say("They were last seen " + bp.pos_description);
	say("You will be rewarded with " + (DetermineNPCTemplateQPs(monster.npctemplate)) + " quest points when you have brought them to justice.");
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
	SendSysMessage( who, "Bounter administrator associated with queststone." );
endfunction

function GoToCurrentBounty(who)
	var monster := GetCurrentBountyMonster();
	if (!monster)
		SendSysMessage( who, "MONSTER NOT FOUND." );
		return 0;	
	endif
	MoveObjectToLocation(who, monster.x, monster.y, monster.z, _DEFAULT_REALM, MOVEOBJECT_FORCELOCATION); //AutoConversion
endfunction

function GetCurrentBountyMonster()
	var mserial := CInt(GetObjProperty (me, "currentbountyserial"));
	if (!mserial || mserial == error)
		return 0;
	endif
	var monster := SystemFindObjectBySerial(mserial);
	if (!monster || monster == error)
		return 0;
	endif
	return monster;
endfunction

function SetNewBountyMonster()
	var stone_serial := GetObjProperty (me, "AssocQuestStoneSer");
	var stone;
	if (stone_serial)
		stone:= SystemFindObjectBySerial(stone_serial);
		if (!stone)
			return 0;
		endif
	endif
	//get random bounty point
	if (!len(GetAllBountyPoints()))
		return 0;
	endif
	var rand_num:= RandomDiceRoll( "1d"+CStr(len(GetAllBountyPoints()) ));
	var all_bps:=GetAllBountyPoints();
	var rand_bp:=all_bps[rand_num];
	
	//spawn monster of that type at location
	var monst:=CreateNpcFromTemplate( CStr(rand_bp.npctemplate),CInt(rand_bp.x),CInt(rand_bp.y),CInt(rand_bp.z));
	if (!monst)
		monst:=CreateNpcFromTemplate( CStr(rand_bp.npctemplate),CInt(me.x),CInt(me.y),CInt(me.z));
		MoveObjectToLocation(monst,rand_bp.x,rand_bp.y,rand_bp.z, _DEFAULT_REALM,MOVEOBJECT_FORCELOCATION); //AutoConversion
	endif
	if (!monst)
		return 0;
	endif
	var currentbountyser:=CInt(GetObjProperty (me, "currentbountyserial"));
	if (currentbountyser)
		RemoveObjective(stone_serial, currentbountyser);
	endif
	var monstanchor:=struct;
	monstanchor.+x:=CInt(rand_bp.x);
	monstanchor.+y:=CInt(rand_bp.y);
	monstanchor.+range:=1;
	monstanchor.+psub:=50;
	SetObjProperty(monst, "Anchor", monstanchor);
	SetObjProperty(monst, "guardignore", 1);
	SetObjProperty(monst, "RemoveFromQuestAfterDeath", 1);	
	AddMonsterObjective(stone_serial, monst, DetermineNPCTemplateQPs(rand_bp.npctemplate));
	SetObjProperty (monst, "bountyindex", Cint(rand_num));
	SetObjProperty (me, "currentbountyserial", Cint(monst.serial));
	monst.title_guild := "Wanted";
	sleep(1);
	SetOneOffCompletedMessage(stone, monst.serial, "The villain " + monst.name + " has been slain. Speak to " + me.name +" for more bounties!");
	return monst;
endfunction

function QuestCheck(who)
	var quest := GetObjProperty (me, "AssocQuestStoneSer");
	if (quest)
		var stone:= SystemFindObjectBySerial(quest);
		if (!stone)
			return 0;
		endif
		if (!IsRegistered(stone, who.serial))
			say("Bounties are only on offer to those registered for the quest.");
			return 0;			
		endif
		if (!IsQuestDeployed(stone))
			say("Bounties are only on offer while the quest is deployed.");
			return 0;			
		endif	
	endif
	return 1;
endfunction