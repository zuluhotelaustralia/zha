include "include/gumps";
//this ai will respond to a set of keywords with responses....
//these are stored in a 2d array...
//each word index must be unique

function main_AI_loop()
	messageSetup();
    var ev;
    //var wanders := 60;
    var next_welcome := 0;
    //var next_news := ReadGameClock();
    EnableMainEvents();

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
							if (ReadGameClock()>next_welcome)
								SayWelcome();
								next_welcome:=Cint(ReadGameClock()+10);
     						endif				
						endif          
					SYSEVENT_SPEECH:
		    			if (!ev.source.isA(POLCLASS_NPC))
		    				if (lower(ev.text) == "set message" && ev.source.cmdlevel >=2)
								MessagesInfoGump(ev.source);
		    				endif
							SayResponse( lower( ev.text ) );
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


function SetMessage(who, messagekey)
	GFInitGump();
	GFPage(1);
	GFResizePic(0, 0, BKGND_SCROLL, 350, 350);
		
	GFTextMid(30, 30, 230, 10, "Enter Message For: " + messagekey);

	var textbox:=array;
	textbox.append(GFTextEntry(30, 50, 300, 20, 0, ""));
	textbox.append(GFTextEntry(30, 70, 300, 20, 0, "" ));
	textbox.append(GFTextEntry(30, 90, 300, 20, 0, "" ));
	textbox.append(GFTextEntry(30, 110, 300, 20, 0, "" ));
	textbox.append(GFTextEntry(30, 130, 300, 20, 0, "" ));
	textbox.append(GFTextEntry(30, 150, 300, 20, 0, "" ));
	textbox.append(GFTextEntry(30, 170, 300, 20, 0, "" ));
	textbox.append(GFTextEntry(30, 190, 300, 20, 0, "" ));
	textbox.append(GFTextEntry(30, 210, 300, 20, 0, "" ));
	textbox.append(GFTextEntry(30, 230, 300, 20, 0, "" ));
	textbox.append(GFTextEntry(30, 250, 300, 20, 0, "" ));
	textbox.append(GFTextEntry(30, 270, 300, 20, 0, "" ));
	textbox.append(GFTextEntry(30, 290, 300, 20, 0, "" ));

	
	GFTextLine(50, 300, 5, "OK");
	GFTextLine(110, 300, 5, "Cancel");

	var OKbutton  := GFButtonID( 40, 305, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var cancelbutton  := GFButtonID( 100, 305, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	
	var choice := GFSendGump(who);
	var mystring := array;
	var tempstr;
	if (choice[OKbutton.keyid])
		foreach m in textbox
			var tempstr:=GFExtractData(m.keyid, choice);
			if (tempstr)
				mystring.append(tempstr);
			endif
		endforeach
		return mystring;
	endif
	return 0;	
endfunction

function GetMessageByKey(messageKey)
//key is the keyword require to trigger the response
	//Broadcast(messageKey);
	foreach q in GetAllMessages()
		//Broadcast(q);
		if (q.keyword==messageKey)
			//Broadcast("*" + q.message);
			return q.message;
		endif
	endforeach
endfunction

function GetMonsterByKey(messageKey)
//key is the keyword require to trigger the monster
	//Broadcast(messageKey);
	foreach q in GetAllMessages()
		//Broadcast(q);
		if (q.keyword==messageKey)
			//Broadcast("*" + q.message);
			return q.monster;
		endif
	endforeach
endfunction


function SetMessageForKey(messagekey, mystring)
	var messages:=array;
	messages:=GetAllMessages();
	var alreadyIndex:=GetIndexForMessageKeyword(messagekey);
	//Broadcast("already index: " +alreadyIndex);
	if (alreadyIndex)
		messages[alreadyIndex].message:=mystring;
	else
		//Broadcast("cant find alreddy");
		var tempmess:=makeMessageStruct(messagekey, mystring);
		messages.append(tempmess);
	endif
	SetObjProperty(me, "messages", messages);
endfunction

function SetMonsterForKey(npctemplate, messagekey)
	var messages:=array;
	messages:=GetAllMessages();
	var alreadyIndex:=GetIndexForMessageKeyword(messagekey);
	//Broadcast("already index: " +alreadyIndex);
	if (!alreadyIndex)
		return;
	endif
	
	messages[alreadyIndex].monster:=npctemplate;
	SetObjProperty(me, "messages", messages);
endfunction


function makeMessageStruct(keyword, message)
	//Broadcast("making struct: " + keyword + "  " + message);
	var messages:=struct;
	messages.+keyword:=keyword;
	messages.+message:=message;
	messages.+monster:="";
	return messages;
endfunction

function SetMessageKeywordByIndex(index, mkeyword)
	var messages:=array;
	messages:=GetObjProperty(me, "messages");
	messages[index].keyword:=mkeyword;
	SetObjProperty(me, "messages", messages);
endfunction

function GetIndexForMessageKeyword(mkeyword)
	var messages:=array;
	var alreadyIndex:=0;
	messages:=GetObjProperty(me, "messages");
	var i:=0;
	for (i:=1; i<=len(messages); i:=i+1)
		if (messages[i].keyword==mkeyword)
			alreadyIndex := i;
		endif
	endfor
	return alreadyIndex;
endfunction



function GetAllMessages()
	return GetObjProperty(me, "messages");
endfunction


function ReturnHome()

	var myhome := GetObjProperty(me,"myhome");
	MoveObjectToLocation(me,myhome[1],myhome[2],myhome[3], _DEFAULT_REALM,MOVEOBJECT_FORCELOCATION); //AutoConversion

endfunction

function MessagesInfoGump(who)
	GFInitGump();
	GFPage(0);
	GFResizePic(0, 0, BKGND_SCROLL, 400, 530);
	GFTextMid(100, 10, 200, 10, "Messages on townperson: " + me.name);
	
	
	var messages := GetAllMessages();
	var i;
	var y := 40;
	
	var hButton:=array;
	var tempstr;
	for(i:=1;i<=len(messages);i:=i+1)	
		GFTextLine(50, y, 5, messages[i].keyword);
		hButton.append(GFButtonID(40, y+3, G_BLUE_SMALL_CIRCLE_1, G_BLUE_SMALL_CIRCLE_0, 1 ));
		y:=y+20;
	endfor
	
	//add new message button
	GFTextLine(50, 470, 53, "ADD NEW MESSAGE" );
	var addnewbutton := GFButtonID(40, 475, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var mycol := GetObjProperty(me, "fontcolour");
	if (!mycol)
		mycol:=0;
	endif
	GFTextLine(50, 500, 53, "SET TEXT COLOUR - currently " + mycol);
	var setcolorbutton := GFButtonID(40, 505, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	
	var choice := GFSendGump(who);
	
	//return values
	if (choice[addnewbutton.keyid])
		AddNewMessage(who);
	elseif(choice[setcolorbutton.keyid])
		SetNewFontColor(who);
	endif
		
	var counter:=0;
	
	foreach b in hButton
		counter:=counter+1;
   	if (choice[b.keyid])
			MessageInfoGump(who, messages[counter]);
		endif
	endforeach
	return 0;
endfunction


function MessageInfoGump(who, messagearr)
	GFInitGump();
	GFPage(1);
	GFResizePic(0, 0, BKGND_PAPER_EX, 230, 400);
		
	GFTextMid(10, 20, 230, 53, "Message: " + messagearr.keyword);
	GFTextLine(20, 40, 5, "Display Response");
	GFTextLine(20, 70, 5, "Delete Message");
	GFTextLine(20, 100, 5, "Edit Message Keyword");
	GFTextLine(20, 130, 5, "Edit Message Response");
	GFTextLine(20, 160, 5, "Become monster after saying this");
		
	GFTextLine(30, 190, 5, "Exit Message");

	var messagescroll  := GFButtonID( 10,  45, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var deleteAmessage  := GFButtonID( 10,  75, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var editmessagekeyword  := GFButtonID( 10,  105, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var editmessageresponse  := GFButtonID( 10,  135, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var monsterbtn  := GFButtonID( 10,  165, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );
	var exitMessageGumpbutton  := GFButtonID( 10,  195, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );

	var choice := GFSendGump(who);

	if (choice[messagescroll.keyid])
		ShowLongMessage(who, messagearr.message) ;
	elseif (choice[deleteAmessage.keyid])
		DeleteMessage(who, messagearr.keyword);
	elseif (choice[editmessagekeyword.keyid])
		EditKeyword(who, messagearr);
	elseif (choice[editmessageresponse.keyid])
		EditMessage(who, messagearr.keyword);
	elseif (choice[monsterbtn.keyid])
		SetMonster(who, messagearr.keyword);
	elseif (choice[exitMessageGumpbutton.keyid])
		return;
	endif
	
endfunction

function DeleteMessage(who, mkeyword)
	var tempMessages:=array;
	foreach q in GetObjProperty(me, "messages")
		if (q.keyword!=mkeyword)
			tempMessages.append(q);
		endif
	endforeach
	SetObjProperty(me, "messages", tempMessages);
	SendSysMessage(who, "Message deleted!");
endfunction

function ShowLongMessage(who, messagestr)
	var i;
	for (i := 1; i <= len(messagestr); i := i + 1)
		SendSysMessage(who, messagestr[i]);
	endfor
endfunction

function EditKeyword(who, messagearr)
	var mindex := GetIndexForMessageKeyword(messagearr.keyword);
	if (!mindex)
		return;
	endif
	var keyw:=lower(SendTextEntryGump( who, "Enter Keyword."));
	if (keyw)
		messagearr.keyword:=keyw;
		SetMessageKeywordByIndex(mindex, keyw);
		SendSysMessage(who, "Keyword changed!");
	endif
endfunction


function AddNewMessage(who)
	var messags := GetAllMessages();
	if (len(messags)>20)
		SendSysMessage(who, "Only 20 responses per person are permitted");
		return;
	endif
	var keyw:=lower(SendTextEntryGump( who, "Enter Keyword."));
	if (!keyw)
		return;
	endif
	var response := SetMessage(who, keyw);
	if (!response)
		return;
	endif
	var tempmess:=makeMessageStruct(keyw, response);
	messags.append(tempmess);
	SetObjProperty(me, "messages", messags);
	SendSysMessage(who, "New message added");
endfunction

function messageSetup()
	var messages := GetAllMessages();
	if (!messages)
		
		var tempmess:=makeMessageStruct("welcome", {"Welcome"});
		messages:={tempmess};
		SetObjProperty(me, "messages", messages);
	endif
endfunction

function SayWelcome()
	SayResponse("welcome");
endfunction

function SayResponse(queryFromPlayer)
	//Broadcast("passed keyword: " + possiblekeyword);
	var querywords := SplitWords(queryFromPlayer);
	var myresponse;
	foreach qword in querywords
		myresponse := GetMessageByKey(qword);
		if (myresponse)
			SayMessage(myresponse);
			CheckBecomeMonster(qword);
			break;
		endif
	endforeach
endfunction

function CheckBecomeMonster(possiblekeyword)
	//Broadcast("passed keyword: " + possiblekeyword);
	var mymonster := GetMonsterByKey(possiblekeyword);
	if (mymonster)
		BecomeMonster(mymonster);
	endif

endfunction


function SayMessage(messagestr)
	var col := GetObjProperty(me, "fontcolour");
	if (!col)
		col:=0;
	enDIF
	sleep(1);
	
	if (!messagestr)
		return;
	endif

	var i;
	for (i := 1; i <= len(messagestr); i := i + 1)
	  //say(messagestr[i]);
	 	PrintTextAbove(me, messagestr[i], 3, col);
		sleep(2);
	endfor

endfunction

function EditMessage(who, mkeyword)
	var myresponse:=SetMessage(who, mkeyword);
	if (myresponse)
		SetMessageForKey(mkeyword, myresponse);
		SendSysMessage(who, "Message edited");
	endif
endfunction

function SetMonster(who, keyword)
	var mt:=lower(SendTextEntryGump( who, "Enter NPC Template. <dbl quotes for none>"));
	SetMonsterForKey(mt, keyword);
endfunction


function SetNewFontColor(who)
	var val:=SendTextEntryGump( who, "Enter Font colour (Int)"); 
	if (!val)
		return;
	endif
	SetObjProperty(me, "fontcolour", CInt(val));
endfunction

function BecomeMonster(npctemplate)
	var thecritter := CreateNpcFromTemplate( npctemplate, CInt(me.x), CInt(me.y), CInt(me.z) );
	if (thecritter)
		PlayStationaryEffect( me.x, me.y, me.z, FX_SMOKE, 0xa, 0xa );
		me.hidden := 1;
		//MoveObjectToLocation( me, 0, 0, 0, _DEFAULT_REALM, MOVEOBJECT_FORCELOCATION ); //AutoConversion
		SetObjProperty( me, "noloot", 1 );
		SetObjProperty( me, "summoned", 1 );
		RevokePrivilege( me, "invul" );
		ApplyRawDamage(me, GetHp(me) + 3);
	endif	
endfunction