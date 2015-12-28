// FSCSystem.inc by Developer Omero, omero@ngx.sm

/*
** Full Shard Control System.
**
** Advanced loggin and bcast system to have major control over:
**
** - Potential risky events (players attacking INVUL npcs, etc...)
** - Staff Members Actions (to track down all the improper actions)
** - Minimal Control over Players Actions (FSCSyStem controls minimal things for every players)
** - MAXIMUM Control over FSC FLAGGED Players Action (FSCSystem allows you to flag a player for FCS)
**
** DONE, TESTED and 100% WORKING:
**
** - The whole FSC System structure (base functions, etc...)
** - Full Command Usage logging, with motivation, target logging, and much more
** - Started the system status & system vars logging (fscmanager)
**
** STILL LEFT to do and TEST:
**
** - skill usage logging for FSC controlled players
** - action usage for FSC controlled players
** - general potential risky events logging for all players (looting, killing, etc...)
** - commands to put someone FSC controlled for a given period
** - system itself routines to put someone under FSC control
** - adding more control modules: not only logger, but also bcast, send mail, sms, etc...
**
**
** ----------------- IMPORTANT NOTE: ---------------------------------
**
** Now I'm currently using ENUMERATEONLINECHAR which is AWFUL and EVIL.
** Switching to the new Shil List from managers.inc as soon as it's ready
**
** Thanks to:
**
** - Syzygy POL dev for the AUX package !!!
** - Shilohen from ZHC for the WONDERFUL work on the POL94 converion
** - Scripter Stige from ZHI for fsc command conversion
** - Scripter Aspox from ZHI for the java logger without it this couldnt be done.
**
*/

use uo;
use cfgfile;

const TEXT_COLOUR := 130;
const WARNING_COLOUR := 135;
const FSCSLEEP_TIME := 2;
const FSCCOMMAND_LOGPREFIX := "### ";
// const CMDLEVEL_PLAYER := 0;
// const CMDLEVEL_COUNSELOR := 1;
const SKILLID_HIGHEST := 48;

include "include/findcity";
//include "include/attributes";
include "../logger/logger";

// Function used to check if the command can be used by that staff member.
// Returns a BOOLEAN
//
// 0= Forbid command usage and warn the user
// 1= Allow command usage and log command usage motivation if needed

function CmdCheckFSC( character, byref cmdname )

	// Read FSC Command Config File and log if error
	var cfg := ReadConfigFile("fsccmds");

	if (!cfg)
	        syslog("Can't access FSC Command config file!!!");

		// Return 0 --> forbid command usage
		return 0;
	endif

	// Find the command and log if error
	var cmd:= FindConfigElem( cfg, cmdname );

	if (!cmd)
	        syslog("Command " + cmdname + " definition missing in FSC cfg!!!");

		// Return 0 --> forbid command usage
		return 0;
	endif

	// Check if command is allowed
	if( !cmd.status )
		SendSysMessage( character, "This command is currently FORBIDDEN.", 3, TEXT_COLOUR );
	
		// Return 0 --> forbid command usage
		return 0;
	endif

	var cmdlog;

	// If the command if FSC Restricted then check
	if( cmd.fsc )
	
		// If FSC Offline then warn the user and return false
		if( !CheckFSC( character ) )

			SendSysMessage( character, "You are now OFFLINE for FSC System", 3, TEXT_COLOUR );
			sleep(FSCSLEEP_TIME);
			SendSysMessage( character, "You can't use this command while being FSC Offline!", 3, TEXT_COLOUR );
		
			// Return 0 --> forbid command usage
			return 0;
	
		// Else check for log or not
		else
			// If requested motivation for usage
			if( cmd.mot )

				// Ask the user
				cmdlog := SendTextEntryGump( character, "State the command use motivation:", TE_CANCEL_ENABLE, TE_STYLE_NORMAL, 50);

				// Check if entered motivation
				if( !cmdlog );
					SendSysMessage( character, "Command Usage aborted.", 3, TEXT_COLOUR );

					// Return 0 --> forbid command usage
					return 0;
				endif

			endif

		endif

	endif

	// Log the motivation
	if( cmdlog )
		ActionFSC( character, cmdname + " motivation: " + cmdlog, 1 );
	endif

	// Return 1 --> allow command usage
	return 1;

endfunction

// Log Command Usage and general info about user with FSC system
// The output will be as follow:
//
// COMMAND_USER COMMAND_TEXT TARGET (OPTIONAL TEXT)
//
// Example for .goto
// Developer_Developer Omero is going to meet Player_Aspox (britain territory)
//
// It also logs target info if needed

function CommandFSC( character, tgt, byref cmdname, byref optiontext  )

	// Read FSC Command Config File and log if error
	var cfg := ReadConfigFile("fsccmds");
	if (!cfg)
	        syslog("Can't access FSC Command config file!!!");
		return;
	endif

	// Find the command and log if error
	var cmd:= FindConfigElem( cfg, cmdname );

	if (!cmd)
	        syslog("Command " + cmdname + " definition missing in FSC cfg!!!");
		return;
	endif

	var fsctext;

	// Read description in cfg and log it
	if( cmd.text )
		fsctext:=cmd.text;
	endif

	// If target of command is present, add it
	if( tgt )

		var tgtname:= GetPrivsName( tgt.cmdlevel ) + " " + tgt.name;
		fsctext:= FSCCOMMAND_LOGPREFIX + fsctext + " " + tgtname;

		// Log command usage also on target if case
		if( tgt.acct )

			var staffname := GetPrivsName( character.cmdlevel ) + " " + character.name;
			var mustlog := 0;

			// if staff automatically log
			if( tgt.cmdlevel > CMDLEVEL_PLAYER )

				mustlog:=1;

			// else check if FSC controlled
			else
				if( CheckFSC( tgt ) )
					mustlog:=1;
				endif
			endif

			// Logs commands usage also on target log if case
			if( mustlog )
				ActionFSC( tgt, FSCCOMMAND_LOGPREFIX + staffname + " used command " + cmdname + " on " + tgtname, 1 );
			endif

			// ----------- OPTIONAL TARGET LOGGING ----------
			// --------------------------------------------

			var targetlog := GetConfigString( cmd, "targetlog" );

			// If not present definition quit =)
			if( !targetlog )
				syslog( cmdname + " targetlog definition missing in FSC cfg!!!");
				return;
			endif

			// Log IP if requested
			if( CInt( targetlog[1] ) )
				IPFSC( tgt, 1 );
			endif

			// Log Position if requested
			if( CInt( targetlog[2] ) )
				PositionFSC( tgt, 1 );
			endif

			// Log general Info if requested
			if( CInt( targetlog[3] ) )
				InfoFSC( tgt, 1 );
			endif

			// Log Stats if requested
			if( CInt( targetlog[4] ) )
				StatsFSC( tgt, 1 );
			endif

			// Log Skills if requested
			if( CInt( targetlog[5] ) )
				SkillsFSC( tgt, 1 );
			endif

		endif
	endif

	// If optional text is present add it
	if( optiontext )
		fsctext:=fsctext + " (" + optiontext + ")";
	endif

	if( !fsctext )
		fsctext:= "ERROR!!!";
	endif

	// Log the command and commit
	ActionFSC( character, fsctext, 1 );


	// ----------- OPTIONAL USER LOGGING ----------
	// --------------------------------------------

	var userlog := GetConfigString( cmd, "userlog" );

	// If not present definition quit =)
	if( !userlog )
		syslog( cmdname + " userlog definition missing in FSC cfg!!!");
		return;
	endif

	// Log IP if requested
	if( CInt( userlog[1] ) )
		IPFSC( character, 1 );
	endif

	// Log Position if requested
	if( CInt( userlog[2] ) )
		PositionFSC( character, 1 );
	endif

	// Log general Info if requested
	if( CInt( userlog[3] ) )
		InfoFSC( character, 1 );
	endif

	// Log Stats if requested
	if( CInt( userlog[4] ) )
		StatsFSC( character, 1 );
	endif

	// Log Skills if requested
	if( CInt( userlog[5] ) )
		SkillsFSC( character, 1 );
	endif

endfunction

// Bcast message to OnlineFSC staff
// Add warning if needed
function BCastFSC( byref text, warning )

	var staff_member;
	var i;
	var plychat:=GetGlobalProperty("plychat");
	var csize:=plychat.size();

	for (i:=1;i<=csize;i:=i+1)

		staff_member:=SystemFindObjectBySerial(plychat[i]);

		if ( staff_member )

			if ( staff_member.cmdlevel > CMDLEVEL_PLAYER )

				// Add warning msg and bcast with WARNING colour
				if( warning )
					SendSysMessage( staff_member, "FSC Warning: " + text, 3, WARNING_COLOUR );

				// Normal staff bcast
				else
					SendSysMessage( staff_member, text, 3, TEXT_COLOUR );
				endif

			endif

		endif

	endfor

endfunction

// Return number of FSC Online staff members
function FSCStaff()

	var count:=0;

	var staff_member;
	var i;
	var plychat:=GetGlobalProperty("plychat");
	var csize:=plychat.size();

	for (i:=1;i<=csize;i:=i+1)

		staff_member:=SystemFindObjectBySerial(plychat[i]);

		if ( staff_member )

			if ( staff_member.cmdlevel > CMDLEVEL_PLAYER )

				// If OnlineFSC
				if( CheckFSC( staff_member ) )
					count := count + 1;
				endif

			endif

		endif

	endfor

	return count;

endfunction

// Logon check and loggin
// If staff put it automatically FSC Offline, log info and welcome him
// If player log info and set prop for time logging and bcast his logon to FSC Online staff
function LogonFSC( character )

	var name:= GetPrivsName( character.cmdlevel ) + " " + character.name;

	if( character.cmdlevel > CMDLEVEL_PLAYER )

		// Loggin name, privs and IP
		LogLine( name + " just logged in. " );
		ActionFSC( character, "connected from IP: " + character.ip, 1 );

		// Loggin position and coords & commit
		PositionFSC( character, 1 );

		// FSC Offline by default
		// Do not log since just logged
		if( CheckFSC( character ) )
			OfflineFSC( character, 0 );
		endif;

		// TO DO : Give a GUMP with useful info and welcome msg instead of
		// using the SendSysMessage

		SendSysMessage( character, "Hail to thee " + name + "!", 3, TEXT_COLOUR );
		SendSysMessage( character, "Welcome on a server powered by FSC System.", 3, TEXT_COLOUR );

	else

		// Loggin name and IP
		LogLine( name + " just logged in. He's under control." );
		ActionFSC( character, "connected from IP: " + character.ip, 1 );

		// Loggin Position and coords
		PositionFSC( character, 0 );

		// Loggin stats
		StatsFSC( character, 0 );

		// Loggin skills & commit
		SkillsFSC( character, 1 );

		// Prop setup
		SetObjProperty( character, "#fsctime", ReadGameClock() );

		// Warn online staff of the logon
		BCastFSC( name + " has logged in " + findcity(character) + " Territory", 1 );

	endif

endfunction

// Logoff check and loggin
function LogoffFSC( character )

	var name:= GetPrivsName( character.cmdlevel ) + " " + character.name;

	if( character.cmdlevel > CMDLEVEL_PLAYER )

		// Automatically go FSC Offline if was still online
		if( CheckFSC( character ) )
			OfflineFSC( character, 1 );
		endif;

		// Loggin Position and coords
		PositionFSC( character, 0 );

		// Loggin name, privs and IP & commit
		LogLine( name + " just logged out." );
		ActionFSC( character, "disconnected from IP: " + character.ip, 1 );

	else
		// Read elapsed time since logon on shard
		var fsctime:= ReadGameClock() - GetObjProperty( character,"#fsctime" );

		if(!fsctime)
			fsctime:=0;
		endif

		// Loggin stats
		StatsFSC( character, 0 );

		// Loggin skills
		SkillsFSC( character, 0 );

		// Loggin position and coords
		PositionFSC( character, 0 );

		// Loggin name and IP
		LogLine( name + " just logged out." );
		LogLine( name + " has been online for " + fsctime );
		ActionFSC( character, "disconnected from IP: " + character.ip, 1 );

		// Warn online staff of the logoff
		BCastFSC( name + " has logged out in " + findcity(character) + " Territory", 1 );
		sleep( FSCSLEEP_TIME );
		BCastFSC( name + " has been online for " + fsctime, 0 );

	endif

endfunction

// Check if FSC is active on this char
function CheckFSC( character )

	var check:= GetObjProperty( character, "FSC");

	// If property not found then initialize it at 0
	if(!check)
		SetObjProperty( character, "FSC", 0);
		check:=0;
	endif

	return check;

endfunction


// Disable FSC on this char
function OfflineFSC( character, verbose )

	var name:= GetPrivsName( character.cmdlevel ) + " " + character.name;

	var fsctime, fsctotal;

	// Log info if requested
	if( verbose )

		if( character.cmdlevel > CMDLEVEL_PLAYER )

			// --- VAR & PROP SETUP ---
		
				// Read the elapsed time since went OnlineFSC
				fsctime:= ReadGameClock() - GetObjProperty( character,"#fsctime" );

				if(!fsctime)
					fsctime:=0;
				endif

				// Read the TOTAL time spent OnlineFSC
				fsctotal:=GetObjProperty( character,"fsctotal" );

				if(!fsctotal)
					fsctotal:=0;
				endif

				// Add the time to the total
				fsctotal:= fsctotal + fsctime;

				// Write prop
				SetObjProperty( character,"fsctotal",fsctotal );

				// Read pages done
				var pages:= GetObjProperty( character,"#pages" );

				if(!pages)
					pages:=0;
				endif

				// Read total pages done
				var totalpages:= GetObjProperty( character,"totalpages" );

				if(!totalpages)
					totalpages:=0;
				endif

				// Add the current pages to total
				totalpages:= totalpages + pages;

				// Write prop
				SetObjProperty( character,"totalpages",totalpages );

			// --- END VAR & PROP SETUP ---


			// --- LOGGING ---

				// Loggin position and coords
				PositionFSC( character, 0 );

				// Loggin pages info
				LogLine( name + " has done " + pages + " pages." );
				LogLine( "Total pages done: " + totalpages );

				// Loggin time info
				LogLine( name + " has been online on FSC for " + fsctime );
				LogLine( "Total FSC time: " + fsctotal );

				// Commit
				ActionFSC( character, "is now FSC Offline.", 1 );

			// --- END LOGGING ---


		else

			// --- VAR & PROP SETUP ---

				// Read the elapsed time since logon on shard
				fsctime:= ReadGameClock() - GetObjProperty( character,"#fsctime" );

				if(!fsctime)
					fsctime:=0;
				endif

				// Read the elapsed time since player was flagged first time
				fsctotal:= GetObjProperty( character,"fsctotal" );

				if(!fsctotal)
					fsctotal:=0;
				endif

				// Add the time to the total
				fsctotal:= fsctotal + fsctime;

				// Prop clean-up
				EraseObjProperty( character,"fsctotal" );

			// --- END VAR & PROP SETUP ---


			// --- LOGGING ---

				// Loggin position
				PositionFSC( character, 0 );

				// Loggin general info
				InfoFSC( character, 0 );

				// Loggin Stats
				StatsFSC( character, 0 );

				// Loggin skills
				SkillsFSC( character, 0 );

				// Loggin total FSC time info
				LogLine( name + " has been under FSC Control for a total of " + fsctotal );

				// Loggin IP & commit
				LogLine( name + " was using IP: " + character.ip );
				ActionFSC( character, name + "is no more under FSC control.", 1 );

			// --- END LOGGING ---


		endif

	endif

	// Set prop to put char OfflineFSC
	SetObjProperty( character, "FSC", 0);

	// Warn ppl of the change
	if( character.cmdlevel > CMDLEVEL_PLAYER )

		if( verbose )

			// Tell staff member of his status
			SendSysMessage( character, "You are now OFFLINE for FSC System", 3, TEXT_COLOUR );

			// Bcast to FSC enabled staff
			BCastFSC( name + " is now OFFLINE for FSC System", 0 );

		endif

	else

		// Warn online staff of the new player flagged for FSC
		BCastFSC( name + " is no more under FSC System control.", 1 );
		sleep( FSCSLEEP_TIME );
		BCastFSC( name + " has been under control for a total of " + fsctotal, 0 );

	endif



endfunction

// Enable FSC on this char
function OnlineFSC( character )

	var name:= GetPrivsName( character.cmdlevel ) + " " + character.name;

	if( character.cmdlevel > CMDLEVEL_PLAYER )

		LogLine( name + " is now FSC Online." );

		// Loggin position and coords & commit
		PositionFSC( character, 1 );

		// Welcome Message and info for loggin on FSC
		WelcomeFSC( character );

		// Props SETUP
		SetObjProperty( character, "#fsctime", ReadGameClock() );
		SetObjProperty( character, "#pages", 0 );

		// Bcast to FSC enabled staff
		BCastFSC( name + " is now ONLINE for FSC System", 0 );


	else

		// Loggin IP
		LogLine( name + " is using IP: " + character.ip );
		LogLine( name + " is now under FSC control." );

		// Loggin position and coords
		PositionFSC( character, 0 );

		// Loggin general info
		InfoFSC( character, 0 );

		// Loggin Stats
		StatsFSC( character, 0 );

		// Loggin skills
		SkillsFSC( character, 1 );

		// Props SETUP
		SetObjProperty( character, "#fsctime", ReadGameClock() );
		SetObjProperty( character, "fsctotal", 0 );

		// Bcast to FSC enabled staff
		BCastFSC( name + " has been flagged for FSC Control.", 1 );

	endif

	// Set FSC prop
	SetObjProperty( character, "FSC", 1);


endfunction

// Tell staff member some useful info
function WelcomeFSC( character )

	var pages;

	if( character.cmdlevel > CMDLEVEL_COUNSELOR )
 		pages := GetGlobalProperty("gmpages");
	else
		pages := GetGlobalProperty("counpages");
	endif


	if(!pages)
		pages:=0;
	endif

	var fscnum:= FSCStaff();

	if(!fscnum)
		fscnum:=0;
	endif


	var name:= GetPrivsName( character.cmdlevel ) + " " + character.name;

	// TO DO : Give a GUMP with useful info and welcome msg instead of
	// using the SendSysMessage and sleep

	SendSysMessage( character, name + " you are now online for FSC System", 3, TEXT_COLOUR );
	sleep( FSCSLEEP_TIME );
	SendSysMessage( character, pages + " pages left to do.", 3, TEXT_COLOUR );
	sleep( FSCSLEEP_TIME  );
	SendSysMessage( character, fscnum + " staff members currently online for FSC System.", 3, TEXT_COLOUR );
	sleep( FSCSLEEP_TIME  );
	SendSysMessage( character, "All your actions will be fully logged.", 3, TEXT_COLOUR );
	sleep( FSCSLEEP_TIME );
	SendSysMessage( character, "You are meant to work for server now.", 3, TEXT_COLOUR );


endfunction

// Log system info and commit if told so.
function SystemFSC( text, commit )

	LogLine( text );

	// If requested Log directly.
	if( commit )

		LogCommit( "system" );

	endif

endfunction

// Log given player position and commit if told so.
function PositionFSC( character, commit )

	var name:= GetPrivsName( character.cmdlevel ) + " " + character.name;

	LogLine( name + " is in " + findcity(character) + " Territory" );
	LogLine(  name + " is at position " + character.x + "," + character.y + ","+ character.z );

	// If requested Log directly.
	if( commit )

		var filename:= character.acctname + " " + name;

		if( character.cmdlevel > CMDLEVEL_PLAYER )
			filename:= "Staff " + filename;
		else
			filename:= "Player " + filename;
		endif

		LogCommit( filename );

	endif

endfunction

// Log given player IP and commit if told so.
function IPFSC( character, commit )

	var name:= GetPrivsName( character.cmdlevel ) + " " + character.name;

	LogLine( name + " is connected from IP: " + character.ip );

	// If requested Log directly.
	if( commit )

		var filename:= character.acctname + " " + name;

		if( character.cmdlevel > CMDLEVEL_PLAYER )
			filename:= "Staff " + filename;
		else
			filename:= "Player " + filename;
		endif

		LogCommit( filename );

	endif

endfunction

// Log a given action =)
function ActionFSC( character, text, commit )

	var name:= GetPrivsName( character.cmdlevel ) + " " + character.name;

	LogLine( name + " " + text );

	// If requested Log directly.
	if( commit )

		var filename:= character.acctname + " " + name;

		if( character.cmdlevel > CMDLEVEL_PLAYER )
			filename:= "Staff " + filename;
		else
			filename:= "Player " + filename;
		endif

		LogCommit( filename );

	endif

endfunction

// Log given player general info and commit if told so
function InfoFSC( character, commit )

	var name:= GetPrivsName( character.cmdlevel ) + " " + character.name;

	// Log name Info
	LogLine( name + " name prefix: " + character.title_prefix );
	LogLine( name + " name: " + character.name );
	LogLine( name + " name suffix: " + character.title_suffix );

	// Log race/sex/graphic info
	LogLine( name + " gender: " + character.gender );
	LogLine( name + " race: " + character.title_race );
	LogLine( name + " graphic: " + character.graphic );
	LogLine( name + " colour: " + character.color );

	// LOG Guild info
	var guild:=GetObjProperty( character,"guild_id" );

	if( guild )
		LogLine( name + " guild ID: " + guild);
		LogLine( name + " guild name: " + FindGuild(guild) );
		LogLine( name + " guild title: " + character.title_guild );

		if( character.serial == guild.getprop("gm") )
			LogLine( name + " is the guild GM" );
		endif

	else
		LogLine( name + " is not into any guild." );
	endif

	// If requested Log directly.
	if( commit )

		var filename:= character.acctname + " " + name;

		if( character.cmdlevel > CMDLEVEL_PLAYER )
			filename:= "Staff " + filename;
		else
			filename:= "Player " + filename;
		endif

		LogCommit( filename );

	endif

endfunction

// Log given player stats and commit if told so.
function StatsFSC( character, commit )

	var name:= GetPrivsName( character.cmdlevel ) + " " + character.name;

	// Loggin Stats
	LogLine( name + " strength: " + GetStrength(character) );
	LogLine( name + " dexterity: " + GetDexterity(character) );
	LogLine( name + " intelligence: " + GetIntelligence(character) );

	// Loggin Vitals
	LogLine( name + " hp/maxhp: " + GetHp(character) + "/" + GetMaxHp(character) );
	LogLine( name + " mana/maxmana: " + GetMana(character) + "/" + GetMaxMana(character) );
	LogLine( name + " stamina/maxstamina: " + GetStamina(character) + "/" + GetMaxStamina(character) );

	// If requested Log directly.
	if( commit )

		var filename:= character.acctname + " " + name;

		if( character.cmdlevel > CMDLEVEL_PLAYER )
			filename:= "Staff " + filename;
		else
			filename:= "Player " + filename;
		endif

		LogCommit( filename );
	endif

endfunction

// Log given player skills and commit if told so.
function SkillsFSC( character, commit )

	var name:= GetPrivsName( character.cmdlevel ) + " " + character.name;

	for ID := 0 to SKILLID_HIGHEST
		LogLine( name + " " + GetSkillName(ID) + ": " + GetEffectiveSkill( character, ID ) );
	endfor

	// If requested Log directly.
	if( commit )

		var filename:= character.acctname + " " + name;

		if( character.cmdlevel > CMDLEVEL_PLAYER )
			filename:= "Staff " + filename;
		else
			filename:= "Player " + filename;
		endif

		LogCommit( filename );

	endif

endfunction

// Return Privs Staff Name for cmdlevel given
function GetPrivsName( cmdlevel )

	case( cmdlevel )

		0: return "Player";

		1: return "Counselor";

		2: return "Seer";

		3: return "GM";

		4: return "Admin";

		5: return "Developer";

		default: return "Unknown";

	endcase

endfunction


 // Returns skill name, from ID
function GetSkillName( skillid )

	case(skillid)
		0: return "alchemy";                 
		1: return "anatomy";
		2: return "animal lore";
		3: return "item identification";
		4: return "arms lore";
		5: return "parrying";
		6: return "begging";
		7: return "blacksmithy";
		8: return "bowcraft";
		9: return "peacemaking";
		10: return "camping";
		11: return "carpentry";
		12: return "cartography";
		13: return "cooking";
		14: return "detect hidden";
		15: return "enticement";
		16: return "evaluating intelligence";
		17: return "healing";
		18: return "fishing";
		19: return "forensic evaluation";
		20: return "herding";
		21: return "hiding";
		22: return "provocation";
		23: return "inscription";
		24: return "lockpicking";
		25: return "magery";
		26: return "magic resistance";
		27: return "tactics";
		28: return "snooping";
		29: return "musicianship";
		30: return "poisoning";
		31: return "archery";
		32: return "spiritspeaking";
		33: return "stealing";
		34: return "tailoring";
		35: return "animal taming";
		36: return "taste identification";
		37: return "tinkering";
		38: return "tracking";
		39: return "veterinary";
		40: return "swordsmanship";
		41: return "mace fighting";
		42: return "fencing";
		43: return "wrestling";
		44: return "lumberjacking";
		45: return "mining";
		46: return "meditation";
		47: return "stealth";
		48: return "remove trap";

		default: return "unknown";

	endcase

endfunction