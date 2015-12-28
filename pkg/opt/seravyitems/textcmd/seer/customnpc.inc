/*     - Custom NPC include file -        **
** Functions used to assign custom        **
** propertys to npcs using a new          **
** npc create function based on the props **
** of the spawn stone/tile.               **
** Seravy - seravy@gmail.com              **
*******************************************/

// Array of the props to get.
var npcPropsA := {"color"};
var itemPropsA := {"name", "color", "usescript"};



// This function returns the props of 'npc' via a storage method.
// Remeber to assign LFucker in calling method!
function GetTemplateFromNpc(who)
  SendSysMessage(who, "Select the npc you wish to add to the template.");
  var npc := Target(who);
  if (!npc.isA(POLCLASS_NPC))
    SendSysMessage(who, "Not a npc!");
    return -1;
  endif
  var templatename := CStr(SendTextEntryGump(who, "Enter the name for this template:", TE_CANCEL_ENABLE, TE_STYLE_NORMAL, 50));
  if (!templatename)
    SendSysMessage(who, "Not a valid template name!");
    return -1;
  endif

  var npctemplate := dictionary; // Build this npc's data.

  // Name
  npctemplate["template"] := templatename;

  // CProps of npc.
  var npctemplateCProps := dictionary;
  var cpropnames := GetObjPropertyNames(npc);
  foreach cprop in cpropnames
    npctemplateCProps[cprop] := GetObjProperty(npc, cprop);
  endforeach
  npctemplate["cprops"] := npctemplateCProps;

  // props of npc
  var npctemplateProps := struct;
  foreach prop in npcPropsA
    npctemplateProps[prop] := npc.get_member(prop);
  endforeach
  npctemplate["props"] := npctemplateProps;

  // Items on layers and their cprops

  // Creator
  npctemplate["creator"] := who.serial;

  return npctemplate;
endfunction

function CreateNpcFromCustomTemplate(template, x, y, z, point := 0)
  if (point)
    var pointTemplates := GetObjProperty(point, "CustomNpcTemplates");
    //if(template in pointTemplates
  endif

  var monster := CreateNpcFromTemplate(template, x, y, z);
  if (!monster)
    MessageGm("CSP: Couldn't create mosnter template '"+template+"' at "+x+","+y+","+z);
    return monster;
  endif

  return monster;
endfunction

function MessageGm(message)
  foreach person in EnumerateOnlineCharacters()
    if (person.cmdlevel)
      SendSysMessage(person, message, 3, 38);
    endif
  endforeach
endfunction



// This function adds the props of 'npc' via a storage 
// method to 'point' with the name 'templatename'.
// It is called by .addcustomnpc and by the gumps of 
// the spawn point. Remeber to assign LFucker in calling method!
// Assumes valid input.
function AddToPoint(npc, point, templatename)
  var customtemplates := GetObjProperty(point, "customtemplates");
  if (!customtemplates)
    customtemplates := dictionary;
  endif

  if(customtemplates.exists(templatename)) // If it already exists, get rid of it.
    customtemplates.erase(templatename);
  endif

  var npctemplate := dictionary; // Build this npc's data.

  var npctemplateCProps := dictionary;
  var cpropnames := GetObjPropertyNames(npc);
  foreach cprop in cpropnames
    npctemplateCProps[cprop] := GetObjProperty(npc, cprop);
  endforeach
  npctemplate["cprops"] := npctemplateCProps;

  var npctemplateProps := struct;
//  foreach prop in npcPropsA
//    npctemplateProps[prop] := npc.get_member(prop);
//  endforeach

//var prop := "color";
//npctemplateProps[prop] := 54; 
npctemplateProps.+color;
npctemplateProps.color := 54;

var prop := Struct;
//prop.+name;
//prop.name := "Jack";
broadcast(CStr(prop));
var value;
value := CreateNpcFromTemplate("skeleton", npc.x, npc.y, npc.z, prop);

if (!value)
  broadcast("FAILED:"+value);
endif
SetObjProperty(point, "props", npctemplateProps);


  

/*
  npctemplate[""] := ;

	//get props from monster and put on spawnpoint
	var counter:=1;
	
	var propnames := GetObjPropertyNames( templatemonster );
	
	foreach pname in propnames
		var pval := GetObjProperty(templatemonster, pname);
		SetObjProperty(point, "MonsterCustomCProp" + counter, {pname, TypeOf(pval), pval}); 
		counter:=counter+1;
	endforeach
	
	counter:=1;
	var e_array;
	foreach equip in ListEquippedItems(templatemonster)
		e_array:=array;
		e_array.append(Hex(equip.objtype));
		e_array.append(CInt(equip.color));
		foreach ipname in GetObjPropertyNames( equip )
			e_array.append(ipname);
			e_array.append(GetObjProperty(equip, ipname));
		endforeach
		if (equip.objtype==0xF021)
			e_array.append("graphic");
			e_array.append(equip.graphic);
		endif
		SetObjProperty(point, "MonsterCustomEquip" + counter, e_array); 
		//Broadcast(e_array);
		counter:=counter+1;
	endforeach

	SetObjProperty(point, "MonsterCustomName", templatemonster.name);
	
	SetObjProperty(point, "MonsterCustomGraphic", templatemonster.graphic);
	
	SetObjProperty(point, "MonsterCustomColor", templatemonster.color);	

*/
  customtemplates.insert(templatename, npctemplate);

  SetObjProperty(point, "customtemplates", customtemplates);
  return 1;
endfunction