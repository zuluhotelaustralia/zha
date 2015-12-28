use util;
const SYSEVENT_PLAYERDUELDEATH := 0x00008000;


const DUELSYS_GUMP_ADMIN := 0;
const DUELSYS_GUMP_AUTH := 1;
const DUELSYS_GUMP_CHECKLOOT := 2;
const DUELSYS_GUMP_OBSERVE := 3;

const DUELSYS_MSG_INFO := 1;
const DUELSYS_MSG_ERROR := 2;

//**************************** LOAD/SAVE/ERASE game options ************************//
function saveGame(stone)
  set_critical(1);
  setobjproperty(stone, "Game", {gameserial, gamename, gametype, GOlooting, GOdispel, teamOneArray, teamTwoArray});
  set_critical(0);
endfunction

function loadGame(stone)
  set_critical(1);
  var set := getobjproperty(stone, "Game");
  gameserial := set[1];
  gamename := set[2];
  gametype := set[3];
  GOlooting := set[4];
  GOdispel := set[5];
  teamOneArray := set[6];
  teamTwoArray := set[7];
  set_critical(0);
endfunction

function eraseGame(stone)
  EraseObjProperty(stone, "#authsent");
  EraseObjProperty(stone, "Game");
endfunction
//**********************************************************************************//


//******************************* Message Functions ********************************//
function broadcastDuelArea(text, stone)
  var broadcastColor := GetObjProperty(stone, "#bcColor");
  if (!broadcastColor)
    broadcastColor := 80;
  endif

  foreach player in EnumerateOnlineCharacters()
    if (IsPersonInArea(player, ArenaArea))
      SendSysMessage(player, text, 3, broadcastColor);
    endif
  endforeach
endfunction

function sendMsg(who, text, msgtype := 1)
  var colour := 0;
  case (msgtype)
    1: colour := 54; // Informative
    2: colour := 37; // Error
  endcase

  SendSysMessage(who, text, 3, colour);
endfunction

function msgPlayers(text, msgtype := 1)
  foreach player in getPlayersObjects(teamOneArray, teamTwoArray)
    sendMsg(player, text, msgtype);
  endforeach
endfunction

function Commentation(stone, msgtype := 0, person := 0, otherperson := 0)
// 1 - Death of player
// 2 - Log out of player

  var message;
  if(msgtype == 1)
    if (person && otherperson)
      var rnd := randomInt(3);
      case (rnd)
        0: message := "Ouch! "+person+" sucks on "+otherperson+"'s Boomstick";
        1: message := person+" is taken out by "+otherperson+"!";
        2: message := "Knock out! "+person+" is outta the game thanks to "+otherperson;
      endcase
    endif
  elseif(msgtype == 2)
    if (person)
      var rnd := randomInt(2);
      case (rnd)
        0: message := "WHAT!? "+person+" logs out on the battle field!";
        1: message := person+" logs out! Its a loss by default.";
      endcase
    endif
  elseif(msgtype == 3)
    if (person)
      var rnd := randomInt(2);
      case (rnd)
        0: message := "EH!? "+person+" recalls out! What a wuss!";
        1: message := person+" recalls out! Its a loss by default.";
      endcase
    endif
  endif

  if (message)
    broadcastDuelArea(message, stone);
  endif
endfunction

function GetRandomColor()
  var rnd := RandomInt(300);
  var color := rnd + 2;
/*  var color := RandomDiceStr( "1d"+1184);
  while( color > 999 && color < 1152 )
    color := RandomDiceStr( "1d"+1184);
  endwhile*/
  return color;
endfunction
//**********************************************************************************//




//********************************* Area Functions *********************************//
function IsPersonInArea(person, area)
  if(person.x >= area[1])
    if(person.x <= area[2])
      if(person.y >= area[3])
        if(person.y <= area[4])
          return 1;
        endif
      endif
    endif
  endif
  return 0;
endfunction

function GetAreaLocation(who)
  var tOne, tTwo;

  SendSysMessage(who, "Select one corner of the boxed area.");  
  tOne := TargetCoordinates(who);

  SendSysMessage(who, "Select the oposite corner of the boxed area.");
  tTwo := TargetCoordinates(who);

  var area := GetAreaLocationArray(tOne.x, tTwo.x, tOne.y, tTwo.y, tOne.z, tTwo.z);
  if(!area)
    SendSysMessage(who, "Z levels of both spots were not the same!");
    return 0;
  endif

  return area;
endfunction

function pickRandomSpotInArea(area)
  var range := area[2] - area[1];
  var randomnumber := RandomInt(range+1);
  var spotx := area[1]+randomnumber;

  range := area[4] - area[3]; 
  randomnumber := RandomInt(range+1);
  var spoty := area[3] + randomnumber;

  return {spotx, spoty, area[5]};
endfunction

function loadAreas(stone)
  var settings := GetObjProperty(stone, "settings");
  battleArea := GetAreaLocationArray(settings[1][1], settings[1][2], settings[1][3], settings[1][4], settings[1][5], settings[1][5]);
  arenaArea := GetAreaLocationArray(settings[2][1], settings[2][2], settings[2][3], settings[2][4], settings[2][5], settings[2][5]);
  teamOneArea := GetAreaLocationArray(settings[3][1], settings[3][2], settings[3][3], settings[3][4], settings[3][5], settings[3][5]);
  teamTwoArea := GetAreaLocationArray(settings[4][1], settings[4][2], settings[4][3], settings[4][4], settings[4][5], settings[4][5]);
  exitArea := GetAreaLocationArray(settings[5][1], settings[5][2], settings[5][3], settings[5][4], settings[5][5], settings[5][5]);
  linkedBoard := settings[6];

  if(!battleArea || !arenaArea || !teamOneArea || !teamTwoArea || !exitArea)
    return 0;
  endif

  //broadcastDuelArea("Duel Stone has now finished setting up and is ready for use.");
  return 1;
endfunction

function GetAreaLocationArray(xOne, xTwo, yOne, yTwo, zOne, zTwo)
  var xMin, xMax, yMin, yMax;

  if (zOne != zTwo)
    return 0;
  endif

  if(xOne < xTwo)
    xMin := xOne;
    xMax := xTwo;
  else
    xMin := xTwo;
    xMax := xOne;
  endif
  if(yOne < yTwo)
    yMin := yOne;
    yMax := yTwo;
  else
    yMin := yTwo;
    yMax := yOne;
  endif

  return({xMin, xMax, yMin, yMax, zOne});
endfunction

function SetupStone(who, stone)
  SetObjProperty(stone, "LFucker", who.name+who.serial);
  var battle, arena, teamOne, teamTwo, exitA, linkedB;

  SendSysMessage(who, "You will now need to select areas for the system. Please make sure the areas are flat.");
  SendSysMessage(who, "There will be a "+SETUPTARGETSLEEP+" second delay so you can teleport to the required area if needed.");

  // Battle Area
    SendSysMessage(who, "Please select Battle Area.");
    sleep(SETUPTARGETSLEEP);
    battle := GetAreaLocation(who);
    if (!battle) return; endif
  // Arena Area
    SendSysMessage(who, "Please select Arena Area.");
    sleep(SETUPTARGETSLEEP);
    arena := GetAreaLocation(who);
    if (!arena) return; endif
  // TeamOneArea
    SendSysMessage(who, "Please select Team One Starting Area.");
    sleep(SETUPTARGETSLEEP);
    teamOne := GetAreaLocation(who);
    if (!teamOne) return; endif
  // TeamTwoArea
    SendSysMessage(who, "Please select Team Two Starting Area.");
    sleep(SETUPTARGETSLEEP);
    teamTwo := GetAreaLocation(who);
    if (!teamTwo) return; endif
  // Exit Area
    SendSysMessage(who, "Please select Exit Area.");
    sleep(SETUPTARGETSLEEP);
    exitA := GetAreaLocation(who);
    if (!exitA) return; endif
 
  // Linked Stat Board
    SendSysMessage(who, "Please select a stats board. Press esc if none.");
    sleep(SETUPTARGETSLEEP);
    linkedB := Target(who, TGTOPT_NOCHECK_LOS+TGTOPT_NEUTRAL);
    if(!linkedB)
      linkedB := -1;
    endif

  SetObjProperty(stone, "settings", {battle, arena, teamOne, teamTwo, exitA, linkedB});
  SendSysMessage(who, "The duel stone has now been set up.");
  SetObjProperty(stone, "setup", 1);
endfunction
//**************************************************************************************//







//**************************** Get functions ****************************************//
function getPlayersArray(teamOne, teamTwo)
  var playersArray := {};
  foreach player in teamOne
    playersArray.append(player);
  endforeach
  foreach player in teamTwo
    playersArray.append(player);
  endforeach
  return playersArray;
endfunction

function getPlayersObjects(teamOne, teamTwo)
  var playersObjects := {};
  foreach player in getPlayersArray(teamOne, teamTwo)
    playersObjects.append(getPerson(player[1]));
  endforeach
  return playersObjects;
endfunction

function getAlivePlayersOnTeam(team)
  var returnArray := {};
  foreach player in team
    if(player[2] == 1)
      returnArray.append(player[1]);
    endif
  endforeach
  return returnArray;
endfunction

function getAliveObjects(teamOne, teamTwo)
  var returnArray := {};
  foreach player in getPlayersArray(teamOne, teamTwo)
    if(player[2] == 1)
      returnArray.append(getPerson(player[1]));
    endif
  endforeach
  return returnArray;
endfunction

function getMemberStatus(memberSerial, teamOne, teamTwo)
  foreach person in getPlayersArray(teamOne, teamTwo)
    if (person[1] == memberSerial)
      return person[2];
    endif
  endforeach
  return 0;
endfunction

function getTeamAsObjects(team)
  var playersObjects := {};
  foreach player in team
    playersObjects.append(getPerson(player[1]));
  endforeach
  return playersObjects;
endfunction

function getNameListOfTeam(teamarray)
  var nameList := "";
  var i;
  for (i := 1; i <= teamarray.size(); i := i + 1)
    nameList := nameList + getNameFromSerial(teamarray[i][1]);
    if (i < teamarray.size())
      nameList := nameList + ", ";
    endif
  endfor
  return nameList;
endfunction

function getLeaderObjectOfTeam(teamArray)
  foreach player in teamArray
    if (player[2] == 1)
      return GetPerson(player[1]);
    endif
  endforeach
  return 0;
endfunction

function getPlayersTeam(serial, teamOne, teamTwo)
  foreach player in teamOne
    if (player[1] == serial)
      return 1;
    endif
  endforeach
  foreach player in teamTwo
    if (player[1] == serial)
      return 2;
    endif
  endforeach
  return 0;
endfunction

//***** System Related Get functions ******//
function getPerson(serial)
  return SystemFindObjectBySerial(serial, SYSFIND_SEARCH_OFFLINE_MOBILES);
endfunction

function getNameFromSerial(serial)
  return getPerson(serial).name;
endfunction
//****************************************//
//**************************************************************************************//

//**************************** Set functions ****************************************//
function setMemberStatus(memberSerial, status, byref teamOne, byref teamTwo)
  foreach person in teamOne
    if (person[1] == memberSerial)
      person[2] := status;
      return 1;
    endif
  endforeach
  foreach person in teamTwo
    if (person[1] == memberSerial)
      person[2] := status;
      return 1;
    endif
  endforeach
  return 0;
endfunction
//**************************************************************************************//

//*****************************  Process Player FUnctions ******************************//
function RefreshPlayer(player)
  SetHp(player,GetMaxHp(player));
  SetMana(player,GetMaxMana(player));
  SetStamina(player,GetMaxStamina(player));
endfunction
//**************************************************************************************//





//**************************** Stat recording functions ********************************//
function RecordStat(playerSerial, win)
  if(linkedBoard != -1)
    playerSerial := 0;
    win := 0;
  endif
  return 1;
endfunction
//**************************************************************************************//






//********************************* Gump Related Functions******************************//
function getStatusColour(status)
  if(status == 0)
    return 0;
  elseif(status == 1)
    return 72;
  else
    return 37;
  endif

endfunction

function getGameTypeString(gametype)
    if (gametype == 1)
      return "PvP";
    else
      return "PvM";
    endif
endfunction

function getunauthenticatedmembers(byref teamOne, byref teamTwo)
  var returnArray := {};
  foreach person in teamOne
    if (person[2] == 0 || person[2] == -1)
      returnArray.append(person[1]);
    endif
  endforeach
  foreach person in teamTwo
    if (person[2] == 0 || person[2] == -1)
      returnArray.append(person[1]);
    endif
  endforeach
  return returnArray;
endfunction

function getAuthOnTeam(byref team)
  var returnArray := {};
  foreach person in team
    if (person[2] == 1)
      returnArray.append(person[1]);
    endif
  endforeach
  return returnArray;
endfunction

function AddTeammates(byref teamArray, who)
  var count := teamArray.size();
  var tgt := 1;
  SendSysMessage(who, "Select the people to Add to this team. Press Esc when finished.");
  while(tgt && count <= 9)
    SendSysMessage(who, "Select Team member "+(count+1)+".");
    tgt := Target(who, TGTOPT_NOCHECK_LOS+TGTOPT_NEUTRAL);
    if(tgt && canaddmember(tgt, who))
      teamArray.append({tgt.serial, 0});
      count := count + 1;
    endif
    if (count > 9)
      SendSysMessage(who, "Max Team size reached.");
    endif
  endwhile
endfunction

function CheckPlayersLeftArena(owner, validpregame)
  foreach person in getPlayersObjects(teamOneArray, teamTwoArray)
    if (!IsPersonInArea(person, ArenaArea))
      SendMsg(owner, person.name+" has left the arena.");
      return -1;
    elseif (!(person in EnumerateOnlineCharacters()))
      SendMsg(owner, person.name+" has logged off.");
      return -1;
    elseif (person.dead)
      SendMsg(owner, person.name+" is dead.");
      return -1;
    endif
  endforeach
  return validpregame;
endfunction

//***** Can functions ******//
function canaddmember(tgt, who)
  var isAlreadyInTeam := 0;
  foreach person in teamOneArray
    if (person[1] == tgt.serial)
      isAlreadyInTeam := 1;
    endif
  endforeach
  foreach person in teamTwoArray
    if (person[1] == tgt.serial)
      isAlreadyInTeam := 1;
    endif
  endforeach

  if(isAlreadyInTeam)
    SendSysMessage(who, "This person is already in a team.");
    return 0;
  elseif(tgt.dead)
    SendSysMessage(who, "This person is dead.");
    return 0;
  elseif(tgt.IsA(POLCLASS_NPC))
    SendSysMessage(who, "This person is a NPC!");
    return 0;
  else
    return 1;
  endif
endfunction

function canStartGame(who)
  if(teamOneArray.size() < 1 || teamTwoArray.size() < 1)
    sendMsg(who, "You must have at least 1 person on each team to start a game.",1);
    return 0;
  elseif(getAuthOnTeam(teamOneArray).size() < 1 || getAuthOnTeam(teamTwoArray).size() < 1)
    sendMsg(who, "You must have at least 1 person on each team authenticated to start a game.",1);
    return 0;
  else
    return 1;
  endif
endfunction

function cansendauth()
  if(teamOneArray.size() > 0 && teamTwoArray.size() > 0)
    return 1;
  else
    return 0;
  endif
endfunction
//**************************//

//***** Is functions ******//
function iseveyonevalid()
  var someoneInvalid := 0;
  foreach person in teamOneArray
    if (person[2] == 0 || person[2] == -1)
      someoneInvalid := 1;
    endif
  endforeach
  foreach person in teamTwoArray
    if (person[2] == 0 || person[2] == -1)
      someoneInvalid := 1;
    endif
  endforeach

  if(someoneInvalid)
    return 0;
  else
    return 1;
  endif
endfunction
//**************************//
//**************************************************************************************//


//***************************************** Gump Functions******************************//
function GameDetails(who, stone, options := 0)
  var width := 0;
  var height := 0;
  var authsent := GetObjProperty(stone, "#authsent");
  var owner := GetObjProperty(stone, "#inuse");

  var lootingNo := 0;
  if (GOlooting)
    lootingNo := 37;
  endif

  var gfxtype := 2600;
  if(options == DUELSYS_GUMP_OBSERVE)
    gfxtype := 3500;
  elseif(options == DUELSYS_GUMP_AUTH || options == DUELSYS_GUMP_CHECKLOOT)
    gfxtype := 5170;
  endif

  var gamelayout := {
                  "nodispose",
                  "page 0",
                  "resizepic "+width+" "+height+" "+gfxtype+" 400 450",
                  "text 45 20 52 0",
                  "page 1",
                  "text 70 40 995 1", // Teams
                  "text 270 40 36 2",


                  "text 20 280 10 3", // Game Optins
                  "text 50 300 "+lootingNo+" 4", // Looting
                  "text 50 320 0 5", // Dispel on entry

                  "GumpPic 200 50 9155", // Middle Line
                  "GumpPic 200 65 9155",
                  "GumpPic 200 80 9155",
                  "GumpPic 200 95 9155",
                  "GumpPic 200 110 9155",
                  "GumpPic 200 125 9155",
                  "GumpPic 200 140 9155",
                  "GumpPic 200 155 9155",
                  "GumpPic 200 170 9155",
                  "GumpPic 200 185 9155",
                  "GumpPic 200 200 9155",
                  "GumpPic 200 215 9155",
                  "GumpPic 200 230 9155",
                  "GumpPic 200 245 9155",
                  "GumpPic 200 260 9155"



                  };

  var lootingString := "No Looting Allowed";
  if (GOlooting)
    lootingString := "LOOTING ALLOWED!";
  endif

  var dispelString := "No dispeling or refreshing at the start";
  if (GOdispel)
    dispelString := "Each player is dispeled at the start";
  endif

  var gamedata := {
                getGameTypeString(gametype)+" Duel: "+gamename,
                "Team One",
		"Team Two",
                "Game options:",
                lootingString,
                dispelString
                };
  var textCount := 6;



  gamelayout.append("button 72 400 2130 2129 1 0 1000"); // Ok
  gamelayout.append("button 272 400 2119 2120 1 0 1001"); // Cancel
  gamelayout.append("button 160 400 4011 4013 1 0 1002"); // Update
  gamelayout.append("text 190 400 0 "+textCount);
  gamedata.append("Update"); 
  textCount := textCount + 1;

  var i := 0;
  foreach player in teamOneArray
    gamelayout.append("text 50 "+CInt(60+(i*20))+" 0 "+textCount); // Number
    gamedata.append(CInt(i+1)+":"); 
    textCount := textCount + 1;
    gamelayout.append("text 65 "+CInt(60+(i*20))+" "+getStatusColour(player[2])+" "+textCount); // Name
    gamedata.append(getNameFromSerial(player[1])); 
    textCount := textCount + 1;
    i := i + 1;
  endforeach

  i := 0;
  foreach player in teamTwoArray
      gamelayout.append("text 240 "+CInt(60+(i*20))+" 0 "+textCount); // Number
      gamedata.append(CInt(i+1)+":"); 
      textCount := textCount + 1;
      gamelayout.append("text 255 "+CInt(60+(i*20))+" "+getStatusColour(player[2])+" "+textCount); // Name
      gamedata.append(getNameFromSerial(player[1])); 
      textCount := textCount + 1;
      i := i + 1;
  endforeach


  if(owner == who.serial && options != 1 && options != 3)
    gamelayout.append("button 20 380 4029 4031 1 0 12"); // Send Request to people.
    gamelayout.append("text 50 380 0 "+textCount);
    gamedata.append("Send Join Request to Unauthenticated Members."); 
    textCount := textCount + 1;

    if(!authsent)
  
      gamelayout.append("button 20 260 4008 4010 1 0 10"); // Add Team One Member
      gamelayout.append("text 50 260 0 "+textCount);
      gamedata.append("Add to Team One"); 
      textCount := textCount + 1;
      gamelayout.append("button 220 260 4008 4010 1 0 11"); // Add Team One Member
      gamelayout.append("text 250 260 0 "+textCount);
      gamedata.append("Add to Team Two"); 
      textCount := textCount + 1;

      for(i := 0; i < teamOneArray.size(); i := i + 1)
        gamelayout.append("button 20 "+CInt(60+(i*20))+" 4002 4004 1 0 "+Cint(100+i)); // Remove Player
      endfor

      for(i := 0; i < teamTwoArray.size(); i := i + 1)
        gamelayout.append("button 210 "+CInt(60+(i*20))+" 4002 4004 1 0 "+Cint(200+i)); // Remove Player
      endfor
    endif
  endif 

  if(options == DUELSYS_GUMP_AUTH)
    gamelayout.append("text 20 360 54 "+textCount);
    gamedata.append("You have been asked to join a duel between these teams.");
    textCount := textCount + 1;
    gamelayout.append("text 20 380 54 "+textCount);
    gamedata.append("Take a look at the rules. Do you accept this game?");
    textCount := textCount + 1;
  endif

  if(options == DUELSYS_GUMP_CHECKLOOT)
    gamelayout.append("gumppic 5 100 1140");
    gamelayout.append("text 70 140 37 "+textCount);
    gamedata.append("This is a looting battle.");
    textCount := textCount + 1;
    gamelayout.append("text 70 160 54 "+textCount);
    gamedata.append("If you accept this battle and die or resign");
    textCount := textCount + 1;
    gamelayout.append("text 70 180 54 "+textCount);
    gamedata.append("you will lose all of the stuff on you!");
    textCount := textCount + 1;
    gamelayout.append("text 70 200 54 "+textCount);
    gamedata.append("Are you sure you wish to join this duel?");
    textCount := textCount + 1;
    gamelayout.append("button 101 250 2130 2129 1 0 1003"); // Ok
    gamelayout.append("button 251 250 2119 2120 1 0 1004"); // Cancel
  endif



  var result := SendDialogGump(who, gamelayout, gamedata);

  if (options == DUELSYS_GUMP_CHECKLOOT)
    if (result[1003])
      return 1;
    elseif (result[1004])
      return 0;
    endif
  endif
  if (result[1000])
    if (options == DUELSYS_GUMP_AUTH || options == DUELSYS_GUMP_CHECKLOOT)
      loadGame(stone);
      if(GOLooting)
        return GameDetails(who, stone, DUELSYS_GUMP_CHECKLOOT);
      else
        return 1;
      endif
    else
      loadGame(stone);
      if(!iseveyonevalid())
        sendMsg(who, "You can only start a game once everyone has accepted.");
        return 0;
      else
        return 1;
      endif
    endif
  elseif (result[1001])
    SendSysMessage(who, "Canceled.");
    return -1;
  elseif (result[1002])
    if (options == 1)
      return 2;
    else
      return 0;
    endif
  elseif(result)
    if((GetObjProperty(stone, "#inuse") == who.serial))
      if (result[10])
        if(options != 1 && options != 3 && !authsent)
          AddTeammates(teamOneArray, who);
        endif
      elseif(result[11])
        if(options != 1 && options != 3 && !authsent)
          AddTeammates(teamTwoArray, who);
        endif
      elseif(result[12])
        if (cansendauth() && options != 3)
          if(GetObjProperty(stone, "#authsent"))
            loadGame(stone);
          else
            saveGame(stone);
            SetObjProperty(stone, "#authsent", 1);
          endif
          if(getunauthenticatedmembers(teamOneArray, teamTwoArray).size() < 1)
            SendSysMessage(who, "There is no one to send the authentication windows to.", 3, 54);
          else
            foreach person in getunauthenticatedmembers(teamOneArray, teamTwoArray)
              Start_Script(":duelsystem:sendauthentication", {stone.serial, person, gameserial});
            endforeach
          endif
        else
          SendSysMessage(who, "You cannot send Authentication windows until there is at least 1 person on each team.", 3, 54);
        endif
      else
        if(options != 1 && options != 3 && !authsent)
          var topkey;    foreach key in (result.keys)      topkey := key;    endforeach
          if (topkey >= 100 && topkey <= 109)
            TeamOneArray.erase(topkey-100+1);
          elseif (topkey >= 200 && topkey <= 209)
            TeamTwoArray.erase(topkey-200+1);
          endif
        endif
      endif
    endif
    savegame(stone);
  endif
  return 0;
endfunction

function setUpNewGame(who)
  var setuplayout := {
                  "nodispose",
                  "page 0",
                  "resizepic 100 100 2600 300 250",
                  "text 145 120 52 0",
                  "page 1",
                  "text 130 140 10 1", // Name
                  "GumpPic 165 140 2446",
                  "TextEntry 175 140 155 20 39 0 2",

                  "text 130 160 10 3", // Game Types
                  "radio 200 165 210 211 1 50", // PvP
                  "text 220 165 0 4",
//                  "GumpPic 200 185 210",
                  "radio 200 185 210 211 0 51", // PvM
                  "text 220 185 0 5",
                  "GumpPic 200 205 210", // Championship
                  "text 220 205 0 6",

                  "text 130 230 10 7", // Game Optins
                  "checkbox 130 250 210 211 0 10", // Looting
                  "text 150 250 0 8",
                  "checkbox 130 270 210 211 1 11", // Dispel
                  "text 150 270 0 9",

//                  "text 130 270 0 9", // Covered Battlefield
//                  "GumpPic 280 270 2443",
//                  "TextEntry 285 270 30 20 39 11 10",

                  "button 150 300 2130 2129 1 0 100", // Ok
                  "button 300 300 2119 2120 1 0 101" // Cancel
                  };

  var setupdata := {
                "Create New Duel Game",
                "Name:",
		CStr(who.name+"'s Battle"),
                "Game Type:",
                "Player Vs Player",
                "Player Vs Monster",
                "Championship Mode",
                "Game Options:",
                "Looting Allowed",
                "Dispel and Refresh players",
                0
                };
  var result := SendDialogGump(who, setuplayout, setupdata);


  if (!result[100])
    SendSysMessage(who, "Canceled.");
    return 0;
  else
    gamename := result[0][4, len(result[0])];
    if (result[50])
      gametype := 1;
    elseif (result[51])
      gametype := 2;
    else
      SendSysMessage(who, "Invalid Game Type");
      return 0;
    endif

    if (result[10])
      GOlooting := 1;
    else
      GOlooting := 0;
    endif

    if (result[11])
      GOdispel := 1;
    else
      GOdispel := 0;
    endif

/*  GOobjects := CInt(result[11][4, len(result[11])]);
    if(!GOobjects)
      GOobjects := 0;
    else
      if(GOobjects < 0 || GOobjects > 100)
        SendSysMessage(who, "Percent of objects needs to be between 0 and 100.");
        return 0;
      endif
    endif*/

  endif

  return 1;
endfunction
//**************************************************************************************//