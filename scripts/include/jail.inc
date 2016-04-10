function JailCheck(who)
	if (who.cmdlevel)
		return;
	endif
	
	var acc := who.acct;

	var release := acc.getprop("Release");
	if( (who.x >= 5272) && (who.x <= 5310) && (who.y >= 1160) && (who.y <= 1190) )
		if( release != 0 )
			var timeremaining := ((release - ReadGameClock())/86400) + 1;
			SendSysMessage(who, "You have " + timeremaining + " day(s) left in jail.");
			
			MoveObjectToLocation( who, 5304, 1184, 0, _DEFAULT_REALM, 0); //AutoConversion
		else
			acc.setprop("Release", 0);
			MoveObjectToLocation( who, 1475, 1645, 20, _DEFAULT_REALM, 0); //AutoConversion
			SendSysMessage(who, "You are freed.");
		endif
	endif
endfunction