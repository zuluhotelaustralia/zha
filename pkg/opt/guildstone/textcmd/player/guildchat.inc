const HEAR_NONE := 0;
const HEAR_OWN := 1;
const HEAR_ALLIED := 2;
const HEAR_ALL := 3;
const SEND_OWN := 1;
const SEND_ALLIED := 2;
const SEND_ALL := 3;


function msgall (msg, myguild)

	var guilds:=ListGuilds();
	var members;
	var hear;

	foreach guild in guilds
		members := guild.members;
		foreach mobile in members
			hear:=GetObjProperty(mobile,"hearchat");
			if  (myguild.ismember(mobile) && hear>=HEAR_OWN )
				SendSysMessage(mobile, msg, 3, 68);
			elseif (guild.isallyguild(myguild) && hear>=HEAR_ALLIED)
				SendSysMessage(mobile, msg, 3, 56);
			elseif (hear == HEAR_ALL)
				SendSysMessage(mobile, msg, 3, 44);
			endif
		endforeach
	endforeach
endfunction

function msgallied (msg, myguild)

	var guilds:= ListGuilds();
	var hear;

	foreach guild in guilds
		if (myguild.isallyguild(guild) || myguild.guildid==guild.guildid)
			var members := guild.members;
			foreach mobile in members
				hear:=GetObjProperty(mobile, "hearchat");
				if(myguild.ismember(mobile) && hear >= HEAR_OWN)
					SendSysMessage(mobile, "[A] "+msg, 3, 68);
				elseif (hear >= HEAR_ALLIED)
					SendSysMessage(mobile, "[A] "+msg, 3, 56);
				endif
			endforeach
		endif
	endforeach

endfunction

function msgown (msg, myguild)

	var members := myguild.members;

        foreach mobile in members
            if (GetObjProperty(mobile,"hearchat") > HEAR_NONE)
                SendSysMessage(mobile, "[O] "+msg, 3, 68);
            endif
        endforeach

endfunction

function CheckSquelch(who)

	var squelch:= CInt(GetObjProperty(who, "#guildchatsquelch"));
	if (squelch)
		if (squelch == -1)
			SendSysMessage(who, "Your guildchat priveleges have been revoked until further notice");
			return 0;
		elseif (ReadGameClock() < squelch)
			SendSysMessage(who, "You cannot guildchat for another "+CInt((squelch - ReadGameClock()) / 60)+" minutes");
			return 0;
		else
			EraseObjProperty (who, "#guildchatsquelch");
		endif
	endif
	return 1;
endfunction