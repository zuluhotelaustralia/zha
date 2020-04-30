use uo;
use os;

include "/include/datestring";
include "setup";

function updateDecayStatus(signserial)
//works by checking whether the current date is the same as the date the sign was last checked by updatedecaystatus
//if its a different day then the decay day counter on the sign is incremented 1
//if decay days is > than normal then the sign changes color
//if decay days is > than normal and alert times then house deleted
	const DECAY_DAYS_NORMAL:=35; //days sign normal coloured
	const DECAY_DAYS_ALERT:=7; //days alert colour


	var sign:=SystemFindObjectBySerial(CInt(signserial));
	if (!sign)
		return 0;
	endif

	var lastclicked := GetObjProperty(sign, "LastClickedByOwner");
	var lastupdated := GetObjProperty(sign, "LastUpdatedDecay");
	var numberDaysDecayed := CInt(GetObjProperty(sign, "NumberDecayDays"));

	if (!lastclicked||!lastupdated||!numberDaysDecayed||lastclicked==error||lastupdated==error||numberDaysDecayed==error)
		ResetDecayStatus(sign);
		return 1;
	endif

	if (compareDateTimeStrings(NowString(), lastupdated) <> 0)
		numberDaysDecayed := numberDaysDecayed +1;
	endif

	if (numberDaysDecayed>DECAY_DAYS_NORMAL+DECAY_DAYS_ALERT)
		//house has not been clicked for normal and alert times
		var house := SystemFindObjectBySerial(CInt(GetObjProperty(sign, "house_serial")));
		deleteHouse(house);
		return 1;
	endif

	//sign goes red cos cos past alert time
	if (numberDaysDecayed>DECAY_DAYS_NORMAL)
		//house has not been clicked before alert time
		AddToDecayingHouses(sign.serial);
		sign.color:=1305;

	endif

	if (numberDaysDecayed>DECAY_DAYS_NORMAL-DECAY_DAYS_ALERT)
		//house has not been clicked before alert time minus - time to decay (so this is yellow alert)
		sign.color:=53;
	endif

	SetObjProperty(sign, "NumberDecayDays", numberDaysDecayed );
	SetObjProperty(sign, "LastUpdatedDecay", NowString());
endfunction

function ResetDecayStatus(sign)
//sets sign status to initial
//i.e. sets lastclicked to now, lastchecked to now, numberOfDecayDays = 1;
	PrintTextAbove(sign, "House decay status reset!");
	SetObjProperty(sign, "LastClickedByOwner", NowString());
	SetObjProperty(sign, "LastUpdatedDecay", NowString());
	SetObjProperty(sign, "NumberDecayDays", 1);
	sign.color:=0;
	RemoveFromDecayingHouses(sign.serial);
endfunction

function AddToDecayingHouses(signser)
	var decayhouses := GetGlobalProperty("decayingSigns");
	if (!decayhouses||decayhouses==error)
		SetGlobalProperty("decayingSigns", {signser});
		return;
	endif
	foreach sser in decayhouses
		if (sser==signser)
			return;
		endif
	endforeach
	decayhouses.append(signser);
	SetGlobalProperty("decayingSigns", decayhouses);
endfunction

function RemoveFromDecayingHouses(signser)
	var decayhouses := GetGlobalProperty("decayingSigns");
	var tempdhouses := array;
	if (!decayhouses||decayhouses==error)
		return;
	endif
	foreach sser in decayhouses
		if (sser!=signser)
			tempdhouses.append(sser);
		endif
	endforeach
	SetGlobalProperty("decayingSigns", tempdhouses);

endfunction