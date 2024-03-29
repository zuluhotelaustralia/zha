///////////////////////////////////////////////////////////////////////////
//
//  buddies.inc
//
//  Author: rje
//
//  Date: 12/31/98
//
//  Functions for assigning leaders and buddies,
//  and for following other critters.
//
///////////////////////////////////////////////////////////////////////////

var myvalue      := 0; // for bu_setLeader() (should be overwritten 1st)
var leader       := 0; // for leader functions

var buddy1       := 0; // for buddy functions
var buddy2       := 0; // for buddy functions

var mindistance  := 2;
var maxdistance  := 8;

////////////////////////////////////////////////////
//
//  value() and setMyValue()
//
//  Part of the leader function,
//  this function establishes an arbitrary value
//  for a unit.
//
////////////////////////////////////////////////////
//include "include/attributes";
function bu_value( dude )

   return dude.maxStrength
        + dude.maxDexterity
        + GetMaxStamina(dude)
        + dude.pack_order;

endfunction

function bu_setMyValue( factor )

   myvalue := bu_value( me ) * factor;

endfunction

////////////////////////////////////////////////////
//
//  setLeader()
//
//  This sets my 'leader' variable, if it is not
//  already set, and if the dude in question is
//  more valuable than me.
//
////////////////////////////////////////////////////
function bu_setLeader( dude )

   if ( leader == 0
     && myvalue < bu_value( dude ) )

      var msg := GetObjProperty( me, "leaderText" );

      leader := dude;
      SetObjProperty( me, "leader", dude.serial );
      if (msg)
         say( msg );
      endif
      return 1;
   else
      return 0;
   endif

endfunction

////////////////////////////////////////////////////
//
//  setBuddy() tries to give a creature a buddy.
//  This creature can have up to two buddies it
//  tags along with.  Together, a group of creatures
//  will exhibit a form of flocking behavior, as
//  each one seeks to be near the others, but not
//  TOO near.  Being too close renders the group
//  more vulnerable to area-effect attacks.
//
////////////////////////////////////////////////////
function bu_setBuddy( dude )

   if ( (dude.objtype == me.objtype )            // same species
     && !(buddy1.serial == dude.serial)         // not buddy1
     && !(buddy2.serial == dude.serial) )       // not buddy2

      var msg := GetObjProperty( dude, "buddyText" );

      if (buddy1 == 0)
         buddy1 := dude;                         // now he's buddy1
         if (msg)
            say( msg );
         endif

      elseif (buddy2 == 0)                        // now he's buddy2
         buddy2 := dude;
         if (msg)
            say( msg );
         endif

      endif
   endif

endfunction

////////////////////////////////////////////////////
//
//  checkBuddies()
//
//  Check the status of buddies and leader, and
//  re-set any dead ones.
//
////////////////////////////////////////////////////
function bu_checkBuddies()

   if (buddy1.dead)
      buddy1 := 0;  // if i could, you know i would ...
   endif

   if (buddy2.dead)
      buddy2 := 0;  // if i could, i would ... 
   endif

   if (leader.dead)
      leader := 0;  // let it go ... (words by U2)
   endif

endfunction

/////////////////////////////////////////////////////////
//
//  follow()
//
//  This function makes sure I am within the proper
//  distance from dude.
//
//  It returns a 1 if some adjustment was necessary,
//  or 0 if I didn't have to move.
//
/////////////////////////////////////////////////////////
function bu_follow( dude )

   if (dude == 0)
      return 0;
   endif

   var d := Distance( me, dude );

   if ( d > CInt( maxdistance ) )
      WalkToward( dude );
   elseif( d < CInt( mindistance ) )
      RunAwayFrom( dude );
   else
      return 0;
   endif

   return 1;

endfunction

///////////////////////////////////////////////////////
//
//  A facade for managing buddies.
//
///////////////////////////////////////////////////////
function bu_followBuddies()

   bu_checkBuddies();
   bu_follow( buddy1 );
   bu_follow( buddy2 );

endfunction

function bu_followLeader()

   bu_checkBuddies();
   bu_follow( leader );

endfunction

///////////////////////////////////////////////////////
//
//  setDistances()
//
//  Set the distances a creature will keep from a 
//  leader or buddy.
//
///////////////////////////////////////////////////////
function bu_setDistances( min, max )

   mindistance := min;
   maxdistance := max;

endfunction