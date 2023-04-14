use uo;

include "include/random";
include "include/attributes";
include "include/classes";
include "include/skillpoints";

function DKSMod(who, skillid, points)

	if( who.IsA(POLCLASS_NPC) )
		return points;
	endif

	//dont do anything if skill set to decrease or locked
	var skills_state_array   := GetObjProperty( who, "SkillsState" );
	if( !skills_state_array )
		skills_state_array := array;
	endif

	var skill_state := skills_state_array[skillid+1];
	if( skill_state == "d" || skill_state == "l" )
		return points;
	endif

	var dks := GetObjProperty(who, "dks");
	if (dks<1)
		return points;
	elseif (dks>1)
		return points;
	endif

	//at this stage skills up to 119 raise by DKS
	var skill := GetEffectiveSkill( who, skillid );
	if (skill>119)
		return points;
	endif

	var mod := 4; //modified to 4 to increase dragon scrolls.

	return CInt(points*mod);

endfunction