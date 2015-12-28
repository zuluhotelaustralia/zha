function enableAosLBRClient(who)
	var account := who.acct;
	
	if(account.GetProp("LBR"))
		SendPacket(who, "B90003");
	else
		//AOS
		SendPacket(who, "B9801B");
	endif

endfunction

function fixPlayerGraphic(who)
		if (who.acct) //fix for uo3d clients - have to get them to refresh player graphic
			if (who.cmdlevel > 0)
				var oldconcealed := who.concealed;
				who.concealed:=0;
				who.concealed:=oldconcealed;	
			else
				who.concealed:=0;
			endif
		endif
endfunction