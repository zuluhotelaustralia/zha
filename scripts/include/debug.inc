use uo;
use os;
//set to 1 to turn on or 0 to turn off.

var DEBUG := getglobalproperty("debugenabled");

function debugmessage( message )

if ( DEBUG == 0)
	return;
endif
	foreach who in EnumerateOnlineCharacters()
		if (getobjproperty(who, "debugon")==1)
			SendSysMessage(who, message);
		endif
	endforeach
endfunction