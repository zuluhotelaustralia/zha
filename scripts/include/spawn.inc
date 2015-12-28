
function Icanspawn(thechar)
foreach person in EnumerateOnlineCharacters()
        if ((GetObjProperty(person,"canspawn")) && (person.serial != thechar.serial))
                return 0;
        endif
endforeach
SetObjProperty(thechar, "canspawn", 1);
return 1;
endfunction