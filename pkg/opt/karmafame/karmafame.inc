use uo;
use math;
use cfgfile;

const MIN_KARMA_LIMIT := -10500;
const MAX_KARMA_LIMIT := 10500;

const MIN_FAME_LIMIT := 0;
const MAX_FAME_LIMIT := 10500;

const FACTOR_KARMA := 0.4; //upped this from 0.3
const FACTOR_FAME  := 0.3; //upped this from 0.1 but see advise attackers

const FAME_LOST_ON_DEATH := -125;

const KARMA_CRIMINAL_LIMIT := -750;
const KARMA_MURDERER_LIMIT := -3000;

//const VALUE_MULTIPLIER_FOR_ALLIES := 0.5;
const VALUE_MULTIPLIER_FOR_ALLIES := 1.0; //upped this :)

const VALUE_MULTIPLIER_FOR_TAMED  := 0.5;

const DELAY_TO_GET_KARMA_FAME := 300;


// Action ids used by AwardKarmaFameForAction function
/*
const ACTIONID_BEGGING       := 1;
const ACTIONID_BLESS         := 2;
const ACTIONID_CAST_NECRO    := 3;
const ACTIONID_CHOP_CORPSE   := 4;
const ACTIONID_CURE          := 5;
const ACTIONID_CURSE         := 6;
const ACTIONID_DEATH         := 7;
const ACTIONID_EXCEPTIONAL   := 8;
const ACTIONID_HEAL          := 9;
const ACTIONID_LOOTING       := 10;
const ACTIONID_POISONING     := 11;
const ACTIONID_PROTECT       := 12;
const ACTIONID_RES           := 13;
const ACTIONID_SNOOPING      := 14;
const ACTIONID_STEALING      := 15;
const ACTIONID_SUMMON_DAEMON := 16;
*/

function AdviseAttackers( who )

	var hit_list := GetObjProperty( who, "Hitlist" );

	if( hit_list )
		EraseObjProperty( who, "Hitlist" );

		foreach arr in hit_list
			if( ReadGameClock() < CInt(arr[3]) + DELAY_TO_GET_KARMA_FAME ) // what is this delay meant to do?
				var attacker := SystemFindObjectBySerial( CInt(arr[1]) );
				if( !attacker )
					attacker := SystemFindObjectBySerial( CInt(arr[1]), SYSFIND_SEARCH_OFFLINE_MOBILES );
				endif
			
				if( attacker )
					//AffectKarmaAndFameForKill( attacker, who, CDbl(arr[2]) / len(hit_list) );
					//Broadcast(arr);
					AffectKarmaAndFameForKill( attacker, who, CDbl(arr[2]) ); //barns
					
				endif
			endif
		endforeach
	endif

endfunction


function AffectKarmaAndFameForKill( attacker, defender, factor := 0 )
//function AffectKarmaAndFameForKill( attacker, defender, factor := 1.0 )
	//Broadcast("factor " + factor);
	if (factor>1)
		factor:=1;
	endif
	//barns: factor must be specified or no fame or karma
	//Broadcast("1");
	if (!factor)
		return;
	endif
	//Broadcast("2");
	// Check if the creature was killed by his owner
	if( GetObjProperty( defender, "master" ) == attacker.serial )
		AwardFame( attacker, -200 );
		if( CInt( GetKarma( attacker ) ) > 0 )
			AwardKarma( attacker, -250 );
		endif
		return;
	endif
	//Broadcast("3");
	//old
	// Award the fame if you killed something more famous than you, if so award fame

	//barns mod
	//you still get fame for monsters not as famous... only less
	
	//only award fame if an NPC was killed -- barns
	//if (defender.IsA(POLCLASS_NPC))
	if (GetObjProperty(defender, "npctemplate"))
		//Broadcast("defender was an NPC");
		var defendersfame := GetFame(defender);
		var delta_fame := defendersfame - GetFame(attacker);
		//Broadcast("delta_fame " + delta_fame);
		if( delta_fame > 0 )
			delta_fame := CInt( delta_fame * FACTOR_FAME * factor );
			if( delta_fame <= 0 )
				delta_fame := 1;
			endif
			//Broadcast("less famous " + delta_fame);
			AwardFame( attacker, delta_fame );
		else //if ur more famous u still get 1% of monsters total fame 
			var myfame:=CInt(defendersfame * 0.01 * FACTOR_FAME * factor);
			//Broadcast("myfame " + myfame);
			if (myfame>0)
				//Broadcast("more famous " + myfame);
				AwardFame( attacker, myfame);
			else
				AwardFame( attacker, 1);
			endif
		endif
	endif

	var attacker_karma := GetKarma(attacker);
	var notodefkarma   := GetKarma(defender);
	var delta_karma    := 0.0;

	if( attacker_karma >= 0 ) // Attacker got positive or null karma

		if( notodefkarma > 0 ) // Attacking positive karma
			// Lose karma
			delta_karma := ( attacker_karma - notodefkarma ) * FACTOR_KARMA;
			if( delta_karma > 0.0 )
				delta_karma := 0.0;
			endif
		else // Attacking negative or null karma
			// Gain karma if the defender is more evil than you're virtueous
			delta_karma := (Abs(notodefkarma) - attacker_karma) * FACTOR_KARMA;
			if( delta_karma < 0.0 )
				delta_karma := 0.0;
			endif
		endif

	else // Attacker got negative karma

		if( notodefkarma >= 0 ) // Attacking positive or null karma
			// Lose karma if the defender is more virtueous than you're evil
			delta_karma := ((-attacker_karma) - notodefkarma) * FACTOR_KARMA;
			if( delta_karma > 0.0 )
				delta_karma := 0.0;
			endif
		elseif( attacker_karma > notodefkarma ) // Attacking something more evil than him
			// Gain karma
			delta_karma := (attacker_karma - notodefkarma) * FACTOR_KARMA;
			if( delta_karma < 0.0 )
				delta_karma := 0.0;
			endif
		endif

	endif

	delta_karma := CInt( delta_karma * factor );

	if( delta_karma != 0.0 )
		AwardKarma( attacker, delta_karma );
	endif

endfunction


function AwardFame( who, amt )

	var fame := GetFame( who );

	if( !fame )
		fame := 0;
	endif

	if( amt > 0 ) // Gaining fame
		if( amt <= 50 )
			SendSysMessage( who, "You have gained a tiny amount of fame!" );
		elseif( amt <= 100 )
			SendSysMessage( who, "You have gained a very small amount of fame!" );
		elseif( amt <= 300 )
			SendSysMessage( who, "You have gained a small amount of fame!" );
		elseif( amt <= 600 )
			SendSysMessage( who, "You have gained a good amount of fame!" );
		elseif( amt <= 1000 )
			SendSysMessage( who, "You have gained a large amount of fame!" );
		else
			SendSysMessage( who, "You have gained a great amount of fame!" );
		endif
	elseif( amt < 0 ) // Losing fame
		if( amt >= -50 )
			SendSysMessage( who, "You have lost a tiny amount of fame!" );
		elseif( amt >= -100 )
			SendSysMessage( who, "You have lost a very small amount of fame!" );
		elseif( amt >= -300 )
			SendSysMessage( who, "You have lost a small amount of fame!" );
		elseif( amt >= -600 )
			SendSysMessage( who, "You have lost a good amount of fame!" );
		elseif( amt >= -1000 )
			SendSysMessage( who, "You have lost a large amount of fame!" );
		else
			SendSysMessage( who, "You have lost a great amount of fame!" );
		endif
	endif

	SetFame( who, fame + amt );

endfunction


function AwardKarma( who, amt )

	var karma := GetKarma( who );

	if( !karma )
		karma := 0;
	endif

	if( amt > 0 ) // Gaining karma
		if( amt <= 50 )
			SendSysMessage( who, "You have gained a tiny amount of karma!" );
		elseif( amt <= 100 )
			SendSysMessage( who, "You have gained a very small amount of karma!" );
		elseif( amt <= 300 )
			SendSysMessage( who, "You have gained a small amount of karma!" );
		elseif( amt <= 600 )
			SendSysMessage( who, "You have gained a good amount of karma!" );
		elseif( amt <= 1000 )
			SendSysMessage( who, "You have gained a large amount of karma!" );
		else
			SendSysMessage( who, "You have gained a great amount of karma!" );
		endif
	elseif( amt < 0 ) // Losing karma
		if( amt >= -50 )
			SendSysMessage( who, "You have lost a tiny amount of karma!" );
		elseif( amt >= -100 )
			SendSysMessage( who, "You have lost a very small amount of karma!" );
		elseif( amt >= -300 )
			SendSysMessage( who, "You have lost a small amount of karma!" );
		elseif( amt >= -600 )
			SendSysMessage( who, "You have lost a good amount of karma!" );
		elseif( amt >= -1000 )
			SendSysMessage( who, "You have lost a large amount of karma!" );
		else
			SendSysMessage( who, "You have lost a great amount of karma!" );
		endif
	endif

	SetKarma( who, karma + amt );

endfunction

/*
function AwardKarmaFameForAction( who , action , on_who := 0 )
*/



function CheckForCriminalsNearMobile( who, range := 5 )

	if( who.criminal || who.murderer )
		return;
	endif

	var criminals := ListMobilesInLineOfSight( who, range );
	var guarr;
	foreach crim in criminals
		if( crim.murderer )
			PrintTextAbove( who, "A murderer is here!!!" );
			sleep( 2 );
			PrintTextAbove( who, "No..." );
			sleepms( 500 );
			PrintTextAbove( who, "Please..." );
			sleepms( 500 );
			PrintTextAbove( who, "Don't hurt me..." );
			sleepms( 500 );
			PrintTextAbove( who, "Guards please help me!" );
			guarr := GetGlobalProperty( "GuardsCalled" );
			if (!guarr)
				guarr:={};
			endif
			guarr.append( who.serial );
			SetGlobalProperty( "GuardsCalled", guarr );
		elseif( crim.criminal )
			PrintTextAbove( who, "Guards!!! There is a criminal here!!!! Come Here!!!!" );
			guarr := GetGlobalProperty( "GuardsCalled" );
			if (!guarr)
				guarr:={};
			endif
			guarr.append( who.serial );
			SetGlobalProperty( "GuardsCalled", guarr );
		endif
	endforeach

endfunction


function GetFame( who )

	var fame := CInt( GetObjProperty( who, "Fame" ) );
	if( !who.isA(POLCLASS_NPC) )
		if( !who.cmdlevel )
			if( fame > MAX_FAME_LIMIT )
				fame := MAX_FAME_LIMIT;
				SetObjProperty( who, "Fame", MAX_FAME_LIMIT );
			elseif( fame < MIN_FAME_LIMIT )
				fame := MIN_FAME_LIMIT;
				SetObjProperty( who, "Fame", MIN_FAME_LIMIT );
			endif
		endif
	endif

	return fame;

endfunction


function GetFameID( fame )

	var famearray  := {0, 1250, 2500, 5000, 10000};
	var index := len(famearray);
	while ((index > 1) && (fame < famearray[index]))
		index := index - 1;
	endwhile

	return index;

endfunction


function GetKarma( who )

	var karma := CInt( GetObjProperty( who, "Karma" ) );
	if( !who.isA(POLCLASS_NPC) )
		if( !who.cmdlevel )
			if( karma > MAX_KARMA_LIMIT )
				karma := MAX_KARMA_LIMIT;
				SetObjProperty( who, "Karma", MAX_KARMA_LIMIT );
			elseif( karma < MIN_KARMA_LIMIT )
				karma := MIN_KARMA_LIMIT;
				SetObjProperty( who, "Karma", MIN_KARMA_LIMIT );
			endif
		endif
	endif

	return karma;

endfunction


function GetKarmaID( karma )

	var karmaarray := {-30000, -9999, -4999, -2499, -1249, -625, +625, +1250, +2500, +5000, +10000}; /*ROWS*/
	var index := len(karmaarray);
	while ((index > 1) && (karma < karmaarray[index]))
		index := index - 1;
	endwhile

	return index;

endfunction


function GetKarmaFameName( who )

	var kfmatrix   := {
	/* Karma\Fame  0              1250            2500         5000             10000                   */
	/* -30000 */ {"Outcast",     "Wretched",     "Nefarious", "Dread",         "Dread Lord/Lady"        },
	/*  -9999 */ {"Despicable",  "Dastardly",    "Wicked",    "Evil",          "Evil Lord/Lady"         },
	/*  -4999 */ {"Scoundrel",   "Malicious",    "Vile",      "Villainous",    "Dark Lord/Lady"         },
	/*  -2499 */ {"Unsavory",    "Dishonorable", "Ignoble",   "Sinister",      "Sinister Lord/Lady"     },
	/*  -1249 */ {"Rude",        "Disreputable", "Notorious", "Infamous",      "Dishonored Lord/Lady"   },
	/*   -624 */ {"",            "Notable",      "Prominent", "Renowned",      "Lord/Lady"              },
	/*   +625 */ {"Fair",        "Upstanding",   "Reputable", "Distinguished", "Distinguished Lord/Lady"},
	/*  +1250 */ {"Kind",        "Respectable",  "Proper",    "Eminent",       "Eminent Lord/Lady"      },
	/*  +2500 */ {"Good",        "Honorable",    "Admirable", "Noble",         "Noble Lord/Lady"        },
	/*  +5000 */ {"Honest",      "Commendable",  "Famed",     "Illustrious",   "Illustrious Lord/Lady"  },
	/* +10000 */ {"Trustworthy", "Estimable",    "Great",     "Glorious",      "Glorious Lord/Lady"     }
};
	var karma := GetKarma( who );
	var fame  := GetFame( who );
	var title := kfmatrix[ GetKarmaID( karma ) ][ GetFameID( fame )];

	if( karma < KARMA_MURDERER_LIMIT )
		who.setCriminal(0);
		who.setMurderer(1);
	elseif( karma < KARMA_CRIMINAL_LIMIT )
		who.setCriminal(1);
		who.setMurderer(0);
	else
		who.setMurderer(0);
	endif

	if( title["Lord/Lady"] )
		if( who.gender == 0 )
			title["Lord/Lady"] := "Lord";
		else
			title["Lord/Lady"] := "Lady";
		endif
	endif

	return title;

endfunction


function SetFame( who, fame )

	if( !fame )
		fame := 0;
	else
		fame := CInt( fame );
	endif

	if( !who.npctemplate && !who.cmdlevel )
		if( fame > MAX_FAME_LIMIT )
			fame := MAX_FAME_LIMIT;
		elseif( fame < MIN_FAME_LIMIT )
			fame := MIN_FAME_LIMIT;
		endif
	endif

	SetObjProperty( who, "Fame", fame );
	SetNewNotoName( who );

endfunction


function SetKarma( who, karma )

	if( !karma )
		karma := 0;
	else
		karma := CInt(karma);
	endif

	if( !who.npctemplate && !who.cmdlevel )
		if( karma > MAX_KARMA_LIMIT )
			karma:=MAX_KARMA_LIMIT;
		elseif( karma < MIN_KARMA_LIMIT )
			karma:=MIN_KARMA_LIMIT;
		endif

		var old := GetKarma( who );

		if( karma < KARMA_MURDERER_LIMIT )
			if( old >= KARMA_MURDERER_LIMIT )
				SendSysMessage( who, "For all yours sins, you're now considered as a murderer..." );
			endif
		elseif( karma < KARMA_CRIMINAL_LIMIT )
			if( old >= KARMA_CRIMINAL_LIMIT )
				SendSysMessage( who, "From now, you're considered as a criminal..." );
			elseif( old < KARMA_MURDERER_LIMIT )
				SendSysMessage( who, "You slowly purge yours sins, you're now considered only as a criminal..." );
			endif
		else
			if( old < KARMA_CRIMINAL_LIMIT )
				SendSysMessage( who, "You are no longer a criminal!" );
			endif
		endif
	endif

	SetObjProperty( who, "Karma", karma );
	SetNewNotoName( who );

endfunction


function SetNewNotoName( who )

	if( who.npctemplate || who.cmdlevel )
		return;
	endif

	var prefix := GetKarmaFameName( who );

	if( len(prefix) > 0 )
		who.title_prefix := "The " + prefix + " ";
	else
		who.title_prefix := "";
	endif

endfunction

function FameExperiencePointModifier(who, skillid, points)
	//FOR FAME BASED 'EXPERIENCE POINTS'
	//returns the points to be awarded on success of CheckSkill after bonuses for high fame taken into account.
	if( who.IsA(POLCLASS_NPC) )	
		return points;
	endif
	
	//dont do anything if skill set to decrease or locked
	var skills_state_array   := GetObjProperty( who, "SkillsState" );
	if( !skills_state_array )
		skills_state_array := array;
	endif
	
	var skill_state := skills_state_array[skillid+1];
	if( skill_state == "d" || skill_state == "l" )
		return points;
	endif

	var fame := GetFame(who);
	if (fame<=0)
		return points;
	endif
	
	//at this stage only skills up to 129 get fame experience point bonus
	var skill := GetEffectiveSkill( who, skillid );
	if (skill>129)
		return points;
	endif	
	
	//get the amount of fame the player is to use for bonus
	//based on players fame

	var fameindex := GetFameID(fame); //returns 1+ based on whether fame is greater than certain increments.

	//random chance of bonus points based on fame
	var rand := RandomInt(20)+1; //between 1 and 20
	//e.g. max fameindex is 5 (for over 10000 fame)... 
	//so if have over 10k fame you have a 5 in 30 chance of bonus points
	//if between 0 and 1249... its 1 in 30
	if (rand>1) 
		return points;
	endif
	////Broadcast(fameindex + " - " +rand);

	var mod;
	var fame_to_use;

	if (fameindex>=3) //2500+
		mod:=20;
		fame_to_use:=160;
	elseif(fameindex>=2) // 1250-2499
		mod:=10;
		fame_to_use:=80;
	else	//0-1249
		mod:=5;
		fame_to_use:=40;
	endif
	if (fame - fame_to_use > 0)
		SetFame(who, fame-fame_to_use);
	else
		SetFame(who, 0);
	endif
	SendSysMessage(who, "Your recent adventures allow you to learn skills faster!", 3, 53);
	return CInt(points*mod);

endfunction