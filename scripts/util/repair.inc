use uo;

include "include/client";
//include "include/attributes";
include "include/classes";
include "include/itemutil";

function repair_item( character, item, skillid )
     
	var damaged 	:= CInt( item.maxhp - item.hp );
	var skill 	:= GetEffectiveSkill( character, skillid );
	var armslore 	:= GetEffectiveSkill( character, SKILLID_ARMSLORE );
	var diff 	:= CInt( damaged + 20 );
	
	// Begin changes to make repairing easier.
	if (GetObjProperty(character, CLASSEID_CRAFTER))
		diff := CInt(diff - GetObjProperty(character, CLASSEID_CRAFTER)* CRAFTER_REPAIR_DIFF_BONUS);
	endif
	if (diff < 0)
		diff := 0;
	elseif (diff > 160)
		diff := 160;
	endif
	// End

	armslore := CInt( armslore * ClasseBonus( character, CLASSEID_CRAFTER ) );

	//The item should be broken already
	if((item.hp < 1) || (item.hp > item.maxhp ) || (item.quality <= 0.1) || (!item.quality))
		SendSysMessage(character, "The brittle material breaks when you handle it!");
		DestroyTheItem( character, item );
		return;
	endif

	//Check if the item is damaged
	if( item.hp == item.maxhp )
		SendSysMessage( character, "That item is in perfect condition already." );
		return;
	endif

	//Check if you have a chance to repair it
	if( armslore >= damaged )

		//Check if he repair it
		if( CheckSkill( character, skillid, diff, 0 ) )

			//Check for a special effect
			var chance := Random(100);				
		
			//Bad luck, the item get's weaker...
			if( chance < 5 )
				var maxhp_mod := item.maxhp_mod - CInt((item.maxhp - item.maxhp_mod) / 20); // reduce the maxhp by 5% of the original max
				if( (-maxhp_mod) > (item.maxhp - item.maxhp_mod) ) // if the maxhp falls below 1, set it to 1.
					item.maxhp_mod := 1 - item.maxhp;
				else
					item.maxhp_mod := maxhp_mod;
				endif
				item.hp := item.maxhp;
				
				SendSysMessage( character, "You completely repair the item, but your careless work leaves it a little weaker." );
				return;
			
			//Good luck, the item's quality improve
			elseif( chance == 99 )
				SendSysMessage( character, "Your chance and skill improved the quality of the item!" );
				item.quality := CDbl(item.quality + 0.05);
				item.hp := item.maxhp;
				return;
		
			//You repair the item
			else
				var amount := CInt((Random(armslore)+1) / 2 );
				if( amount >= damaged )
					item.hp := item.maxhp;
					SendSysMessage(character, "You repair the item completely.");
				else
					item.hp := item.hp + amount;
					SendSysMessage(character, "You successfully repair some of the damage");
				endif
				return;
			endif
	
		//You fail to repair it	
		else
			var dmg := CInt(item.hp / 2) + RandomInt( CInt(item.hp / 2) + 1 );
			if( dmg < 1 )
				dmg := 1;
			endif
			
			if( dmg >= item.hp )
				SendSysMessage( character, "You fail and destroy the item" );
				DestroyTheItem( character, item );
			else
				SendSysMessage( character, "You fail in your repairing attempt and damage the item!" );
				item.hp := item.hp - dmg;
			endif
			return;
		endif

	//You don't even have a chance to repair it	
	else	
		SendSysMessage( character, "You don't even know where to start!" );
		return;
	endif

endfunction