include "include/gumps";

function SecurityCheck(who)

	var password := CStr(SendTextEntryGump(who, "Enter your current ZHA email password:", TE_CANCEL_ENABLE, TE_STYLE_NORMAL, 16));
	var emailpassword := who.acct.getprop("EmailPassword");
	if (!emailpassword)
		SendSysMessage(who, "Error...");
		return 0;
	endif
	if (password!=emailpassword)
		SendSysMessage(who, "Password incorrect!");
		return 0;
	endif
	return 1;
	
endfunction

function SetPassword(who)

	var newpassword := CStr(SendTextEntryGump(who, "Enter new ZHA email password (4-16 chars):", TE_CANCEL_ENABLE, TE_STYLE_NORMAL, 16));
	if (!newpassword || !ValidateCharacters(newpassword) || len(newpassword) < 4)
		SendSysMessage(who, "Cancelled");
		return;
	endif
	if (!who.acct.setprop("EmailPassword", newpassword))
		SendSysMessage(who, "Error...");
		return;
	endif
	SendSysMessage(who, "Your email password has been set to '"+who.acct.getprop("EmailPassword")+"'");
	return;
	
endfunction

function ValidateCharacters(thestring)

	var i, validstr:={"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9","@","_",".","-"};
	for (i := 1; i <= len(thestring); i := i + 1)
		if(!(lower(thestring[i]) in validstr))
			return 0;
		endif
	endfor
	return 1;	
endfunction
	