<%
use uo;
use os;
if (QueryIP() != "127.0.0.1") WriteHtmlRaw("Access Denied."); return 0; endif
%>

<html>
<head>
<title>Script Profiles</title>
</head>

<body>

<% 
  include header; 
  PrintHeader( "Script Profiles" );
%>

<div align=center>
<table border=1 cellspacing=0 cellpadding=5>
  <tr bgcolor=#99CCFF>
    <td colspan=3>Packets Sent</td>
  </tr>
  <tr bgcolor=#99CCFF>
    <td>Message Type</td>
    <td>Count</td>
    <td>Bytes</td>
  </tr>
<%
  var iostats;

  iostats := polcore().iostats;
  for msgtype := 0 to 255
    if (iostats.sent[msgtype+1].count)
%>
  <tr>
	<td><%=msgtype%></td>
    <td><%=iostats.sent[msgtype+1].count%></td>
    <td><%=iostats.sent[msgtype+1].bytes%></td>
  </tr>
<%
    endif
  endfor
%>

</table>
</div>

</BODY>
</HTML>
