use uo;
use util;

/*
const POISON_GAS_ANIM:=4410;
const POISON_GAS_SFX:=0x48;

const ASTRAL_ANIM:=8912;
const ASTRAL_SFX:=0x1D7;

const ICE_ANIM:=9006;
const ICE_SFX:=0;

const LAVA_ANIM:=13401;
const LAVA_SFX:=0x35A;

const EXPLOSION_ANIM:=14000;
const EXPLOSION_SFX:=0;

const LG_FIREBALL_ANIM:=14036;
const SM_FIREBALL_ANIM:=14052;

const FIRESNAKE_ANIM:=14068;
const FIRESNAKE_SFX:=0x348;

const EXPLOSIONBALL_ANIM:=14078;
const SMOKE_ANIM:=14120;
const SPARKLE_ANIM:=14138;


const RED_SPARKLE_ANIM:=14155;

const ENERGY_ANIM:=14360;
const ENERGY_SFX:=0x213;
*/



function GetRandomAttackAnim()
	var animarr:= { ANIM_ATTACK_1HAND_WIDE, ANIM_ATTACK_1HAND_JAB, ANIM_ATTACK_1HAND_DOWN};
	return animarr[RandomInt(animarr.size())+1];
endfunction



function DoSpecialMonsterAttack(attacker, defender, weaponType, specialAttackEffect)
	var attackskill := GetWeaponSkillID(weaponType.Attribute);
	//PerformAction(attacker, GetRandomAttackAnim());
	if (CheckHit(attacker, defender, attackskill))
		PlayMovingEffectXYZ( attacker.x, attacker.y, attacker.z+9, defender.x, defender.y, defender.z+5, specialAttackEffect, 6, 0, 0 );

		PlaySoundEffect( defender, weaponType.hitSound );
		var hitscript := attacker.weapon.hitscript;

		var wbasedamage:= RandomDiceRoll( weaponType.damage );
		if (attacker.weapon.dmg_mod)
			wbasedamage := wbasedamage + attacker.weapon.dmg_mod;
		endif
		//further bonus damage based on tactics weapon and strength skill
		var attackers_wepskillval := GetEffectiveSkill(attacker,attackskill );
		var attackers_tacticsval := GetEffectiveSkill(attacker,SKILLID_TACTICS );

		var dambonus := 1 + (attackers_wepskillval /65) + (attackers_tacticsval /65);
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
		start_script (hitscript, {attacker, defender, attacker.weapon, def_armor, wbasedamage, 0});
		else
			PlaySoundEffect( attacker, weaponType.missSound );
		endif
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