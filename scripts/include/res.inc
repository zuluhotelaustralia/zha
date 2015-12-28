use uo;
use os;
use basic;

include "../../pkg/opt/karmafame/karmafame";
include "include/attributes";

const MAX_LOSS_PERCENT	:= 0.5;

function ResurrectMobile( who, by_who := 0 )

//Harl - added if statement as it was resurrect was still res'ing 
//dead people and i cant figure out why in the spell itself.
if (who.dead)
	Resurrect( who );
	FixColor( who );
	SetStamina(who, Cint(GetMaxStamina(who) / 10));
	SetHp(who, Cint(GetMaxHp(who) / 10));
	SetMana(who, Cint(GetMaxMana(who) / 10));
	//MurdererStatLoss( who );

	//more thing to do on res
	
	CheckResRobe(who);
else
	return;
endif

endfunction


function FixColor( who )

   var newcolor, racepicked := who.title_race;

   if (racepicked=="Elf")
	newcolor := 770;
   elseif (racepicked=="Dark-Elf")
	newcolor := 33877;
   elseif (racepicked=="Goblin")
	newcolor := 34186;
   elseif (racepicked=="Barbarian")
	newcolor := 33804;
   elseif (racepicked=="Dwarf")
	newcolor := 33888;
   else
	newcolor := who.truecolor;
   endif

   who.color := newcolor;

endfunction


function MurdererStatLoss( who )

	var shortc := GetObjProperty( who, "shortmurders" );
	if(!shortc)
		return;
	endif

	if(shortc>4) //then murderer]
		var multiplier := CInt(shortc)/100.0;
		if(multiplier > MAX_LOSS_PERCENT)
			multiplier := MAX_LOSS_PERCENT;
		endif
		multiplier := 1.0 - multiplier;

		var str := GetStrength(who) - GetStrengthMod(who);
		var dex := GetDexterity(who) - GetDexterityMod(who);
		var inte := GetIntelligence(who) - GetIntelligenceMod(who);

		str := CInt(str * multiplier);
		dex := CInt(dex * multiplier);
		inte := CInt(inte * multiplier);

		SetBaseStrength(who,str);
		SetBaseDexterity(who,dex);
		SetBaseIntelligence(who,inte);

		AwardFame( who, -250 );
		SendSysMessage( who, "Thy body pays the price for your sins." );
		if( shortc == 5 )
			EraseObjProperty( who, "shortmurders" );
		else
			SetObjProperty( who, "shortmurders", shortc - 5 );
		endif
	endif

endfunction

function CheckResRobe(who)
	if (who.multi)
		var robe := GetEquipmentByLayer(who, LAYER_ROBE);
		if ((robe.objtype == 0x1F03) && !robe.newbie)
			DestroyItem(robe);
		endif
	endif
endfunction