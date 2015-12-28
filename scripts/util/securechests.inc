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
		securechest := CreateRootItemInStorageArea( securechest, securechest, UOBJ_BANKBOX );
	endif

	// should never happen.  FIXME make sure and document that fact
	if (!securechest )
		syslog( "Unable to find or create securechest " + chestserial);
	endif

	return chestserial;

endfunction

function WipeSecureChest( chestserial )

	var storagearea := OpenSecureChestStorage();

	var secchestareaname := "SecureChest " + chestserial;

	return DestroyRootItemInStorageArea( storagearea , secchestareaname );

endfunction

function OpenSecureChestStorage()

	var bank := FindStorageArea( "World Bank" );
	if (!bank)
		bank := CreateStorageArea( "World Bank" );
	endif

	// this should not happen. FIXME, make sure and document that fact.
	if (!bank)
		syslog( "Unable to open or create world bank!" );
	endif

	return bank;

endfunction