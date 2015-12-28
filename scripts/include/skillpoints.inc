use uo;
use cfgfile;

include "include/classes";
include "include/attributes";
include "include/areas";

function get_default_points(skillid)
        var cfg := ReadConfigFile("skills");

        var default_points := cfg[skillid].default_points;
       
        if (!default_points)
                return 0;
        else
                return default_points;

        endif
endfunction

function AwardSkillPoints( who, skillid, points )

	if( who.IsA(POLCLASS_NPC) )
		return 0;
	elseif( who.cmdlevel )
		return 0;
	elseif(IsInSafeArea(who) && !GetObjProperty(who, "#duel")) // Is in safe area and not dueling (Seravy)
		return 0;
	endif

	// START MODIFYING 'points'

	var skills_state_array   := GetObjProperty( who, "SkillsState" );
	if( !skills_state_array )
		skills_state_array := array;
	endif
	
	var skill_state := skills_state_array[skillid+1];
	if( skill_state == "d" )
		var baseskill := GetBaseSkill( who, skillid );
		var amount   := 1;
		if( amount > baseskill )
			amount := baseskill;
			skills_state_array[skillid+1] := "l";
			SetObjProperty( who, "SkillsState", skills_state_array );
		endif
		SetBaseSkill( who, skillid, Cint(baseskill - amount) );
		return;
		
	elseif( skill_state == "l" )
		return;
	endif
	
	if (points == 0)
		return;
	endif
	
	points := Cint( points * GetSkillPointsMultiplier( who , skillid ) );
	var special_multipliers := GetObjProperty( who, "SkillMultipliers" );
	if( len(special_multipliers) )
		if( special_multipliers[skillid+1] )
			if( Cint(special_multipliers[skillid+1]) == 9999 )
				points := 0;
			else
				var skill_mult := Cdbl( special_multipliers[skillid+1] );
				points := Cint( points * skill_mult );
			endif
		endif
	endif
	
	var global_multipliers := GetGlobalProperty( "SkillPointsMultiplier" );
	if( len(global_multipliers) )
		if( global_multipliers[skillid+1] )
			if( Cint(global_multipliers[skillid+1]) == 9999 )
				points := 0;
			else
				var mult := Cdbl( global_multipliers[skillid+1] );
				points := Cint( points * mult );
			endif
		endif
	endif

	// FINISHED MODIFYING 'points'
	
	AwardPoints(who, skillid, points);
endfunction