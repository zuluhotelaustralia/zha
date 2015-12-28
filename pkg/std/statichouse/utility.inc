include "setup";

function IsOwner( who, house )
	if( who.serial == CInt( GetObjProperty( house, "ownerserial" ) ) )
		return 1;
	elseif( GetObjProperty( house, "owneracct" )==who.acctname )
		return 1;
	elseif(who.cmdlevel>=4) // Admin+
		return 1;	
	endif
	return( 0 );
endfunction

function IsCoOwner( who, house )
	if( who.serial == CInt( GetObjProperty( house, "ownerserial" ) ) )
		return 1;
	elseif( GetObjProperty( house, "owneracct" )==who.acctname )
		return 1;
	elseif(who.cmdlevel>=4) // Admin+
		return 1;
	else
		
		var i;
		var coownerserial := GetObjProperty( house, "coowners" );
		foreach i in coownerserial
			if( i==who.serial )
				Return 1;
			endif
		endforeach
	
	endif
	return( 0 );
endfunction

function IsFriend( who, house )
	if (who.cmdlevel)
		return 1;
	else
		var i;
		var friendserial := GetObjProperty( house, "friends" );
	foreach i in friendserial
		if( i==who.serial )
			Return 1;
		endif
	endforeach
	endif
	return( 0 );
endfunction