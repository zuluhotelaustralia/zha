use npc;
use cfgfile;

var speech_cfg := ReadConfigFile("speechgroup");


function process_text(inspeech, mood, templatename)
var retval;
var elem;
var tmpword;
var words := SplitWords(inspeech);
elem := speech_cfg[templatename];

foreach word in words
        var wordgroup := GetConfigInt(elem, mood);
        if (!wordgroup)
                 wordgroup := GetConfigInt(elem, "default");
        endif
        if (!wordgroup)
                wordgroup := templatename;
        endif
         elem := speech_cfg[wordgroup];
         tmpword := GetConfigString(elem,  word );
         if (tmpword)
                 retval := tmpword;
         endif
endforeach

if (!retval)
        retval := GetConfigString(elem, "default" );
endif

if ( (!retval) || (retval == "5") )
        retval := "";
endif

return retval;

endfunction

function TakeItem(you,it)

	TurnToward(you);
	if (!GetObjProperty(me,"serial"))
		SetObjProperty(me, "serial", me.serial);
	endif
	var mypack := FindMyPack(me.serial);

	var whatdesc := "";
	if (!len(it.name))
    		whatdesc := it.desc;
    	else
		whatdesc := it.name;
    	endif

	var keyitem := lower("keyitem" + whatdesc);
	var returnthing := lower("return" + whatdesc);

	var reply := GetObjProperty( me, keyitem );
	if (reply)
		TurnToward(you );
		say(reply);
	else
		if (speech)
			if ( GetObjProperty(it,"slave") )
				TurnToward( you );
				GiveQuestieDirections(you, it);
			else
				TurnToward( you );
				say( "Eh? What's this?" );
			endif
		endif
		MoveItemToContainer(it,you.backpack);
		return;
	endif

	var gavething := 0;
	foreach item in EnumerateItemsInContainer(mypack)
		var returnit := GetObjProperty(item,"returnfor");
		if (returnit == returnthing)
			gavething := 1;
			EraseObjProperty(item,"returnfor");
			MoveItemToContainer(item, you.backpack);
		endif
	endforeach

	if (!me.backpack)
		var newbackpack := CreateItemAtLocation(me.x, me.y, me.z, 0xe75,1);
		if (!EquipItem(me, newbackpack))
			return;
		endif
	endif

	foreach item in EnumerateItemsInContainer(me.backpack)
		var returnit := GetObjProperty(item,"returnfor");
		if (returnit == returnthing)
			gavething := 1;
			EraseObjProperty(item,"returnfor");
			MoveItemToContainer(item, you.backpack);
		endif
	endforeach

	if (!gavething)
		return;
	endif

	var newresponse := GetObjProperty( me, "done"+keyitem);
	if (newresponse)
		SetObjProperty( me, keyitem, newresponse);
		EraseObjProperty( me, "done"+keyitem);
	else
		EraseObjProperty(me,keyitem);
	endif

endfunction

function check_speech(text, speaker)

	if (BarkToKeyWord(text,speaker))
		return;
	endif

	if (DefaultHello(text,speaker))
		return;
	endif

	if (!speech)
		return;
	endif

	if (Random(20) != 1 )
		return;
	endif

	if ( GetObjProperty(me,"nofriends") )
		return;
	endif

	if (speaker.cmdlevel)
		return;
	endif

	var myfriends := { };
	var i := 0;

        for( i := 1; i <= 3; i := i+1 )
		myfriends[i] := GetObjProperty(me,"friend"+i);
		if (!myfriends[i])
			break;
		elseif (myfriends[i] == speaker.name)
			if ( (i == 1) || (Random(2)==1) )
				TurnToward( speaker );
				GreetFriend(text,speaker.name,i);
				return;
			else
				TurnToward( speaker );
				ChatAboutFriends(text,speaker, i-1);
				return;
			endif
		endif
	endfor

	//the speaker is not a friend yet
	var maxfriend := i - 1;

	if (i > 3)
		//too many friends, pick a slot at random
		i := Random(3)+1;
		maxfriend := 3;
	endif

	//check loyalty to the friend in that friendslot
	var loyalty := GetObjProperty(me,"loyalty"+i);
	if (!loyalty)
		loyalty:=0;
	endif

	if ( Random(20) <= (loyalty+10) )
		//don't ditch the old friend
		TurnToward( speaker );
		i := Random(maxfriend)+1;
		ChatAboutFriends(text,speaker, i);
		return;
	endif

	var nexthello := GetObjProperty(me,"nexthello");
	if (!nexthello)
		nexthello := ReadGameClock();
	endif
	if ( nexthello <= ReadGameClock() )
		//make a new friend
		TurnToward( speaker );
		SetObjProperty(me,"friend"+i,speaker.name);
		SetObjProperty(me,"loyalty"+i,1);
		SetObjProperty(me,"nexthello",ReadGameClock()+300);
	else
		TurnToward( speaker );
		i := Random(maxfriend)+1;
		ChatAboutFriends(text,speaker, i);
	endif

endfunction

function GreetFriend(unused text, friend, friendnum)

	var loyalty := GetObjProperty(me,"loyalty"+friendnum);
	var nexthello := GetObjProperty(me,"nexthello");

	if (!loyalty)
		loyalty := 0;
	endif

	if (!nexthello)
		nexthello := ReadGameClock();
	endif

	if (loyalty >= 3)
		friend := "My "+ GetFriendTitle(loyalty) + friend;
	endif

	var chatcfg := ReadConfigFile("::friendchat");
	var elem := FindConfigElem( chatcfg, "talktofriend" );
	var spamlist := GetConfigStringArray( elem, "say" );
	var spamline := spamlist[ Random(len(spamlist))+1 ];
	spamline["friendname"] := friend;

	say(spamline);

	if ( Random(8) == 1 )
		loyalty := loyalty + 1;
		if (loyalty > 9)
			loyalty := 9;
		endif
	endif

	if ( nexthello <= ReadGameClock() )
		SetObjProperty(me,"nexthello",ReadGameClock()+600);
		SetObjProperty(me,"loyalty"+friendnum,loyalty);
	endif

endfunction

function ChatAboutFriends(text, speaker, friendnum)

	var loyalty := GetObjProperty(me,"loyalty"+friendnum);
	if (!loyalty)
		loyalty := 0;
	endif
	var friend := GetObjProperty(me,"friend"+friendnum);

	if ( (!friend ) || ( Random(3)==1 ) )
		say(process_text(text, "default",speech));
		return;
	endif

	if (loyalty >= 3)
		friend := "My "+ GetFriendTitle(loyalty) + friend;
	endif

	var chatcfg := ReadConfigFile("::friendchat");
	var elem := FindConfigElem( chatcfg, "talkaboutfriends" );
	var spamlist := GetConfigStringArray( elem, "say" );
	var spamline := spamlist[ Random(len(spamlist))+1 ];

	spamline["charname"] := speaker.name;
	spamline["friendname"] := friend;

	say(spamline);

endfunction

function GetFriendTitle(loyalty)

	case (loyalty)
	3:
	4:
		return "Friend, ";
	5:
		return "Good Friend, ";
	6:
		return "Great Friend, ";
	7:
		return "Old Friend, ";
	8:
		return "Dear Friend, ";
	9:
		return "Dearest Friend, ";
	default:
		return "";
	endcase

endfunction

function YellToFriend(friend)

	if (friend.isA(POLCLASS_NPC))
		return;
	endif

	var myfriends := { };
	var i := 0;
	var afriend := 0;

        for( i := 1; i <= 5; i := i + 1 )
		myfriends[i] := GetObjProperty(me,"friend"+i);
		if (!myfriends[i])
			break;
		elseif (myfriends[i] == friend.name)
			afriend := 1;
			break;
		endif
	endfor

	if (!afriend)
		return;
	endif

	TurnToward(friend);
	var loyalty := GetObjProperty(me,"loyalty"+i);
	if (!loyalty)
		loyalty := 0;
	endif

	var friendname := friend.name + "!";

	if (loyalty >= 3)
		friendname := "My "+ GetFriendTitle(loyalty) + friendname;
	endif

	TurnToward( friend );
	say(friendname);

endfunction

function BarkToKeyWord(text,speaker)

	var words := SplitWords(text);
	var tmpword;
	var theword;
	var resp := 0;
	foreach word in words
		tmpword := GetObjProperty( me, "keyword"+ lower(word));
		if (tmpword)
			TurnToward( speaker );
			say(tmpword);
			theword := lower(word);
		  	resp := 1;
			break;
		endif
	endforeach

	if (!resp)
		return 0;
	endif

	if (!GetObjProperty(me,"serial"))
		SetObjProperty(me, "serial", me.serial);
	endif
	var mypack := FindMyPack(me.serial);

	var returnthing := "return" + theword;
	var doneword := "done" + theword;
	var keyword := "keyword" + theword;

	var gavething := 0;
	foreach item in EnumerateItemsInContainer(mypack)
		var returnit := GetObjProperty(item,"returnfor");
		if (returnit == returnthing)
			gavething := 1;
			EraseObjProperty(item,"returnfor");
			MoveItemToContainer(item, speaker.backpack);
		endif
	endforeach

	if (!me.backpack)
		var newbackpack := CreateItemAtLocation(me.x, me.y, me.z, 0xe75,1);
		if (!EquipItem(me, newbackpack))
			return;
		endif
	endif

	foreach item in EnumerateItemsInContainer(me.backpack)
		var returnit := GetObjProperty(item,"returnfor");
		if (returnit == returnthing)
			gavething := 1;
			EraseObjProperty(item,"returnfor");
			MoveItemToContainer(item, speaker.backpack);
		endif
	endforeach

	if (!gavething)
		return 1;
	endif

	var newresponse := GetObjProperty( me, doneword);
	if (newresponse)
		SetObjProperty( me, keyword, newresponse);
		EraseObjProperty( me, doneword);
	else
		EraseObjProperty( me, keyword);
		RemoveFromArray(me, keyword);
	endif

	return 1;

endfunction

function DefaultHello(unused text,speaker)

	if ( GetObjProperty(me,"#lastgreet") )
		if ( ReadGameClock() > GetObjProperty(me,"#lastgreet") )
			BarkToKeyWord("hi",speaker);
			SetObjProperty(me,"#lastgreet",ReadGameClock()+30);
			return 1;
		else
			return 0;
		endif
	else
		BarkToKeyWord("hi",speaker);
		SetObjProperty(me,"#lastgreet",ReadGameClock()+30);
		return 1;
	endif

endfunction

function GiveQuestieDirections(you, it)

	var questie := SystemFindObjectBySerial(CInt(GetObjProperty(it,"slave")));
	if (!questie)
		say("I don't think you'll ever find who you are looking for.");
		return;
	endif

	var ns_desc := "";
	var ew_desc := "";
	var dir;

	if (questie.y < you.y)
        	ns_desc := "north";
	elseif  (questie.y > you.y)
		ns_desc := "south";
	endif

	if (questie.x < you.x)
		ew_desc := "west";
	elseif  (questie.x > you.x)
		ew_desc := "east";
	endif

	if (ns_desc == "" && ew_desc == "")
		dir := "nearby";
	else
		dir := ns_desc + ew_desc;
	endif

	say("Looking for " + questie.name + "?  I think they are " + dir );

endfunction

function RemoveFromArray( me, keyword )

	var newarray := {};
	var mykeywords := {};
	if ( GetObjProperty(me,"mykeywords") )
		mykeywords := GetObjProperty(me,"mykeywords");
	endif

	foreach word in mykeywords
		if ( word != keyword )
			newarray[ len(newarray)+1 ] := word;
		endif
	endforeach

	SetObjProperty(me,"mykeywords", newarray);

endfunction