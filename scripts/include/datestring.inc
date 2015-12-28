//Barnabus 20-3-2002 
//datestring.inc includes functions designed to deal with the comparison and manipulation of the date strings produced by 
//Now() in time.inc

function compareDateTimeStrings(datestr1, datestr2)
	//compares to date time strings (e.g. type produced by NowString()) and gives comparison int
	//in relation to 2nd. If 1st > 2nd then res:=1, 1st == 2nd then res:=0, 1st < 2nd res :=-1 
	
	//a date is greater than another if
	//year > year || (year==year&&month>month) || year==year&&month==month&&day>day)
	
	var res:=0;
	var year1:=getYear(datestr1);
	var year2:=getYear(datestr2);
	var month1:=getMonth(datestr1);
	var month2:=getMonth(datestr2);
	var day1:=getDay(datestr1);
	var day2:=getDay(datestr2);

	//matching case
	if ((year1==year2)&&(month1==month2)&&(day1==day2))
		return 0;
	endif
	
	//test whether date1 is greater than date2.. assumes cut off OR evaluation
	if ((year1 > year2) || (year1==year2&&month1>month2) || (year1==year2&&month1==month2&&day1>day2))
		return 1;
	endif
	return -1;
endfunction

function getYear(datestr)
	//returns year int from date string
	var myStr := "" + datestr[1] + datestr[2]+ datestr[3] + datestr[4]; 
	return CInt(myStr);
endfunction

function getMonth(datestr)
	//returns month int from date string
	var myStr := "" + datestr[6] + datestr[7]; 
	return CInt(myStr);
endfunction

function getDay(datestr)
	//returns day int from date string
	var myStr := "" + datestr[9] + datestr[10]; 
	return CInt(myStr);
endfunction

function takeMonthsFromDateString(datestr, monthsToSubtract)
//this function simply returns a date string to which 3 months have been subtracted
	var year:=getYear(datestr) ;
	var month:=getMonth(datestr);
	var day:=getDay(datestr);
	if (month-monthsToSubtract>=1)
		month:=month+monthsToSubtract;
	else
		year:=year-1;
		month:=12-(monthsToSubtract-month);
	endif
	
	var monthstr;
	//make sure monthstr is two digit
	if (month <10) 
		monthstr := "0"+month;
	else
		monthstr := CStr(month);
	endif
	
	var daystr;
	if (day <10) 
		daystr := "0"+day;
	else
		daystr := CStr(day);
	endif
	
	return "" + year + "-"  + monthstr +"-" + daystr;
endfunction


function addMonthsToDateString(datestr, monthsToAdd)
//this function simply returns a date string to which 3 months have been added
	var year:=getYear(datestr) ;
	var month:=getMonth(datestr);
	var day:=getDay(datestr);
	if (month+monthsToAdd<=12)
		month:=month+monthsToAdd;
	else
		year:=year+1;
		month:=monthsToAdd-(12-month);
	endif
	
	
	var monthstr;
	//make sure monthstr is two digit
	if (month <10) 
		monthstr := "0"+month;
	else
		monthstr := CStr(month);
	endif
	
	var daystr;
	if (day <10) 
		daystr := "0"+day;
	else
		daystr := CStr(day);
	endif
	
	
	return "" + year + "-"  + monthstr +"-" + daystr;
endfunction