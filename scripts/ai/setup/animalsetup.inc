// include "include/starteqp";

	set_critical(0);
	var me := Self();
	var flee_point := 10;
	drop_anchor();

	if (!me.backpack)
		var npccfg := ReadConfigFile("npcdesc");
		var newbackpack := CreateItemAtLocation(5288, 1176, 0, 0xe75,1);
		EquipItem(me, newbackpack);
		if (npccfg[me.npctemplate].colorrange)
			var parms := SplitWords(npccfg[me.npctemplate].colorrange);
			me.color := Random(CInt(parms[2]) - CInt(parms[1]))+CInt(parms[1]);
			SetObjProperty(me,"color",me.color);
		endif

		if( me.name["<random>"] )
			SetName( me, RandomName( me ) );
		endif

		// MagicalEquip(me);

	endif