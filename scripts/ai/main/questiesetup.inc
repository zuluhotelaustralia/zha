var me := Self();
drop_anchor();
var questtype := 0;
var npccfg := ReadConfigFile("npcdesc");
var speechelem := npccfg[me.npctemplate];
var speech := GetConfigString(speechelem, "speech");
var equipname := speechelem.equip;
var saywords := speechelem.saywords;
var flee_point := speechelem.flee_point;
if (!flee_point)
        flee_point := 10;
endif

if (!me.backpack)
	var newbackpack := CreateItemAtLocation(me.x, me.y, me.z, 0xe75,1);
	EquipItem(me, newbackpack);
endif

if( me.name["<random>"] )
	me.graphic := 400 + Random(2);
	var title := Random(20);
	var ttitle := "";
	case (title)
          	0: ttitle := "";
          	1: ttitle := " the townsperson";
          	2: ttitle := " the farmer";
          	3: ttitle := " the sailor";
          	4: ttitle := " the drunkard";
          	5: ttitle := " the wanderer";
          	6: ttitle := " the villager";
          	7: ttitle := " the gypsie";
          	8: ttitle := " the vagabond";
          	9: ttitle := " the laborer";
	       	  default: ttitle := "";
	endcase

	var tname := randomname(me);
	me.name := tname + ttitle;
	var style := "poor";
	case (Random(4))
	0:style := "rich";
	1:style := "warrior";
	2:style := "mage";
	endcase
	var parms := {};
	parms[1]:= me;
	parms[2]:= style;
	start_script("::/misc/dressme", parms);
endif

var master;
var slave;

sleep(2);
QuestieSetup();

//include "include/attributes";
function QuestieSetup()

	master := 0;
	slave := 0;

	if (GetObjProperty(me,"master"))
		if (!SystemFindObjectBySerial(GetObjProperty(me,"master")))
			//I should have a master but I don't!
			RevokePrivilege( me, "invul" );
			//MoveObjectToLocation(me,5288,1176,0, _DEFAULT_REALM,MOVEOBJECT_FORCELOCATION); //AutoConversion
			SetObjProperty(me,"guardkill",1);
			me.kill();
		endif
	endif

	master := SystemFindObjectBySerial(GetObjProperty(me,"master"));
	if (!master)
		master := me;
	endif

	if (master.serial == me.serial)
		slave := SystemFindObjectBySerial(GetObjProperty(me,"slave"));
		if (!slave)
			slave := MakeSlave();
		endif
		questtype := Random(1)+1;
		case (questtype)
			1: SetupMailboy();
		endcase
	endif

	SetObjProperty(me,"master",master.serial);

endfunction

function SetupMailboy()

	foreach item in EnumerateItemsInContainer(me.backpack)
		if ( (GetObjProperty(item,"slave")) && (GetObjProperty(item,"slave") != slave.serial) )
			DestroyItem(item);
		endif
	endforeach

	foreach item in EnumerateItemsInContainer(me.backpack)
		if ( GetObjProperty(item,"slave") == slave.serial )
			return;
		endif
	endforeach

	CreateQuestItem(me.backpack,slave.serial);

endfunction

function CreateQuestItem(mypack,slaveserial)

	var itemcfg := ReadConfigFile( "questitems" );
       	var itemfile := itemcfg[ 1 ];
	var itemlist := GetConfigStringArray( itemfile, "Item" );
	var item := 0;

	repeat
		var itemobjtype := itemlist[Random(len(itemlist)+1)];
		item := CreateItemInContainer(mypack,CInt(itemobjtype));
	until (item);

	SetObjProperty(item,"slave",slaveserial);
	item.movable := 1;

endfunction

function MakeSlave()

	var newslave;

	var slavetown;
	var x1, y1, x2, y2, x, y, z;

	var tries := 0;
	repeat
	case ( Random(31) )
	0:
	1:
	2:
		slavetown := "Moonglow";
		x1 := 4379;
		y1 := 1045;
		x1 := 4487;
		y2 := 1178;
	3:
	4:
	5:
	6:
		slavetown := "Britain";
		x1 := 1244;
		y1 := 1530;
		x2 := 1704;
		y2 := 1758;
	7:
	8:
	9:
		slavetown := "Jhelom";
		x1 := 1294;
		y1 := 3664;
		x2 := 1506;
		y2 := 3886;
	10:
	11:
	12:
		slavetown := "Yew";
		x1 := 494;
		y1 := 926;
		x2 := 600;
		y2 := 1034;
	13:
	14:
	15:
		slavetown := "Minoc";
		x1 := 2394;
		y1 := 370;
		x2 := 2624;
		y2 := 632;
	16:
	17:
	18:
		slavetown := "Trinsic";
		x1 := 1794;
		y1 := 2638;
		x2 := 2136;
		y2 := 2902;
	19:
	20:
	21:
		slavetown := "Skara Brae";
		x1 := 538;
		y1 := 2106;
		x2 := 686;
		y2 := 2274;
	22:
	23:
	24:
		slavetown := "Magincia";
		x1 := 3646;
		y1 := 2040;
		x2 := 3814;
		y2 := 2280;
	25:
		slavetown := "Occlo";
		x1 := 3588;
		y1 := 2446;
		x2 := 3762;
		y2 := 2690;
	26:
		slavetown := "Nujelm";
		x1 := 3496;
		y1 := 1062;
		x2 := 3808;
		y2 := 1400;
	27:
	28:
	29:
		slavetown := "Vesper";
		x1 := 2794;
		y1 := 634;
		x2 := 3062;
		y2 := 1000;
	30:
		slavetown := "Cove";
		x1 := 2206;
		y1 := 1116;
		x2 := 2290;
		y2 := 1236;
	endcase
	x := Random(x2-x1)+x1;
	y := Random(y2-y1)+y1;
	z := GetMapInfo(x,y).z;
	newslave := CreateNpcFromTemplate("questie",x,y,z,0);
	until (newslave);

	SetObjProperty(newslave,"master",me.serial);
	SetObjProperty(me,"slave",newslave.serial);
	SetObjProperty(me,"slavetown",slavetown);

	return newslave;

endfunction

function WhatTown(who)

	if (!who)
		return;
	endif

	var x := who.x;
	var y := who.y;
	var town;
	var x1, y1, x2, y2;

	town := "Moonglow";
	x1 := 4379;
	y1 := 1045;
	x1 := 4487;
	y2 := 1178;
	if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return town;
	endif

	town := "Britain";
	x1 := 1244;
	y1 := 1530;
	x2 := 1704;
	y2 := 1758;
	if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return town;
	endif

	town := "Jhelom";
	x1 := 1294;
	y1 := 3664;
	x2 := 1506;
	y2 := 3886;
	if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return town;
	endif

	town := "Yew";
	x1 := 494;
	y1 := 926;
	x2 := 600;
	y2 := 1034;
	if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return town;
	endif

	town := "Minoc";
	x1 := 2394;
	y1 := 370;
	x2 := 2624;
	y2 := 632;
	if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return town;
	endif

	town := "Trinsic";
	x1 := 1794;
	y1 := 2638;
	x2 := 2136;
	y2 := 2902;
	if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return town;
	endif

	town := "Skara Brae";
	x1 := 538;
	y1 := 2106;
	x2 := 686;
	y2 := 2274;
	if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return town;
	endif

	town := "Magincia";
	x1 := 3646;
	y1 := 2040;
	x2 := 3814;
	y2 := 2280;
	if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return town;
	endif

	town := "Occlo";
	x1 := 3588;
	y1 := 2446;
	x2 := 3762;
	y2 := 2690;
	if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return town;
	endif

	town := "Nujelm";
	x1 := 3496;
	y1 := 1062;
	x2 := 3808;
	y2 := 1400;
	if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return town;
	endif

	town := "Vesper";
	x1 := 2794;
	y1 := 634;
	x2 := 3062;
	y2 := 1000;
	if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return town;
	endif

	town := "Cove";
	x1 := 2206;
	y1 := 1116;
	x2 := 2290;
	y2 := 1236;
	if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return town;
	endif

	say("I dont know what town he's in.");
	return;

endfunction