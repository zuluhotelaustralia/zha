include "include/client";

function PlayMusicConverted( character, music_id )

    var packet_str :=  ( "6D00" + music_id );

//       print("packetstr: " + packet_str);

    SendPacket( character, packet_str );

endfunction