use uo;
include "include/time";

const LOG_KILL := "::log_kill";

function logKiller(byref victim, byref killer, dmg, attack_type)
    var logDetails := {
        {"killerName", killer.name},
        {"killerAccount", killer.acctname},
        {"killerSerial", killer.serial},
        {"killerLocation", killer.x + "," + killer.y + "," + killer.z},
        {"victimName", victim.name},
        {"victimAccount", victim.acctname},
        {"victimSerial", victim.serial},
        {"victimLocation", victim.x + "," + victim.y + "," + victim.z},
        {"damage", dmg},
        {"attack_type", attack_type},
        {"datetime", NowString()}
    };
	
	AppendConfigFileElem(LOG_KILL, "Kill", victim.serial, logDetails );
	UnloadConfigFile(LOG_KILL);
endfunction