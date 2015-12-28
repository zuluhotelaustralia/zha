use uo;
use cfgfile;
use os;

include "include/constants/cmdlevels";
include "include/time";

//change these to change password
const ADD_ACCOUNT_PASSWORD := "mjb Rcllcqqcc";
const ADD_ACCOUNT_PASSWORD_HINT := "Id like to go back to old Tennessee";

//Notes: councies to GM can be assigned in game...
//devs and admins are hardcoded and require recompile

// Account names of current staff. LOWER CASE!
var developers := {"admin", "flimstaff", "lainstaff"};
var administrators := {"chrisstaff", "spyhard"};
//var gamemasters := {"vash", "chrisstaff", "staffaccount2", "banestaff"};
//var seers := {"sstaff","fuel", "arreatstaff", "katstaff", "dagstaff"};
//var counselors := {"emielstaff", "nathanstaff", "meepstaff", "noobstaff", "nerdstaff",
//"staffaccount1","staffaccount3","staffaccount4","staffaccount5"};

function checkStaff(who, commandname, requiredCommandLevel)
	var realCommandLevel := getCommandLevel(who);
	if ((realCommandLevel >= requiredCommandLevel) && who.cmdlevel>=requiredCommandLevel)
		return 1;
	else
		return handleFake(who, commandname);
	endif
endfunction

function getCommandLevel(who)
	if (lower(who.acctname) in developers)
		return CMDLEVEL_DEVELOPER;
	elseif (lower(who.acctname) in administrators)
		return CMDLEVEL_ADMINISTRATOR;
	elseif (lower(who.acctname) in GetGlobalProperty("gamemasters"))
		return CMDLEVEL_GAME_MASTER;
	elseif (lower(who.acctname) in GetGlobalProperty("seers"))
		return CMDLEVEL_SEER;
	elseif (lower(who.acctname) in GetGlobalProperty("counselors"))
		return CMDLEVEL_COUNSELOR;
	else
		return CMDLEVEL_PLAYER;
	endif
endfunction

function checkCommandLevelArrays()
	//at boot up simply cleans arrays of non-existent or banned accounts
	var index:=1;
	var accobj;
	var temparray:=array;
	while (index<=3)
		foreach accname in GetCommandLevelArrayByIndex(index)
			accobj := FindAccount(accname);
			if (accobj) 
				if(!accobj.banned)
					temparray.append(accname);
				endif
			endif

		endforeach
		SetGlobalProperty(GetCommandLevelArrayNameByIndex(index), temparray);
		temparray:=array;
		index:=index+1;
	endwhile
	syslog( "Staff command level security arrays updated." );
endfunction

function GetCommandLevelArrayByIndex(index)
	//at boot up simply cleans arrays of non-existent or banned accounts
	case(index)
		1: return GetGlobalProperty("counselors");
		2: return GetGlobalProperty("seers");
		3: return GetGlobalProperty("gamemasters");
		4: return administrators;
		5: return developers;
	endcase
endfunction

function GetCommandLevelArrayNameByIndex(index)
	//at boot up simply cleans arrays of non-existent or banned accounts
	case(index)
		1: return "counselors";
		2: return "seers";
		3: return "gamemasters";
		4: return "administrators";
		5: return "developers";
	endcase
endfunction

function AddToCommandLevelArrays(who, accountname)
	//adds an account name to appropriate global array
	var cmdlevel_int := FindHighestCmdLevelForAccount(accountname);
	if (cmdlevel_int == -1)
		SendSysMessage(who, "Error: Invalid account name");	
		return;
	endif

	if (cmdlevel_int == 0)
		SendSysMessage(who, "Error: No staff character on this account");	
		return;
	endif
	
	if (cmdlevel_int>3)
		SendSysMessage(who, "Error: Admin+ must be hardcoded.");	
		return;
	endif

	SendSysMessage(who, "Please enter the password for adding new staff to power groups.",9,53);
	SendSysMessage(who, "The current hint is:",9,53);
	SendSysMessage(who, ADD_ACCOUNT_PASSWORD_HINT,9,53 );
	var result := SendTextEntryGump( who, "Enter the password for adding new staff to power groups");
	if (result!=ADD_ACCOUNT_PASSWORD)
		SendSysMessage(who, "Error: Invalid password");	
		return;
	endif

	var staffarray := GetCommandLevelArrayByIndex(cmdlevel_int);
	if (!staffarray)
		staffarray := array;
	endif
	staffarray.append(accountname);
	SetGlobalProperty(GetCommandLevelArrayNameByIndex(cmdlevel_int), staffarray);
	SendSysMessage(who, accountname + " added to " + GetCommandLevelArrayNameByIndex(cmdlevel_int) + " powers group.",3,53);
endfunction

function RemoveFromCommandLevelArrays(who, accountname)
	//removes an account name from global arrays

	if (accountname == "")
          SendSysMessage(who, "No account given.",3,53);
          return;
        endif

	SendSysMessage(who, "Please enter the password for adding new staff to power groups.",9,53);
	SendSysMessage(who, "The current hint is:",9,53);
	SendSysMessage(who, ADD_ACCOUNT_PASSWORD_HINT,9,53 );
	var result := SendTextEntryGump( who, "Enter the password for adding new staff to power groups");
	if (result!=ADD_ACCOUNT_PASSWORD)
		SendSysMessage(who, "Error: Invalid password");	
		return;
	endif

       // var currentno;
        for currentno := 1 to 3
          var oldarray := GetCommandLevelArrayByIndex(currentno);
          var temparray := array;
          foreach account in oldarray
            if (account != accountname)
              temparray.append(account);
            endif
          endforeach
          if (oldarray.size() != temparray.size())
            SendSysMessage(who, accountname + " removed from " + GetCommandLevelArrayNameByIndex(currentno) + " powers group.",3,53);
            SetGlobalProperty(GetCommandLevelArrayNameByIndex(currentno), temparray);
          endif
        endfor
endfunction



function FindHighestCmdLevelForAccount(accountname)
     	var accobj := FindAccount(accountname);
	if (!accobj)
		return -1;
	endif
	
	var charNum;
	var player;
	var maxcmdlevel := 0;
	for (charNum:=1; charNum <=5; charNum := charNum+1)
		player:=accobj.GetCharacter(charNum);
		if (player != error && player.cmdlevel)
			if (player.cmdlevel > maxcmdlevel)
				maxcmdlevel := player.cmdlevel;
			endif
		endif
	endfor
	return maxcmdlevel ;
        
endfunction


function PropNameIsACommandLevelArray(propname)
	//returns 1 is propname is a commandlevel array name... to stop ppl using setglobal to assign ppl to arrays
	var propnames:= {"counselors", "seers", "gamemasters", "administrators", "developers"};
	foreach p in propnames
		if (propname == p)
			return 1;
		endif
	endforeach
	return 0;
endfunction


/*
function getCommandLevel(who)
	if (lower(who.acctname) in developers)
		return CMDLEVEL_DEVELOPER;
	elseif (lower(who.acctname) in administrators)
		return CMDLEVEL_ADMINISTRATOR;
	elseif (lower(who.acctname) in gamemasters)
		return CMDLEVEL_GAME_MASTER;
	elseif (lower(who.acctname) in seers)
		return CMDLEVEL_SEER;
	elseif (lower(who.acctname) in counselors)
		return CMDLEVEL_COUNSELOR;
	else
		return CMDLEVEL_PLAYER;
	endif
endfunction */

function handleFake(who, commandname)
	logFake(who, commandname);
	foreach person in EnumerateOnlineCharacters()
		if (person.cmdlevel && person.serial!=who.serial)
			SendSysMessage(person, who.name + " attempted to access priveleges they do not have! [" + commandname + "]", 3, 38);
		endif
	endforeach
	
	return 0;
endfunction

function logFake(who, commandname)
	var desc := who.name + " / " + who.acctname;
	if (desc)
	endif
	
	var elemkey := who.acctname;
	var props := array;
	var prop := array;
	prop .+ pname;
	prop .+ pvalue;
	
	prop.pname := "charname";
	prop.pvalue := who.name;
	props[1] := prop;
	
	prop.pname := "serial";
	prop.pvalue := who.serial;
	props[2] := prop;
	
	prop.pname := "commandname";
	prop.pvalue := commandname;
	props[3] := prop;
	
	prop.pname := "datetime";
	prop.pvalue := NowString();
	props[4] := prop;
	
	AppendConfigFileElem( "staffcommands", "haxor", elemkey, props );
	UnloadConfigFile("staffcommands");
endfunction
