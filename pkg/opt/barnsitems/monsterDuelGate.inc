
function GetAllMonsterPoints(monsterDuelController)
	return GetObjProperty(monsterDuelController, "monsterPoints");
endfunction

function GetAssociatedQuestName(monsterDuelController)
	var qname:=GetQuestTitle(CInt(GetObjProperty (monsterDuelController, "AssocQuestStoneSer")));
	if (!qname || qname==error ||  qname == "To be named")
		return 0;
	endif
	return qname;
endfunction

function GetAssociatedQuestSerial(monsterDuelController)
	return GetObjProperty (monsterDuelController, "AssocQuestStoneSer");
endfunction

function GetMonsterPoint(monsterDuelController, index)
	var mps:=GetAllMonsterPoints(monsterDuelController);
	if (index<=len(mps))
		return mps[index];
	else
		return 0;
	endif
endfunction

function RunMonsterPoint(monsterDuelController, player, positionindex)
		var mp := GetMonsterPoint(monsterDuelController, positionindex);
		if(!mp)
			EndMonsterDuel(monsterDuelController, player);
			return 0;
		endif
		
		var monstlist := SplitWords(mp.npctemplate);
		var monsttemplate;
		if (len(monstlist)>1)
			monsttemplate:=monstlist[Random(len(monstlist))+1];
		else
			monsttemplate:=mp.npctemplate;
		endif
		   
		//create monster
	   var monst := CreateNpcFromTemplate( monsttemplate, mp.monsterlocation.x, mp.monsterlocation.y, mp.monsterlocation.z);
		if (!monst)
			EndMonsterDuel(monsterDuelController, player);
			return 0;
		endif
		SetObjProperty(monsterDuelController, "currentMonster", CInt(monst.serial));

		SetObjProperty(monst, "guardkill", 1);
		SetObjProperty(monst, "guardignore", 1);
		SetObjProperty(monst, "monsterDuelController", CInt(monsterDuelController.serial));
		SetObjProperty(monst, "monsterDuelControllerIndex", CInt(positionindex));
				
	   //transport
  		set_critical(1); 
  	  	SetObjProperty(player,"#moved", 1);
      
 		if (!MoveCharacterToLocation( player, mp.playerlocation.x, mp.playerlocation.y, mp.playerlocation.z, MOVECHAR_FORCELOCATION))
			//kill monst
			ApplyRawDamage(monst, CInt(GetHp(monst)+3) );
			
			EndMonsterDuel(monsterDuelController, player);
		 	return 0;
		endif

 		SetSpeedCheckNote(player);
		PlaySoundEffect(player, 0x246);
		PlayStationaryEffect( player.x, player.y, player.z, 0x372a, 0xa, 0xa );
		sleep(1);
		SendSysMessage(player, monst.name + " has come forth to challenge you!",3,53);
endfunction

function EndMonsterDuel(monsterDuelController, player)
	
	
	//send player back to duel gate

 		MoveCharacterToLocation( player, monsterDuelController.x, monsterDuelController.y+1, monsterDuelController.z, MOVECHAR_FORCELOCATION);
		SetSpeedCheckNote(player);
	
	//kill current monster (if one)
	var monstser := CInt(GetObjProperty(monsterDuelController, "currentMonster"));
	if (monstser)
		var mymonst:=SystemFindObjectBySerial(monstser);
		if (mymonst)
			mymonst.hidden:=1;
			EraseObjProperty(mymonst, "monsterDuelController");
			ApplyRawDamage(mymonst, CInt(GetHp(mymonst)+3) );
		endif
	endif
	//let ppl walk thru gate	
		EraseObjProperty(monsterDuelController, "PlayerDuelling");
		EraseObjProperty(monsterDuelController, "GateActive");
	
			
	//remove recall restriction
		//EraseObjProperty(player, "CantRecall");
		
	//remove at monsterduelprop
		
		EraseObjProperty(player, "InMonsterDuelGate");
		UnLockLinkGate(monsterDuelController);
		SendSysMessage(player, "You have left the Monster Duel Gate",3,53);
endfunction

function ReserveGate(monsterDuelController, player)
	LockLinkGate(monsterDuelController, player);
	SetObjProperty(monsterDuelController, "PlayerDuelling", CInt(player.serial));

	
	SetObjProperty(monsterDuelController, "GateActive",1);			
	// recall restriction on player
	//SetObjProperty(player, "CantRecall", 1);
		
	//monsterduelprop
	SetObjProperty(player, "InMonsterDuelGate", CInt(monsterDuelController.serial));
	
endfunction

function IncrementMonsterPoint(monsterDuelController, npctemplate, positionindex)
	var playerser:=CInt(GetObjProperty(monsterDuelController, "PlayerDuelling"));
	var mps:=GetAllMonsterPoints(monsterDuelController);
	var player;
	if (playerser)
		player := SystemFindObjectBySerial(playerser);
		if (!player)
			return;
		endif
	endif
	var assocquestser := CInt(GetAssociatedQuestSerial(monsterDuelController));
	//Broadcast(assocquestser + " " + corpse_serial);
	if (assocquestser && mps[positionindex].boolqps)
		var qps:=DetermineNPCTemplateQPs(npctemplate);
		AwardQuestPoints(assocquestser, player.serial, qps);
	endif
	positionindex:=positionindex+1;
	
	if (positionindex<=len(mps))
		RunMonsterPoint(monsterDuelController, player, positionindex);
	else
		SendSysMessage(player, "You have completed the duel!",3,53);
		EndMonsterDuel(monsterDuelController, player);
		PlaySoundEffect(player, 0x216);
	endif

endfunction

function RunBoundsChecker(moongate,who)
	//SetObjProperty(monsterDuelController, "BoundaryCoords", {loc1.x, loc1.y, loc2.x, loc2.y});
	var mgBounds:=GetObjProperty(moongate, "BoundaryCoords");
	var gateactive := GetObjProperty(moongate, "GateActive");
	while(gateactive)
		//Broadcast("checking if in box");
		if (!IsInBox( who, mgBounds.x1, mgBounds.x2, mgBounds.y1, mgBounds.y2 ))
			EndMonsterDuel(moongate, who);
			//SendSysMessage(who, "You have left the duel area",3,53);
			foreach person in EnumerateOnlineCharacters()
				if (person.cmdlevel)
					SendSysMessage(person, who.name + " left the duel gate area ... duel gate at " + moongate.x +", " + moongate.y +", " + moongate.z, 3, 38);
				endif
			endforeach
			return;
		endif
		//unhides player
		who.hidden:=0;
		
		//checks that another player hasnt broken in
		foreach mob in ListMobilesInLineOfSight( who, 30 )
			if (mob.acct && ! mob.cmdlevel)
				MoveCharacterToLocation( mob, moongate.x, moongate.y+1, moongate.z, MOVECHAR_FORCELOCATION);
				SetSpeedCheckNote(mob);
				//tell staff
				foreach person in EnumerateOnlineCharacters()
					if (person.cmdlevel)
						SendSysMessage(person, mob.name + " was an unauthorised player in monster duel gate at " + moongate.x +", " + moongate.y +", " + moongate.z, 3, 38);
					endif
				endforeach					
			elseif(mob.npctemplate && IsHostile(mob))
				//make sure monster duel gate monster fights guy in there
				var ev := struct;
				ev.+ type:= SYSEVENT_DAMAGED;
				ev.+ source:=who;
				SendEvent( mob, ev );
			endif	
		endforeach
		sleep(10);
		gateactive := GetObjProperty(moongate, "GateActive");
	endwhile
endfunction

function LockLinkGate(mdgate, player)
	var linkedgateser := Cint(GetObjProperty (mdgate, "LinkedDuelGateSer"));
	if (!linkedgateser)
		return;
	endif
	
	var linkedgate:=SystemFindObjectBySerial(linkedgateser);
	if (linkedgate)
		SetObjProperty(linkedgate, "PlayerDuelling", CInt(player.serial));
	endif
	linkedgate.invisible:=1;
endfunction

function UnLockLinkGate(mdgate)
	var linkedgateser := Cint(GetObjProperty (mdgate, "LinkedDuelGateSer"));
	if (!linkedgateser)
		return;
	endif
	
	var linkedgate:=SystemFindObjectBySerial(linkedgateser);
	if (linkedgate)
		EraseObjProperty(linkedgate, "PlayerDuelling");
	endif
	linkedgate.invisible:=0;
endfunction



