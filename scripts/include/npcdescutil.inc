//Barns - 7/03/04
//Started this to put in convenient functions for accessing npcdesc.cfg

use cfgfile;
use uo;

function GetNpcDescGraphic(npctemplate)
	var npccfg := ReadConfigFile("::npcdesc");
	var monsterelem := npccfg[npctemplate];
	return GetConfigInt(monsterelem, "graphic");
endfunction