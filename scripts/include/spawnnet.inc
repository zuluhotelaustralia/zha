//ver 0.9.5b

use uo;
use os;
//include "include/attributes";

const CLEAR_RANGE :=100;

function spawnnet_clear(obj)
if(GetObjProperty(obj,"spawnnet"))
	AddDeath(CInt(GetObjProperty(obj,"spawnnet")));
	EraseObjProperty(obj,"spawnnet");
	return 1;
endif

return 0;
endfunction

//adds the rune object the array should only be executed after a prop check is done
function AddCreated(rune)
	set_critical(1);
	var pool:=GetGlobalProperty("createpool");
	if(!pool)
		pool := {};
	endif
	if(rune in pool)
		return 0;
	endif
	pool.append(rune);

	SetGlobalProperty("createpool", pool );
	set_critical(0);
	return 1;

endfunction


//rune is the serial of the rune in question.
function AddRefresh(rune)
	set_critical(1);
	var pool:=GetGlobalProperty("refreshpool");
	if(rune in pool)
		return 0;
	endif
	if(!pool)
		pool :={};
	endif
	pool.append(rune);
	SetGlobalProperty("refreshpool", pool);
	set_critical(0);
	return 1;
endfunction


//rune is the "spawnnet" cprop of the rune to the pool
function AddDeleted(rune)
	set_critical(1);
	var pool:=GetGlobalProperty("deletedpool");
	if(!pool)
		pool :={};
	endif
	pool.append(rune);
	SetGlobalProperty("deletedpool", pool);

endfunction


function AddDeath(rune)
	//syslog("creature died, rune: "+rune);
	rune := CInt(rune);
	set_critical(1);
	var pool:=GetGlobalProperty("deathpool");
	if(!pool)
		pool := {};
	endif
	pool.append(rune);
	SetGlobalProperty("deathpool", pool);
	set_critical(0);
endfunction



function ResetSpawn(item, flag)

var dist:=CInt(GetObjProperty(item, "range")) + CLEAR_RANGE;
var spawnnet:=CInt(GetObjProperty( item, "spawnnet"));
if(!spawnnet)
	return 0;
endif

if (flag == 1)
	AddDeleted(spawnnet);
endif

var props :=CStr(Hex(CInt(GetObjProperty(item, "flags"))));
var type := props[3,1];
type := CInt(type);
if(!type)
	return 0;
endif
var flags := props[4,1];
flags := "0x"+flags;
flags := CInt(flags);
flags := GetFlags(flags);

var aux;

case(type)
	1: foreach npc in ListMobilesNearLocation( item.x, item.y, item.z, dist );
		aux:=CInt( GetObjProperty( npc, "spawnnet" ));
		if (aux == spawnnet)
			if (flag == 1)
				EraseObjProperty( npc, "spawnnet");
			endif
			RevokePrivilege( npc, "invul" );
			SetObjProperty(npc, "guardkill", 1);
			Damage( npc, (GetMaxHp(npc)) );
		endif
	   endforeach

	2: var list;
	   if(flags[2])
	   	aux := item.container;
	   	while(aux.container.serial)
	   		aux := aux.container;
	   	endwhile
	   	if(!aux)
	   		syslog("Item spawn with inside container flag is outside of any");
	   		return 0;
	   	endif

		list := EnumerateItemsInContainer(aux);
		var tmp := CInt(GetObjProperty(aux, "spawnnet"));
		if(tmp == spawnnet)
			EraseObjProperty(aux, "spawnnet");
		endif
 	   else
		list := ListObjectsInBox( (item.x-dist), (item.y-dist), -127, (item.x+dist), (item.y+dist), 127 );
	   endif
	   foreach it in list
		if(it.amount)
			aux:=CInt( GetObjProperty( it, "spawnnet" ));
			if(aux == spawnnet && it.objtype != 0xa300)
				 DestroyItem(it);
			endif
		endif
	   endforeach
	3: aux := CInt(GetObjProperty(item, "aux"));
	   if(aux)
		var cnt := SystemFindObjectBySerial(CInt(aux));
		if(cnt)
			DestroyItem(cnt);
		endif
	   endif
endcase

endfunction



function GetFlags(x)
x:=CInt(x);

var result := {0,0,0,0};

if(x>7)
	x:=x-8;
	result[4]:=1;
endif

if(x>3)
	x:=x-4;
	result[3]:=1;
endif
if(x>1)
	x:=x-2;
	result[2]:=1;
endif

if(x>0)
	x:=x-1;
	result[1]:=1;
endif

return result;

endfunction


function is_item_spawn(rune)

var props :=CStr(Hex(runes.flags[rune]));
var type := props[3,1];
type := CInt(type);
if(type >1)
	return 1;
endif
return 0;
endfunction


function exp2( power) //optimized version from sigmund one's
var retval := 1,i;
for (i:=1; i<=power; i:=i+1)
	retval := retval *2;
endfor
return retval;
endfunction