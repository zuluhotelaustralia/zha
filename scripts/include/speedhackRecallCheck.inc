function SetSpeedCheckNote(who)
	if (GetObjProperty(who, "CheckingSpeed"))
		SetObjProperty(who, "RecalledGatedTeled", 1);
	endif
endfunction