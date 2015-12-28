use uo;
use os;


// time to wait for house events before checking for banned people inside
const EVENT_TIMEOUT := 20;

// reload list of bans and owners every CACHE_RELOAD times the script cycles (EVENT_TIMEOUT seconds)
const CACHE_RELOAD := 6;

// maximum number of co-owners of the house
const MAX_CO_OWNERS := 5;

// maximum number of friends of the house
const MAX_FRIENDS := 30;

// maximum number of friends of the house
const MAX_BANNED := 20;

// if set to 1 the house can decay over time
const DECAY := 0;

const UOBJECT_BRASS_SIGN	:= 0x0bd2;
const UOBJECT_WOOD_SIGN		:= 0x0bd0;


function addsign(byref sign)

	set_critical(1);
	var signs:=GetGlobalProperty("signs");
	var csize:=signs.size();
	if (signs[1]=="NONE") 
		csize:=0; 
	endif
	signs[csize+1]:=sign.serial;
	SetGlobalProperty("signs",signs);
	SetObjProperty(sign,"#Z",1);
	set_critical(0);
endfunction

function deletesign(signserial)
	if (!signserial)
		return 0;
	endif
	set_critical(1);
	var signs:=GetGlobalProperty("signs");
	var csize:=signs.size();
	if (!signs[1]) 
		csize:=0; 
		return 0;
	endif
	
	//delete
	var i:=1;
	var tempsigns := array;
	
	while (i <= csize)
		if (signs[i]!=signserial)
			tempsigns.append(signs[i]);
		endif
		i:=i+1;
	endwhile
	
	SetGlobalProperty("signs",tempsigns);
	set_critical(0);
	return 1;
endfunction

function deleteHouse(house)
//deletes house items, removes serial from global prop and deletes house
	
	//get house sign
	var item, sign;
	foreach item in (house.components)
		if( (item.objtype == UOBJECT_BRASS_SIGN) || (item.objtype == UOBJECT_WOOD_SIGN) )
			sign := item;
			break;
		endif
	endforeach
	
	
	var signser := sign.serial;

	//delete house objects and items
	var addonitem,founditem;
//
	foreach item in (house.components)
		DestroyItem(item);
	endforeach
//
	foreach item in (house.items)
		DestroyItem(item);
	endforeach
//
//	addonitem := CInt(GetObjProperty( house, "component1" ));
//	if (addonitem) DestroyItem(SystemFindObjectBySerial(addonitem)); endif
//
//	addonitem := CInt(GetObjProperty( house, "component2" ));
//	if (addonitem) DestroyItem(SystemFindObjectBySerial(addonitem)); endif
//
//	addonitem := CInt(GetObjProperty( house, "component3" ));
//	if (addonitem) DestroyItem(SystemFindObjectBySerial(addonitem)); endif
//
	addonitem := CInt(GetObjProperty( house, "builtdeed" ));
	if (addonitem)
		founditem := DestroyItem(SystemFindObjectBySerial(addonitem));
		if (founditem!=1) founditem:=DestroyItem(SystemFindObjectBySerial(addonitem,1)); endif
		if (founditem!=1) DestroyItem(SystemFindObjectBySerial(addonitem,2)); endif
	endif
//
	//if (house.objtype==0x608d)
	//	foreach items in ListItemsNearLocation( sign.x, sign.y, 0, 8 )
	//		if((items.objtype==0x177d) || (items.objtype==0xb41) || (items.objtype==0xb42))
	//			DestroyItem(items);
	//		endif
	//	endforeach
	//endif
	
	//delete house and its global sign prop ref

	var res := DestroyMulti( house );
	if (res)
	    deletesign(signser);
		RemoveFromDecayingHouses(sign.serial);
	endif
endfunction
