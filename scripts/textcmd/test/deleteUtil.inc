//Barnabus 5/3/03
//DELETING CHARACTERS, BOATS, HOUSES INCLUDE FILE

use uo;
use os;
include "util/bank";
include "../pkg/std/boat/setup";
include "../pkg/std/housing/housemanagement";
include "include/constants/locations";

function DeleteAccountChars(account) //account object as parameter
//Deletes all characters, items and bankboxes of that account
	set_critical(1);
	var charNum, i, equipAtLayer; 
	var player;
	//Broadcast("WIPING CHARS for account: " + account.name); 
	//deletes all characters, items and bankboxes for account
	for (charNum:=1; charNum <=5; charNum := charNum+1)
		player:=account.GetCharacter(charNum);
		//MoveCharacterToLocation(player, 0, 0, 0, MOVECHAR_FORCELOCATION);
		//DisconnectClient(player);
		if (player != error)
			
			Broadcast("player " + player.name + " deleted. ");
		
			WipeBankBox(player);
			for (i := 1; i <= 24; i := i + 1)
		   		equipAtLayer := GetEquipmentByLayer(player, i);
		   		MoveItemToContainer(equipAtLayer, player.backpack);
			endfor
	       		DestroyItem(player.backpack);
	       		account.DeleteCharacter(charNum);
		endif
	endfor
	set_critical(0);
endfunction

function DeleteBoats(account) //account object as parameter
//Deletes all ships of that account
				
	set_critical(1);
	var holds := GetGlobalProperty("holds");
	
	var maxShips:=holds.size();
	
	var i:=1;
	var ship, hold, ownerSer, ownerPlayer;
	
	var hisShips:=array;
	
	//make a brit location

		
	for(i:=1;i<=maxShips;i:=i+1)
		hold:=SystemFindObjectBySerial(CInt(holds[i]));
		ownerSer := GetObjProperty(hold,"owner");
		ownerPlayer := SystemFindObjectBySerial( ownerSer, SYSFIND_SEARCH_OFFLINE_MOBILES );

		if (ownerPlayer.acct.name == account.name)
			ship := SystemFindObjectBySerial(CInt(hold.multi.serial));
			deleteboat(ship); //deletes boat, moves offline chars to brit, deletes on board items
		endif
	endfor
	set_critical(0);
endfunction

function DeleteHouses(account)

	set_critical(1);
	var signs := GetGlobalProperty("signs");
	
	var maxHouses:=signs.size(),sign;
	
	var hisHouses:=array;
	var house, acct;
	var i;
	
	
	for(i:=1;i<=maxHouses;i:=i+1)
		sign:=SystemFindObjectBySerial(CInt(signs[i]));
		house := SystemFindObjectBySerial(CInt(GetObjProperty(sign, "house_serial")));
		acct:=GetObjProperty(house, "owneracct");
		//Broadcast("owneracct: " + acct);
		
		if (acct==account.name)

			deleteHouse( house );
		endif
	endfor
	set_critical(0);
endfunction

function DeleteHousesOfAccounts(byref bannedaccs)
	//deletes all houses that have owner account listed in bannedaccs (a list of account names)

	set_critical(1);
	var signs := GetGlobalProperty("signs");
	
	var maxHouses:=signs.size(),sign;
	
	var hisHouses:=array;
	var house, acct;
	var i;
	var bannedacc;
	
	for(i:=1;i<=maxHouses;i:=i+1)
		sign:=SystemFindObjectBySerial(CInt(signs[i]));
		house := SystemFindObjectBySerial(CInt(GetObjProperty(sign, "house_serial")));
		acct:=GetObjProperty(house, "owneracct");
		//Broadcast("owneracct: " + acct);
		
		foreach bannedacc in bannedaccs
			if (acct==bannedacc)
				Broadcast("House belonging to account: " + acct + " deleted.");
				deleteHouse( house );
			endif
		endforeach
	endfor

	set_critical(0);
endfunction

function DeleteBoatsOfAccounts(byref bannedaccs)
//deletes all boats that have owner account listed in bannedaccs (list of account names)
				
	set_critical(1);
	var holds := GetGlobalProperty("holds");
	
	var maxShips:=holds.size();
	
	var i:=1;
	var ship, hold, ownerSer, ownerPlayer;
	
	var hisShips:=array;
	
	var bannedacc;
					
	for(i:=1;i<=maxShips;i:=i+1)
		hold:=SystemFindObjectBySerial(CInt(holds[i]));
		ownerSer := GetObjProperty(hold,"owner");
		ownerPlayer := SystemFindObjectBySerial( ownerSer, SYSFIND_SEARCH_OFFLINE_MOBILES );
		foreach bannedacc in bannedaccs
			if (ownerPlayer.acct.name == bannedacc)
				ship := SystemFindObjectBySerial(CInt(hold.multi.serial));
				Broadcast("Ship belonging to account: " + ownerPlayer.acct.name + " deleted.");
				deleteboat(ship); //deletes boat, moves offline chars to brit, deletes on board items
			endif
		endforeach
	endfor
	set_critical(0);
endfunction


function UpdateBannedAccountNames()
	var accobj;
	var accName;
	var tempBannedAccNames := array;
	var i:=1;

	foreach accName in ListAccounts()
        	accobj := FindAccount(accName);
			if (accobj.banned)
				tempBannedAccNames[i] := accName;
				i:=i+1;				
			endif
	endforeach
	
	SetGlobalProperty("bannedAccountNames", tempBannedAccNames);

endfunction