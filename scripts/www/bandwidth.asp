<%
use os;
use uo;
if (QueryIP() != "127.0.0.1") WriteHtmlRaw("Access Denied."); return 0; endif
%>

<HTML>
<BODY>
<% 
  include header; 
  PrintHeader( "Bandwidth Usage" );
%>

<table border=1 cellspacing=0 cellpadding=5>
  <tr bgcolor=#99CCFF>
    <td>Bytes Sent</td>
    <td>bps out</td>
    <td>Received</td>
    <td>bps in</td>
  </tr>

<% foreach elem in (GetGlobalProperty( "#:servperf:bandwidth" )) %>
  <tr>
    <td><%=elem[2]%></td>
    <td><%=elem[2]/6%></td>
    <td><%=elem[1]%></td>
    <td><%=elem[1]/6%></td>
  </tr>
<% endforeach %>
</table>

</body>
</html>
