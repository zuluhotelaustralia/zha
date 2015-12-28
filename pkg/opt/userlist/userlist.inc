use os;
use uo;

const EVID_USERLIST_ADD       := 0x00008001;
const EVID_USERLIST_DEL       := 0x00008002;
const EVID_USERLIST_UPDATE    := 0x00008004;
const EVID_USERLIST_ALIASES   := 0x00008008;
const EVID_USERLIST_CLEAR     := 0x00008010;
const EVID_USERLIST_REBUILD   := 0x00008020;
const EVID_USERLIST_MSGBYGUMP := 0x00008040;
const EVID_USERLIST_MSGBYNAME := 0x00008080;
const EVID_USERLIST_LISTEN    := 0x00008100;

function UserList(type, opt1:=0, opt2:=0)
	var manager:=getprocess(GetGlobalProperty("#UserListPID"));
	if(manager)
		case(type)
			EVID_USERLIST_ADD:
			EVID_USERLIST_DEL:
			EVID_USERLIST_UPDATE:
			EVID_USERLIST_CLEAR:
			EVID_USERLIST_REBUILD:
			EVID_USERLIST_MSGBYGUMP:
				manager.SendEvent({type, opt1});
			EVID_USERLIST_ALIASES:
			EVID_USERLIST_MSGBYNAME:
			EVID_USERLIST_LISTEN:
				manager.SendEvent({type, opt1, opt2});
			default:
				syslog("Unknown UserList event type: "+type);
		endcase
	else
		syslog("UserList: manager process can't be retrieved.");
	endif
endfunction

/*
UserList(EVID_USERLIST_ADD, who);                  // to add a user (which is NOT already in the list, checks serial for dupes)
UserList(EVID_USERLIST_DEL, who);                  // to del a user from the list (all records with same serial)
UserList(EVID_USERLIST_UPDATE, who);               // updates a user (it does del and add it)
UserList(EVID_USERLIST_ALIASES, who, number:=num); // adds aliases for a user (and doesn't check for serial dupe)
UserList(EVID_USERLIST_CLEAR);                     // it clears the list (watch out!)
UserList(EVID_USERLIST_REBUILD);                   // it rebuilds the list from scratch, adding every EnumerateOnlineCharacters
UserList(EVID_USERLIST_MSGBYGUMP, who);
UserList(EVID_USERLIST_MSGBYNAME, who, message);
UserList(EVID_USERLIST_LISTEN, who, listen);
*/