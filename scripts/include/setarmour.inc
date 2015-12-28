const SET_POISONPLATE   := 1;
const SET_POISONLEATHER := 2;
const SET_FIREPLATE     := 3;
const SET_FIRELEATHER   := 4;
const SET_EARTHPLATE    := 5;
const SET_EARTHLEATHER  := 6;
const SET_WATERPLATE    := 7;
const SET_WATERLEATHER  := 8;
const SET_SHADOWPLATE   := 9;
const SET_SHADOWLEATHER := 10;
const SET_AIRPLATE      := 11;
const SET_AIRLEATHER    := 12;

var sets :=
{
	{ // SET_POISONPLATE
	 0xF141,
	 0xF142,
	 0xF143,
	 0xF144,
	 0xF145,
	 0xF146
	},

	{ // SET_POISONLEATHER
	 0xF147,
	 0xF148,
	 0xF149,
	 0xF150,
	 0xF151,
	 0xF152
	},

	{ // SET_FIREPLATE
	 0xF097,
	 0xF098,
	 0xF099,
	 0xF100,
	 0xF101,
	 0xF102
	},
	
	{ // SET_FIRELEATHER
	 0xF103,
	 0xF104,
	 0xF105,
	 0xF106,
	 0xF107,
	 0xF108
	},

	{ // SET_EARTHPLATE
	 0xF042,
	 0xF043,
	 0xF044,
	 0xF045,
	 0xF046,
	 0xF047
	},
	
	{ // SET_EARTHLEATHER
	 0xF049,
	 0xF050,
	 0xF051,
	 0xF052,
	 0xF053,
	 0xF054
	},
	
	{ // SET_WATERPLATE
	 0x7591,
	 0x7592,
	 0x7593,
	 0x7594,
	 0x7595,
	 0x7596
	},
	
	{ // SET_WATERLEATHER
	 0x7597,
	 0x7598,
	 0x7599,
	 0x759A,
	 0x759B,
	 0x75a0
	},
	
	{ // SET_SHADOWPLATE
	 0xF119,
	 0xF120,
	 0xF121,
	 0xF122,
	 0xF123,
	 0xF124
	},
	
	{ // SET_SHADOWLEATHER
	 0xF125,
	 0xF126,
	 0xF127,
	 0xF128,
	 0xF129,
	 0xF130
	},
	
	{ // SET_AIRPLATE
	 0xF075,
	 0xF076,
	 0xF077,
	 0xF078,
	 0xF079,
	 0xF080
	},
	
	{ // SET_AIRLEATHER
	 0xF081,
	 0xF082,
	 0xF083,
	 0xF084,
	 0xF085,
	 0xF086
	}
};

function isSetPiece(item)
	var numSet;
	for (numSet := 1; numSet <= sets.size(); numSet := numSet + 1)
		if (item.objtype in sets[numSet])
			return numSet;
		endif
	endfor
	
	return 0;
endfunction

function DoSetEquipment(who, item)
	// Get the set number. If it's not in a set, return.
	var numSet := isSetPiece(item);
	if (!numSet)
		return;
	endif
	
	// Count the number of pieces. If it's not a complete set, return.
	var numEquipped := 1; // Start at 1, because we have to cound the item we're equipping.
	foreach it in ListEquippedItems(who)
		if (it.objtype in sets[numSet])
			numEquipped := numEquipped + 1;
		endif
	endforeach
	if (numEquipped != sets[numSet].size())
		return;
	endif
	
	case (numSet)
		SET_POISONPLATE:
		SET_POISONLEATHER:	SetObjProperty(item, "PermPoisonImmunity", 2);
					break;
		SET_FIREPLATE:
		SET_FIRELEATHER:	SetObjProperty(item, "FireProtection", 2);
					break;
		SET_EARTHPLATE:
		SET_EARTHLEATHER:	SetObjProperty(item, "EarthProtection", 2);
					break;
		SET_WATERPLATE:
		SET_WATERLEATHER:	SetObjProperty(item, "WaterProtection", 2);
					break;
		SET_SHADOWPLATE:
		SET_SHADOWLEATHER:	SetObjProperty(item, "ShadowProtection", 2);
					break;
		SET_AIRPLATE:
		SET_AIRLEATHER:		SetObjProperty(item, "AirProtection", 2);
					break;
	endcase
endfunction

function UndoSetEquipment(who, item)
	// Get the set number. If it's not in a set, return.
	var numSet := isSetPiece(item);
	if (!numSet)
		return;
	endif
	
	// Fix the prots up on the individual pieces.
	foreach it in ListEquippedItems(who)
		if (it.objtype in sets[numSet])
			case (numSet)
				SET_POISONPLATE:
				SET_POISONLEATHER:	SetObjProperty(it, "PermPoisonImmunity", 1);
							break;
				SET_FIREPLATE:
				SET_FIRELEATHER:	SetObjProperty(it, "FireProtection", 1);
							break;
				SET_EARTHPLATE:
				SET_EARTHLEATHER:	SetObjProperty(it, "EarthProtection", 1);
							break;
				SET_WATERPLATE:
				SET_WATERLEATHER:	SetObjProperty(it, "WaterProtection", 1);
							break;
				SET_SHADOWPLATE:
				SET_SHADOWLEATHER:	SetObjProperty(it, "ShadowProtection", 1);
							break;
				SET_AIRPLATE:
				SET_AIRLEATHER:		SetObjProperty(it, "AirProtection", 1);
							break;
			endcase
		endif
	endforeach

	case (numSet)
		SET_POISONPLATE:
		SET_POISONLEATHER:	fixImmunity(who, "PermPoisonImmunity", {item.serial});
					break;
		SET_FIREPLATE:
		SET_FIRELEATHER:	fixImmunity(who, "FireProtection", {item.serial});
					break;
		SET_EARTHPLATE:
		SET_EARTHLEATHER:	fixImmunity(who, "EarthProtection", {item.serial});
					break;
		SET_WATERPLATE:
		SET_WATERLEATHER:	fixImmunity(who, "WaterProtection", {item.serial});
					break;
		SET_SHADOWPLATE:
		SET_SHADOWLEATHER:	fixImmunity(who, "ShadowProtection", {item.serial});
					break;
		SET_AIRPLATE:
		SET_AIRLEATHER:		fixImmunity(who, "AirProtection", {item.serial});
					break;
	endcase
endfunction

function fixImmunity(who, immunity, itemsToIgnore)
	var val := 0, nextval;
	foreach item in ListEquippedItems(who)
		if (item.serial in itemsToIgnore)
			continue;
		endif
		
		nextval := GetObjProperty(item, immunity);
		if (nextval)
			if ((nextval > val) || ((val<=0) && (nextval < val)) )
				val := nextval;
			endif
		endif
	endforeach
	
	if (val)
		SetObjProperty(who, immunity, val);
	else
		EraseObjProperty(who, immunity);
	endif
endfunction