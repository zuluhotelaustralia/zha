function OrbReturnTimeElapsed(orbitemreturner)
	//returns true if time elapsed
	var lastwalkedTime := CInt(GetObjProperty(orbitemreturner, "#timeorbtoreturn"));
	if (ReadGameClock()>lastwalkedTime) //that is current time is greater than reserved time
		return 1;
	endif
	return 0;
endfunction

function SetOrbReturnTime(orbitemreturner)
	SetObjProperty(orbitemreturner, "#timeorbtoreturn", CInt(ReadGameClock()+1800)); //1800 = 30mins
endfunction

function RunOrbReturnManager(orbitemreturner)
	var orb := findorb(orbitemreturner);
	if (!orb)
		ResetOrbItemReturnerState(orbitemreturner);
		return;
	endif
	
	var orbreturntime := GetObjProperty(orbitemreturner, "#timeorbtoreturn");
	while (! OrbReturnTimeElapsed(orbitemreturner))
		sleep(60);//1 minute sleep		
	endwhile
	if (orb.container)
		PrintTextAbove(orb.container, "The magic crystal shudders suddenly and fades away!");
	endif
	DestroyItem(orb);
	ResetOrbItemReturnerState(orbitemreturner);
endfunction

function findorb(orbitemreturner)
	var orbser:=GetObjProperty(orbitemreturner, "OrbSerial");
	if (!orbser)
		return 0;
	endif
	var orb := SystemFindObjectBySerial(orbser, SYSFIND_SEARCH_OFFLINE_MOBILES);
	if (! orb)
		return 0;
	endif
	return orb;
endfunction

function ResetOrbItemReturnerState(orbitemreturner)
	orbitemreturner.graphic:=2583;	
	PrintTextAbove(orbitemreturner, "The crystal generator infuses a new magic crystal!");
endfunction