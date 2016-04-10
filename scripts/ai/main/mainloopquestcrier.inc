include "include/gumps";


function main_AI_loop()

    var ev;
    var wanders := 60;
    var next_wander := ReadGameClock();
    var next_news := ReadGameClock();
    EnableMainEvents();

	var mydestx := 0;
	var mydesty := 0;
	var steps := 0;

	SetAnchor( CInt(me.x), CInt(me.y), 4, 50 );

    while (1)

	if (ReadGameClock() > next_wander)
		if (readgameclock() > next_news)
			SpoutTheQuestMessage();
			next_news := ReadGameClock()+600;
		endif

		if (mydestx != 0)
			WalkTowardLocation(mydestx, mydesty);
			if ( (me.x == mydestx && me.y == mydesty) || ( steps > 10 ) )
				mydestx := 0;
			endif
		else
			wander();
			wanders := wanders +1;
		endif

		next_wander := ReadGameClock()+5;
		if (wanders > 60)
			wanders :=0;
			walkhome();
			ev := sleepmode();
    		endif
	else
		ev := os::wait_for_event( 2 );
	endif

	if (ev)
        repeat
            case (ev.type)
                SYSEVENT_SPEECH:
		    if (!ev.source.isA(POLCLASS_NPC))
		    	if (lower(ev.text) == "news")
				wanders :=0;
		    		next_wander := ReadGameClock()+60;
					next_news := ReadGameClock()+600;
		    		SpoutTheQuestMessage();
		    	endif
		    endif
		    if (!ev.source.isA(POLCLASS_NPC))
		    	if (lower(ev.text) == "set message" && ev.source.cmdlevel >=2)
				SetTheQuestMessage(ev.source);
		    	endif
		    endif
	
                SYSEVENT_ENGAGED:
                SYSEVENT_DAMAGED:
                    Fight( ev.source );
		    ReturnHome();
		EVID_ALL_FOLLOW_CMD:
			mydestx := CInt(ev.x);
			mydesty := CInt(ev.y);
			steps := 0;
			SetAnchor( mydestx, mydesty, 4, 50 );
			WalkTowardLocation(mydestx, mydesty);
		SYSEVENT_ENTEREDAREA:
		saynews:=1;
		SpoutTheQuestMessage();
		next_news := ReadGameClock()+600;
		saynews:=0;
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

function SpoutTheQuestMessage();

	var news := GetObjProperty(me, "message");
	if (!news)
		return;
	endif

	var i;
	for (i := 1; i <= len(news); i := i + 1)
	  say(news[i]);
	  sleep(2);
	endfor

endfunction

function SetTheQuestMessage(who)
	GFInitGump();
	GFPage(1);
	GFResizePic(0, 0, BKGND_SCROLL, 350, 350);
		
	GFTextMid(30, 30, 230, 10, "Enter Message For Quest Crier:");

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
		SetObjProperty(me, "message", mystring);
	endif
	return 0;	
endfunction


function ReturnHome()

	var myhome := GetObjProperty(me,"myhome");
	MoveObjectToLocation(me,myhome[1],myhome[2],myhome[3], _DEFAULT_REALM,MOVEOBJECT_FORCELOCATION); //AutoConversion

endfunction