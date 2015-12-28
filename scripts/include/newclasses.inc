use uo;
use basic;
include "include/classes";
include "include/attributes";
include "include/gumps";



function ShowClassSkillsGump( who, classeid )
	var class_skills:=GetClasseSkills( classeid );
	var class_name := GetClasseName( classeid );
	var class_skills_state:=GetSkillStateArray(who,class_skills);
	GFInitGump();
	GFPage(1);
	GFResizePic(0, 0, BKGND_SCROLL, 350, 250);
	GFTextLine(40, 10, 5, class_name + " skills");
	var y:=20;
	var hButton:=array;
	var aButton:=array;
	var i:=1;

	foreach c_sk in class_skills
		var skillval := CStr(GetEffectiveSkillBaseValue(who, c_sk));
		var stringname := FormatSkillName(GetAttributeIdBySkillId( c_sk ));
				
		GFTextLine(70, 20+y, 0, stringname);
		GFTextLine(200, 20+y, 0, skillval);
		hButton.append(GFButtonID(40, 25+y, G_BLUE_SMALL_CIRCLE_1, G_BLUE_SMALL_CIRCLE_0, 1 ));
		
		if (class_skills_state[i]=="l")
			aButton.append(GFButtonID(270, 25+y, 0x82c, G_BLACKGOLD_ARROW_UP_1, 1 ));
		elseif(class_skills_state[i]=="d")
			aButton.append(GFButtonID(270, 25+y, G_BLACKGOLD_ARROW_DOWN_1, 0x82c, 1 ));
		else
			aButton.append(GFButtonID(270, 25+y, G_BLACKGOLD_ARROW_UP_1, G_BLACKGOLD_ARROW_DOWN_1, 1 ));
		endif
		
		i:=1+1;
		y:=y+20;
	endforeach
	
	GFTextLine(70, 220, 5, "Close");

	var closeButton := GFButtonID( 50, 225, G_BLUE_SMALL_CIRCLE_0, G_BLUE_SMALL_CIRCLE_1, 1 );

	var choice := GFSendGump(who);
	
	var counter:=0;

	if (choice[closeButton.keyid])
		return;
	endif
		
	foreach b in hButton
		counter:=counter+1;
   	if (choice[b.keyid])
		endif
	endforeach
	
	counter:=0;
	foreach a in aButton
		counter:=counter+1;
   	if (choice[a.keyid])
			ModifySkillState(class_skills[counter], class_skills_state[counter], who);
		endif
	endforeach


	
endfunction

function FormatSkillName(skillname)
	return upper(skillname[1,1]) + lower(skillname[2,len(skillname)]);
endfunction

function fillstring(string1, string2, length)
	//puts spaces between strings so that returned string is equal to length
	var len1:=len(string1);
	Broadcast(string1 + " len: " + len1);
	Broadcast(length + " " + len1 + " " + length-len1);
	//var len2:=len(string2);
	var spacestoadd := length - len1 -1;
	Broadcast("spacestoadd " + spacestoadd);
	while (spacestoadd>0)
		string1:=string1+" ";
		spacestoadd := spacestoadd -1;
	endwhile
	return string1+string2;

endfunction

function GetSkillStateArray(who, classeskills)
	var skills_state_array   := GetObjProperty( who, "SkillsState" );
	if( !skills_state_array )
		return {};
	endif
	
	var class_skill_state_array:=array;
	
	foreach cs in classeskills
		class_skill_state_array.append(skills_state_array[cs+1]);
	endforeach
	return class_skill_state_array;
endfunction

function ModifySkillState(class_skill, skills_state, who)
	var skills_state_array   := GetObjProperty( who, "SkillsState" );
	if (skills_state == "d")
		skills_state_array[class_skill+1]:="l";
	elseif (skills_state=="l")
		skills_state_array[class_skill+1]:="r";
	else
		skills_state_array[class_skill+1]:="d";
	endif
	SetObjProperty( who, "SkillsState", skills_state_array );
endfunction