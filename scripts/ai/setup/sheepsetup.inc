set_critical(0);
var me := Self();

var flee_point;

drop_anchor();

flee_point := 10;

if (!me.backpack)
	var newbackpack := CreateItemAtLocation(me.x, me.y, me.z, 0xe75,1);
	EquipItem(me, newbackpack);
	if (!Random(100) )
		me.color := 1109;
		SetObjProperty(me,"color",1109);
	endif
endif