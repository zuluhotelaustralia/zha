use cfgfile;
use uo;
use os;
use util;

include "include/gumps";
include "include/classes";
//include "newclassbook";
include "include/spelldata";
include "newclassspelldata";
//good gumps 898 to 89B, 8AC, 1FE, 1F4


//necro spell gump pics :  5000-520c

//To add a new spell book
//1. Create a new spellbook type constant
//2. Add to GetSpellBookTypes()
//3. Create a new spellbook item  with appropriate CProps (see druidbook CProps)
//4. Enter constants below and update GetBookTypeScrollFeatures(booktype)
//5. Specify which classes can use book in GetBookClassRestriction(booktype) //note first 4 spells can be used by anyone
//6. Specify spell ids (129+) of new spell book (each can have max of 16). The first 4 listed will be given to a book at creation.
//  function GetBookSpellIDs(booktype)
//7. Fill in spell.cfg in this package for each new spell id created
//8. Write spell script in this package

//druid book constants

const SPELLBOOK_TYPE_DRUID := 1;
const SPELLBOOK_DRUID_SPELLSCROLLNAME := "Gem of";
const SPELLBOOK_DRUID_GRAPHIC := 0xf91;
const SPELLBOOK_DRUID_COLOUR := 1380;
const SPELLBOOK_DRUID_OBJTYPE := 0xA5C;

//bard book constants
const SPELLBOOK_TYPE_BARD := 2;
const SPELLBOOK_BARD_SPELLSCROLLNAME := "";
const SPELLBOOK_BARD_GRAPHIC := 0xec0;
const SPELLBOOK_BARD_COLOUR := 1463;
const SPELLBOOK_BARD_OBJTYPE := 0xa5a;

//holy book
const SPELLBOOK_TYPE_HOLY := 3;
const SPELLBOOK_HOLY_SPELLSCROLLNAME := "Prayer of";
const SPELLBOOK_HOLY_GRAPHIC := 0x1F6E;
const SPELLBOOK_HOLY_COLOUR := 1590;
const SPELLBOOK_HOLY_OBJTYPE := 0xa5e;

//unholy book
const SPELLBOOK_TYPE_UNHOLY := 4;
const SPELLBOOK_UNHOLY_SPELLSCROLLNAME := "Rite of";
const SPELLBOOK_UNHOLY_GRAPHIC := 0x1853;
const SPELLBOOK_UNHOLY_COLOUR := 1400;
const SPELLBOOK_UNHOLY_OBJTYPE := 0xa5d;

//psionics knowledge constants
const SPELLBOOK_TYPE_PSIONICS := 5;
const SPELLBOOK_PSIONICS_SPELLSCROLLNAME := "Art of";
const SPELLBOOK_PSIONICS_GRAPHIC := 0x1f13;
const SPELLBOOK_PSIONICS_COLOUR := 1577;
const SPELLBOOK_PSIONICS_OBJTYPE := 0xa5f;



function GetBookGump(book)
	return Hex(GetObjProperty(book, "bookgump"));
endfunction

function GetBookTitle(book)
	return GetObjProperty(book, "booktitle");
endfunction

function GetBookType(book)
	return GetObjProperty(book, "booktype");
endfunction

function GetSpellBookTypes()
	return{
		SPELLBOOK_TYPE_DRUID,
		SPELLBOOK_TYPE_BARD,
		SPELLBOOK_TYPE_HOLY,
		SPELLBOOK_TYPE_UNHOLY,
		SPELLBOOK_TYPE_PSIONICS
	};

endfunction


function GetClassSpellDiffBonus(booktype, who)
	var class := GetClass(who);
	if (GetClass(who)==CLASSEID_POWERPLAYER)
		return SEMI_SPELLCASTER_BONUS;
	endif
	case (booktype)
		SPELLBOOK_TYPE_DRUID:
			if (GetObjProperty(who, CLASSEID_DRUID))
				return PURE_SPELLCASTER_BONUS ;
			endif
			if (GetObjProperty(who, CLASSEID_RANGER))
				return SEMI_SPELLCASTER_BONUS ;
			endif
		break;
		SPELLBOOK_TYPE_BARD:
			if (GetObjProperty(who, CLASSEID_BARD))
				return PURE_SPELLCASTER_BONUS ;
			endif
		break;
		SPELLBOOK_TYPE_HOLY:
			if (GetObjProperty(who, CLASSEID_PRIEST))
				return PURE_SPELLCASTER_BONUS ;
			endif
			if (GetObjProperty(who, CLASSEID_PALADIN))
				return SEMI_SPELLCASTER_BONUS ;
			endif
		break;
		SPELLBOOK_TYPE_UNHOLY:
			if (GetObjProperty(who, CLASSEID_NECROMANCER))
				return PURE_SPELLCASTER_BONUS ;
			endif
			if (GetObjProperty(who, CLASSEID_DEATHKNIGHT))
				return SEMI_SPELLCASTER_BONUS ;
			endif
		break;
		SPELLBOOK_TYPE_PSIONICS:
			if (GetObjProperty(who, CLASSEID_MONK))
				return PURE_SPELLCASTER_BONUS ;
			endif
	endcase
endfunction



function GetBookTypeSkillID(booktype)
	/*
	SKILLID_NATURELORE,  SKILLID_PIETY, SKILLID_NECROMANCY, SKILLID_PSIONICS
	*/
		case (booktype)
			SPELLBOOK_TYPE_DRUID:		return SKILLID_NATURELORE;
			SPELLBOOK_TYPE_BARD:		return SKILLID_ENTICEMENT;
			SPELLBOOK_TYPE_HOLY:		return SKILLID_PIETY;
			SPELLBOOK_TYPE_UNHOLY:		return SKILLID_NECROMANCY;
			SPELLBOOK_TYPE_PSIONICS:	return SKILLID_PSIONICS;
			default:  			return SKILLID_MAGERY;
		endcase
endfunction

function GetBookTypeSpellIDs(booktype)
		case (booktype)
			SPELLBOOK_TYPE_BARD:	return {129} ; //add every spell id for that book
			SPELLBOOK_TYPE_DRUID: return {145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160};
			SPELLBOOK_TYPE_HOLY: return {161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176};
			SPELLBOOK_TYPE_UNHOLY: return {177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192};
			SPELLBOOK_TYPE_PSIONICS: return {193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208};
		endcase
endfunction


function GetSpellName(spellid)
	var conf	 := ReadConfigFile( "spells" );
	var elem	 := FindConfigElem( conf, spellid );
	return GetConfigString( elem, "Name" );
endfunction

function GetBookSpellIDs(book)
	return GetObjProperty(book, "storedspells");
endfunction


//spell scroll functions

function GetBookTypeScrollFeatures(booktype)
		case (booktype)
			SPELLBOOK_TYPE_DRUID:	return { SPELLBOOK_DRUID_SPELLSCROLLNAME, SPELLBOOK_DRUID_GRAPHIC, SPELLBOOK_DRUID_COLOUR};
			SPELLBOOK_TYPE_BARD:	return { SPELLBOOK_BARD_SPELLSCROLLNAME, SPELLBOOK_BARD_GRAPHIC, SPELLBOOK_BARD_COLOUR};
			SPELLBOOK_TYPE_HOLY:	return { SPELLBOOK_HOLY_SPELLSCROLLNAME, SPELLBOOK_HOLY_GRAPHIC, SPELLBOOK_HOLY_COLOUR};
			SPELLBOOK_TYPE_UNHOLY:	return { SPELLBOOK_UNHOLY_SPELLSCROLLNAME, SPELLBOOK_UNHOLY_GRAPHIC, SPELLBOOK_UNHOLY_COLOUR};
			SPELLBOOK_TYPE_PSIONICS:	return { SPELLBOOK_PSIONICS_SPELLSCROLLNAME, SPELLBOOK_PSIONICS_GRAPHIC, SPELLBOOK_PSIONICS_COLOUR};


		endcase
endfunction

function AssignNewSpellID(spellscroll)
	var booktypes := GetSpellBookTypes();
	var rand:= RandomInt(len(booktypes))+1;
	if (rand == 2)
		rand := 1;
	endif
	var newbooktype:= booktypes[rand];

	var spellids := GetBookTypeSpellIDs(newbooktype);
	var myspellid := spellids[RandomInt(len(spellids))+1];
	SetObjProperty(spellscroll, "SpellID", myspellid);

	var features:=GetBookTypeScrollFeatures(newbooktype);
	spellscroll.name := features[1] + " " + GetSpellName(myspellid);
	spellscroll.graphic := features[2];
	spellscroll.color := features[3];
	spellscroll.usescript := ":newclassspells:newbookspellscroll_use";
endfunction

function AssignThisSpellID(spellid, spellscroll)
	var newbooktype := GetBookTypeForSpellID(spellid);
	if (newbooktype==0)
		return 0;
	endif
	SetObjProperty(spellscroll, "SpellID", spellid);

	var features:=GetBookTypeScrollFeatures(newbooktype);
	spellscroll.name := features[1] + " " + GetSpellName(spellid);
	spellscroll.graphic := features[2];
	spellscroll.color := features[3];
	spellscroll.script := ":newclassspells:newbookspellscroll_use";
	return 1;
endfunction

function GetBookTypeForSpellID(spellid)
	foreach bkt in GetSpellBookTypes()
		if (SpellIDInBookTypeSpellIDs(spellid, bkt))
			return bkt;
		endif
	endforeach
	return 0;
endfunction

function SpellIDInBookTypeSpellIDs(spellid, booktype)
	foreach sp in GetBookTypeSpellIDs(booktype)
		if (spellid == sp)
			return 1;
		endif
	endforeach
	return 0;
endfunction

function SpellIDAlreadyStored(spellid, spellsstored)
	foreach sp in spellsstored
		if (spellid == sp)
			return 1;
		endif
	endforeach
	return 0;
endfunction



function IsPlayersClassInClassList(classe_id, classarray)
	//classarray is a list of classes, who is the person to check whether their class is in that classarray
	if (len(classarray)==0)
		return 1;
	endif

	if (! classe_id)
		return 0;
	endif
	foreach cl in classarray
		if (classe_id == cl)
			return 1;
		endif
	endforeach
	return 0;
endfunction



function MakeBookContentsArray(contentsString)
	//Broadcast(contentsString);
	//converts a single string into an array of string with max individual characters !> 22
	if (!contentsString)
		return {""};
	endif

	var wordsArray:= SplitWords(contentsString);//now broken into an array of words
	//Broadcast("wordsArray: ", wordsArray);
	var tempContentArray:=array;

	//var i:=1;
	var tempString:="";

	foreach word in wordsArray
		//Broadcast("word: " + word + "   size: " + len(word));
		if (len(tempString)+len(word)<21)
			tempString := tempString + " " + word;
			//Broadcast(tempString);
		else
			tempContentArray.append(tempString);
			//Broadcast("tempString: " + tempString);
			tempString:=word;
			//i:=i+1; //next line of content
		endif
	endforeach

	//last word
	tempContentArray.append(tempString);

	return tempContentArray;
endfunction

function BookIsEquipped(unused me, book)

	if (GetObjProperty(book, "EquippedAlready"))
		return 1;
	endif
	return 0;
/*
   var weaponone := GetEquipmentByLayer( me,  LAYER_HAND1 );
   var weapontwo := GetEquipmentByLayer( me,  LAYER_HAND2 );

   if ( ( weaponone == book ) or (weapontwo == book ) )
      return 1;
   else
      return 0;
   endif
*/
endfunction