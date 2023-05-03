use uo;
use party;

CONST PARTY_MESSAGE_DELAY := 30;
CONST PARTY_MESSAGE_CPROP := "#PartyMessages";


function party_SendPartyMsg(party, message_to, message, override_delay:= 0)

	var message_history := array{};

	if(!override_delay)
		var old_message_history := GetObjProperty(message_to, PARTY_MESSAGE_CPROP);
		if(!old_message_history)
			old_message_history := array{};
		endif

		foreach old_message in old_message_history
			if (message == old_message.message && old_message.timeout > ReadGameClock())
				return 0;
			endif
			if (old_message.timeout <= ReadGameClock())
				continue;
			endif
			message_history.append(old_message);
		endforeach
		message_history.append(struct{message:=message,timeout:=ReadGameClock()+PARTY_MESSAGE_DELAY});
	endif

	SendPartyMsg(party, message_to, message);
	SetObjProperty(message_to, PARTY_MESSAGE_CPROP, message_history);
endfunction