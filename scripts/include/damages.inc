use uo;
use os;
use guilds;

const DMGID_FIRE	:= 0x0001;
const DMGID_AIR		:= 0x0002;
const DMGID_EARTH	:= 0x0004;
const DMGID_WATER	:= 0x0008;
const DMGID_NECRO	:= 0x0010;
const DMGID_HOLY	:= 0x0020;
const DMGID_POISON	:= 0x0040;
const DMGID_ACID	:= 0x0080;
const DMGID_PHYSICAL	:= 0x0100;
const DMGID_MAGIC	:= 0x0200;
const DMGID_ASTRAL	:= 0x0400;
const DMGID_NO_RESIST	:= 0x0800;

include "include/constants";
//include "include/attributes";
include ":karmafame:karmafame";
include "include/mobileutil";
include "include/astralfights";
include "include/newbieprotection";
include "include/areas";
include "include/log";

const MAX_SERIAL_IN_HITLIST := 5;

//barns -- fame modifications for fame exp system
//modified so that fame gain is based on proportion of damage done to monster
//fame awards will be a bit easier to get


function ApplyTheDamage( who, by_who, dmg, attack_type := DMGID_PHYSICAL )

	dmg := CInt(dmg);
	if( !who.IsA(POLCLASS_MOBILE) )
		return 0;
	elseif( dmg <= 0 )
		return 0;
	elseif( who.dead )
		return 0;
	elseif( IsImmunedFromThisDamageType( who, dmg, attack_type ) )
		return 0;
	elseif( IsInArea( who, AREAID_CRAFTMINE ) ) //crafter
		return 0;
	elseif(IsInSafeArea(who)) // Is in safe area? (Seravy)
          if (!who.isA(POLCLASS_NPC) && !GetObjProperty(who, "#duel")) // Is player and isn't in duel?
            return 0;
          elseif (who.isA(POLCLASS_NPC) && !IsInDuelArea(who))
            return 0;
          endif
	elseif(EitherIsNewbie(who, by_who)) //newbies cant deal damage to players or receive damages from players
		return 0;
	endif
	

	//var factor := 1.0;
	var factor := CDbl(dmg)/GetMaxHp(who); //barns mod for fame-based experience system
	//Broadcast("dmg " + dmg + " GetMaxHp(who)  " + GetMaxHp(who));
	//Broadcast("after damage factor " + factor);
	if( by_who.IsA(POLCLASS_NPC) )
		var master := FindMaster(by_who);
		if( master )
			by_who := master;
			factor := factor * VALUE_MULTIPLIER_FOR_TAMED;
		endif
	endif

	if( dmg < GetHp(who) )
		factor := factor*VALUE_MULTIPLIER_FOR_ALLIES;
	else
		if( GetObjProperty(who,PROPID_MOBILE_RISE) )
			if( TryToInflictFinalDeath( who, attack_type ) )
                ProcessPlayerKill(who, by_who, dmg, attack_type);
			else
				factor := factor * VALUE_MULTIPLIER_FOR_ALLIES;
			endif
		else
            ProcessPlayerKill(who, by_who, dmg, attack_type);
		endif
	endif


	if( !by_who )
		return ApplyRawDamage( who, dmg );
	elseif( !by_who.IsA(POLCLASS_MOBILE) )
		return ApplyRawDamage( who, dmg );
	elseif( by_who == who )
		return ApplyRawDamage( who, dmg );
	endif

	SetScriptController(by_who);

	if( !by_who.IsA(POLCLASS_NPC) )
		if( !by_who.cmdlevel )
			// The defender is a npc
			if( who.IsA(POLCLASS_NPC) )
					GenerateHitlist( who, by_who.serial, factor );
			else
				var guild1 := GetObjProperty( by_who, "guild_id" );
				if( guild1 )
					var guild2 := GetObjProperty( who, "guild_id" );
					if( guild2 )
						if( guild1 == guild2 )
							return ApplyRawDamage( who, dmg );
						else
							guild1 := FindGuild( guild1 );
							guild2 := FindGuild( guild2 );
							if( guild1.IsEnemyGuild(guild2) )
								return ApplyRawDamage( who, dmg );
							elseif( guild1.IsAllyGuild(guild2) )
								return ApplyRawDamage( who, dmg );
							endif
						endif
					endif
				endif
			
				

				if ( !who.criminal)
				
					var defending;
					foreach entry in (by_who.reportables)
						if( who.serial == entry.serial )
							// The attacker is defending himself
							defending := 1;
						endif
					endforeach
				
					if( defending )
						if( (GetKarma(who) < GetKarma(by_who)) )
							// Add to hitlist only if the target have less karma
							GenerateHitlist( who, by_who.serial, factor );
						endif
					else
						// The attacker is the agressor
						GenerateHitlist( who, by_who.serial, factor );
					endif
				endif
			endif
		endif
	endif

	return ApplyRawDamage( who, dmg );

endfunction


function ApplyTheAstralDamage( who, by_who, dmg, attack_type := DMGID_ASTRAL )

	dmg := CInt(dmg);
	if( dmg <= 0 )
		return 0;
	elseif( who.dead )
		return 0;
	elseif( IsImmunedFromThisDamageType( who, dmg, attack_type ) )
		return 0;
	endif

	//var factor := 1.0;
	var factor := 1.0 * (dmg/GetMaxHp(who)); //barns mod for fame-based experience system
	var master := FindMaster(by_who);
	if( master )
		by_who := master;
		factor := factor * VALUE_MULTIPLIER_FOR_TAMED;
	endif

	if( dmg <= GetMana(who) )
		factor := factor * VALUE_MULTIPLIER_FOR_ALLIES;
		SetMana(who, GetMana(who) - dmg);
	else
		dmg := dmg - GetMana(who);
		SetMana(who, 0);
	
		if( dmg <= GetStamina(who) )
			factor := factor * VALUE_MULTIPLIER_FOR_ALLIES;
			SetStamina(who, GetStamina(who) - dmg);
		else
			if( GetObjProperty( who, PROPID_MOBILE_RISE ) )
				TryToInflictFinalDeath( who, attack_type );
			endif
	
			SetStamina(who, 0);
			SetAstralIncapacity( who );
		endif
	endif

	if( !by_who )
		return 1;
	elseif( by_who.IsA(POLCLASS_NPC) )
		return 1;
	elseif( by_who.cmdlevel )
		return 1;
	else
		if( IsSummoned(who) || IsAnimated(who) || IsCharmed(who) )
			master := FindMaster(who);
			if( master )
				GenerateHitlist( master, by_who.serial, factor );
			else
				GenerateHitlist( who, by_who.serial, factor );
			endif
		else
			GenerateHitlist( who, by_who.serial, factor );
		endif

		return 1;
	endif

endfunction


function GenerateHitlist( who, serial, factor )

	var hitlist := GetObjProperty( who, PROPID_MOBILE_HITLIST );
	if( !hitlist )
		hitlist := array;
	endif

	if( len(hitlist) > MAX_SERIAL_IN_HITLIST )
		ReplaceOldestInHitlist( hitlist, serial, factor );
	else
		AddSerialToHitlist( hitlist, serial, factor );
	endif

	SetObjProperty( who, PROPID_MOBILE_HITLIST, hitlist );

endfunction


function ReplaceOldestInHitlist( byref hitlist, serial, factor )

	var min_timer	:= ReadGameClock();
	var min_pos	:= 0;
	var current;

	for( current := 1; current <= len(hitlist); current := current + 1 )
		if( hitlist[current][1] == serial )
			hitlist[current][3] := ReadGameClock();
			if( hitlist[current][2] < factor )
				hitlist[current][2] := factor;
			endif
			return hitlist;
		
		elseif( hitlist[current][3] < min_timer )
			min_timer := hitlist[current][3];
			min_pos   := current;
		endif
	endfor

	if( min_pos )
		hitlist[min_pos][1]	:= serial;
		hitlist[min_pos][2]	:= factor;
		hitlist[min_pos][3]	:= ReadGameClock();
	endif

	return 1;

endfunction

		
function AddSerialToHitlist( byref hitlist, serial, factor )
	//barns note: looks for serial in hitlist first
	//if there just updates gameclocktime and factor
	//if not it adds it
	//Broadcast("hitlist " + hitlist);
	//Broadcast("add serial to hit list " + serial + "  " + factor);
	//modified ... factor now is dmg/GetMaxHp of monster .. so factor will equal factor = factor after each hit
	foreach arr in hitlist
		if( arr[1] == serial )
			arr[3] := ReadGameClock();
			/*
			if( arr[2] < factor )
				arr[2] := factor;
			endif
			*/
			//barns added adding up of factor 
			if (!arr[2] || arr[2]==error)
				arr[2] :=factor;
			else
				arr[2] := arr[2] + factor;
			endif
			return 1;
		endif
	endforeach

	var add	:= { serial, factor, ReadGameClock() };
	return hitlist.append(add);

endfunction



function IsImmunedFromThisDamageType( who, byref dmg, attack_type )

	var immunities := GetObjProperty( who, PROPID_MOBILE_ATTACK_TYPE_IMMUNITIES );
	if( !immunities )
		return 0;
	endif

	if( attack_type & DMGID_NO_RESIST )
		return 0;
	endif

	var attack_amount	:= 0;
	var is_immuned_amount	:= 0;     
	foreach bit in { DMGID_FIRE, DMGID_AIR, DMGID_EARTH, DMGID_WATER, DMGID_NECRO, DMGID_HOLY, DMGID_POISON, DMGID_ACID, DMGID_PHYSICAL, DMGID_MAGIC, DMGID_ASTRAL }
		if( attack_type & bit )
			attack_amount := attack_amount + 1;
			if( immunities & bit )
				is_immuned_amount := is_immuned_amount + 1;
			endif
		endif
	endforeach

	if( !attack_amount || attack_amount == is_immuned_amount )
		return 1;
	else
		dmg := CInt( dmg * CInt(attack_amount-is_immuned_amount) / attack_amount );
		return 0;
	endif

endfunction


function TryToInflictFinalDeath( who, attack_type )

	var final_death := GetObjProperty(who, PROPID_MOBILE_FINAL_DEATH);
	if( !final_death )
		return 0;
	endif

	final_death := CInt(final_death);
	foreach bit in { DMGID_FIRE, DMGID_AIR, DMGID_EARTH, DMGID_WATER, DMGID_NECRO, DMGID_HOLY, DMGID_POISON, DMGID_ACID, DMGID_PHYSICAL, DMGID_MAGIC, DMGID_ASTRAL }
		if( final_death & bit )
			if( !(attack_type & bit) )
				return 0;
			endif
		endif
	endforeach

	return 1;

endfunction


function FindMaster( byref mobile )

	var serial := CInt( GetObjProperty( mobile, PROPID_MOBILE_MASTER_SERIAL ) );
	if( !serial )
		return 0;
	endif

	return FindMobile( serial );

endfunction


function FindMobile( byref serial )

	if( !serial )
		return 0;
	endif

	var mobile := SystemFindObjectBySerial( serial );

	if( !mobile )
		mobile := SystemFindObjectBySerial( serial, SYSFIND_SEARCH_OFFLINE_MOBILES );
	endif

	return mobile;

endfunction


function IsAnimated( byref mobile )

	return GetObjProperty( mobile, PROPID_MOBILE_ANIMATED );

endfunction


function IsCharmed( byref mobile )

	return GetObjProperty( mobile, PROPID_MOBILE_CHARMED );

endfunction


function IsSummoned( byref mobile )

	return GetObjProperty( mobile, PROPID_MOBILE_SUMMONED );

endfunction

function ProcessPlayerKill(byref victim, byref killer, dmg, attack_type)
    SetObjProperty(victim, PROPID_MOBILE_KILLED_BY, killer.name);
    SetObjProperty(victim, PROPID_MOBILE_KILLED_BY_SERIAL, killer.serial);
    logKiller(victim, killer, dmg, attack_type);
endfunction