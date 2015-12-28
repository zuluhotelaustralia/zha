//include file for guild uniform functions
function AddGuildUniformToGuild(guilduni, guild)
	var guilduniserials:= guild.getprop("GuildUniSers");
	if (!guilduniserials)
		guilduniserials:=array;
	endif
	guilduniserials.append(guilduni.serial);
	guild.setprop("GuildUniSers", guilduniserials);
endfunction

function RemoveGuildUniformFromGuild(guilduni, guild)
	var guilduniserials:= guild.getprop("GuildUniSers");
	if (!guilduniserials)
		return 0;
	endif
	var tempsers:=array;
	foreach gs in guilduniserials
		if (gs !=  guilduni.serial)
			tempsers.append(gs);
		endif
	endforeach
	guild.setprop("GuildUniSers", tempsers);
	//Broadcast(guild.getprop("GuildUniSers"));
endfunction

function UpdateGuildUniformColours(guild)
	var guildunis:=ListGuildUniforms(guild);
	//Broadcast(guildunis);
	if (!guildunis)
		return 0;
	endif
	var guildunicolor := guild.getprop("guildunicolour");
	foreach g in guildunis
		g.color := guildunicolor;
		//Broadcast(g);
	endforeach
endfunction

function UpdateGuildUniformDescs(guild)
	var guildunis:=ListGuildUniforms(guild);
	if (!guildunis)
		return 0;
	endif
	var guildabbrev := guild.getprop("guildabv");
	foreach g in guildunis
		g.desc := "a " + guildabbrev + " guild uniform";
		g.name := "a " + guildabbrev + " guild uniform";
	endforeach
endfunction

function ListGuildUniforms(guild)
	var guilduniserials:= guild.getprop("GuildUniSers");
	//Broadcast(guilduniserials);
	var guildunis:=array;
	var g;
	foreach gs in guilduniserials
		g := SystemFindObjectBySerial(gs, SYSFIND_SEARCH_OFFLINE_MOBILES);
		if (g)
			guildunis.append(g);
		endif
	endforeach
	return guildunis;

endfunction