function StaffIsEquipped(me, staff) 

   var weaponone := GetEquipmentByLayer( me,  LAYER_HAND1 ); 
   var weapontwo := GetEquipmentByLayer( me,  LAYER_HAND2 ); 

   if ( ( weaponone == staff ) or (weapontwo == staff ) ) 
      return 1; 
   else 
      return 0; 
   endif 

endfunction 

function FindSpellIdForObjType(objtype)
	case (objtype) //Conversion
          0x1f2d: return 7;
          0x1f2e: return 1;
          0x1f2f: return 2;
          0x1f30: return 3;
          0x1f31: return 4;
          0x1f32: return 5;
          0x1f33: return 6;
          0x1f34: return 8;
          0x1f35: return 9;
          0x1f36: return 10;
          0x1f37: return 11;
          0x1f38: return 12;
          0x1f39: return 13;
          0x1f3a: return 14;
          0x1f3b: return 15;
          0x1f3c: return 16;
          0x1f3d: return 17;
          0x1f3e: return 18;
          0x1f3f: return 19;
          0x1f40: return 20;
          0x1f41: return 21;
          0x1f42: return 22;
          0x1f43: return 23;
          0x1f44: return 24;
          0x1f45: return 25;
          0x1f46: return 26;          
			 0x1f47: return 27;
          0x1f48: return 28;
          0x1f49: return 29;
          0x1f4a: return 30;
          0x1f4b: return 31;
          0x1f4c: return 32;
          0x1f4d: return 33;
          0x1f4e: return 34;
          0x1f4f: return 35;
          0x1f50: return 36;
          0x1f51: return 37;
          0x1f52: return 38;
          0x1f53: return 39;
          0x1f54: return 40;
          0x1f55: return 41;
          0x1f56: return 42;
          0x1f57: return 43;
          0x1f58: return 44;
          0x1f59: return 45;
          0x1f5a: return 46;
          0x1f5b: return 47;
          0x1f5c: return 48;
          0x1f5d: return 49;
          0x1f5e: return 50;
          0x1f5f: return 51;
          0x1f60: return 52;
          0x1f61: return 53;
          0x1f62: return 54;
          0x1f63: return 55;
          0x1f64: return 56;
          0x1f65: return 57;
          0x1f66: return 58;
          0x1f67: return 59;
          0x1f68: return 60;
          0x1f69: return 61;
          0x1f6a: return 62;
          0x1f6b: return 63;
          0x1f6c: return 64;
          0xA101: return 65;
          0xA102: return 66;
          0xA103: return 67;
          0xA104: return 68;
          0xA105: return 69;
          0xA106: return 70;
          0xA107: return 71;
          0xA108: return 72;
          0xA109: return 73;
          0xA10A: return 74;
          0xA10B: return 75;
          0xA10C: return 76;
          0xA10D: return 77;
          0xA10E: return 78;
          0xA10F: return 79;
          0xA110: return 80;
          0xB101: return 81;
          0xB102: return 82;
			 0xB103: return 83;
          0xB104: return 84;
          0xB105: return 85;
          0xB106: return 86;
          0xB107: return 87;
          0xB108: return 88;
          0xB109: return 89;
          0xB10A: return 90;
          0xB10B: return 91;
          0xB10C: return 92;
          0xB10D: return 93;
          0xB10E: return 94;
          0xB10F: return 95;
          0xB110: return 96;
			 default: return 0;
     endcase
endfunction

function GetSpellNameFromId(spellid)
		case (spellid) //Conversion
                7: return "Reactive Armor";
                1: return "Clumsy";
                2: return "Create Food";
                3: return "Feeblemind";
                4: return "Heal";
                5: return "Magic Arrow";
                6: return "Night Sight";
                8: return "Weaken";
                9: return "Agility";
                10: return "Cunning";
                11: return "Cure";
                12: return "Harm";
                13: return "Magic Trap";
                14: return "Magic Untrap";
                15: return "Protection";
                16: return "Strength";
                17: return "Bless";
                18: return "Fireball";
                19: return "Magic Lock";
                20: return "Poison";
                21: return "Telekinesis";
                22: return "Teleport";
                23: return "Unlock";
                24: return "Wall of Stone";                
					 25: return "Arch Cure";
                26: return "Arch Protection";
                27: return "Curse";
                28: return "Fire Field";
                29: return "Greater Heal";
                30: return "Lightning";
                31: return "Mana Drain";
                32: return "Recall";
                33: return "Blade Spirit";
                34: return "Dispel Field";
                35: return "Incognito";
                36: return "Magic Reflection";
                37: return "Mind Blast";
                38: return "Paralyze";
                39: return "Poison Field";
                40: return "SummonCreature";
                41: return "Dispel";
                42: return "Energy Bolt";
                43: return "Explosion";
                44: return "Invisibility";
                45: return "Mark";
                46: return "Mass Curse";
                47: return "Paralyze Field";
                48: return "Reveal";
                49: return "Chain Lightning";
                50: return "Energy Field";
                51: return "Flamestrike";
                52: return "Gate";
                53: return "ManaVampire";
                54: return "Mass Dispel";
                55: return "Meteor Swarm";
                56: return "Polymorph";
                57: return "Earthquake";
                58: return "EnergyVortex";
                59: return "Resurrection";
                60: return "Summon Air Elemental";
                61: return "Summon Daemon";
                62: return "Summon Earth Elemental";
                63: return "Summon Fire Elemental";
                64: return "Summon Water Elemental";
                65: return "Control Undead";
                66: return "Darkness";
                67: return "Decaying Ray";
                68: return "Spectre's Touch";
                69: return "Abyssal Flame";
                70: return "Animate Dead";
                71: return "Sacrifice";
                72: return "Wraith's Breath";
                73: return "Sorcerer's Bane";
                74: return "Summon Spirit";
                75: return "Wraithform";
                76: return "Wyvern Strike";
                77: return "Kill";
                78: return "Liche";
                79: return "Plague";
                80: return "Spellbind";
                81: return "Antidote";
					 82: return "Owl Sight";
                83: return "Shifting Earth";
                84: return "Summon Mammals";
                85: return "Call Lightning";
                86: return "Earth Blessing";
                87: return "Earth Portal";
                88: return "Nature's Touch";
                89: return "Gust of Air";
                90: return "Rising Fire";
                91: return "Shapeshift";
                92: return "Ice Strike";
                93: return "Earth Spirit";
                94: return "Flame Spirit";
                95: return "Storm Spirit";
                96: return "Water Spirit";
					 default: return "Error";
			endcase
endfunction



//staff spellid array handling

function GetSpellIdArray(staff)
	return GetObjProperty(staff, "SpellIDArray");

endfunction

function AddToSpellIdArray(staff, spellid)
	var spellarr := GetSpellIdArray(staff);
	if (!spellarr || spellarr==error)
		spellarr:=array;
	endif
	spellarr.append(spellid);
	SetObjProperty(staff, "SpellIDArray", spellarr);
endfunction

function RemoveFromSpellIdArray(staff, spellid)
	var spellarr := GetSpellIdArray(staff);
	var temparr := array;
	var found:=0;
	if (!spellarr || spellarr==error)
		return;
	endif
	var i;
	for (i:=1; i<=len(spellarr);i:=i+1)
		if (!found)
			if (spellarr[i]!=spellid)
				temparr.append(spellarr[i]);
			else
				found:=1; //so it only removes one
			endif
		else
			//found a spellid of that type so all others are just copied
			temparr.append(spellarr[i]);
		endif
	endfor
	
	SetObjProperty(staff, "SpellIDArray", temparr);
endfunction

function IsInSpellIdArray(staff, spellid)
	var spellarr := GetSpellIdArray(staff);
	if (!spellarr || spellarr==error)
		return;
	endif
	foreach si in spellarr
		if (si==spellid)
			return 1;
		endif
	endforeach
	
	return 0;
endfunction
