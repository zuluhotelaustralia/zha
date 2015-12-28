// These are all the speckled/pure black colours, which are illegal on ZHA for uniforms/hair dye
/*
Barns 24/6/04
Added these
1184-1200   	4a0-4b0
1379-1400	563-578
1455-1500	5af-5dc
1555-1600	613-640
1655-1700	677-6a4

*/

function isIllegalColour(colour)
	if (  	   (colour <= 0x1)
	   	|| ((colour>=0x423) && (colour<=0x44D))
	   	|| ((colour>=0x4E7) && (colour<=0x4FF))
	   	|| ((colour>=0x54B) && (colour<=0x562))
	   	|| ((colour>=0x6DB) && (colour<=0x708))
	   	|| ((colour>=0x775) && (colour<=0x7D0))
	   	|| ((colour>=0x7E3) && (colour<=0x834))
	   	|| ((colour>=0x853) && (colour<=0x898))
	   	|| ((colour>=0x8B1) && (colour<=0x8FC))
	   	|| ((colour>=0x90F) && (colour<=0x960))
	   	|| (colour>=0x97F) || (colour==0x521))
	   	
	   	return 1;
	else
		return 0;
	endif
endfunction

/*function isIllegalColour(colour)
	if (  	   (colour <= 0x1)
	   	|| ((colour>=0x423) && (colour<=0x44D))
	   	|| ((colour>=0x4A0) && (colour<=0x4B0))
	   	|| ((colour>=0x4E7) && (colour<=0x4FF))
	   	|| ((colour>=0x54B) && (colour<=0x578))
	   	|| ((colour>=0x5AF) && (colour<=0x5DC))
	   	|| ((colour>=0x613) && (colour<=0x640))
	   	|| ((colour>=0x677) && (colour<=0x6A4))
	   	|| ((colour>=0x6DB) && (colour<=0x708))
	   	|| ((colour>=0x775) && (colour<=0x7D0))
	   	|| ((colour>=0x7E3) && (colour<=0x834))
	   	|| ((colour>=0x853) && (colour<=0x898))
	   	|| ((colour>=0x8B1) && (colour<=0x8FC))
	   	|| ((colour>=0x90F) && (colour<=0x960))
	   	|| (colour>=0x97F) || (colour==0x521))
	   	
	   	return 1;
	else
		return 0;
	endif
endfunction
*/