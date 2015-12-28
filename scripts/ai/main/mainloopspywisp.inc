function main_AI_loop()
	var ev;
	StillAlive();
   EnableMainEvents();
	me.hidden:=1;
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
					me.hidden:=1;
					ReturnHome();
					ReportToMaster(ev.source);
	    
	endcase

        until (! (ev := os::wait_for_event(2)) );
		endif

   endwhile        

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

function ReturnHome()
	var myhome := GetObjProperty(me,"myhome");
	MoveCharacterToLocation(me,myhome[1],myhome[2],myhome[3],MOVECHAR_FORCELOCATION);
endfunction

function ReportToMaster(mobile_entered)
	StillAlive();
	SendSysMessage(me.master, me.name +": " + mobile_entered.name + " is near me!",3,73);
endfunction

function StillAlive()
	var timeLeft := GetObjProperty(me, "dispelTime");
	if (!timeLeft || (timeLeft + 1) < ReadGameClock())
		var backpack := FindExistingPack( me.serial );
		if( backpack )
			foreach item in EnumerateItemsInContainer( backpack );
				MoveItemToLocation( item, me.x, me.y, me.z, MOVEITEM_FORCELOCATION );
			endforeach
		endif
		MoveCharacterToLocation( me, 0, 0, 0, MOVECHAR_FORCELOCATION );
		ApplyRawDamage(me, GetHp(me) + 3);
	endif
endfunction