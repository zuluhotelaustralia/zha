use uo;

function getexistinglogons(who)
	if( who.cmdlevel > 2)
		return 0;
	endif 

	var count := 0;
	var maxchars := getglobalproperty("maxcharsperip");
	var account := who.acct;
	var adds := account.getprop("moreips");
	
	foreach char in EnumerateOnlineCharacters()
			if (char.ip == who.ip)
				count := count + 1;
			endif
		endforeach
	var actual := cint(maxchars) + cint(adds);
	if( cint(count) > cint(actual))
		return 1;
	else
		return 0;
	endif

endfunction