include "include/gumps";
//this guy simulates decay in multis...
//can also move instead of delete items
//if u use MoveItemsToLocation CProp string x y z

function main_AI_loop()
	cleanItemsSetup();
 	RunCleaningMode();  
	var ev;
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
					if (! ev.npctemplate)
						me.hidden:=1;
						ReturnHome();
						RunCleaningMode();
	   			endif     
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

function RunCleaningMode()
	DecayOldItems();
	AddNewItems();
endfunction

function makeItemStruct(item)
	var itemstr:=struct;
	itemstr.+serial:=CInt(item.serial);
	itemstr.+decay:=CInt(ReadGameClock()+1800);//decay delay (30 mins)
	itemstr.+x:=item.x;
	itemstr.+y:=item.y;
	return itemstr;
endfunction

function DecayOldItems()
	//if item is to decay and is still there
	//and hasnt moved
	//it decays
	var tempItem;
	foreach it in GetItemsThatDecay()
		tempItem:=SystemFindObjectBySerial(it.serial);
		if (tempItem)
			if (tempItem.x == it.x && tempItem.y == it.y)
				DecayThisItem(tempItem);
			endif
			DeleteItem(it);
		endif
	endforeach
	
endfunction

function AddNewItems()
	//look around... if it is in range and is in my multi then add to list
	//items must not be corpses, must be movable, and not newbie
	foreach item in (me.multi.items)
		if (!item.newbie && item.movable && item.graphic!=8198)
			if (!ItemInList(item))
				AddNewItem(item);
			endif
		endif
	endforeach

endfunction


function GetItemsThatDecay()
	var items := GetAllItems();
	var decayitems := array;
	foreach it in items
		if (ReadGameClock()>CInt(it.decay))
			decayitems.append(it);
		endif
	endforeach
	return decayitems;
endfunction

function GetAllItems()
	return GetObjProperty(me, "cleaningItems");
endfunction

function ItemInList(item)
	foreach q in GetAllItems()
		if (CInt(q.serial)==item.serial)
			return 1;
		endif
	endforeach
	return 0;
endfunction

function ReturnHome()
	var myhome := GetObjProperty(me,"myhome");
	MoveCharacterToLocation(me,myhome[1],myhome[2],myhome[3],MOVECHAR_FORCELOCATION);
endfunction

function DeleteItem(item)
	var tempItems:=array;
	foreach q in GetAllItems()
		if (CInt(q.serial)!=item.serial)
			tempItems.append(q);
		endif
	endforeach
	SetObjProperty(me, "cleaningItems", tempItems);
endfunction

function AddNewItem(item)
	var items := GetAllItems();
	var tempitem:=makeItemStruct(item);
	items.append(tempitem);
	SetObjProperty(me, "cleaningItems", items);
endfunction

function cleanItemsSetup()
	var cleanItems := GetAllItems();
	if (!cleanItems)
		SetObjProperty(me, "cleaningItems", {});
	endif
endfunction

function DecayThisItem(item)
	var posstr := GetObjProperty(me, "MoveItemsToLocation");
	if (posstr)
		var moveLocation := SplitWords(posstr);
		var x := CInt(moveLocation[1]);
		var y := CInt(moveLocation[2]);
		var z := CInt(moveLocation[3]);
		if (! MoveItemToLocation( item, x, y, z))	
			DestroyItem(item);
		endif	
	else
		DestroyItem(item);
	endif

endfunction