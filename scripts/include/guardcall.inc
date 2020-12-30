include "include/areas";

function CallGuardOnNPC(npc, unused caller)
//used by merchants
	if (!npc.npctemplate)
		return;
	endif
	if (GetObjProperty(npc, "guardignore"))
		return;
	endif

	set_critical(1);
	var guard := CreateNpcFromTemplate( "guard", npc.x, npc.y, npc.z );
	if( guard )
		SetObjProperty( guard, "target", npc.serial );
		PrintTextAbove(guard, "Be gone interloper!!", 3, 53);
	endif
	set_critical(0);
	return 1;

endfunction

function AutoCallGuardCheck(attacker, defender, melee_mode:=0)
	//modified from pkg/opt/areas/callguards.src
	//returns 1 if attack is by a crim in a guarded area and summons guard onto them
	//otherwise 0
	if (getglobalproperty( "autoguardsoff") == 1)
		return 0;
	endif
	if(attacker.IsA(POLCLASS_NPC) || defender.IsA(POLCLASS_NPC))
		return 0;
	endif

	if(! IsInGuardedArea(attacker) )
		return 0;
	endif

	var guard;
	/*if (!defender && attacker.acct) //no defender and attacker is player
			set_critical(1);
			guard := CreateNpcFromTemplate( "guard", attacker.x, attacker.y, attacker.z );
			if( guard )
					SetObjProperty( guard, "target", attacker.serial );
					PrintTextAbove(guard, attacker.name + ", thou shalt pay for thy crime!",3,53);
			endif
			SendSysMessage(attacker, "The town guard intervenes during your criminal action!");
			set_critical(0);
			return 1;
	endif
	*/

	var npc_cfg := ReadConfigFile(":*:npcdesc");
	var elem;

	if (defender.criminal)
		return 0;
	endif

	//guild check
	var guild1id := GetObjProperty(attacker, "guild_id");
	var guild2id := GetObjProperty(defender, "guild_id");
	if (guild1id!=error && guild2id!=error)
		if (guild1id==guild2id)
			return 0;
		endif
	endif

	//allied guilds check
	var guild1 := FindGuild(guild1id);
	var guild2 := FindGuild(guild2id);
	if (guild1.isallyguild(guild2))
		return 0;
	endif

	if( attacker.IsA(POLCLASS_NPC) )
		elem := FindConfigElem( npc_cfg, attacker.npctemplate );
		if( (attacker.script=="tamed" && attacker.master.criminal)
		|| (attacker.script!="tamed" && !elem.guardignore && !GetObjProperty(attacker,"guardignore")))
			set_critical(1);
			guard := CreateNpcFromTemplate( "guard", attacker.x, attacker.y, attacker.z );
			if( guard )
				SetObjProperty( guard, "target", attacker.serial );
			endif
			set_critical(0);
			return 1;
		endif
	elseif( !attacker.cmdlevel )
		if( attacker.criminal )
			set_critical(1);
			if (melee_mode)
				//to prevent crims that have been attacked recently from being auto-guarded
				if (!InAttackersArray(defender, attacker.serial)) //means that attacker didnt attack first
					return 0;
				endif
			endif

			guard := CreateNpcFromTemplate( "guard", attacker.x, attacker.y, attacker.z );
			if( guard )
					SetObjProperty( guard, "target", attacker.serial );
					PrintTextAbove(guard, attacker.name + ", thou shalt pay for thy crime!",3,53);
			endif
			SendSysMessage(attacker, "The town guard intervenes during your criminal action!");
			SendSysMessage(defender, "The town guard appears from nowhere to defend you!");
			set_critical(0);
			return 1;
		endif
	endif
	return 0;
endfunction

function PreventedCastInGuardedArea(caster)
	//spells call this if they arent to be cast in guarded areas
	if(! IsInGuardedArea(caster) )
		return 0;
	endif
	SendSysMessage(caster, "This spell cannot be cast in a guarded area.");
	return 1;
endfunction

//ATTACK LIST ... called by POLHOOK

function InAttackersArray(defender, attacker_serial)
	//checks whether attackers serial is in list of recent player attackers
	var attacklist := GetObjProperty( defender, "PlayerAttackList" );
	if( !attacklist  )
		return 0;
	endif

	foreach apr in attacklist
		if (apr[1]==attacker_serial)
			if (CInt(ReadGameClock()-apr[2])<120)   //less than 2 min old
				return 1;
			endif
			return 0;
		endif

	endforeach

	return 0;
endfunction



function GenerateAttacklist( who, serial)
	//if its already there dont bother.
	if (InAttackersArray(who, serial))
		return;
	endif
	var attacklist := GetObjProperty( who, "PlayerAttackList" );
	if( !attacklist  )
		attacklist := array;
	endif

	if( len(attacklist ) > 5 )
		ReplaceOldestInAttacklist( attacklist, serial);
	else
		AddSerialToAttacklist( attacklist, serial);
	endif

	SetObjProperty( who, "PlayerAttackList", attacklist);

endfunction


function ReplaceOldestInAttacklist( byref attacklist, serial )

	var min_timer	:= ReadGameClock();
	var min_pos	:= 0;
	var current;

	for( current := 1; current <= len(attacklist); current := current + 1 )
		if( attacklist[current][1] == serial )
			attacklist[current][2] := ReadGameClock();

			return attacklist;

		elseif( attacklist[current][2] < min_timer )
			min_timer := attacklist[current][2];
			min_pos   := current;
		endif
	endfor

	if( min_pos )
		attacklist[min_pos][1]	:= serial;
		attacklist[min_pos][2]	:= ReadGameClock();
	endif

	return 1;

endfunction


function AddSerialToAttacklist( byref attacklist, serial)
	foreach arr in attacklist
		if( arr[1] == serial )
			arr[2] := ReadGameClock();

			return 1;
		endif
	endforeach

	var add	:= { serial, ReadGameClock() };
	return attacklist.append(add);

endfunction
