use uo;

function getexistinglogons(who)
	if( who.cmdlevel > 2)
		return 0;
	endif

	var count := 0;
	var maxchars := getglobalproperty("maxcharsperip");
	var account := who.acct;
	var adds := account.getprop("moreips");

	if( !maxchars )
		setglobalproperty("maxcharsperip", 1);
		maxchars := 1;
	endif

	foreach char in EnumerateOnlineCharacters()
			if ((char.ip == who.ip) && (!char.cmdlevel))
				count := count + 1;
			endif
	endforeach
	var actual := cint(maxchars) + cint(adds);
	print("["+account.name+"] count: "+cint(count)+" actual: "+cint(actual)+" maxchars: "+cint(maxchars)+" adds: "+cint(adds));
	if( cint(count) > cint(actual))
		return 1;
	else
		return 0;
	endif

endfunction