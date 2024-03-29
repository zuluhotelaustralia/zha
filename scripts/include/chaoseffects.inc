use uo;
use os;
use cfgfile;

include "include/client";
//include "include/attributes";
include "include/random";


function CheckForChaosEffect(who, basechance, minchance, avoidance)

// This function checks to see if a chaos effect will happen.
// Basically, it's just a random number generator.  The following
// variables must be passed through:
//     who = victim
//     basechance = the unmodified chance for a chaos effect
//     minchance = the minimum modified chance for a chaos effect
//     avoidance = the # of magery pts needed to reduce chance by 1

	var i;

	i :=Random(100) + 1;
	avoidance := GetEffectiveSkill(who, SKILLID_MAGERY) / avoidance;
	basechance := basechance - avoidance;

	if (basechance < minchance)
		basechance := minchance;
	endif

	if (i <= basechance)
		return 1;
	else
		return 0;
	endif

endfunction


function GenerateRandomChaosEffects(who, what, alterchance, mutatechance, gatechance, divertchance, effects)

// This will generate one or more chaos effects.  You need to
// specify the following information:
//     who = the subject (usually the caster)
//     what = the item or target (gate, rune, corpse, NPC, etc...)
//     alterchance = the chance, relative to the others, of effect
//     mutatechance = as above
//     gatechance = as above
//     divertchance = as above
//     effects = the total number of effects
// For example: GenerateRandomEffect(caster, target, 5, 5, 0, 1, 1)
// would center the effects on the caster, with a 5/11 chance of 
// a alteration or mutation, no chance for a gate, and 1/11 chance
// of being diverted.  If a creature is gated, it will go to the
// target.

	var i, x;

	while (effects > 0)
		x := alterchance + mutatechance + gatechance + divertchance;
		i := Random(x) + 1;
		x := x - alterchance;
		if (x < i)
			ChaosAlteration(who);
		else
			x := x - mutatechance;
			if (x < i)
				ChaosMutation(who);
			else
				x := x - gatechance;
				if (x < i)
					GatedDaemon(what.x, what.y, what.z);
				else
					DivertPlayer(who);
				endif
			endif
		endif
		effects := effects - 1;
	endwhile

endfunction


function ChaosAlteration(who)

// This effect will cause minor alterations to the poor victim.  These
// alterations are minor, but some are permanent.

	var i, x, success :=0;
	i := Random(8) + 1;

	case (i) //Conversion
		1:
			x := (GetStrength(who) - 1);
			SetBaseStrength(who,x);
			SendSysMessage(who, "You feel weakened...");
			return;
		2:
			x := (GetDexterity(who) - 1);
			SetBaseDexterity(who,x);
			SendSysMessage(who, "You feel disoriented...");
			return;
		3:
			x := (GetIntelligence(who) - 1);
			SetBaseIntelligence(who,x);
			SendSysMessage(who, "You feel confused...");
			return;
		4:
			x := CInt( GetObjProperty(who, "poison_level") );
			x := x + 2;
			SetObjProperty(who, "poison_level", CInt(x) );
			SendSysMessage(who, "You feel nauseous...");
			return;
		5:
			x := (Random(GetHp(who)) / 3) + 1;
			ApplyRawDamage(who, x);
			SendSysMessage(who, "You are wracked with pain!");
			return;
		6:
			x := (Random(GetMana(who)) / 3) + 1;
			SetMana(who, GetMana(who) - x);
			SendSysMessage(who, "Your head starts throbbing!");
			return;
		7:
			while (success == 0)
				x := (Random(SKILLID__HIGHEST) + 1);
				if (GetEffectiveSkill(who, x) > 0)
					success := GetBaseSkill(who, x) - 1;
					if (success < 1)
						success := 1;
					endif
					SetBaseSkill(who, x, success);
				endif
			endwhile
			SendSysMessage(who, "You feel disoriented...");
			return;
		8:
			SendSysMessage(who, "Uh-oh...");
			Detach();
			x := who.graphic;
			PlaySoundEffect(who, SFX_SPELL_POLYMORPH);
			who.graphic := 0xd0;
			i := RandomDiceStr(5d60);
			sleep(i);
			PlaySoundEffect(who, SFX_SPELL_POLYMORPH);
			who.graphic := x;
			return;
	endcase

endfunction	


function ChaosMutation(who)

// These effects are more severe than the alterations, and all are
// permanent.   Many are downright nasty.  Use at your discretion!

	var i, x, y, success :=0;
	i := Random(10) + 1;

	case (i) //Conversion
		1:
		
			x := (GetStrength(who) - 5);
			SetBaseStrength(who,x);
			SendSysMessage(who, "You feel weakened...");
			return;
		2:
			x := (GetDexterity(who) - 5);
			SetBaseDexterity(who,x);
			SendSysMessage(who, "You feel disoriented...");
			return;
		3:
			x := (GetIntelligence(who) - 5);
			SetBaseIntelligence(who,x);
			SendSysMessage(who, "You feel confused...");
			return;
		4:
			x := ListEquippedItems(who);
			success := Random( len(x) ) + 1;
			DestroyItem(x[success]);
			SendSysMessage(who, "Your " + x[success].name + " has disappeared!");
			PlaySoundEffect(who, SFX_SPELL_EXPLOSION);
			return;
		5:
			ApplyRawDamage(who, GetHp(who));
			return;
		6:
			SetMana(who, 0);
			SendSysMessage(who, "Your head starts throbbing!");
			return;
		7:
			y := RandomDiceStr(2d4);
			while (y > 0)
				while (success == 0)
					x := (Random(SKILLID__HIGHEST) + 1);
					if (GetEffectiveSkill(who, x) > 0)
						success := GetBaseSkill(who, x) - 4;
						if (success < 1)
							success := 1;
						endif
						SetBaseSkill(who, x, success);
					endif
				endwhile
				y := y - 1;
			endwhile

			SendSysMessage(who, "You feel disoriented...");
			return;
		8:
			SendSysMessage(who, "Uh-oh...");
			x := who.graphic;
			PlaySoundEffect(who, SFX_SPELL_POLYMORPH);
			return;

		9:
			foreach success in ListEquippedItems(who);
				x := who.x + Random(11) - 5;
				y := who.y + Random(11) - 5;
				MoveObjectToLocation(success, x, y, GetWorldHeight(x, _DEFAULT_REALM, y),0 ); //AutoConversion
			endforeach
			SendSysMessage(who, "You feel colder...");
			return;

		default:
			if (who.gender == 1)
				who.gender := 0;
				who.graphic := 0x190;
				SendSysMessage(who, "You seem to have shrunk...");
			else
				who.gender := 1;
				who.graphic := 0x191;
				SendSysMessage(who, "You seem to have grown...");
			endif
			return;
	endcase

endfunction	



function GatedDaemon(xpos, ypos, zpos)

// This effect will create a very unhappy daemon, who will likely
// take his frustrations out upon the poor caster.  Suggest uses:
// random effect from a Gate spell, miscast summon, etc. etc.

	CreateNpcFromTemplate("daemon", xpos, ypos, zpos);

endfunction


function DivertPlayer(who)

// This effect will change the player's location after casting a
// transportation type spell (i.e., gating, recalling).  This will
// read the "diversion" key from the "chaoseffects.cfg" file, and
// choose a random location.

	var i, desc, newx, newy, newz;
	var cfg, elem;

	cfg := ReadConfigFile("chaoseffects");
	elem := FindConfigElem(cfg, "diversion");

	desc := GetConfigStringArray(elem, "desc");
	newx := GetConfigStringArray(elem, "x");
	newy := GetConfigStringArray(elem, "y");
	newz := GetConfigStringArray(elem, "z");

	i := len(desc);
	i := Random(i) + 1;

	print("Diverting " + who.name + " to " + desc[i]);

	SendSysMessage(who, "Something is amiss...");

	MoveObjectToLocation(who, CInt(newx[i]), CInt(newy[i]), CInt(newz[i]) );
	return;

endfunction