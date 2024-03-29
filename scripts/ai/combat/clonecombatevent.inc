
include "include/random";
//include "include/attributes";

function process_combat_event(ev)

	if (spellcast)
		spellattack(ev.source);
	endif

	if (GetHp(me) < GetMaxHp(me)/2)
		ApplyHealing();
	endif

        if (ev.type == SYSEVENT_DAMAGED)
		if ((GetHp(me) * 100/GetMaxHp(me)) < flee_point)
			EraseObjProperty(me,"#flees");
			flee(ev.source);
		endif
	endif

endfunction

function in_combat_event_loop(opponent)

	if (spellcast)
		spellattack(opponent);
	endif

	if (GetHp(me) < GetMaxHp(me)/2)
		ApplyHealing();
	endif

endfunction

function post_combat()

	DisableCombatEvents();
	EnableMainEvents();

	SetWarMode( 0 );
	SetOpponent( 0 );

	sleep(1);
	look_around();

endfunction

function prepare_for_fight(opponent)

	DisableMainEvents();

	var weaponone := GetEquipmentByLayer( me,  LAYER_HAND1 );
	var weapontwo := GetEquipmentByLayer( me,  LAYER_HAND2 );
	if ( (weaponone.objtype == 0x13b2) || (weapontwo.objtype == 0x13b2) )
		runaway:=0;
		CreateItemInInventory(me.backpack,0xf3f,50);
	elseif ( (weaponone.objtype == 0x0f4f) || (weapontwo.objtype == 0x0f4f) ||
		(weaponone.objtype == 0x13fd) || (weapontwo.objtype == 0x13fd) )
		runaway:=0;
		CreateItemInInventory(me.backpack,0x1bfb,50);
	elseif ( (weaponone.objtype == 0x6050) || (weapontwo.objtype == 0x6050) )
		runaway:=0;
		CreateItemInInventory(me.backpack,0x6051,25);
	else
		runaway := 100;
	endif

	if ( (!weaponone) && (!weapontwo) )
		spellcast := 1;
	elseif	( (!weaponone.maxhp) && (!weapontwo.maxhp) )
		spellcast := 1;
	else
		spellcast := 0;
	endif

	EnableCombatEvents();

endfunction

function ApplyHealing()

	if ( GetObjProperty(me,"#lasthealed") > ReadGameClock())
		SetHp(me, GetHp(me) + (Random(GetMaxHp(me) - GetHp(me))+1));
		SetObjProperty(me,"#lasthealed", ReadGameClock()+20);
	endif

endfunction

function spellattack(opponent)

	if (!GetObjProperty(me,"#lastbreath"))
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);
	endif

	if (GetObjProperty(me,"#lastbreath") > ReadGameClock() )
		return;
	endif

	if (Random(2)==1)
		sleep(1);
		cast_offensive_spell(me,opponent);
	endif

	SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);

endfunction