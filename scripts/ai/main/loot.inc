
const STARTHAIR := 0x2030;
const ENDHAIR := 0x2060;

function grabloot()
	if( GetGlobalProperty( "NoNpcLooting" ) )
		return;
	endif
	var numitems := 0;
	var mypack := 0;
	if (!GetObjProperty(me,"serial"))
		SetObjProperty(me, "serial", me.serial);
	endif

	mypack := me.backpack;
	if( !mypack )
		var bpack := CreateItemAtLocation( me.x, me.y, me.z, 0x0e75, 1 );
		if( bpack )
			if( EquipItem( me, bpack ) )
				mypack := bpack;
			else
				return;
			endif
		else
			bpack := CreateItemAtLocation( me.x, me.y, me.z, 0x0e75, 1 );
			if( bpack )
				if( EquipItem( me, bpack ) )
					mypack := bpack;
				else
					return;
				endif
			else
				return;
			endif
		endif
	endif

	var near_items := ListItemsNearLocation(me.x, me.y, me.z,6);

   foreach myitem in near_items

    	if ((GetObjProperty(myitem,"houseserial")!=error) || (myitem.multi) || (!myitem.movable && myitem.objtype != UOBJ_CORPSE))
			continue;
		endif


		if( CheckLineOfSight( me, myitem ) )
			var sh := GetStandingHeight( CInt(myitem.x), CInt(myitem.y), CInt(myitem.z) );
			if (sh.multi || sh.multi.serial)
				continue;
			endif
			/*var info := CInt(GetMapInfo( CInt(myitem.x), CInt(myitem.y) ).landtile);
			if ((info > CInt(0x405)) && (info < CInt(0x456)))
				continue;
			endif*/

			if ( (myitem.movable) && ((myitem.objtype < STARTHAIR || myitem.objtype > ENDHAIR))  )
				if (GetObjProperty(myitem,"houseserial")!=error)
					continue;
				endif
				RunToIt(me,myitem);
				if ( Distance(me, myitem)<2)
					if (playeritem(myitem, mypack))
						say( "*yoink*" );
						sleep(2);
						return;
					endif
				else
					return;
				endif
			elseif (myitem.objtype == UOBJ_CORPSE)

	 		   if (!GetObjProperty(myitem,"#ignoreit") && !GetObjProperty( myitem, "nocut" ) )
					var items := { };

					foreach thingie in EnumerateItemsInContainer(myitem)
						if (    (thingie.objtype < STARTHAIR || thingie.objtype > ENDHAIR)
						&& (!CInt(GetObjProperty(thingie,"#ignoreit"))) )
							items.append( thingie );
						endif
					endforeach

					if ( len(items) )
						RunToIt(me,myitem);
						if (Distance(me, myitem) < 2)
							foreach thingie in items
				   			 if ( playeritem(thingie, mypack) )
					    			var looters := GetObjProperty( myitem, "LootedBy" );
									if( !looters )
										looters := {};
									endif

									if( !(me.name in looters) )
										looters.append( me.name );
										SetObjProperty( myitem, "LootedBy", looters );
									endif

									say( "*yoink*" );
									sleep(2);
									return;
				    			else
									SetObjProperty(thingie,"#ignoreit",1);
				    			endif
							endforeach
						endif
					else
						SetObjProperty(myitem,"#ignoreit",1);
					endif

		   	 endif
				endif

			endif
     	endforeach

endfunction

function RunToIt(me,item)

	var numsteps;
	for (numsteps := 0; numsteps < 8; numsteps := numsteps +1)
		if ( Distance(me, item) < 2 )
			return;
		else
			RunToward(item);
		endif
	endfor

endfunction

function playeritem(byref item, byref mypack)

SetObjProperty( item, "dropitem", 1 );

var usescript:=GetObjProperty( item, "old_us");
if(usescript)
	EraseObjProperty( item, "old_us");
	item.usescript := CStr(usescript);
else
	item.usescript := "";
endif

return MoveItemToContainer(item, mypack);

endfunction