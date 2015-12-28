

function IsInStaticHouse( x, y, z)


	var houserange1x, houserange1y, houserange2x, houserange2y;
										//	    x		y		 z		x2		y2		z2	
	foreach obj in ListObjectsInBox( x - 100, y - 100, -100, x + 100, y + 100, 200 ) // 300 z range.
		//Broadcast("Object: " + obj.name + " ("  + obj.serial + ")");
		if(obj.isa(POLCLASS_ITEM) && obj.objtype == 0x2000)
			//Broadcast("Static sign: " + obj.name + " ("  + obj.serial + ")");
			if(Cint(GetObjProperty( obj, "static")) == 1)
				//Broadcast("Static sign == 1: " + obj.name + " ("  + obj.serial + ")");
				houserange1x := Cint(GetObjProperty(obj, "tlcx"));
				houserange1y := Cint(GetObjProperty(obj, "tlcy"));
				houserange2x := Cint(GetObjProperty(obj, "brcx"));
				houserange2y := Cint(GetObjProperty(obj, "brcy"));
				
				if ( ( x >= houserange1x ) && ( x <= houserange2x ) )
					if ( ( y >= houserange1y  ) && ( y <= houserange2y ) )
						//Broadcast(obj.serial);
						return obj;
					endif
				endif
				
				houserange1x := Cint(GetObjProperty(obj, "tlc2x"));
				houserange1y := Cint(GetObjProperty(obj, "tlc2y"));
				houserange2x := Cint(GetObjProperty(obj, "brc2x"));
				houserange2y := Cint(GetObjProperty(obj, "brc2y"));
				
				if ( ( x >= houserange1x ) && ( x <= houserange2x ) )
					if ( ( y >= houserange1y  ) && ( y <= houserange2y ) )
						//Broadcast(obj.serial);
						return obj;
					endif
				endif
			endif
		endif
	endforeach
	
	return( 0 );

endfunction

function GetObjectsInStaticHouse( Sign )

	var Objects := array;
	if(Sign.objtype != 0x2000)
		return Objects;
	endif
	
	var houserange1x, houserange1y, houserange2x, houserange2y;
	
	houserange1x := Cint(GetObjProperty(Sign, "tlcx"));
	houserange1y := Cint(GetObjProperty(Sign, "tlcy"));
	houserange2x := Cint(GetObjProperty(Sign, "brcx"));
	houserange2y := Cint(GetObjProperty(Sign, "brcy"));

	foreach obj in ListObjectsInBox(houserange1x, houserange1y, -127, houserange2x, houserange2y, 127)
		Objects.append(obj);
	endforeach
	
	if(Cint(GetObjProperty(Sign, "brc2x") < 1))
		return Objects;
	endif
	

	foreach obj in ListObjectsInBox(houserange1x, houserange1y, -127, houserange2x, houserange2y, 127)
		Objects.append(obj);
	endforeach
	
	return Objects;

endfunction

function IsBanned( house, who)
	
	var bannedlist := GetObjProperty( house, "bannedlist");
	
	foreach banned in bannedlist
		if( banned == who.serial )
			return 1;
		endif
	endforeach
	return 0;
		
	endfunction

function IsStaticOwner( who, house )
	if( who.serial == CInt( GetObjProperty( house, "ownerserial" ) ) )
		return 1;
	elseif( GetObjProperty( house, "owneracct" )==who.acctname )
		return 1;
	elseif(who.cmdlevel>=4) // Admin+
		return 1;	
	endif
	return( 0 );
endfunction

function IsStaticCoOwner( who, house )
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

function IsStaticFriend( who, house )
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