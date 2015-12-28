use uo;
use os;


include "include/gumps";
include "include/string";
include "include/yesno";

function ShowPartyStatus(who)
	if (GetObjProperty(who,"partygump"))
		return;
	endif
	SetObjProperty(who,"partygump",1);
	
	var partyleader;
	if (!GetObjProperty (who, "PartyMembers"))
		partyleader :=  GetPartyLeader(who);
	else
		partyleader :=  who;
	endif
	var tempstr := "You are currently not in a party";
	GFInitGump();
	GFPage(0);
	GFResizePic(0, 0, BKGND_GOLDTRIM, 550, 550);
	GFTextMid(100, 10, 300, 1152, "Party Status" );
	
	var partyplayers;
	if (partyleader)
		partyplayers:= GetPartyPlayers(partyleader);
	endif
	var i;
	var y := 110;
	var hButton:=array;

	GFTextLine(40, 80, 1152,  "Party Members:" );
	if (!partyleader||partyleader==error)
		GFTextLine(70, y, 70,  tempstr );
	else
		GFTextLine(70, y, 1152,  partyplayers[1].name + " - Party Leader");
		if (partyleader.serial==who.serial)
			hButton.append(GFButtonID(40, y+3, G_BLUE_SMALL_CIRCLE_1, G_BLUE_SMALL_CIRCLE_0, 1 ));
		endif
		y:=y+30;
		for(i:=2;i<=len(partyplayers);i:=i+1)
			GFTextLine(70, y, 53,  partyplayers[i].name );
			if (partyleader.serial==who.serial)
				hButton.append(GFButtonID(40, y+3, G_BLUE_SMALL_CIRCLE_1, G_BLUE_SMALL_CIRCLE_0, 1 ));
			endif
			y:=y+30;
		endfor
	endif
		
		
	var addButton := GFButtonID( 40, 45, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var quitButton := GFButtonID( 320, 45, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var exitButton := GFButtonID( 40, 515, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	GFTextLine(70, 40, 53, "Add new party member");
	GFTextLine(350, 40, 53, "Quit party");
	GFTextLine(40, 470, 70, "Note: Use .pc to talk to party members");
	GFTextLine(70, 510, 53, "Exit");
	var choice := GFSendGump(who);
	
	//return values	
	var counter:=0;
	
	foreach b in hButton
		counter:=counter+1;
   	if (choice[b.keyid])
			//RemovePartyMember(who, partyleader)
			RemovePartyMember(partyplayers[counter],who);
		endif
	endforeach
	
	if(choice[exitButton.keyid])
		EraseObjProperty(who,"partygump");
		return;
	elseif(choice[addButton.keyid])
		EraseObjProperty(who,"partygump");
		AddPartyPlayer(who);
		return;
	elseif(choice[quitButton.keyid])
		if (YesNo(who, "Quit party?"))
			DeleteFromParty(who);
		endif
	endif
	EraseObjProperty(who,"partygump");
endfunction

function GetPartyLeader(who)
	var partyleaderser:=CInt(GetObjProperty (who, "PartyLeader"));
	if (partyleaderser&&partyleaderser!=error)
		return SystemFindObjectBySerial(partyleaderser); //must be online
	endif
	return 0;
endfunction

function GetPartyPlayers(partyleader)
	var mypartyplayers:=array;
	mypartyplayers.append(partyleader);
	var partymemberSers := GetObjProperty (partyleader, "PartyMembers");
	if (!partymemberSers||partymemberSers==error)
		return mypartyplayers;
	endif
	
	var player;
	foreach ps in partymemberSers
		player:=SystemFindObjectBySerial(ps);
		if (player)
			mypartyplayers.append(player);
		endif
	endforeach
	return mypartyplayers;
endfunction

function AddPartyPlayer(who)
	var partyleader := GetPartyLeader(who);
	
	//check max size of party
	if (GetObjProperty (who, "PartyMembers"))
		if (len(GetObjProperty (who, "PartyMembers"))>9)
			SendSysMessage( who, "You can't have more than 10 players in a party",3,53 );
			return;
		endif
	elseif (partyleader && len(GetObjProperty (partyleader, "PartyMembers"))>9)
		SendSysMessage( who, "You can't have more than 10 players in a party",3,53 );
		return;
	endif
	
	
	SendSysMessage( who, "Select player to add to party.",3,53 );
	var player := Target( who );
	
   if( !player || player==error || player.npctemplate || who.serial==player.serial)
   	SendSysMessage( who, "canceled." );
     	return;
   endif

	var oldparty:=CInt(GetObjProperty (player, "PartyLeader"));
	
	//already being offered
	if (GetObjProperty(player, "#partyoffer"))
		SendSysMessage( who, "That player has already been made a party offer.",3,53 );
     	return;
	endif
	
	//is a party leader check
	if (GetObjProperty (player, "PartyMembers"))
		SendSysMessage( who, "That player is a party leader. They need to resign from that party first before joining another.",3,53 );
		return;
	endif
	
	if (oldparty==partyleader.serial)
		SendSysMessage( partyleader, "That player is already a member of your party.",3,53 );
		return;
	endif
	
	if (oldparty)
		SendSysMessage( who, "That player is already in a party. They need to resign from that party first before joining another.",3,53 );
		return;
	endif

	SendSysMessage( who, "Asking " + player.name +" if they want to join party.",3,53 );
	SendSysMessage( player, "Do you want to join " + who.name + "'s party?",3,53 );
	SetObjProperty(player, "#partyoffer", CInt(ReadGameClock()));
	if (!YesNo(player,"Join party?"))
		SendSysMessage( who, "" + player.name +" did not want to join party.",3,53 );
		SendSysMessage( player, "You refused to join " + who.name + "'s party",3,53 );
		EraseObjProperty(player, "#partyoffer");
		return;
	endif
	
	var partyoffertime := CInt(GetObjProperty(player, "#partyoffer"));
	if (ReadGameClock()>(partyoffertime+20))
		SendSysMessage( player, "You had 20 seconds to respond to the party join request. And so you have declined the offer.",3,53 );
		SendSysMessage( who, "" + player.name +" party join response time elapsed.",3,53 );
		EraseObjProperty(player, "#partyoffer");
		return;
	endif

	EraseObjProperty(player, "#partyoffer");
	
	if (partyleader && partyleader.serial!=who.serial)
		SendSysMessage( who, "Requesting permission from party leader",3,53 );
		SendSysMessage( player,"Requesting permission from party leader",3,53 );
		if (GetObjProperty(partyleader, "addrequest"))
			SendSysMessage( who, "Party leader examining another add request. Try again soon.",3,53 );
			SendSysMessage( player,"Party leader examining another add request. Try again soon.",3,53 );
			return;
		endif

		if (!RequestAddPartyOfPlayerLeader(who, player, partyleader))
			SendSysMessage( who, "Leader permission declined",3,53 );
			SendSysMessage( player,"Leader permission declined",3,53 );
			return;
		endif
		SendSysMessage( who, "Leader permission granted",3,53 );
		SendSysMessage( player,"Leader permission granted",3,53 );
		
		//do these checks again because in time taken to get permission this player may have done stuff
		//is a party leader check
		if (GetObjProperty (player, "PartyMembers"))
			SendSysMessage( who, "That player is a party leader. They need to resign from that party first before joining another.",3,53 );
			return;
		endif
		
		oldparty:=CInt(GetObjProperty (player, "PartyLeader"));
	
		if (oldparty==partyleader.serial)
			SendSysMessage( partyleader, "That player is already a member of your party.",3,53 );
			return;
		endif
	
		if (oldparty)
			SendSysMessage( who, "That player is already in a party. They need to resign from that party first before joining another.",3,53 );
			return;
		endif

		AddToParty(who, player, partyleader);
	else
		AddToParty(who, player, who);
	endif
	
endfunction

function RequestAddPartyOfPlayerLeader(who, player, partyleader)
	SetObjProperty(partyleader, "addrequest", 1);
	GFInitGump();
	GFPage(1);
	//GFResizePic(0, 0, BKGND_SCROLL, 500, 200);
	GFResizePic(0, 0, BKGND_STONESLAB, 500, 200);
	   
	
	GFTextMid(10, 10, 440, 1152, "ADD PLAYER REQUEST");
	GFTextLine(50, 40, 53, "Party leader " + partyleader.name +",");
	GFTextLine(50, 70, 68, "Party member " + who.name + " asks to recruit " );
	GFTextLine(50, 100, 68, player.name + " into the party.");
	GFTextLine(50, 130, 53, "Do you allow " + player.name +" to join the party?");
	
	GFTextLine(90, 160, 1152, "YES");
	GFTextLine(260, 160, 1152, "NO");

	var yesButton := GFButtonID( 60, 165, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var noButton := GFButtonID( 230, 165, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );

	var choice := GFSendGump(partyleader);
	EraseObjProperty(partyleader, "addrequest");
	
	if(choice[yesButton.keyid])
		return 1;
	elseif(choice[noButton.keyid])
		return 0;
	endif
endfunction

function RemovePartyMember(who, partyleader)
	//doable by partyleader only
	if (YesNo(partyleader, "Remove member?"))
		DeleteFromParty(who, partyleader);
	endif	
endfunction

//player addition and deletion functions
function AddToParty(who, newplayer, partyleader:=0)
	if (!partyleader)
		partyleader:=GetPartyLeader(who);
	endif

	var pms := GetObjProperty (partyleader, "PartyMembers");
	if (!pms || pms ==error)
		pms := array;
	endif
	pms.append(newplayer.serial);
	SetObjProperty (partyleader, "PartyMembers", pms);
	SetObjProperty (newplayer, "PartyLeader", CInt(partyleader.serial));
	TellAllPartyMembers(partyleader, ""+newplayer.name + " has joined the party");
endfunction


function DeleteFromParty(delplayer, partyleader:=0)
	
	//if is partyleader
	//returns partyleader serial if true
	if (GetObjProperty (delplayer, "PartyMembers"))
		//is partyleader
		InheritParty (delplayer);
		return;
	endif


	if (!partyleader)
		partyleader:=GetPartyLeader(delplayer);
	endif
	if (!partyleader)
		return;
	endif
	var tempPMs:=array;
	var pms := GetObjProperty (partyleader, "PartyMembers");
	foreach pmser in pms
		if (pmser != delplayer.serial)
			tempPMs.append(pmser);
		endif
	endforeach
	TellAllPartyMembers(partyleader, ""+delplayer.name + " has left the party");
	if (tempPMS.size()<1)
		TellAllPartyMembers(partyleader, "Party disbanded");
		EraseObjProperty (partyleader, "PartyMembers");
	else
		SetObjProperty (partyleader, "PartyMembers", tempPMs);
	endif
	EraseObjProperty (delplayer, "PartyLeader");
endfunction

function DisbandParty (partyleader)
	TellAllPartyMembers(partyleader, "Party disbanded");
	var pms := GetObjProperty (partyleader, "PartyMembers");
	var playermember;
	foreach pmser in pms
		playermember := SystemFindObjectBySerial(pmser);
		if (playermember)
			EraseObjProperty (playermember, "PartyLeader");	
		endif
	endforeach
	EraseObjProperty (partyleader, "PartyMembers");
endfunction

function InheritParty (partyleader)

	var pms := GetObjProperty (partyleader, "PartyMembers");
	if (pms.size()<2)
	//not big enough to have 2 member minimum after current leader removed
		DisbandParty (partyleader);
		return;
	endif
	var playermember;
	var newcommander;
	if (pms[1])
		newcommander := SystemFindObjectBySerial(pms[1]);
		if (!newcommander)
			DisbandParty (partyleader);
		endif
		var temparr := array;
		EraseObjProperty (partyleader, "PartyMembers");
		EraseObjProperty (newcommander, "PartyLeader");
		var i;
		for (i:=2; i<=len(pms); i:=i+1)
			playermember := SystemFindObjectBySerial(pms[i]);
			if (playermember)
				temparr.append(pms[i]);
				SetObjProperty (playermember, "PartyLeader", CInt(newcommander.serial));	
			endif
		endfor
		SetObjProperty (newcommander, "PartyMembers",temparr);
	endif
	TellAllPartyMembers(partyleader, ""+partyleader.name + " has left the party");
	TellAllPartyMembers(newcommander, ""+newcommander.name + " is the new party leader");
endfunction

function TellAllPartyMembers(who, message, colour := 89)
	//is who party leader?
	var pms := GetObjProperty (who, "PartyMembers");
	var partymembers;
	if (!pms||pms==error)
		partymembers:=GetPartyPlayers(GetPartyLeader(who));
	else
		partymembers:=GetPartyPlayers(who);
	endif
	if (!partymembers)
		return 0;
	endif
	
	foreach p in partymembers
		if (p && p!=error)
			SendSysMessage( p, "["+who.name+ " - Party]: " + message,3,colour );
		endif
	endforeach

endfunction

function IsInParty(who)
	//returns partyleader serial if true
	if (GetObjProperty (who, "PartyMembers"))
		return who.serial;
	endif
	var plser:=Cint(GetObjProperty (who, "PartyLeader"));
	if (plser)
		return plser;
	endif
	return 0;
endfunction

function NotInSameParty(attacker, defender)
//for attackhook partyshout check
	if (attacker.serial==defender.serial || attacker.npctemplate)
		return 0;
	endif
	
	if (IsInParty(attacker)!=IsInParty(defender))
		return 1;
	endif
	return 0;

endfunction


function PartyShout(who, message, colour:=89)
	//used by external packages to send important messages to all party members
	//does check to see if in party first
	if (IsInParty(who))
		TellAllPartyMembers(who, message, colour);
	endif

endfunction


