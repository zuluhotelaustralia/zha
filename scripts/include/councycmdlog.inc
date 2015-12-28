include "include/time";

function councycommandlog (councy, commandstr, who, fromloc, toloc)

	var desc := councy.name + " / " + councy.acctname;
	
	var elemkey := desc;
	var props := array;
	var prop := array;
	prop .+ pname;
	prop .+ pvalue;

	prop.pname := "councy";
	prop.pvalue := councy.name;
	props[1] := prop;

	prop.pname := "councy serial";
	prop.pvalue := councy.serial;
	props[2] := prop;

	prop.pname := "command";
	prop.pvalue := commandstr;
	props[3] := prop;

	prop.pname := "who done to";
	prop.pvalue := who.name;
	props[4] := prop;
	
	prop.pname := "who serial";
	prop.pvalue := who.serial;
	props[5] := prop;
	
	prop.pname := "from";
	prop.pvalue := fromloc.x + " " + fromloc.y + " " + fromloc.z;
	props[6] := prop;

	prop.pname := "to";
	prop.pvalue := toloc.x + " " + toloc.y + " " + toloc.z;
	props[7] := prop;
	
	prop.pname := "datetime";
	prop.pvalue := NowString();
	props[8] := prop;

	AppendConfigFileElem( "councycommandusage", "councycommandusage", elemkey, props );
	UnloadConfigFile("councycommandusage");
	
endfunction