use util;
include "include/random";
include "include/attributes";
include "include/starteqp";
include "include/dist";

const MAP_TIME := 600; //10 mins
const ADDITIONAL_LOOT_CHEST_TIME := 180; //3 mins
//items last for MAP_TIME + ADDITIONAL_LOOT_CHEST_TIME;

function createItemStruct(itemtemplate, x, y, z)
	var itemstruct := struct;
	itemstruct.+template:=itemtemplate;
	itemstruct.+x:=x;
	itemstruct.+y:=y;
	itemstruct.+z:=z;
	return itemstruct;	
endfunction

function MakeMonsters(difficulty, chief, monsters, x, y, z)
	var i;
	var guardians := array;
	//Broadcast("making chief " + chief);
	//Broadcast("locations : " +x + ", " +  y + ", " + z);
	for (i:=0; i<difficulty; i:=i+1)
		guardians.append(CreateGuardian(chief, x, y, z));
	endfor
	foreach m in monsters
		//Broadcast("making monster " + m);
		guardians.append(CreateGuardian(m, x, y, z));
	endforeach
	return guardians;
endfunction


function CreateGuardian( template, x, y, z )
	var nasty := CreateNpcFromTemplate( template, x, y, z);
	if( nasty )
		SetObjProperty( nasty, "untamable", 1 );
		SetObjProperty( nasty, "guardkill", 1 );
		SetObjProperty( nasty, "noloot", 1 );
		SetObjProperty( nasty, "nocut", 1 );
		SetObjProperty( nasty, "guardignore", 1);
		SetObjProperty( nasty, "NecroProtection", 8);
		SetObjProperty( nasty, "createdxyz", {x,y,z});

		var anchor	:= struct;
		anchor.+x	:= x;
		anchor.+y	:= y;
		anchor.+range	:= 10;
		anchor.+psub	:= 25;
		SetObjProperty( nasty, "Anchor", anchor);
	endif
	return nasty.serial;
endfunction


function MakeItems(items)
	var myitems := array;
	foreach item in items
		var current_items := ListItemsAtLocation( item.x, item.y, item.z );
		foreach thing in current_items
			DestroyItem( thing );
		endforeach
		var anitem := CreateItemAtLocation( item.x, item.y, item.z, item.template);
		anitem.movable := 0;
		anitem.decayAt := CInt(ReadGameClock()) + MAP_TIME + ADDITIONAL_LOOT_CHEST_TIME;
		myitems.append(anitem.serial);
	endforeach
	//Broadcast("created these items " + myitems);
	return myitems;
endfunction

function KillAllMonsters(spawnedmonsters)
	foreach monstser in spawnedmonsters
		var monst := SystemFindObjectBySerial(monstser);
		monst.hidden:=1;
		SetObjProperty( monst, "guardkill", 1);
		ApplyRawDamage( monst, CInt(GetHp(monst)+3) );
	endforeach
endfunction


function DestroyAllItems(spawneditems)
	foreach itemser in spawneditems
		var item := SystemFindObjectBySerial(itemser);
		if (item)
			DestroyItem(item);
		endif
	endforeach

endfunction

function MakeChest(lootgroup, title, difficulty, colour, x, y, z)
		var chest := CreateItemAtLocation( x, y, z, "metalchest", 1 );
		SetObjProperty( chest, "treasurechest", 1 );
		chest.saveonexit := 0;
		chest.decayAt	 := CInt(ReadGameClock()) + 120;
		chest.locked	 := 0;
		chest.movable := 0;
		chest.color := colour;
		var i;
		for (i:=0; i<difficulty; i:=i+1)
			MakeLoot( chest, CInt(lootgroup), 100, (7+(3*difficulty)) );
		endfor
		//var nextmap:= CreateItemInContainer( chest, 0x6000 );
		////Broadcast("current maps difficulty: " + difficulty);
		//SetObjProperty (nextmap, "difficulty", difficulty+1);
		PrintTextAbove( chest, "The hidden treasures of " + title + " have been found!", 3, 53 );
		sleep(60);
		PrintTextAbove( chest, "The chest begins to fade!", 3, 53 );
		sleep(30);
		PrintTextAbove( chest, "The chest slowly fades away!", 3, 53 );
		sleep(30);
		DestroyItem( chest );
endfunction

function MakeMapPlaceHolder(spawnedmonsters, spawneditems, difficulty, title, loot, colour, who, x, y, z)
	var level6mapplaceholder := CreateItemAtLocation( x, y, z, "level6mapplaceholder", 1 );
	//level6mapplaceholder.decayAt	 := CInt(ReadGameClock()) + MAP_TIME + ADDITIONAL_LOOT_CHEST_TIME; //13 minutes from now -- 10 mins to complete 3 mins to loot
	
	SetObjProperty(level6mapplaceholder, "title", title);
	SetObjProperty(level6mapplaceholder, "timer", CInt(MAP_TIME/10));
	SetObjProperty(level6mapplaceholder, "difficulty", difficulty);
	SetObjProperty(level6mapplaceholder, "spawnedMonsters", spawnedmonsters);
	SetObjProperty(level6mapplaceholder, "spawnedItems", spawneditems);
	SetObjProperty(level6mapplaceholder, "loot", loot);
	SetObjProperty(level6mapplaceholder, "colour", colour);
	SetObjProperty(level6mapplaceholder, "whoserial", who.serial);
	return level6mapplaceholder;
endfunction

function RunPlaceHolderMapCheck(mylevel6mapplaceholder)
	//Broadcast("RUNNING THE MAP!!!");
	//SetObjProperty(mylevel6mapplaceholder, "running", 1);
	var timer := GetObjProperty(mylevel6mapplaceholder, "timer");
	var spawnedmonsters := GetObjProperty(mylevel6mapplaceholder, "spawnedMonsters");
	var num_monsters_left := len(spawnedmonsters);
	var monst;
	while (timer >0 && num_monsters_left>0)
		num_monsters_left:=0;
		foreach m in spawnedmonsters
			monst := SystemFindObjectBySerial(m);
			if (monst || GetHP(monst))
				num_monsters_left:=num_monsters_left+1;
				if (CoorDist(monst.x, monst.y, mylevel6mapplaceholder.x, mylevel6mapplaceholder.y)>20)
					var howmanyloopsout:=GetObjProperty(monst, "timesaway");
					if (!howmanyloopsout||howmanyloopsout==0)
						SetObjProperty(monst, "timesaway", 1);
					endif
					if (howmanyloopsout>4)
						PrintTextAbove(monst, "Kal Ort Por");
						PlaySoundEffect( monst, SFX_210 );
						sleepms(250);
						MoveObjectToLocation( monst, mylevel6mapplaceholder.x, mylevel6mapplaceholder.y, mylevel6mapplaceholder.z, _DEFAULT_REALM, MOVEOBJECT_FORCELOCATION); //AutoConversion
						PlaySoundEffect( monst, SFX_SPELL_RECALL );
						EraseObjProperty(monst, "timesaway");
					else
						SetObjProperty(monst, "timesaway", CInt(howmanyloopsout+1));
					endif
				endif
			endif
		endforeach
		timer:= timer-1;
		//Broadcast("time remaining : " + timer);
		//Broadcast("num_monsters_left : " + num_monsters_left);
		SetObjProperty(mylevel6mapplaceholder, "timer", timer);
		sleep(10);
	endwhile
	//DestroyItem(mylevel6mapplaceholder);
	if (timer == 0)
		LostMap(mylevel6mapplaceholder);
		return 0;	//didnt do it in time
	endif
	WonMap(mylevel6mapplaceholder);
	return 1; //did it
	
endfunction

function LostMap(mylevel6mapplaceholder)
	var title := GetObjProperty(mylevel6mapplaceholder, "title");
	var whoser := GetObjProperty(mylevel6mapplaceholder, "whoserial");
	var who := SystemFindObjectBySerial(whoser);
	SendSysMessage( who, "You have taken too long to defeat the guardians of " + title +", and the magic attracting them to this place has dispersed!",3,53);

	DestroyItem(mylevel6mapplaceholder);
endfunction

function WonMap(mylevel6mapplaceholder)
	var title := GetObjProperty(mylevel6mapplaceholder, "title");
	var difficulty := GetObjProperty(mylevel6mapplaceholder, "difficulty");
	var loot := GetObjProperty(mylevel6mapplaceholder, "loot");
	var colour := GetObjProperty(mylevel6mapplaceholder, "colour");
	var whoser := GetObjProperty(mylevel6mapplaceholder, "whoserial");
	var spawneditems := GetObjProperty(mylevel6mapplaceholder, "spawnedItems");
	//spawn chest
	var who := SystemFindObjectBySerial(whoser);
	SendSysMessage( who, "Now that all guardians were defeated, the treasure is revealed!",3,70 );
	DestroyAllItems(spawneditems);
	MakeChest(loot, title, difficulty, colour, mylevel6mapplaceholder.x, mylevel6mapplaceholder.y, mylevel6mapplaceholder.z);
	DestroyItem(mylevel6mapplaceholder);
endfunction