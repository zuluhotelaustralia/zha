// ITEM PROPERTY CONSTANTS
const GM_ITEM_ID:=         1;   
const DMG_MOD_ID:=         2;   
const MAXHP_MOD_ID:=       3;
const AR_MOD_ID:=          4;
const AR_BONUS_ID:=        5;
const SPELL_HIT_ID:=       6;
const G_SLAYER_HIT_ID:=    7;
const SLAYER_HIT_ID:=      8;
const EFFECT_HIT_ID:=      9;
const G_EFFECT_HIT_ID:=    10;
const ITEM_SKILL_ID:=      11;
const ITEM_SKILL_MULT_ID:= 12;
const WEAP_SKILL_ID:=      13;
const SPELL_ONHIT_ID:=     14;
const RESIST_ONHIT_ID:=    15;
const EFFECT_ONHIT_ID:=    16;
const STR_MOD_ID:=         17;
const INT_MOD_ID:=         18;
const DEX_MOD_ID:=         19;
const NECRO_PROT_ID:=      20;
const HOLY_PROT_ID:=       21;
const AIR_PROT_ID:=        22;
const EARTH_PROT_ID:=      23;
const FIRE_PROT_ID:=       24;
const WATER_PROT_ID:=      25;
const PHYS_PROT_ID:=       26;
const ACID_PROT_ID:=       27;
const TEMP_PROT_ID:=       28;
const MOONSTONE_ID:=       29;
const POISON_PROT_ID:=     30;
const BLACKROCK_ID:=       31;
const STYGIAN_ID:=         32;
const MYSTICAL_ID:=        33;
const SWIFT_ID:=           34;
const SPELL_POWER_ID:=     35;
const FREE_ACTION_ID:=     36;
const HEAL_BONUS_ID:=      37;
const ORE_BONUS_ID:=  	   38;
const STATIC_ED_ID:=	   39;

// CLASS EQUIP SLOTS PER LEVEL
const BASE_ES:=            500;
const BARD_ES:=            10;
const CRAFTER_ES:=         10;
const MAGE_ES:=            10;
const RANGER_ES:=          10;
const THIEF_ES:=           10;
const WARRIOR_ES:=         10;
const DRUID_ES:=           10;
const DEATHKNIGHT_ES:=     10;
const NINJA_ES:=           10;
const PALADIN_ES:=         10;
const PRIEST_ES:=          10;
const MONK_ES:=            10;
const NECROMANCER_ES:=     10;
const POWERPLAYER_ES:=     30;

const GM_ITEM_ES:=         5;   // all GM items worth 5 regardless of props (+3 for weaps)
const ES_PENALTY:=         2;   // the ES penalty multiplier for items with non-class props
const ORE_TYPE1_ES:=	   1;	// ES cost if OreES cprop value is 1
const ORE_TYPE2_ES:=	   1;	// ES cost if OreES cprop value is 1
const ORE_TYPE3_ES:=	   1;	// ES cost if OreES cprop value is 1
const ORE_TYPE4_ES:=	   2;	// ES cost if OreES cprop value is 2
const ORE_TYPE5_ES:=	   2;	// ES cost if OreES cprop value is 2

var TEMP_MODS:= {"PoisonImmunity", "MagicImmunity", "SpellReflect"};

include "include/classes";



function GetEquipSlotProperties (item)

         var props:= array;
         var slaytype;

	 if (GetObjProperty(item, "StaticES"))
	 	return {STATIC_ED_ID};
	 endif
	 
	 if (GetObjProperty(item, "OreES"))
	 	return {ORE_BONUS_ID};
	 endif
	 
         if (GetObjProperty(item, "IsGMItem"))
            props.append(GM_ITEM_ID);
         endif	         
         
         if (item.dmg_mod)
            props.append(DMG_MOD_ID);
         endif
         
         if (item.maxhp_mod)
            props.append(MAXHP_MOD_ID);
         endif
         
         if (item.ar_mod)
            props.append(AR_MOD_ID);
         endif
         
         if (GetObjProperty(item, "ArBonus"))
            props.append(AR_BONUS_ID);
         endif
         
         if (item.hitscript && item.hitscript!="mainhit")
            case (item.hitscript)
             "spellstrikescript":     props.append(SPELL_HIT_ID);
             "slayerscript":          slaytype:=GetObjProperty(item, "SlayType");
                                      case (slaytype)
                                          "Human":
                                          "Dragon":
                                          "Undead":
                                          "Daemon":      props.append(G_SLAYER_HIT_ID);
                                          default:       props.append(SLAYER_HIT_ID);
                                      endcase
             "blackrockscript":
             "trielementalscript":
             "voidscript":            props.append(G_EFFECT_HIT_ID);
             default:                 props.append(EFFECT_HIT_ID);
            endcase
         endif
         
         if (GetObjProperty(item, "OnHitScript"))
            case(GetObjProperty(item, "OnHitScript"))
             ":combat:spellonhit":            props.append(SPELL_ONHIT_ID);
             
             ":combat:raceresistonhit":       props.append(RESIST_ONHIT_ID);
             default:                         props.append(EFFECT_ONHIT_ID);
            endcase
         endif
         
         if (GetObjProperty(item, "skilladv"))
            if (GetObjProperty(item, "SpecMulti"))
               props.append(ITEM_SKILL_MULT_ID);
            elseif (item.isa(POLCLASS_WEAPON))
               props.append(WEAP_SKILL_ID);
            else
                props.append(ITEM_SKILL_ID);
            endif
         endif
         
         if (GetObjProperty(item, "str"))
            props.append(STR_MOD_ID);
         endif
         if (GetObjProperty(item, "int"))
            props.append(INT_MOD_ID);
         endif
         if (GetObjProperty(item, "dex"))
            props.append(DEX_MOD_ID);
         endif

         if (GetObjProperty(item, "NecroProtection"))
            props.append(NECRO_PROT_ID);
         endif
         if (GetObjProperty(item, "HolyProtection"))
            props.append(HOLY_PROT_ID);
         endif
         if (GetObjProperty(item, "AirProtection"))
            props.append(AIR_PROT_ID);
         endif
         if (GetObjProperty(item, "EarthProtection"))
            props.append(EARTH_PROT_ID);
         endif
         if (GetObjProperty(item, "FireProtection"))
            props.append(FIRE_PROT_ID);
         endif
         if (GetObjProperty(item, "WaterProtection"))
            props.append(WATER_PROT_ID);
         endif
         if (GetObjProperty(item, "PhysicalProtection"))
            props.append(PHYS_PROT_ID);
         endif
         if (GetObjProperty(item, "AcidProtection"))
            props.append(ACID_PROT_ID);
         endif

         if (GetObjProperty(item, "Permmr"))
            props.append(MOONSTONE_ID);
         endif
         if (GetObjProperty(item, "PermPoisonImmunity"))
            props.append(POISON_PROT_ID);
         endif
         if (GetObjProperty(item, "PermMagicImmunity"))
            props.append(BLACKROCK_ID);
         endif
         
         foreach prot in TEMP_MODS
                 if (GetObjProperty(item, prot))
                    props.appends(TEMP_PROT_ID);
                    break;
                 endif
         endforeach
         
         if (GetObjProperty(item, "Stygian"))
            props.append(STYGIAN_ID);
         endif
         
         if (GetObjProperty(item, "Mystical"))
            props.append(MYSTICAL_ID);
         endif
         
         if (GetObjProperty(item, "Swift"))
            props.append(SWIFT_ID);
         endif
         
         if (GetObjProperty(item, "MagicPenalty") < 0)
            props.append(SPELL_POWER_ID);
         endif
         
         if (GetObjProperty(item, "FreeAction"))
            props.append(FREE_ACTION_ID);
         endif

         if (GetObjProperty(item, "HealingIncrease"))
            props.append(HEAL_BONUS_ID);
         endif
         
         if (props)
            return props;
         else
             return 0;
         endif
         
endfunction

function CalculateEquipSlots (who, item)

         if (GetObjProperty(item, "Cursed"))
         	return 0;
         endif
         var totales:=0;
         var es:=0;
         var temp;
         var itemcprops:= GetObjPropertyNames(item);
         foreach prop in GetEquipSlotProperties(item)
                 case (prop)
                      STATIC_ED_ID:	   temp:= GetObjProperty(item, "StaticES");
                      			   if (!temp)
                      			   	return 0;
                      			   else
                      			   	return temp;
                      			   endif
                      ORE_BONUS_ID:	   case (GetObjProperty(item, "OreES"))
                      				1:	temp:= ORE_TYPE1_ES;
                      				2:	temp:= ORE_TYPE2_ES;
                      				3:	temp:= ORE_TYPE3_ES;
                      				4:	temp:= ORE_TYPE4_ES;
                      				5:	temp:= ORE_TYPE5_ES;
                      			   endcase
                      			   if (!temp || temp < 0)
                      			   	temp:=0;
                      			   endif
                      			   return temp;
                      GM_ITEM_ID:          temp:=GM_ITEM_ES;
                      			   if (item.isa(POLCLASS_WEAPON))
                      			   	return 8;
                      			   endif
                      			   if (GetObjProperty(item, "FortifiedBy"))
                      			   	temp:=0;
                      			   else
                      			   	return temp;
                      			   endif           
                      DMG_MOD_ID:          temp:= item.dmg_mod / 5;
                                           if (!temp || temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      MAXHP_MOD_ID:        if(item.maxhp_mod > 30)
                                              es:=es + 1;
                                           endif
                      AR_MOD_ID:           temp:= (item.ar_mod-24) / 8;
                                           if (!temp || temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      AR_BONUS_ID:         temp:= CInt(GetObjProperty(item, "ArBonus") /2);
                                           if (!temp || temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      SPELL_HIT_ID:        es:=es + 1;
                      G_SLAYER_HIT_ID:     es:=es + 3;
                      SLAYER_HIT_ID:       es:=es + 1;
                      EFFECT_HIT_ID:       es:=es + 2;
                      G_EFFECT_HIT_ID:     es:=es + 3;
                      ITEM_SKILL_ID:       temp:= GetObjProperty(item, "skilladvamount") - 2;
                                           if (!temp || temp < 0);
                                              temp:=0;
                                           endif
                                           if (!(GetObjProperty(item,"skilladv") in GetClasseSkills(GetClass(who))))
                                              temp:= temp * ES_PENALTY;
                                           endif
                                           es:=es + temp;
                      ITEM_SKILL_MULT_ID:  temp:= GetObjProperty(item, "skilladvamount") / GetObjProperty(item, "SpecMulti") - 2;
                                           if (!temp || temp < 0)
                                              temp:=0;
                                           endif
                                           if (!(GetObjProperty(item,"skilladv") in GetClasseSkills(GetClass(who))))
                                              temp:= temp * ES_PENALTY;
                                           endif
                                           es:=es+temp;
                      WEAP_SKILL_ID:       temp:= (GetObjProperty(item, "skilladvamount") / 5) - 2;
                                           if (!(GetObjProperty(item,"skilladv") in GetClasseSkills(GetClass(who))))
                                              temp:= temp * ES_PENALTY;
                                           endif
                                           if (!temp || temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      SPELL_ONHIT_ID:      es:=es + 1;
                      RESIST_ONHIT_ID:     es:=es + 1;
                      STR_MOD_ID:          temp:=GetObjProperty(item, "str") / 5;
                                           if (!temp || temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      INT_MOD_ID:          temp:=GetObjProperty(item, "int") / 5;
                                           if (!temp || temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      DEX_MOD_ID:          temp:=GetObjProperty(item, "dex") / 5;
                                           if (!temp || temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      NECRO_PROT_ID:       temp:= GetObjProperty(item, "NecroProtection");
                                           if (!temp or temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      HOLY_PROT_ID:        temp:= GetObjProperty(item, "HolyProtection");
                                           if (!temp or temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      AIR_PROT_ID:         temp:= GetObjProperty(item, "AirProtection");
                                           if (!temp or temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      EARTH_PROT_ID:       temp:= GetObjProperty(item, "EarthProtection");
                                           if (!temp or temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      FIRE_PROT_ID:        temp:= GetObjProperty(item, "FireProtection");
                                           if (!temp or temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      WATER_PROT_ID:       temp:= GetObjProperty(item, "WaterProtection");
                                           if (!temp or temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      PHYS_PROT_ID:        temp:= GetObjProperty(item, "PhysicalProtection");
                                           if (!temp or temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      ACID_PROT_ID:        temp:= GetObjProperty(item, "AcidProtection");
                                           if (!temp or temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      TEMP_PROT_ID:        temp:=0;
                                           foreach prop in itemcprops
                                                   if (prop in TEMP_MODS)
                                                      temp:=temp + 1;
                                                   endif
                                                   es:=es + temp;
                                           endforeach
                      MOONSTONE_ID:        temp:= GetObjProperty(item, "Permmr");
                                           if (!temp || temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      POISON_PROT_ID:        temp:= GetObjProperty(item, "PermPoisonImmunity");
                                           if (!temp || temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      BLACKROCK_ID:        temp:= GetObjProperty(item, "PermMagicImmunity");
                                           if (!temp || temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;
                      STYGIAN_ID:          es:=es + 3;
                      MYSTICAL_ID:         es:=es + 2;
                      SWIFT_ID:            es:=es + 2;
                      SPELL_POWER_ID:      temp:= abs(GetObjProperty(item, "MagicPenalty") / 2);
                                           if (!temp || temp < 0)
                                              temp:=0;
                                           endif
                      FREE_ACTION_ID:      es:=es + 2;
                      HEAL_BONUS_ID:       temp:=GetObjProperty(item, "HealingIncrease") / 2;
                                           if (!temp || temp < 0)
                                              temp:=0;
                                           endif
                                           es:=es + temp;

                 endcase
                 if (prop in GetClassePenaltyProps(GetClass(who)))
                    es:=es * ES_PENALTY;
                 endif
                 totales:= totales + es;
                 es:=0;
         endforeach
         
         return CInt(totales);
         
endfunction

function GetClassePenaltyProps (class)

         case (class)
              CLASSEID_BARD:          return {};
              CLASSEID_POWERPLAYER:   return {};
              CLASSEID_CRAFTER:       return {INT_MOD_ID,
                                             SPELL_POWER_ID};
              CLASSEID_MAGE:          return {DMG_MOD_ID,
                                             WEAP_SKILL_ID,
                                             AR_MOD_ID,
                                             AR_BONUS_ID,
                                             STR_MOD_ID,
                                             DEX_MOD_ID,
                                             PHYS_PROT_ID};
              CLASSEID_RANGER:        return {STR_MOD_ID,
                                             INT_MOD_ID,
                                             SPELL_POWER_ID};
              CLASSEID_THIEF:         return {AR_BONUS_ID,
                                             STR_MOD_ID,
                                             INT_MOD_ID,
                                             SPELL_POWER_ID};
              CLASSEID_WARRIOR:       return {SPELL_HIT_ID,
                                             INT_MOD_ID,
                                             DEX_MOD_ID,
                                             NECRO_PROT_ID,
                                             HOLY_PROT_ID,
                                             AIR_PROT_ID,
                                             EARTH_PROT_ID,
                                             FIRE_PROT_ID,
                                             WATER_PROT_ID,
                                             ACID_PROT_ID,
                                             TEMP_PROT_ID,
                                             MOONSTONE_ID,
                                             BLACKROCK_ID,
                                             FREE_ACTION_ID,
                                             SPELL_POWER_ID};
              CLASSEID_DRUID:         return {AR_BONUS_ID,
                                             STR_MOD_ID,
                                             DEX_MOD_ID,
                                             NECRO_PROT_ID,
                                             HOLY_PROT_ID};
              CLASSEID_NINJA:         return {AR_BONUS_ID,
                                             INT_MOD_ID,
                                             MOONSTONE_ID,
                                             BLACKROCK_ID,
                                             SPELL_POWER_ID};
              CLASSEID_DEATHKNIGHT:   return {DEX_MOD_ID,
                                             HOLY_PROT_ID,
                                             MOONSTONE_ID,
                                             BLACKROCK_ID};
              CLASSEID_NECROMANCER:   return {AR_BONUS_ID,
                                             WEAP_SKILL_ID,
                                             STR_MOD_ID,
                                             DEX_MOD_ID,
                                             HOLY_PROT_ID,
                                             PHYS_PROT_ID};
              CLASSEID_MONK:          return {AR_MOD_ID,
                                             AR_BONUS_ID};
              CLASSEID_PALADIN:       return {DEX_MOD_ID,
                                             NECRO_PROT_ID,
                                             MOONSTONE_ID,
                                             BLACKROCK_ID,
                                             SPELL_POWER_ID};
              CLASSEID_PRIEST:        return {DEX_MOD_ID,
                                             NECRO_PROT_ID,
                                             POISON_PROT_ID,
                                             FREE_ACTION_ID};
       endcase
       
endfunction
                                             
function GetMaxEquipSlots(who)

         var class:= GetClass(who);
         var ret:=0;
         case (class)
              CLASSEID_BARD:        ret:= (BASE_ES + (GetObjProperty(who, CLASSEID_BARD) * BARD_ES));
              CLASSEID_CRAFTER:     ret:= (BASE_ES + (GetObjProperty(who, CLASSEID_CRAFTER) * CRAFTER_ES));
              CLASSEID_MAGE:        ret:= (BASE_ES + (GetObjProperty(who, CLASSEID_MAGE) * MAGE_ES));
              CLASSEID_THIEF:       ret:= (BASE_ES + (GetObjProperty(who, CLASSEID_THIEF) * THIEF_ES));
              CLASSEID_RANGER:      ret:= (BASE_ES + (GetObjProperty(who, CLASSEID_RANGER) * RANGER_ES));
              CLASSEID_WARRIOR:     ret:= (BASE_ES + (GetObjProperty(who, CLASSEID_WARRIOR) * WARRIOR_ES));
              CLASSEID_DRUID:       ret:= (BASE_ES + (GetObjProperty(who, CLASSEID_DRUID) * DRUID_ES));
              CLASSEID_NINJA:       ret:= (BASE_ES + (GetObjProperty(who, CLASSEID_NINJA) * NINJA_ES));
              CLASSEID_DEATHKNIGHT: ret:= (BASE_ES + (GetObjProperty(who, CLASSEID_DEATHKNIGHT) * DEATHKNIGHT_ES));
              CLASSEID_NECROMANCER: ret:= (BASE_ES + (GetObjProperty(who, CLASSEID_NECROMANCER) * NECROMANCER_ES));
              CLASSEID_MONK:        ret:= (BASE_ES + (GetObjProperty(who, CLASSEID_MONK) * MONK_ES));
              CLASSEID_PALADIN:     ret:= (BASE_ES + (GetObjProperty(who, CLASSEID_PALADIN) * PALADIN_ES));
              CLASSEID_PRIEST:      ret:= (BASE_ES + (GetObjProperty(who, CLASSEID_PRIEST) * PRIEST_ES));
              CLASSEID_POWERPLAYER: ret:= (BASE_ES + (GetObjProperty(who, CLASSEID_POWERPLAYER) * POWERPLAYER_ES));
              default:              ret:= BASE_ES;
         endcase
         if (who.title_race=="Human")
         	ret:=ret + (GetClasseLevel(who) * HUMAN_BONUS_ES_PER_LEVEL);
         endif
         return ret;
endfunction


                                             
                      
                                           



                      
                                           
                                           
                                           
                                           
                                           
                                           
                                           




