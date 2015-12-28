use uo;
use util;

//include "include/classes";



function ArcheryAttack( attacker, defender ) 
	var weaponType := cfg[attacker.weapon.objtype];
	//if (!coordDistance (attacker.x, attacker.y, attackx, attacky))
	//endif
	if (!checkForAndDecrementAmmunition( attacker, weaponType))
		return;
	endif
	if (! ValidArcheryWeapon(weaponType))
		return;
	endif
	DoBowAttack(attacker, defender, weaponType);
endfunction 

function DoBowAttack(attacker, defender, weaponType, TO_HIT_SKILLID:= SKILLID_ARCHERY)
	PlayMovingEffectXYZ( attacker.x, attacker.y, attacker.z+9, defender.x, defender.y, defender.z+5, weaponType.ProjectileAnim, 2, 0, 0 );
	PerformAction( attacker, weaponType.Anim);
	sleepms((100-weaponType.Speed)/10);
	if (CheckArcheryHit(attacker, defender, TO_HIT_SKILLID))
		PlaySoundEffect( defender, weaponType.hitSound );
	else
		PlaySoundEffect( attacker, weaponType.projectileSound );
		return;
	endif


	//Broadcast("weaponType.hitscript : " + weaponType.hitscript + "  dmg mod: " +  attacker.weapon.dmg_mod);
	var hitscript := attacker.weapon.hitscript;
	//Broadcast("hitscript: " + hitscript);
	if (!hitscript)
		//Broadcast("cant fint it!");
		return;
	endif
	var wbasedamage:= RandomDiceRoll( weaponType.damage ); 
	if (attacker.weapon.dmg_mod)
		wbasedamage := wbasedamage + attacker.weapon.dmg_mod;
	endif
	//further bonus damage based on archery skill - temporarily removed pending testing
	var attackers_archery := GetEffectiveSkill(attacker,TO_HIT_SKILLID);
	var dambonus := 1 + (attackers_archery/65); //i.e. at 130 theyd do 3x times base... 
	wbasedamage := CInt(wbasedamage * dambonus);
	
	//find random piece of armor to hit -- need this for onhit script
	var wornarmor := array;
	foreach li in ListEquippedItems(defender)
		if (li.isA(POLCLASS_ARMOR) && GetObjProperty(li, "OnHitScript"))
			wornarmor.append(li);
		endif
	endforeach
	var def_armor;
	if (len(wornarmor)>0)
		def_armor:=wornarmor[RandomInt(len(wornarmor))+1];
	endif
	start_script (":combat:"+hitscript, {attacker, defender, attacker.weapon, def_armor, wbasedamage, 0});
endfunction

function CheckArcheryHit(attacker, defender, TO_HIT_SKILLID:= SKILLID_ARCHERY)
	
	var defenders_ar := CInt(1.2 * defender.ar); //based on the idea that monsters dont tend to have more than 100 AR
	var rangeToDef :=Distance (attacker, defender);
	//Broadcast("rangeToDef " + rangeToDef);
	var normalrange := GetObjProperty(attacker.weapon, "NormalRange");
	//Broadcast("normalrange: " + normalrange);
	
	if (!normalrange)
		normalrange:=0;
	endif
	
	if (rangeToDef > normalrange)
		rangeToDef:=rangeToDef + ((rangeToDef-normalrange) * 5); //target past normal range is much harder :)
	endif
	
	var difficulty := defenders_ar + rangeToDef;
	var points:= difficulty * 10;
	if (points > 1300)
		points := 1300;
	endif
	
	//random component
	var randdiff := RandomDiceRoll("5d12");
	if (randdiff <=30)
		difficulty := difficulty - randdiff;
	else
		difficulty := difficulty + randdiff;
	endif
	
	//difficulty reduction for dex
	difficulty := difficulty - CInt(attacker.dexterity/4);
	
	if (difficulty<0)
		difficulty:=0;
	elseif(difficulty>150)
		difficulty:=150;
	endif

	//Broadcast("difficulty : " + difficulty + "    points : " + points + "   rangeToDef: " + rangeToDef);
	return CheckSkill( attacker, TO_HIT_SKILLID, difficulty, points);
	
endfunction


function checkForAndDecrementAmmunition( who, weaponType )
	var mypack := EnumerateItemsInContainer( who.backpack );
	foreach item in mypack
		if( item.objtype == weaponType.ProjectileType )
			if (SubtractAmount( item, weaponType.Projectile))
				return 1;
			endif
		endif
	endforeach
	return 0;
endfunction

function ValidArcheryWeapon(weaponType)
//returns true if itemdesc elem contains the miniumum requires props for processing
	if (!weaponType.ProjectileAnim || !weaponType.Anim || !weaponType.hitSound || !weaponType.projectileSound ||
	!weaponType.hitscript || !weaponType.damage || !weaponType.Projectile || !weaponType.ProjectileType )
		return 0;
	endif
	return 1;
endfunction