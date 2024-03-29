//
// Include for clothing merchants or other NPCs 'randomly'
// In the ai script, use Habille instead of EquipFromTemplate (or whatever)
//
// First release on November 29 by Helldrige (boum0@frankiz.polytechnique.fr)
//

use cfgfile;
use os;

var couleurs;
var vetements;
var template;
var lastcolor;

function Habille(who, categ)
  var group;
  var equipt;

// New files for describing what we need
  couleurs := ReadConfigFile( "::colors" );
  vetements := ReadConfigFile( "::cloths" );

// Using config/equip2.cfg to leave equip.cfg alone
  template := ReadConfigFile( "::equip2" );

// Put the NPC in underwear in order to cloth him from head to feet
  foreach equipt in ListEquippedItems(who)
    DestroyItem(equipt);
  endforeach


  if (couleurs && vetements && template)
    group := FindConfigElem( template, categ );
    if (group)            // Si on trouve le groupe d'affaires
      var str;
      foreach str in GetConfigStringArray( group, "Equip" )
        EquipItemString( who, str );
      endforeach
 
      foreach str in GetConfigStringArray( group, "Armor" )
       	EquipItemString( who, str );
      endforeach

      foreach str in GetConfigStringArray( group, "Weapon" )
       	EquipItemString( who, str );
      endforeach
    endif
  endif

endfunction

function Clothe(who, categ)
        Habille(who, categ);
endfunction


/////////////////////////////////////////////////////////////////////////////
//
//  An 'Item' string can have one of the following forms:
//  Function shamely duplicated from another file...
//
//  FORM                                    EXAMPLE
//  --------------------------------------------------------------------
//  "Item [Name]"                        "Item Mortar"
//  "Item [Name] [Color]"                "Item Robe 0xE9"
//  "Item [Name] [Color] [%tospawn]"     "Item Longsword 0 100"
//
/////////////////////////////////////////////////////////////////////////////
function EquipItemString( who, str )
    var params := SplitWords( str );
    var objname := params[1];
    var color;
    var chance;

// If the color is not a number, we look in the lists of colors.cfg
    if ( CInt(params[2]) )
        color := CInt( params[2] );
    else
	color := 0;
        if((params[2] == "lastcolor") || (params[2] == "lastcolour"))
            color := lastcolor;
        else
          var colorg := FindConfigElem( couleurs, params[2] );
    	  if (colorg)            // Si on trouve le groupe d'affaires
            var str := GetConfigStringArray( colorg, "Color" );
            var rndint := CInt(Random(len(str)) + 1);
            var ar := SplitWords(str[rndint]);

            if ((len(ar)>1) && (CInt(ar[2])>0))
                color := Random(CInt(ar[2]) - CInt(ar[1])) + CInt(ar[1]);
            else
                color := CInt(str[rndint]);
            endif
          endif
       endif
    endif
   
// Saves the color for a future usage
    lastcolor := color;

    if ( CInt(params[3]) )
        chance := CInt( params[3] );
    else
        chance := 100;
    endif

// Get the objet type number, if it fails, we will look in cloths.cfg
    if ( CInt(objname) )
        objname := CInt(objname);
    endif

  if ( CInt(Random(100)+1) <= CInt(chance) )
    var item := CreateItemAtLocation( who.x, who.y, who.z, objname, 1 );
    if (!item)
        var itemg := FindConfigElem( vetements, objname );
    	if (itemg)            // Si on trouve le groupe de vetements
          var str := GetConfigStringArray( itemg, "Item" );
          var gend := SplitWords(str[Random(len(str))+1]);
          objname := CInt(gend[1]);
          if ((len(gend)<2) ||
              ((gend[2]=="m") && (who.graphic==400)) ||
              ((gend[2]=="f") && (who.graphic==401)))
            item := CreateItemAtLocation( who.x, who.y, who.z, objname, 1 );
            if (!item)
              syslog( "Unable to create " + objname );
            endif
          endif
        endif
    endif

    item.color := color;
    if (!EquipItem(who, item))
      DestroyItem(item);
      syslog( "Unable to equip " + item.name + " on " + who.name);
    endif
  endif

endfunction