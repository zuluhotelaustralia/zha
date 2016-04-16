// Try mimic what the old check line of sight did.
//POl97     Line of sight to items changed: will be from head of mobile to center of item
// I believe the old method used to check the ground tile of the mobile.
function CheckLineOfSight95(object1, object2)
        return CheckLosAt(object1, object2.x, object2.y, object2.z);
endfunction

function Target95(by_character, options := TGTOPT_CHECK_LOS+TGTOPT_NEUTRAL)
        var result := Target(by_character, options);
        if (result && HasLOSFlag(options) && by_character.enabled("losany") != 1)
                if (!CheckLineOfSight95(by_character, result)
                        SendSysMessage( by_character, "That is not in your line of sight." );
                        return 0;
                endif
        endif
        
        return result;
endfunction

function HasLOSFlag(flags)
        // Can't find a way to do a bitwise check :(
        return flags == TGTOPT_CHECK_LOS || flags = TGTOPT_CHECK_LOS + TGTOPT_HARMFUL || TGTOPT_CHECK_LOS + TGTOPT_HELPFUL;
endfunction