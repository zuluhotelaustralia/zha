//
// securechest.inc: var bankbox functions
//
// public functions:
//   FindSecureChest( chestserial )      returns securechest storage are for a chest
//   WipeSecureChest( chestserial )     does what you might think.
//   OpenSecureChestStorage()              returns handle to SecureChestStorageArea.
//

use os;

include "include/objtype";

function FindSecureChest( chestserial )

	var storagearea := OpenSecureChestStorage();
	var secchestareaname := "SecureChest " + chestserial;
	var securechest := FindRootItemInStorageArea( storagearea , secchestareaname );

	if (!securechest )
		securechest := CreateRootItemInStorageArea( storagearea, secchestareaname, UOBJ_BANKBOX );
	endif

	// should never happen.  FIXME make sure and document that fact
	if (!securechest )
		syslog( "Unable to find or create securechest " + chestserial);
	endif

	return securechest;

endfunction

function WipeSecureChest( chestserial )

	var storagearea := OpenSecureChestStorage();

	var secchestareaname := "SecureChest " + chestserial;

	return DestroyRootItemInStorageArea( storagearea , secchestareaname );

endfunction

function OpenSecureChestStorage()

	var chests := FindStorageArea( "Secure Chests" );
	if (!chests)
		chests := CreateStorageArea( "Secure Chests" );
	endif

	// this should not happen. FIXME, make sure and document that fact.
	if (!chests)
		syslog( "Unable to open or create secure chest storage!" );
	endif

	return chests;

endfunction