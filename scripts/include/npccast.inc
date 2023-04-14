use uo;
use os;
use npc;

include "include/client";
//include "include/attributes";
include "include/dotempmods";
include "include/spelldata";
include "include/random";

function castspell( caster, cast_on, spellname )

	case (spellname)
	"summonskel":
		SummonSkel(caster,cast_on);
	"summonelelord":
		SummonEleLord(caster,cast_on);
	"summonbalronspawn":
		SummonBalronSpawn(caster, cast_on);
	"summonelf":
		SummonElf(caster, cast_on);
	"summondarkelf":
		SummonDarkElf(caster, cast_on);
	"summonundead":
		SummonUndead(caster, cast_on);
	"summonspider":
		SummonSpider(caster, cast_on);
	"summondarkelfqueen":
		SummonDarkElfQueen(caster, cast_on);
	"teletoplayer":
		TeleToPlayer(caster, cast_on);
	"summonelflord":
		SummonElfLord(caster, cast_on);
	"summonjuggernaught":
		SummonJuggernaught (caster, cast_on);
	"spitweb":
		SpitWeb(caster, cast_on);

	"grow_hoardedemon":
		IncreaseHoardeDemonSize(caster, cast_on);

	default:
		var castingcmd := SplitWords(spellname);
		if (castingcmd[1] == "MassCast")
			spellname:=castingcmd[2];
		endif

		var spellid := GetSpellID(spellname);
		if (!spellid)
			return;
		endif

		if (!PrepareToCast(caster,cast_on,spellid))
			return;
		endif

		if (castingcmd[1] == "MassCast")
			CastOnAllPlayersSpell(caster, spellid);
		else
			var parms array;
			parms[1] := "#MOB";
			parms[2] := caster;
			parms[3] := cast_on;
			parms[4] := spellid;
			Detach();
			start_script( GetScript( spellid ), parms );
		endif
	endcase

endfunction

function SummonSkel(me, unused onwho)

	var timescasted := GetObjProperty(me, "timescasted");

	if (!timescasted)  timescasted :=0; endif

	timescasted := timescasted +1;
	SetObjProperty(me, "timescasted", timescasted);

	if (timescasted>=2) return; endif

	var utterance := Random(4);

	case (utterance)
        	0: say("Awake, my children! Come forth and take your rightful place!");
        	1: say("Come forth my minions!");
        	2: say("I call upon the forces of darkness to attend me!");
        	3: say("Mortal, you shall soon be my slave!");
	endcase

	NpcSummonCreature( me, "skeleton" );

endfunction

function SummonBalronSpawn(me, unused onwho)

	var timescasted := GetObjProperty(me, "timescasted");

	if (!timescasted) timescasted :=0; endif

	timescasted := timescasted +1;
	SetObjProperty(me, "timescasted", timescasted);

	if (timescasted>=2) return; endif

	var utterance := Random(4);

	case (utterance)
        	0: say("Awake, my children! Come forth and take your rightful place!");
        	1: say("Come forth my minions!");
        	2: say("I call upon the forces of darkness to attend me!");
        	3: say("Mortal, you shall soon be my slave!");
	endcase

	NpcSummonCreature(me, "balronspawn" );

endfunction

function SummonEleLord(me, unused onwho)

	var timescasted := GetObjProperty(me, "timescasted");

	if (!timescasted) timescasted :=0; endif

	timescasted := timescasted +1;
	SetObjProperty(me, "timescasted", timescasted);

	if (timescasted>=3) return; endif

	var utterance := Random(4);
	PlaySoundEffect(me, 0x218);

	case (utterance)
        	0: say("I summon forth the Living Rock to protect me!");
			NpcSummonCreature(me, "earthelementallord");
        	1: say("The power of Fire shall purge your soul!");
			NpcSummonCreature(me, "fireelementallord");
        	2: say("The Winds of fate shall sweep you away!");
			NpcSummonCreature(me, "airelementallord");
        	3: say("Thou shalt drown in nature's fury!");
			NpcSummonCreature(me, "waterelementallord");
	endcase


endfunction

function SummonElf(me, unused onwho)

	var timescasted := GetObjProperty(me, "timescasted");

	if (!timescasted) timescasted :=0; endif

	timescasted := timescasted +1;
	SetObjProperty(me, "timescasted", timescasted);

	if (timescasted>=3) return; endif

	var utterance := Random(3);

	case (utterance)
        	0: say("Druids, protect me!");
			NpcSummonCreature(me, "elfdruid");
        	1: say("Warriors, to arms!");
			NpcSummonCreature(me, "elfwarrior");
        	2: say("Archers, cover me!");
			NpcSummonCreature(me, "elfarcher");
	endcase

endfunction

function SummonDarkElf(me, unused onwho)

	var timescasted := GetObjProperty(me, "timescasted");
	if (!timescasted) timescasted :=0; endif

	timescasted := timescasted +1;
	SetObjProperty(me, "timescasted", timescasted);

	if (timescasted>=4) return; endif

	var utterance := Random(3);

	case (utterance)
        	0: say("Necromancers, protect me!");
			NpcSummonCreature(me, "darkelfnecromancer");
        	1: say("Warriors, to arms!");
			NpcSummonCreature(me, "darkelfwarrior");
        	2: say("Assassins, cover me!");
			NpcSummonCreature(me, "darkelfassassin");
	endcase

endfunction

function SummonSpider(me, unused onwho)

	var timescasted := GetObjProperty(me, "timescasted");

	timescasted := timescasted +1;
	SetObjProperty(me, "timescasted", timescasted);

	if (timescasted>=4) return; endif
	say ("Feast, my children!");
	var spider := Random(3);

	case (spider)
        	0: NpcSummonCreature(me, "direweaver");
        	1: NpcSummonCreature(me, "flamingspider");
        	2: NpcSummonCreature(me, "phasespider");
	endcase

endfunction

function SummonUndead(me, unused onwho)

	var timescasted := GetObjProperty(me, "timescasted");

	if (!timescasted) timescasted :=0; endif

	timescasted := timescasted +1;
	SetObjProperty(me, "timescasted", timescasted);

	if (timescasted>=3) return; endif

	var utterance := Random(4);

	case (utterance)
        	0: say("Awake, my children! Come forth and take your rightful place!");
			NpcSummonCreature(me, "skeletalwarrior");
        	1: say("Come forth my minions!");
			NpcSummonCreature(me, "lichelord");
        	2: say("I call upon the forces of darkness to attend me!");
			NpcSummonCreature(me, "bloodliche");
        	3: say("Mortal, you shall soon be my slave!");
			NpcSummonCreature(me, "vampire1");
	endcase

endfunction

function SummonDarkElfQueen (caster, unused cast_on)
	var timescasted := GetObjProperty(caster, "timescasted");
	if (!timescasted) timescasted :=0; endif
	timescasted := timescasted +1;
	SetObjProperty(caster, "timescasted", timescasted);
	if(timescasted>=5)
		return;
	endif
	say("To me, my Queens!");
	PlaySoundEffect(caster, 0x24e);
	NpcSummonCreature(caster, "darkelfqueen");
endfunction

function TeleToPlayer(caster, cast_on)
	say("Rel Por");
	sleep(1);
	PlaySoundEffect(caster, 0x1ff);
	MoveObjectToLocation(caster, cast_on.x, cast_on.y, cast_on.z, _DEFAULT_REALM); //AutoConversion
endfunction

function SummonElfLord (caster, unused cast_on)
	var timescasted := GetObjProperty(caster, "timescasted");
	if (!timescasted) timescasted :=0; endif
	timescasted := timescasted +1;
	SetObjProperty(caster, "timescasted", timescasted);
	if(timescasted>=5)
		return;
	endif
	say("To me, my Lords!");
	PlaySoundEffect(caster, 0x24e);
	NpcSummonCreature(caster, "elflord");
endfunction

function SummonJuggernaught (caster, unused cast_on)
	var timescasted := GetObjProperty(caster, "timescasted");
	if (!timescasted) timescasted :=0; endif
	timescasted := timescasted +1;
	SetObjProperty(caster, "timescasted", timescasted);
	if(timescasted>=5)
		return;
	endif
	say("Intruder Alert! Intruder Alert!");
	PlaySoundEffect(caster, 0x360);
	NpcSummonCreature(caster, "juggernaught", 0x362, 20, 0x20F,0x1AE5);
endfunction

function GetSpellID(spellname)

	var spellid := 0;

	case (spellname)
	"clumsy"	: spellid := 1;
	"feeblemind"	: spellid := 3;
	"heal"		: spellid := 4;
	"magicarrow"	: spellid := 5;
	"reactivearmor"	: spellid := 7;
	"weaken"	: spellid := 8;
	"agility"	: spellid := 9;
	"cunning"	: spellid := 10;
	"cure"		: spellid := 11;
	"harm"		: spellid := 12;
	"protection"	: spellid := 15;
	"strength"	: spellid := 16;
	"bless"		: spellid := 17;
	"fireball"	: spellid := 18;
	"poison"	: spellid := 20;
	"teleport"	: spellid := 22;
	"wallofstone"	: spellid := 24;
	"archcure"	: spellid := 25;
	"archprot"	: spellid := 26;
	"curse"		: spellid := 27;
	"firefield"	: spellid := 28;
	"gheal"		: spellid := 29;
	"lightning"	: spellid := 30;
	"manadrain"	: spellid := 31;
	"blade_spirit"	: spellid := 33;
	"dispel_field"	: spellid := 34;
	"reflect"	: spellid := 36;
	"mindblast"	: spellid := 37;
	"paralyze"	: spellid := 38;
	"poisonfield"	: spellid := 39;
	"summon_creature": spellid := 40;
	"dispel"	: spellid := 41;
	"ebolt"		: spellid := 42;
	"explosion"	: spellid := 43;
	"invisibility"	: spellid := 44;
	"masscurse"	: spellid := 46;
	"parafield"	: spellid := 47;
	"reveal"	: spellid := 48;
	"chain_lightning": spellid := 49;
	"energy_field"	: spellid := 50;
	"fstrike"	: spellid := 51;
	"manavamp"	: spellid := 53;
	"massdispel"	: spellid := 54;
	"meteor_swarm"	: spellid := 55;
	"polymorph"	: spellid := 56;
	"earthquake"	: spellid := 57;
	"vortex"	: spellid := 58;
	"resurrect"	: spellid := 59;
	"summon_air"	: spellid := 60;
	"summon_daemon"	: spellid := 61;
	"summon_earth"	: spellid := 62;
	"summon_fire"	: spellid := 63;
	"summon_water"	: spellid := 64;
	"controlundead"	: spellid := 65;
	"darkness"	: spellid := 66;
	"decayingray"	: spellid := 67;
	"spectretouch"	: spellid := 68;
	"abyssalflame"	: spellid := 69;
	"animate_dead"	: spellid := 70;
	"sacrifice"	: spellid := 71;
	"wraithbreath"	: spellid := 72;
	"sorcerersbane"	: spellid := 73;
	"summon_spirit"	: spellid := 74;
	"wraithform"	: spellid := 75;
	"wyvernstrike"	: spellid := 76;
	"kill"		: spellid := 77;
	"liche"		: spellid := 78;
	"plague"	: spellid := 79;
	"spellbind"	: spellid := 80;
	"antidote"	: spellid := 81;
	"owlsight"	: spellid := 82;
	"shiftingearth"	: spellid := 83;
	"summonmammals"	: spellid := 84;
	"calllightning"	: spellid := 85;
	"earthblessing"	: spellid := 86;
	"earthportal"	: spellid := 87;
	"naturestouch"	: spellid := 88;
	"gustofair"	: spellid := 89;
	"risingfire"	: spellid := 90;
	"shapeshift"	: spellid := 91;
	"icestrike"	: spellid := 92;
	"earthspirit"	: spellid := 93;
	"flamespirit"	: spellid := 94;
	"stormspirit"	: spellid := 95;
	"waterspirit"	: spellid := 96;

	endcase

	return spellid;

endfunction

function PrepareToCast(caster, cast_on, spellid)

	if (!CheckLineOfSight( caster, cast_on ))
		return 0;
	endif

	if (saywords)
		SpeakPowerWords( me, spellid);
		PerformAction(caster,ANIM_CAST_DIR);
		sleep(1);
	endif

	if (GetEffectiveSkill(caster, SKILLID_MAGERY))
		if (!CheckSkill(caster, SKILLID_MAGERY, GetSpellDifficulty( spellid ), 100))
			PlaySoundEffect(caster,0x005d);
			PlayObjectCenteredEffect(caster, 0x3735,10, 30);
			return 0;
		endif
	endif

	if (! ConsumeMana( caster, spellid ))
		return 0;
	endif

	return 1;

endfunction

function cast_offensive_spell(caster, cast_on)

	var thespell := spells[Random( len(spells) +1 )];

if(thespell == 29)
        castspell(caster, caster, thespell);
else

	castspell(caster, cast_on, thespell);
endif


endfunction

function NpcSummonCreature( caster, npctemplate, sfx := 0x0218, duration:=0, departingsfx:=0, departingeffect:=0 )

	var parms := {};

	parms[1] := caster;
	parms[2] := npctemplate;
	parms[3] := sfx;

	if(duration!=0)
		parms[4] := duration;
	endif

	if(departingsfx!=0)
		parms[5] := departingsfx;
	endif

	if(departingeffect!=0)
		parms[6] := departingeffect;
	endif

	start_script( ":summoning:npcsummoning", parms );

endfunction

function SpitWeb(caster, cast_on)

	if (CheckLineOfSight(caster, cast_on))
		var x := cast_on.x;
		var y := cast_on.y;
		var z := cast_on.z;
		PrintTextAbove(caster, caster.name+" spits a web!");
		PlayMovingEffect(caster, cast_on, 0xbf01, 6);
		sleep(1);

		CreateItemAtLocation(x, y, z, 0xbf01,  1);

		if ( cast_on.x == x && cast_on.y == y)
			SendSysMessage(cast_on, "You are trapped in a spider web!");
			DoTempMod(cast_on, "p", 1, 15);
		endif
	endif
endfunction

function CastOnAllPlayersSpell(caster, spellid)
//casts spell on all players within LOS and range
	var RANGE := GetObjProperty(caster, "MassCastRange");
	if(!RANGE)
		RANGE := 7;
	endif
	var circle	:= GetCircle( spellid );

	var spellparms := { };
	spellparms[1] := "#MOB";
	spellparms[2] := caster;
	spellparms[4] := circle;


	foreach mob in ListMobilesInLineOfSight( caster, RANGE )
		if (mob.acct || mob.script == "tamed")  //player or tamed monster
			spellparms[3] := mob;
			start_script(GetScript(spellid), spellparms);
		endif
	endforeach
endfunction

function IncreaseHoardeDemonSize(caster, unused cast_on)
	var targettemplate;
	case (caster.npctemplate)
		"smallhoardedemon":
			targettemplate:="mediumhoardedemon";
		"mediumhoardedemon":
			targettemplate:="largehoardedemon";
		"largehoardedemon":
			targettemplate:="multicolouredhoardedemon";
		default:
			return 0;
	endcase
	var myx:=caster.x;
	var myy:=caster.y;
	var myz:=caster.z;
	PlayObjectCenteredEffect(caster, 0x3709, 3, 0);
	PlaySoundEffect(caster, 0x305);
	sleepms(300);
	caster.hidden:=1;
	sleepms(100);
	CreateNpcFromTemplate(targettemplate, myx, myy, myz);
	RevokePrivilege( caster, "invul" );
	SetObjProperty( caster, "guardkill", 1);
	ApplyRawDamage( caster, CInt(GetHp(caster)+3) );


endfunction
