//Barnabus 1/10/03
//to prevent ppl resing ppl in house doors

function checkIfGhostNearHouseDoor(ghost)
	foreach item in ListItemsNearLocation( ghost.x, ghost.y, ghost.z, 1 )
		if (GetObjProperty(item, "house_serial"))
			return 1;
		endif
	endforeach
	return 0;
endfunction