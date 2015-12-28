/* Barnabus 
Contains constants for class bonuses
and documentation on bonuses (if relevant)
*/

/*
Notes:
fields.inc -- Mages get TrueMage cprop on fields.

hitscriptinc.inc -- 
ModByDist - Rangers get +1 max range per level before base damage is 1/4 (normally 10). //now RANGER_LEVEL_ARCHERY_RANGE_BONUS
	Rangers do basedamage * ClasseBonus(level) to NPCs if range > 1 and weapon uses Archery
	Warriors do basedamage * ClasseBonus(level) to NPCs. 
	Warriors do basedamage * ClasseBonusPerLevel(level-2) against players.
	Mages have damage they do basedamage/ClasseBonus(level)
	Mages have damage they receive basedamage * ClasseBonus( defender , CLASSEID_MAGE );

DealDamage -
	Warriors take less damage from NPCs: rawdamage/ClasseBonus(level)
	Warriors take less damage from players: rawdamage/ClasseBonus(level-1) and basedamage/ClasseBonus(level-1)

spelldata.inc
BurnSelf-
	Mages do less damage to self: dmg := CInt( dmg / ClasseBonus( caster, CLASSEID_MAGE ) );
	Warriors do more: dmg := CInt( dmg * ClasseBonus( caster, CLASSEID_WARRIOR ) );

IsProtected( caster, cast_on, circle )-
	Warriors find it harder to be protected: circle * ClasseBonus( cast_on, CLASSEID_WARRIOR );

ModifyWithMagicEfficiency( who, val )-

Resisted( caster, cast_on, circle, dmg )
	Mages have typical per level bonus ClasseBonus(level). Their spells harder to resist against, 
	easier for them to resist others. Take less damage after resist.
	Warrior have per level penalty. Their spells easier to resist, harder for them to resist others. 
	Take more damage after resist.

ResistedDefinedSkillGain - 
	Mages have typical per level bonus ClasseBonus(level)
	Warrior have per level penalty

TryToCast - 
	Mages cast faster: spelldelay := CInt( spelldelay / ClasseBonus( caster, CLASSEID_MAGE ) );
	Warriors cast slower:  spelldelay := CInt( spelldelay * ClasseBonus( caster, CLASSEID_WARRIOR ) );
	
//New generic functions could be used instead of some of the above (also in spelldata.inc)
ModifyWithNewClassMagicEfficiency
NewClassSpellCast (new version of TryToCast)

repair.inc
repair_item( character, item, skillid )
	Crafters can repair easier:
	diff := CInt(diff - GetObjProperty(character, CLASSEID_CRAFTER)* 2.5); //modifed so uses CRAFTER_REPAIR_DIFF_BONUS
	and get bonus to arms lore (armslore := CInt( armslore * ClasseBonus( character, CLASSEID_CRAFTER ) );)

vet.inc
TryToVetCure
	Rangers are faster:	delay := CInt( delay / CLASSE_BONUS );
	
TryToVetHeal
	Rangers are faster and heal more: 		delay := CInt( delay / CLASSE_BONUS ); healed := CInt( healed * CLASSE_BONUS );
	
TryToVetRes
	Only Rangers can do this
*/


const CLASSE_BONUS        := 1.5;
const SMALL_BONUS_PER_LEVEL := 0.05;
const BONUS_PER_LEVEL     := 0.1;  				//used in the function ClasseBonus( who, classeid ) 
const LARGE_BONUS_PER_LEVEL  := 0.15;  
const PENALTY_PER_LEVEL := -0.1;
const LARGE_PENALTY_PER_LEVEL := -0.15;

const RANGER_LEVEL_ARCHERY_RANGE_BONUS := 1; 			//1 range per level further range before 1/4 base damage done

const MONK_DISARM_CHANCE := 3;   				// (% chance per level) Used in wrestlinghitscript
const MONK_STUN_CHANCE := 2;	   				// (% chance per level) Used in wrestlinghitscript
const CRAFTER_REPAIR_DIFF_BONUS := 2.5;

const NINJA_BACKSTAB_BONUS_DAMAGE := 5; 			//2 per level.. used in hitscriptinc.inc -- only melee

const GET_STEED_AT_LEVEL :=	8; 				// Level at which Paladins/Deathknights get special steed
const GET_RISE_AT_LEVEL :=	6;  				// Level at which Paladins/Deathknights can auto-res
const GET_ESCAPE_AT_LEVEL := 8; 				// Level at which ninjas get insta recall
const GET_POSSESS_UNDEAD_AT_LEVEL := 7;  			// Level at which necromancers get the possess skill

// replaced next 2 with warrior bonus when using special weaps
//const DEATHKNIGHT_BONUS_DAMAGE :=3; 				// +3 per level from 1st -- only melee
//const DEATHKNIGHT_SPECIAL_WEAPON_BONUS_DAMAGE := 3; 		// +3 dmg per level from bloody/vampiric/leech
const DEATHKNIGHT_SPECIAL_WEAPON_BONUS_EFFECTIVENESS :=3; 	// +3 bonus per level drained when using bloody/vampiric/leech

// replaced next 2 with warrior bonus when using special weaps
//const PALADIN_BONUS_DAMAGE :=3; 				// +3 per level from 1st -- only melee
//const PALADIN_SPECIAL_WEAPON_BONUS_DAMAGE :=3; 			//1 per level from Holy, Silver, Dragonslayer weps
const PALADIN_KARMA_DAMAGE_MOD_DENOMINATOR:=40000; 		// used in karma comparison damage modifier 
//Damage = BaseDamage * (1 + ((PaladinKarma - DefenderKarma) / PALADIN_KARMA_DAMAGE_MOD_DENOMINATOR))  

const MONK_BONUS_DAMAGE :=5; 					// +5 per level - only wrestling (used in wrestlinghitscript)
const MONK_BONUS_DAMAGE_WITH_STAFF :=2;				// +2 dmg per level when using staves
const PRIEST_SPECIAL_WEAPON_BONUS_DAMAGE :=2; 			// +2 per level from Holy and Silver after 5th

const HUMAN_BONUS_ES_PER_LEVEL:= 1;				// +1 ES per level for humans

var VALID_PALADIN_WEAPS := {	3936,				// longsword
				3934, 				// broadsword
				5049, 				// vikingsword
                                9920, 				// lance
                                3939, 				// spear
                                5121, 				// kryss
                                5182, 				// halberd
                                3918};				// bardiche
var VALID_MONK_WEAPS := {	3568, 				// blackstaff
                                3722, 				// quarterstaff
                                3713, 				// shepherdscrook
                                5113,				// gnarledstaff
                                9904};				// martialgloves
                                
//SPELL CASTING DIFFICULTY MODIFIER FOR NEW SPELLS
const PURE_SPELLCASTER_BONUS := 40;
const SEMI_SPELLCASTER_BONUS := 20;

