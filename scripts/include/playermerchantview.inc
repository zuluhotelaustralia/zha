// inc file for merchant container browsing
// 23-March-2001
// Maintainer Shurugwi
// natie@locktrinity.com

var layout := {
"page 0",
"nodispose",
"resizepic 25 0 2600 700 600",
"resizepic 45 60 5100 660 25",
"resizepic 45 545 5100 660 7",
"text 150 25 995 0",
"text 55 63 10 1",
"text 270 63 10 2",
"text 485 63 10 3",
"button 610 558 2130 2129 1 1 4", // Close Button
"text 75 558 17 5",
"page 1"
};

var data := {
"",
"Container",
"",
"",
"",
"Thank you for browsing our goods!",
""
};

function browseplayermerchantstock(parms)

    var who := parms[1];
    var merchant := parms[2];
    var inv_fs := parms[3];

    while(ShowStock(who, merchant, inv_fs) == 1);
	sleepms(250);
    endwhile

endfunction

function ShowStock(who, merchant, inv_fs)

    data[1] := "Containers for " + merchant.name;

    var displayx := 35;
    var displayy := 88;

    var containers := ListRootItemsInContainer(inv_fs);
    var container;
    var itemno := 6;
    var pagecount := 1;
   
	foreach container in containers

    	    if( container.isa( POLCLASS_CONTAINER ) || container.objtype == 0xefa )

    	    	itemno := itemno + 1;

		layout.append("text " + (displayx+60) + " " + displayy + " 995 " + (itemno - 1));
		layout.append("button " + (displayx + 35) + " " + displayy + " 2118 2117 1 1 " + container.serial);
		layout.append("tilepic " + displayx + " " + displayy + " " + container.graphic);

		if(container.name)
		    data[itemno] := container.name;
		  else
		    data[itemno] := container.desc;
		endif

		displayy := displayy + 25;

		if(itemno == 24)
		    data[3] := "Container";
		    displayx := displayx + 215;
		    displayy := 88;
		    layout.append("resizepic 255 85 5100 7 460");

		  elseif(itemno == 42)
		    data[4] := "Container";
		    displayx := displayx + 215;
		    displayy := 88;
		    layout.append("resizepic 470 85 5100 7 460");

		  elseif(itemno == 60)
		    itemno := 6;
		    pagecount := pagecount + 1;
		    layout.append("button 550 440 5540 5541 0 " + pagecount);
		    layout.append("page " + pagecount);
		    layout.append("button 460 440 5537 5538 0 " + (pagecount - 2));
		    layout.append("tilepic " + 35 + " " + 63 + " " + container.graphic);
		    displayx := 35;
    		    displayy := 88;
		endif
	    endif

	endforeach

    if( itemno == 6 )
    	//SendSysmessage(who, "I currently hold no containers for thee to browse.",3,48);
    	say("I currently hold no containers for thee to browse.");
    	//SendSysmessage(who, "This merchant currently holds no containers.",3,48);
    	return 0;
    endif

    var res :=    SendDialogGump( who, layout, data );

    if(res[0] > 4)
    	ShowContents(res[0], who);
    	return 1;
      else
      	return 0;
    endif

endfunction

function ShowContents(itemserial, who)

	var container := SystemFindObjectBySerial(CInt(itemserial),2);
	if(!container)
	    SendSysMessage(who, "Could not find the container.");
	    return;
	endif

var detlayout := {
"page 0",
"nodispose",
"resizepic 25 0 2600 700 500",
"resizepic 45 60 5100 660 25",
"resizepic 45 445 5100 660 7",
"text 150 25 995 0",
"text 75 63 10 1",
"text 270 63 10 2",
"text 485 63 10 3",
"button 610 458 2130 2129 1 1 4", // Close Button
"text 75 458 17 5",
"page 1"
};

var detdata := {
"Container Contents",
"",
"",
"",
"",
"",
""
};

    detlayout.append("tilepic " + 35 + " " + 63 + " " + container.graphic);

    var items := ListRootItemsInContainer(container);
    var itemno := 6;
    var displayx := 35;
    var displayy := 88;
    var item;
    var pagecount := 2;
    var itemcount := 0;

	foreach item in items

    	    	itemno := itemno + 1;

		detlayout.append("text " + (displayx+60) + " " + displayy + " 995 " + (itemno - 1));
		itemcount := itemcount + 1;

		if( item.isa( POLCLASS_CONTAINER ) || container.objtype == 0xefa )
		    detlayout.append("button " + (displayx + 35) + " " + displayy + " 2118 2117 1 1 " + item.serial);
		endif
		detlayout.append("tilepic " + displayx + " " + displayy + " " + item.graphic);

		if(item.name)
		    detdata[itemno] := item.name;
		  else
		    detdata[itemno] := item.desc;
		endif

		displayy := displayy + 25;

		if(itemno == 20)
		    data[3] := "Container";
		    displayx := displayx + 215;
		    displayy := 88;
		    detlayout.append("resizepic 255 85 5100 7 360");

		  elseif(itemno == 34)
		    data[4] := "Container";
		    displayx := displayx + 215;
		    displayy := 88;
		    detlayout.append("resizepic 470 85 5100 7 360");

		elseif(itemno == 48)
		    itemno := 6;
		    pagecount := pagecount + 1;
		    detlayout.append("button 550 440 5540 5541 0 " + pagecount);
		    detlayout.append("page " + pagecount);
		    detlayout.append("button 460 440 5537 5538 0 " + (pagecount - 2));
		    detlayout.append("tilepic " + 35 + " " + 63 + " " + container.graphic);
		    displayx := 35;
    		    displayy := 88;
		endif

	endforeach

	if(container.name)
	    detdata[2] := container.name + " : " + itemcount + " items.";
	  else
	    detdata[2] := container.desc + " : " + itemcount + " items.";
	endif

    if( itemno == 6 )
    	itemno := itemno + 1;
    	detlayout.append("text " + (displayx+60) + " " + displayy + " 995 " + (itemno - 1));
    	detdata[itemno] := "This container is empty";
    endif

    var res :=    SendDialogGump( who, detlayout, detdata );

    if(res[0] > 4)
    	ShowContents(res[0], who);
    	ShowContents(itemserial, who);
    endif

endfunction























