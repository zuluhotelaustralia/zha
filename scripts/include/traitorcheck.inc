// Archaean 30/5/03

// NOTE: use .setglobal in game to set the
// coords for elf base and darkelf base
// CO-ORD names:
// ELF BASE: (elfx1, elfy1) (elfx2, elfy2)
// DARK BASE: (darkelfx1, darkelfy1) (darkelfx2, darkelfy2)
// Elves/Darkelves will only be labelled traitor if they
// are within their own base and they cast a beneficial
// spell or use Healing skill on a PC with different race

const TIME_AS_TRAITOR := 600; 

function CheckTraitor(source, targetted)

	if (!IsElf(source) && !IsDarkElf(source))
		return;
	elseif (targetted.title_race != source.title_race && LocationCheck(source))
		start_script(":spells:settraitor", source);
	endif

endfunction

function IsElf(who)

	if (who.title_race == "Elf")
		return 1;
	endif
	return 0;

endfunction


function IsDarkElf(who)

	if (who.title_race == "Dark-Elf")
		return 1;
	endif
	return 0;

endfunction


function LocationCheck(source)
	
	var elfx1 := GetGlobalProperty("elfx1");
	var elfy1 := GetGlobalProperty("elfy1");
	var elfx2 := GetGlobalProperty("elfx2");
	var elfy2 := GetGlobalProperty("elfy2");
	var darkelfx1 := GetGlobalProperty("darkelfx1");
	var darkelfy1 := GetGlobalProperty("darkelfy1");
	var darkelfx2 := GetGlobalProperty("darkelfx2");
	var darkelfy2 := GetGlobalProperty("darkelfy2");
	case (source.title_race)
		"Elf":
			if ((source.x > elfx1 && source.x < elfx2) && (source.y > elfy1 && source.y < elfy2))
				return 1;
			endif

		"Dark-Elf":
			if ((source.x > darkelfx1 && source.x < darkelfx2) && (source.y > darkelfy1 && source.y < darkelfy2))
				return 1;
			endif			
	endcase
	return 0;

endfunction