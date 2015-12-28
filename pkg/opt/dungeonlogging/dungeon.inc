use cfgfile;

include "include/areas";
var areas_cfg	:= ReadConfigFile( CFGFILE_AREAS );

function ListDungeonAreas()
//first
	return{
		"Covetous",
		"Deceit",
		"Despise",
		"Destard",
		"Fire",
		"Hythloth",
		"Ice",
		"Shame"
		};

endfunction

function GetDungeonName(index)
	var dungnames := ListDungeonAreas();
	var tempname := dungnames[index];
	if (tempname=="Ice"||tempname=="Fire")
		tempname := tempname + " Dungeon";
	endif
	return tempname;
endfunction

function ListSendPoints()

	return{
		"2499 921 0", //Covetous
		"4111 434 5", //Deceit
		"1300 1081 0", //Despise
		"1176 2639 0", //Destard
		"5765 2914 34", //Fire Dungeon
		"4722 3825 0", //Hythloth
		"5210 2322 30", //Ice Dungeon
		"510, 1563, 0" //Shame Part 1
	};

endfunction


function IsADungeon(area_desc)
	var i:=0;
	foreach a_desc in ListDungeonAreas()
		i:=i+1;
		if (a_desc==area_desc)
			return i;
		endif
	endforeach
	return 0;
endfunction

function IsInDungeonArea(who)
	var areas	:= GetConfigStringArray( areas_cfg[1], "Area" );
	if(!areas)
		return 0;
	endif
	//print("found areas");
	var i;
	var splited;
	for( i := 1; i <= areas.size(); i := i + 1 )
		splited:=SplitWords(areas[i]);
		//int( " Is In Box " + IsInBox( who, CInt(splited[1]), CInt(splited[2]), CInt(splited[3]), CInt(splited[4])));
		//int (splited[5]);
		//int (IsADungeon(splited[5]));
		if( IsInBox( who, CInt(splited[1]), CInt(splited[2]), CInt(splited[3]), CInt(splited[4])))
		 	var mydung:=IsADungeon(splited[5]);
			if (mydung)
				return mydung;
			endif
		endif
	endfor
	return 0;
	
endfunction