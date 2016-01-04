//Barnabus 23/3/2003 - functions for newbie protection scripts

use uo;
include "include/constants/propids";
include "include/attributes";
include "include/classes";

function IsNonNewbieAttackingNewbie(byref who, byref by_who)
	//checks whether damage is done by a newbie to a non-newbie, or viceversa
	Broadcast("CHECKING IF NEWBIE ATTACKING NON-NEWBIE");
	var who_newbie := (GetObjProperty(who, "newbie")==1);
	if( (!who.IsA(POLCLASS_NPC)) && (! by_who.IsA(POLCLASS_NPC))) //both are players
		var who_newbie := (GetObjProperty(who, "newbie")==1);
		var by_who_newbie := (GetObjProperty(by_who, "newbie")==1);
		Broadcast("who newbie : " + who_newbie);
		Broadcast("by who newbie : " + by_who_newbie);
		if ((who_newbie&&!by_who_newbie)||(!who_newbie&&by_who_newbie))
			Broadcast("attacking a newbie!");
			return 1;
		endif
	endif
	
	//checks for tamed or summoned masters newbie status
	if( (!who.IsA(POLCLASS_NPC)) || (! by_who.IsA(POLCLASS_NPC))) //one is a player
		var who_summoned := FindMobileMaster(who);
		if (who_summoned)
			if ((who_summoned.newbie==1&&by_who.newbie!=1)||(who_summoned.newbie!=1&&by_who.newbie==1))
				Broadcast("attacking a newbie!");
				return 1;
			endif
		endif

		var by_who_summoned := FindMobileMaster(by_who);
		if (by_who_summoned)
			if ((who.newbie==1&&by_who_summoned.newbie!=1)||(who.newbie!=1&&by_who_summoned.newbie==1))
				Broadcast("attacking a newbie!");
				return 1;
			endif
		endif
	endif
	return 0;
endfunction 

function EitherIsNewbie(who, by_who)
	if (!who.acct||!by_who.acct)
		return 0; //only protects against player vs player combat
	endif
	var who_newbie := (GetObjProperty(who, "newbie")==1);
	if (who_newbie)
		return 1;
	endif
	var by_who_newbie := (GetObjProperty(by_who, "newbie")==1);
	if (by_who_newbie)
		return 1;
	endif
	return 0;	
endfunction


function FindMobileMaster( byref mobile )

	var masterserial := CInt( GetObjProperty( mobile, PROPID_MOBILE_MASTER_SERIAL ) );
	if( !masterserial )
		return 0;
	endif

	var master := SystemFindObjectBySerial( masterserial, SYSFIND_SEARCH_OFFLINE_MOBILES );
	return master;

endfunction

function DetermineNewbie(player)
	//checks if still newbie... players no longer newbie cannot become newbie again...
	var res:=0;
	if (GetObjProperty(player, "newbie")==1)//do following check if player is newbie
		res:=SetNewbieStatus(player);
	endif
	return res; //res is 1 if player was newbie but then changed not to newbie
endfunction

function SetNewbieStatus(player)
	const STATCAP := 95;
	
	if (player.cmdlevel>0) 
		return 0; 
	endif
	var skilltotal:=GetSkillTotal(player);
	var unmod_str := GetStrength(player) - GetStrengthMod(player);
	var unmod_dex := GetDexterity(player) - GetDexterityMod(player);
	var unmod_int := GetIntelligence(player) - GetIntelligenceMod(player);

	//update newbie status based on time
	var gametime := GetObjProperty(player, "onlinetimer");
	//Broadcast("gametime " + gametime);
	//Broadcast("str: " + unmod_str);
	//Broadcast("dex: " + unmod_dex);
	//Broadcast("int: " + unmod_int);

	if (((!gametime || gametime < 100000)) 
		&& ((unmod_str < STATCAP) && (unmod_dex < STATCAP) && (unmod_int < STATCAP))
		&& skilltotal < 760)
		MakeNewbie(player);
	else
		RemoveNewbie(player);
	endif
	return (GetObjProperty(player, "newbie")!=1); //returns true if player no longer newbie ==1	
endfunction

function MakeNewbie(player)
//sets all props to newbie status
	SetObjProperty(player,"newbie", 1);
	AddYoungTag(player);
endfunction

function RemoveNewbie(player)
//removes all newbie status props
	SetObjProperty(player,"newbie", 999);
	RemoveYoungTag(player);
endfunction

function AddYoungTag(player)
	//find out if they have [Young] in name at end then return
	var result:=find(player.name," [Young]",1);

	if (result)		
		return;
	endif

	//if they dont put [Young] at end of name
	player.name := player.name + " [Young]";


endfunction

function RemoveYoungTag(player)
	//find out if they dont have [Young] in name at end then return
	var result:=find(player.name," [Young]",1);

	if (!result)		
		return;
	endif

	//if they do remove [Young] from end of name
	var name:=player.name;
	if (result)
		result:=result-1;
		name:=name[1,result];
		player.name:=name;
 	endif

endfunction