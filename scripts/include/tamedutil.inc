function AddToMastersTamedArray()

	if (! AlreadyInMastersTamedArray())
		var tamedarray:= GetObjProperty(me.master, "tamed_serials");
		if (!tamedarray)
			tamedarray:=array;
		endif
		tamedarray.append(me.serial);
		SetObjProperty(me.master, "tamed_serials", tamedarray);
	endif
	
endfunction

function RemoveFromMastersTamedArray()

	var tamedarray:= GetObjProperty(me.master, "tamed_serials");
	if (!tamedarray)
		return 0;
	endif
	var temparray:=array;
	foreach ts in tamedarray
		if (ts!=me.serial)
			temparray.append(ts);
		endif
	endforeach
	if (temparray.size()==0)
		EraseObjProperty(me.master, "tamed_serials");
	else
		SetObjProperty(me.master, "tamed_serials", temparray);
	endif
	

  // Temp by seravy to clean ALL serials on the person as animals are still getting stuck on it somewhere.
  var tamedserials := GetObjProperty(me.master, "tamed_serials");
  if (tamedserials)
    var newtamedserials := {};
    foreach serial in tamedserials
      if (SystemFindObjectBySerial(serial))
        newtamedserials.append(serial);
      endif
    endforeach
    if (newtamedserials.size() == 0)
      EraseObjProperty(me.master, "tamed_serials");
    else
      SetObjProperty(me.master, "tamed_serials", newtamedserials);
    endif
  endif

endfunction

function RemoveFromTamedArray(masterserial, myserial)
	if (!masterserial || !myserial)
		return;
	endif
	var master:= SystemFindObjectBySerial(masterserial, SYSFIND_SEARCH_OFFLINE_MOBILES );
	if (!master)
		return;
	endif
	var tamedarray:= GetObjProperty(master, "tamed_serials");
	if (!tamedarray)
		return 0;
	endif
	var temparray:=array;
	foreach ts in tamedarray
		if (ts!=myserial)
			temparray.append(ts);
		endif
	endforeach
	if (temparray.size()==0)
		EraseObjProperty(master, "tamed_serials");
	else
		SetObjProperty(master, "tamed_serials", temparray);
	endif
	
endfunction

function AlreadyInMastersTamedArray()
	var tamedarray:= GetObjProperty(me.master, "tamed_serials");
	if (!tamedarray)
		return 0;
	endif
	foreach ts in tamedarray
		if (ts==me.serial)
			return 1;
		endif
	endforeach
	return 0;
	
endfunction

function CheckIfMasterUnderMaxTamed()
	//if monster is 'tamed'..i.e. not totem, summoned or spellbound
	//check whether the master already has max tamed serials
	//if so fail and release the monster
	if (len(GetObjProperty(me.master, "tamed_serials"))>=MAX_TAMED_SERIALS)
		return 1;
	endif
	return 0;
endfunction

function IsASkillTamedOrSpellboundMonster()
	if( GetObjProperty( me, "summoned" ) ||
	    GetObjProperty( me, "totem" ) ||
	    GetObjProperty( me, "specialsteed" ))
		return 0;  
	endif
	return 1;
endfunction

function WasASkillTamedOrSpellboundMonster(corpse)
	if( GetObjProperty( corpse, "summoned" ) ||
	    GetObjProperty( corpse, "totem" ) ||
	    GetObjProperty( corpse, "charmed" ) ||
	    GetObjProperty( corpse, "specialsteed" ))
		return 0;  
	endif
	return 1;
endfunction	