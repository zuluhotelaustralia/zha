use uo;
use util;

//include "include/classes";
include "include/hitscriptinc";


function MeleeAttack( attacker, defender ) 
	
	//if (!coordDistance (attacker.x, attacker.y, attackx, attacky))
	//endif
	DoAttack(attacker, defender, weaponType);
endfunction 

function DoAttack(attacker, defender)
	var attackerweaponType := cfg[attacker.weapon.objtype];
	var attackerweaponType := cfg[defender.weapon.objtype];

	//PlayMovingEffectXYZ( attacker.x, attacker.y, attacker.z+9, defender.x, defender.y, defender.z+5, weaponType.ProjectileAnim, 10, 0, 0 );
	PerformAction( attacker, weaponType.Anim);
	sleep((100-weaponType.Speed)/10);
	var combat_skill_id := GetWeaponSkillID(weaponType.Attribute);
	
	if (combat_skill_id == -1)
		return;
	endif
					
	if (CheckHit(attacker, defender, combat_skill_id))
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
	
	var attackers_skill := GetEffectiveSkill(attacker,combat_skill_id);
	var dambonus := 1 + (attackers_skill/130); //i.e. at 130 theyd do 2x times base... 
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

function CheckHit(attacker, defender, combat_skill_id)
	
	var defenders_ar := CInt(1.2 * defender.ar); //based on the idea that monsters dont tend to have more than 100 AR
		
	var difficulty := defenders_ar ;
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
	//difficulty := difficulty - CInt(attacker.dexterity/4);
	
	if (difficulty<0)
		difficulty:=0;
	elseif(difficulty>150)
		difficulty:=150;
	endif

	//Broadcast("difficulty : " + difficulty + "    points : " + points + "   rangeToDef: " + rangeToDef);
	return CheckSkill( attacker, combat_skill_id, difficulty, points);
	
endfunction

function GetWeaponSkillID(weaponTypeAttribute)
	if (weaponTypeAttribute=="Mace")
		return 41;
	elseif	(weaponTypeAttribute=="Swords")
		return 40;
	elseif	(weaponTypeAttribute=="Fencing")
		return 42;
	
	/*elseif	(weaponTypeAttribute=="Wrestling")
		return 43;
	*/
	endif
	return 43; 
endfunction