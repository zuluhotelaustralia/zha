var me := Self();

var flee_point;

drop_anchor();

var npccfg := ReadConfigFile("npcdesc");
var speechelem := npccfg[me.npctemplate];
flee_point := speechelem.flee_point;
if (!flee_point)
        flee_point := 10;
endif

if (!me.backpack)
	var newbackpack := CreateItemAtLocation(me.x, me.y, me.z, 0xe75,1);
	EquipItem(me, newbackpack);
	if (npccfg[me.npctemplate].colorrange)
		var parms := SplitWords(npccfg[me.npctemplate].colorrange);
		me.color := Random(CInt(parms[2]) - CInt(parms[1]))+CInt(parms[1]);
	endif
endif

if( me.name["<random>"] )
	SetName( me, RandomName( me ) );
endif