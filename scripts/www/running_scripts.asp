<%
use uo;
use os;
if (QueryIP() != "127.0.0.1") WriteHtmlRaw("Access Denied."); return 0; endif
%>

<html>
<head>
<title>Running Scripts</title>
</head>

<body>
<% 
  include header; 
  PrintHeader( "Running Scripts" );
%>

<div align=center>
<table border=1 cellspacing=0 cellpadding=5>
  <tr bgcolor=#99CCFF>
    <td>Script Name</td>
    <td>Cycles</td>
	<!--<td>Sleep Cycles</td>-->
    <td>Cycles since Sleep</td>
    <td>Prog Counter</td>
	<td>Call Depth</td>
	<td>Globals</td>
	<td>Var Size</td>
  </tr>

<%
  foreach script in (polcore().running_scripts)
%>
  <tr>
    <td><%=script.name%></td>
	<td><%=script.instr_cycles%></td>
	<!--<td><%=script.sleep_cycles%></td>-->
    <td><%=script.consec_cycles%></td>
	<td><%=script.PC%></td>
	<td><%=script.call_depth%></td>
	<td><%=script.num_globals%></td>
	<td><%=script.var_size%></td>
  </tr>
<%
    sleepms(1);
  endforeach
%>

</table>
</div>

</BODY>
</HTML>
