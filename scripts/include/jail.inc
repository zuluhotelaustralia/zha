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
			
			MoveCharacterToLocation( who, 5304, 1184, 0, 0);
		else
			acc.setprop("Release", 0);
			MoveCharacterToLocation( who, 1475, 1645, 20, 0);
			SendSysMessage(who, "You are freed.");
		endif
	endif
endfunction