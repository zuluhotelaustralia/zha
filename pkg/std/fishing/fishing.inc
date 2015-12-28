//include "include/attributes";
function povernut(ch, us)
    var xd, yd;

    xd := ch.x - us.x;
    yd := ch.y - us.y;
	var signt;


if ((xd==0) || (yd==0))
	if (xd==0)
		if(yd>=0)
			ch.facing:=0;
		else
			ch.facing:=4;
		endif
	else
		if(xd>=0)
			ch.facing:=6;
		else
			ch.facing:=2;
		endif
	endif
else
    if (xd<0)
	if (yd<0)
		signt := 1;
	else
		signt := 2;
	endif
    else
	if (yd<0)
		signt := 3;
	else
		signt := 4;
	endif
    endif
	case (signt)
        1: if ( Tan(-xd, -yd)>=0.4142)
		if (Tan(-xd, -yd)<=2.4142)
			ch.facing:=3;
		else
			ch.facing:=2;
		endif
	   else
			ch.facing:=4;
	   endif
        2: if ( Tan(-xd, yd)>=0.4142)
		if (Tan(-xd, yd)<=2.4142)
			ch.facing:=1;
		else
			ch.facing:=2;
		endif
	   else
			ch.facing:=0;
	   endif
        3: if ( Tan(xd, -yd)>=0.4142)
		if (Tan(xd, -yd)<=2.4142)
			ch.facing:=5;
		else
			ch.facing:=6;
		endif
	   else
			ch.facing:=4;
	   endif
        4: if ( Tan(xd, yd)>=0.4142)
		if (Tan(xd, yd)<=2.4142)
			ch.facing:=7;
		else
			ch.facing:=6;
		endif
	   else
			ch.facing:=0;
	   endif

 	endcase

endif
endfunction

function CreateSosBottle(char)

var cfg := ReadConfigFile("sosarea");
var maxI := GetConfigMaxIntKey(cfg);
var elem := cfg[Random(maxI)+1];

var X := CInt(elem.x)+Random(CInt(elem.range)*2) - CInt(elem.range);
var Y := CInt(elem.y)+Random(CInt(elem.range)*2) - CInt(elem.range);
// var Z := GetWorldHeight(X,Y);

// syslog(X+" "+Y+" "+Z);

var bottle :=CreateItemInContainer(char.backpack, 0xa360);

SetObjProperty(bottle, "tileX", X);
SetObjProperty(bottle, "tileY", Y);
// SetObjProperty(bottle, "tileZ", (Z+1));

UnloadConfigFile( "sosarea");

endfunction

function RandomFish(character)

var currentskill := GetEffectiveSkill( character, SKILLID_FISHING);
var thisone := Random(currentskill) + 1;
var fishtype;

if (thisone<50 )
	fishtype := Random(UOBJ_FISH_END - UOBJ_FISH_START);
	fishtype := UOBJ_FISH_START + fishtype;
elseif (thisone<100)
	fishtype := Random(UOBJ_SFISH_END - UOBJ_SFISH_START);
	fishtype := UOBJ_SFISH_START + fishtype;
elseif (thisone<130)
        fishtype := Random(8) + 0xa368;
endif
return fishtype;
endfunction

function createfish( who,fish,fish_amount, equipped )

foreach item in ListItemsNearLocationOfType( CInt(who.x), CInt(who.y), CInt(who.z), 1, CInt(fish) )

if(!ReserveItem(item)) return; endif
fish_amount := item.amount+fish_amount;
DestroyItem(item);
break;
endforeach

if (equipped==1)
CreateItemAtLocation(CInt(who.x), CInt(who.y), CInt(who.z), CInt(fish), CInt(fish_amount));
else
PlaySoundEffect(who, SFX_57);
PerformAction( who, ANIM_SALUTE );
CreateItemInBackpack(who, UOBJ_FISHSTEAK, (4 * fish_amount));
SendSysMessage(who, "You carve up the fish.");
endif

endfunction

function whatequipped(who, pole)

var item := GetEquipmentByLayer( who, 2 );
var object1:=item.graphic;

item := GetEquipmentByLayer( who, 1 );
var object2:=item.graphic;
var hands:=0;

if (object1==0xdbf) hands := 1; endif

if (object2==0xf51 || object2==0x13f6) hands := 2; endif

if (hands==0 && pole!=666)
if (EquipItem(who,pole)) hands:=1; endif
endif

return hands;

endfunction