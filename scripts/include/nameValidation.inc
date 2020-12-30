use uo;
use os;

function ValidStr( thestring )

         var i,
         valid_chars := {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
  "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", ",", ".", "?", "/", "[", "]", "(", ")", "%", "$", "#", "@", "!",
  "^", "&", "*", ":", ";", "+", "-", "'", "`", " "};

         thestring := Lower( thestring );

         var flag := 1;

         for( i:=1; i<=Len( thestring ); i += 1 )
                  if( !( thestring[i] in valid_chars ))
                           flag := 0;
                  endif
                  SleepMS(5);
         endfor

         return flag;
endfunction