function TempDisableDoorTrap(door)
	SetObjProperty(door, "#trapnextactive", CInt(ReadGameClock())+60);
	EraseObjProperty(door, "trapped");
endfunction

function ReenableDoorTrap(door)
	var traptype	:= GetObjProperty(door,"trap_type");
	if (! traptype)
		SetObjProperty(door, "trap_type",Random(3+1));
	endif	
endfunction

function DoorTrapReady(door)
	var trapnextactive := CInt(GetObjProperty(door, "#trapnextactive"));
	if (ReadGameClock()>trapnextactive)
		return 1;
	endif
	return 0;
endfunction

function FireTrapIfPresent(character,door)
	var traptype	:= GetObjProperty(door,"trap_type");
	if (traptype && DoorTrapReady(door))
		ReenableDoorTrap(door);
		var trapstrength	:= GetObjProperty(door,"trap_strength");
		if (!trapstrength)
			trapstrength := 4;
		endif
	
		case( traptype )
			1 :   	PlaySoundEffect( character, SFX_225 );
					var dmg := (Random(15)+1) * CInt(trapstrength) * 2;
					SendSysMessage( character, "You set off a needle trap!");
					ApplyDamage( character, dmg );
			2 :		PlaySoundEffect(character, SFX_206);
					PlayObjectCenteredEffect( character, FX_CURSE_EFFECT, 10,10);
					SetPoison( character, CInt(trapstrength), "You have been poisoned!" );
			3 :		SendSysMessage(character, "You set off a trap!");
					var dmg := (Random(15)+1) * CInt(trapstrength) * 2;
					ApplyDamage( character, dmg );
					PlayObjectCenteredEffect( character, FX_EXPLODE_3, 10,10);
					PlaySoundEffect(character, SFX_208);
					ApplyElementalDamageNoResist( character, character, dmg, FIRE );
			default:	SendSysMessage( character, "You narrowly avoid the trap!" );
		endcase

		TempDisableDoorTrap(door);
	endif
endfunction
