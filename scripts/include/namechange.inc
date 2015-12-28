use datafile;

include "include/time";

function openFile()
	var dfile := OpenDataFile("nameChanges");
	if (!dfile)
		Broadcast("Creating nameChanges file...");
		dfile := CreateDataFile( "nameChanges", DF_KEYTYPE_INTEGER );
	endif
	
	return dfile;
endfunction

function closeFile()
	UnloadDatafile("nameChanges");
endfunction

// param: who - whose name to record.
// param: by  - either a string (eg. "Name-change Stone", or a mobile-ref.)
function addNameChange(who, by)
	var nameChange := struct;
	nameChange .+ name;
	nameChange .+ date;
	nameChange .+ whoby;
	
	nameChange.name := who.name;
	nameChange.date := NowString();
	if (by.isA(POLCLASS_MOBILE))
		nameChange.whoby	:= by.name + " (" + CStr(by.serial) + ")";
	else
		nameChange.whoby	:= by; // it's a string.
	endif
	
	var dfile := openFile();
	var elem;
	if (!(who.serial in dfile.Keys()))
		elem := dfile.CreateElement(who.serial);
	else
		elem := dfile.FindElement(who.serial);
	endif
	
	var changeList := array;
	if ("changeList" in elem.PropNames())
		changeList := elem.GetProp("changeList");
		changeList.append(Pack(nameChange));
	else
		changeList[1] := Pack(nameChange);
	endif
	
	elem.SetProp("changeList", changeList);
	closeFile();
endfunction

// returns: a list of nameChange structs.
function getNameChanges(who)
	var dfile := openFile();
	
	var elem;
	if (!who.serial in dfile.Keys())
		elem := dfile.CreateElement(who.serial);
	else
		elem := dfile.FindElement(who.serial);
	endif
	
	var changeList := array;
	if ("changeList" in elem.PropNames())
		changeList := elem.GetProp("changeList");
	endif
	
	closeFile();
	
	var unpackedList := array;
	foreach change in changeList
		unpackedList.append(Unpack(change));
	endforeach
	
	return unpackedList;
endfunction

function removeNameChange(who, index)
	var dfile := openFile();
	
	var elem;
	if (!who.serial in dfile.Keys())
		elem := dfile.CreateElement(who.serial);
	else
		elem := dfile.FindElement(who.serial);
	endif
	
	var changeList := array;
	if ("changeList" in elem.PropNames())
		changeList := elem.GetProp("changeList");
	endif
	
	if ((index > 0) && (index <= len(changeList))
		changeList.erase(index);
		elem.SetProp("changeList", changeList);
	endif
	
	closeFile();
endfunction

// COMPLETELY clears changes. Should only be used by administrator 
function clearChanges(who)
	var dfile := openFile();
	
	var elem;
	if (!who.serial in dfile.Keys())
		elem := dfile.CreateElement(who.serial);
	else
		elem := dfile.FindElement(who.serial);
	endif
	
	var changeList := array; // empty array.
	elem.SetProp("changeList", changeList);
	
	closeFile();
endfunction