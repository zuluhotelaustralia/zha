/////////////////////////////////////////////////////////
//vet.inc
//
//Modified to be an inc so that vet would be constrained like healing with "I am already doing something else."
//
//////////////////////////////////////////////////////////

//const BLOODY_BANDAGE 	  := 0x0e20;

function vet ( parms )
 
	var character := parms[1];
	var patient := parms[2];
	var bandages := parms[3];

	if (parms[5]=="res") //res attempt on vet-able corpse type
		TryToVetRes( character, patient, bandages, parms[4] ); //parms[4] is npctemplate;
		return;
	endif

	if( patient.poisoned )
		TryToVetCure( character, patient, bandages );
	elseif( GetHp(patient) < GetMaxHp(patient) )
		TryToVetHeal( character, patient, bandages );
	else
		SendSysMessage( character, "This creature don't even have a bruise!" );
	endif


endfunction


function TryToVetCure( character, patient, bandages )

	if( bandages.amount < BANDAGES_TO_CURE )
		SendSysMessage( character, "You don't have enough bandages." );
		return;
	endif

	var delay 	:= DELAY_TO_CURE;

	if( patient.serial == character.serial )
		delay := CInt( delay * DELAY_MULT_FOR_SELF );
		SendSysMessage( character, "You start to cure yourself" );
	else
		SendSysMessage( character, "You start to cure " + patient.name );
	 	SendSysMessage( patient, character.name + " starts to cure you" );
	endif

	if( GetObjProperty( character, CLASSEID_DRUID ) )
		delay := CInt( delay / CLASSE_BONUS );
	endif

	sleep( delay );

	if( Distance( character, patient ) > 1 )
		SendSysMessage( character, "The creature is too far." );
		return;
	endif

	if( character.dead )
		return;
	endif

	if( patient.dead )
		SendSysMessage( character, "The creature is dead." );
	endif

	var plvl 	:= PoisonLevel( patient );
	var diff 	:= CInt(plvl * 20 + 15);
	var points 	:= diff * POINTS_MULTIPLIER;

	if( CheckSkill( character, SKILLID_VETERINARY, diff, points ) )
		CurePoison( patient, 0, character.name + " has cured you!" );
		SendSysMessage( character, "You cured the creature." );
	else
		SendSysMessage( character, "You couldn't cure the creature." );
	endif

	SubtractAmount( bandages, BANDAGES_TO_CURE );
	CreateItemInBackpack( character, BLOODY_BANDAGE, BANDAGES_TO_CURE );

endfunction


function TryToVetHeal( character, patient, bandages )

	if( bandages.amount < BANDAGES_TO_HEAL )
		SendSysMessage( character, "You don't have enough bandages." );
		return;
	endif

	var delay 	:= DELAY_TO_HEAL;
	var healing	:= CInt( GetEffectiveSkill( character, SKILLID_VETERINARY ) / 10 );
	var animal	:= CInt( GetEffectiveSkill( character, SKILLID_ANIMALLORE ) / 5 );
	var healed	:= RandomDiceStr( healing + "d4+" + healing ) + RandomDiceStr( "1d8+2" ) + animal;

	if( patient.serial == character.serial )
		delay := CInt( delay * DELAY_MULT_FOR_SELF );
		SendSysMessage( character, "You start to heal yourself." );
	else
		SendSysMessage( character, "You start to heal " + patient.name );
	 	SendSysMessage( patient, character.name + " starts to heal you" );
	endif

	if( GetObjProperty( character, CLASSEID_RANGER ) )
		delay := CInt( delay / CLASSE_BONUS );
		healed := CInt( healed * CLASSE_BONUS );
	endif

	var count	:= 10;
	var shp	:= GetHp(character);
	while( delay )
		sleep( 1 );
		delay := delay - 1;
		if( GetHp(character) < shp )
			SendSysMessage( character, "Yours fingers slip..." );
			count := count - 1;
			shp   := GetHp(character);
		endif
	endwhile

	if( Distance( character, patient ) > 1 )
		SendSysMessage( character, "The creature is too far." );
		return;
	endif

	if( character.dead )
		return;
	endif

	if( patient.dead )
		SendSysMessage( character, "The creature is dead." );
	endif

	var diff 	:= CInt(GetMaxHp(patient) - GetHp(patient));
	var points 	:= diff * POINTS_MULTIPLIER;

	var bonus := GetObjProperty( patient, "HealingIncrease" );
	if( bonus )
		healed := healed + CInt( healed * bonus * 0.1 );
	endif

	if( count < 1 )
		count := 1;
	endif

	healed := CInt( healed * count / 10 );

	//if( !GetObjProperty( character, CLASSEID_DRUID ) )
	//healed := CInt(healed*0.85);
	//endif

	if( diff > 150 )
	diff:= 150;
	endif

	if( CheckSkill( character, SKILLID_VETERINARY, diff, points ) )
		HealDamage( patient, healed );
		SendSysMessage( character, "You healed " + healed + " damage." );
		//SendSysMessage( patient, character.name + " healed " + healed + " damage." );
	else
		SendSysMessage( character, "You couldn't heal the creature." );
	endif

	SubtractAmount( bandages, BANDAGES_TO_HEAL );
	CreateItemInBackpack( character, BLOODY_BANDAGE, BANDAGES_TO_HEAL );

endfunction

function TryToVetRes( character, patient, bandages, npctemplate ) 
	if( ! GetObjProperty( character, CLASSEID_RANGER ) && ! GetObjProperty( character, CLASSEID_DRUID ) )
		SendSysMessage( character, "Only a person in touch with nature could resurrect such a creature!");
		return;
	endif

	if( bandages.amount < BANDAGES_TO_RES )
		SendSysMessage( character, "You don't have enough bandages." );
		return;
	endif
	
	//check to make sure that two people dont do this at same time thus spawning many monsters
	if( ! GetObjProperty( patient, "resattempt" ) )
		SetObjProperty( patient, "resattempt", 1 );
	else
		SendSysMessage( character, "Someone is already resurrecting the creature." );
		return;
	endif

	var conf := ReadConfigFile( ":*:npcdesc" );
	var elem := FindConfigElem( conf, npctemplate );
	var difficulty;
	
	if( !elem || GetObjProperty(patient, "noresurrect"))
		SendSysMessage( character, "You can't resurrect that!" );
		EraseObjProperty( patient, "resattempt" );
		return;
	endif
	var pname := GetConfigString( elem, "name" );
	
	var delay 	:= DELAY_TO_RES;
	//var vetskill	:= CInt( GetEffectiveSkill( character, SKILLID_VETERINARY ));
	//Broadcast ("" + vet);

	SendSysMessage( character, "You start to resurrect " + pname + ".");

	var shp	:= GetHp(character);
	while( delay )
		sleep( 1 );
		delay := delay - 1;
		if( GetHp(character) < shp )
			SendSysMessage( character, "Your fingers slip..." );
			shp   := GetHp(character);
		endif
	endwhile

	if( Distance( character, patient ) > 1 )
		EraseObjProperty( patient, "resattempt" );
		SendSysMessage( character, "The creature is too far." );
		return;
	endif

	if( character.dead )
		EraseObjProperty( patient, "resattempt" );
		return;
	endif
	
	//difficulty: look up npctemplate and get tame skill required.
	
	if( !GetObjProperty( patient, "untamable" ) )
		difficulty := GetObjProperty( patient, "tamableAt" );
		if( !difficulty )
			difficulty := GetConfigInt( elem, "tameskill" );
		endif
	endif

	if( !difficulty )
		SendSysMessage( character, "You can't resurrect that!" );
		EraseObjProperty( patient, "resattempt" );
		return;
	endif

	// Removed druid bonus. Druids have a spell. Seravy.
	//If(IsDruid(character))
	difficulty := difficulty+30;
	//elseif(IsRanger(character))
	//difficulty := difficulty+50;
	//endif
	//Broadcast ("diff: " + difficulty);
	
	//points
	var points 	:= difficulty * POINTS_MULTIPLIER;
	
	if( CheckSkill( character, SKILLID_VETERINARY, difficulty, points ) )
		SendSysMessage( character, "You resurrected " + pname +"." );
		var newmonster := CreateNpcFromTemplate( npctemplate, patient.x, patient.y, patient.z);
		DestroyItem(patient);
		SetObjProperty(newmonster, "noloot", 1);
	else
		SendSysMessage( character, "You couldn't resurrect " + pname +".");
		EraseObjProperty( patient, "resattempt" );
	endif

	SubtractAmount( bandages, BANDAGES_TO_RES );
	CreateItemInBackpack( character, BLOODY_BANDAGE, BANDAGES_TO_RES );

endfunction