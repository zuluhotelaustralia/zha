// AutoLoop.inc by Lapo 'Aspox' Luchini <lapo@lapo.it>

/*
** AutoLoop_init asks character how many time it wants to iterate
** [if "autoloop" CProp is set it doesn't ask but uses that value, unless is 0]
** AutoLoop_more retunrs the number of loops remaining
**
** Use like this (for a crafter-like skill):
**
** AutoLoop_init(who);
** while(AutoLoop_more()>0 && <material_not_finished>)
**   <create_item>;
** endwhile
** AutoLoop_finish();
*/

const al_consecutive := 10; // number of consecutive items before a anti-macroer gump appears to ask to continue
const al_timeout := 15; // number of seconds the player has to press "continue"

var al_number	:= 0;
var al_remain	:= 0;
var al_char;
var al_delay;
var al_charx;
var al_chary;
var al_setting;

// init the autoloop variables
function AutoLoop_init( character, maxx := 10000, delay := 6 )

	al_setting := CInt( GetObjProperty( character, "autoloop" ) );
	if( !al_setting )
		SetObjProperty( character, "autoloop", 0 );
		al_setting := 0;
	endif

	if( al_setting == 0 )
		al_number := CInt( SendTextEntryGump(character, "How many loops? [0-"+maxx+"]", TE_CANCEL_ENABLE, TE_STYLE_NORMAL, 4));
	else
		al_number := al_setting;
	endif

	if( !al_number )
		al_number := 0;
	elseif( al_number > maxx )
		al_number := maxx;
	endif

	al_remain := al_number;
	al_char   := character;
	al_charx  := al_char.x;
	al_chary  := al_char.y;
	al_delay  := delay;

endfunction


// he numer returned is the number of things STILL to do, use BEFORE creating, not after
function AutoLoop_more()

	if( (al_charx == al_char.x) && (al_chary == al_char.y) )
		if( al_remain > 0 )
			if( al_remain != al_number )
				sleep( al_delay );
				if( (al_number - al_remain) % al_consecutive == 0)
// Removed autoloop stopper
// 					if( !TimedYesNo(al_char) )
//						al_remain := -1;
//						al_number := 0; // to print "looping aborted"
//					endif
				endif
			endif
			if( al_remain > 0 )
				SendSysMessage( al_char, "Looping [" + al_remain + " more to go].", 3, 89 );
				al_remain := CInt( al_remain ) - 1;
			endif
		else
			al_remain := -1;
		endif
	else
		SendSysMessage( al_char, "You moved.", 3, 89 );
		al_remain := -1;
		al_number := 0; // to print "looping aborted"
	endif

	return( al_remain+1 );

endfunction


// to be used after all, to clean up the variables and print the finishing text
function AutoLoop_finish()

	if( (al_remain < 0) && al_number )
		SendSysMessage( al_char, "You finished looping.", 3, 89);
	else
		SendSysMessage( al_char, "Looping aborted.", 3, 89);
	endif

	al_number := 0;
	al_remain := 0;

endfunction


// anti-macroers jumping gump
function TimedYesNo( who )

	var x := 220 + Random( 200 );
	var y := 140 + Random( 200 );
	var al_layout := {
		"page 0",
		"gumppic " + x + " " + y + " 2070",
		"button " + (x+30) + " " + (y+75) + " 2130 2129 1 0 1",
		"button " + (x+90) + " " + (y+75) + " 2119 2120 1 0 0",
		"text " + (x+40) + " " + (y+30) + " 1355 0"
	};
	var al_data := { "Continue?" };

	var timeover := ReadGameClock();
	var res:=SendDialogGump( who, al_layout, al_data );

	if( ReadGameClock() - timeover > al_timeout)
		SendSysMessage( who, "You waited too long to decide.", 3, 89 );
		return( 0 );
	elseif( res )
		return( 1 );
	else
		return( 0 );
	endif

endfunction