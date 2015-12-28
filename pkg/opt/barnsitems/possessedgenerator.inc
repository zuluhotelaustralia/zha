include "ai/main/npcinfo"; 

function CleanupPossession(possessedgenerator)
	var myperson := GetPersonPossessed(possessedgenerator);
	if (myperson)
		EraseObjProperty(myperson, "PossessedByEvil");
	endif
endfunction

function FindPersonToPossess(possessedgenerator)
	var posRange := CInt(GetObjProperty(possessedgenerator, "PossessionRange"));
	if (!posRange||posRange==error)
		return;
	endif
	var localmobs := ListMobilesNearLocation( possessedgenerator.x, possessedgenerator.y, possessedgenerator.z, posRange );
	var goodhumans := array;
	foreach m in localmobs
		if (m.npctemplate && GetObjProperty(m, "Type")=="Human" && (IsGood(m) || m.script == "merchant") && !m.master && m.script != "playermerchant")
			//Broadcast("looking for a good human " + m.name);
			goodhumans.append(m);
		endif
	endforeach
	//Broadcast("Goodhumans! " + goodhumans);
	//Broadcast("Length! " + len(goodhumans));
	var posnum := Random(len(goodhumans))+1;
	//Broadcast("Rand num! " + posnum);
	//Broadcast(goodhumans[posnum].name + " possessed!");
	SetObjProperty(goodhumans[posnum], "PossessedByEvil", CInt(possessedgenerator.serial));
	SetObjProperty(possessedgenerator, "TownpersonSerial", goodhumans[posnum].serial);
endfunction

function GetPersonPossessed(possessedgenerator)
	var personser := CInt(GetObjProperty(possessedgenerator, "TownpersonSerial"));
	if (!personser || personser == error)
		return 0;
	endif
	var person := SystemFindObjectBySerial(possessedgenerator);
	if (!person||person==error)
		return 0;	
	endif
	return person;
endfunction

function IsPersonPossessed(person)
	if (GetObjProperty(person, "PossessedByEvil"))
		return 1;
	endif
	return 0;
endfunction

function DetectPossessedPerson(who)
//who is person using symbolofamberyl
	var karma := CInt( GetObjProperty( who, "Karma" ) );
	var range :=3;
	if (karma >2000)
		range := range + CInt(karma/2000);
	endif
	
	var localmobs := ListMobilesNearLocation( who.x, who.y, who.z, range );
	foreach m in localmobs
		if (IsPersonPossessed(m))
			UnleashEvil(m);
		endif
	endforeach
endfunction

function UnleashEvil(person)
	PrintTextAbove(person, "Argh! The pain!");
	PerformAction(person, 0x001f);
	PlaySoundEffect(person, 0x25c);
	sleep(1);
	PerformAction(person, 0x001f);
	sleep(1);
	PerformAction(person, 0x001f);
	sleep(1);
	PerformAction(person, 0x001f);
	sleep(1);
	var monster := CreateNpcFromTemplate( "shadeofzanroth", person.x, person.y, person.z);
	SetObjProperty(monster, "PossessionGenerator", CInt(GetObjProperty(person, "PossessedByEvil")));
	PrintTextAbove(monster, "Ha ha ha! You've found me you pitiful little human!", 3, 38);
	PrintTextAbove(monster, "I will have my vengeance on thee for the disturbance!", 3, 38);
	sleep(1);
	EraseObjProperty(person, "PossessedByEvil");
	PlaySoundEffect(monster, 0x024f);
endfunction