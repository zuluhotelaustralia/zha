//Barnabus 16/2/2003
//Sets up a global array 'holds' to store boats at creation (addHold)
//and a function for removing from 'holds' when boat destroyed or drydocked
//uses #Z (to stay with the houses convention) to remember when a boat has been properly processed

use uo;
use os;

include "include/constants/locations";

const DELBOAT_DELITEMS :=0;
const DELBOAT_MOVEITEMS :=1;

function addHold(byref hold)
	set_critical(1);
	var holds:=GetGlobalProperty("holds");
	var hsize:=holds.size();
	if (holds[1]=="NONE"||!holds[1]) 
		hsize:=0; 
	endif
	holds[hsize+1]:=hold.serial;
	SetGlobalProperty("holds",holds);
	SetObjProperty(hold,"#Z",1);
	set_critical(0);
endfunction

function deleteHold(holdserial)
	set_critical(1);
	var holds:=GetGlobalProperty("holds");
	var hsize:=holds.size();
	if (!holds[1]) 
		hsize:=0; 
		return;
	endif
	
	//delete
	var i:=1;
	var tempHolds := array;
	
	while (i <= hsize)
		if (holds[i]!=holdserial)
			tempHolds.append(holds[i]);
		endif
		i:=i+1;
	endwhile
	
	SetGlobalProperty("holds",tempHolds);
	set_critical(0);
endfunction

function deleteboat(boat, x:=DEFAULT_LOCATION_CHARACTER_RELOCATION_X ,y:=DEFAULT_LOCATION_CHARACTER_RELOCATION_Y, z:=DEFAULT_LOCATION_CHARACTER_RELOCATION_Z, moveOrDeleteItemFlag:=DELBOAT_DELITEMS)
	//deletes a boat - option to move onboard items or delete them
	//Broadcast("new loc: " + newloc); 
	if (boat.has_offline_mobiles)
		//Broadcast("offline mobile moved to " + x + ", " + y + ", " + z); 
        	boat.move_offline_mobiles( x,y,z);
	endif
	 
	var inhold := boat.hold;
	foreach item in EnumerateItemsInContainer( inhold );
		if (moveOrDeleteItemFlag == DELBOAT_MOVEITEMS)
			MoveObjectToLocation(item, x,y, z, _DEFAULT_REALM); //AutoConversion
		else
			//delete all items
		endif
	endforeach

	foreach item in (boat.items) //move onboard items
		if (moveOrDeleteItemFlag == DELBOAT_MOVEITEMS)
			MoveObjectToLocation(item, x,y, z, _DEFAULT_REALM); //AutoConversion
		else
			//delete all items
		endif
	endforeach
	
	var holdserial := boat.hold.serial;
	var res := DestroyMulti( boat );
  	if (res)
  		deleteHold( holdserial ); //removes from holds global prop array
  	else
		return 0;
  	endif
	return 1;
endfunction