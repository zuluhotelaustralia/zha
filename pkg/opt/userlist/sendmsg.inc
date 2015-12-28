use uo;

function SendTheMessage(who, whom)

	var themessage:=RequestInput(who, who.backpack, "What message would you like to send "+whom.name+"?");
	if(!themessage)
		SendSysMessage(who, "Canceled.");
		return;
	endif
	SendSysMessage(whom, who.name+": "+themessage, color:=0x36);
	SetObjProperty(whom, "#msgfrom", who.serial);

endfunction