use math;


function Log(base, answer)

	var x := 1.000;
	var logcount := 0.000;
	var diff := 1.000;
	var success := 0;

	while (success == 0)

		x := x * base;

		if (Abs(answer - x) < 0.001)
			logcount := logcount + 1;
			success := 1;
		else
			if (x > answer)
				diff := diff / 2;
				logcount := logcount - diff;
				x := Pow(base, logcount);
			else
				logcount := logcount + diff;
			endif
		endif

		count := count + 1;

		if (count > 200)
			success := 1;
		endif

	endwhile

	return logcount;

endfunction


function Exp( x, y )
	return Pow( x, y );
endfunction


function Trunc(number, places)

	var base := Pow(10, places);

	number := number * base;
	number := CInt(number);
	number := CDbl(number);
	number := number / base;

	return number;

endfunction


function Fac(factorial);

	var x;
	while (factorial > 0)
		x := x * factorial;
		factorial := factorial - 1;
	endwhile
	return x;

endfunction


function YRoot(x, y)


	var i := 0.000;
	var n := 0.000;
	var root := 2.000;
	var a := CInt(Sqrt(x));

	if (a < 4)
		a := 4;
	endif

	var success := 0;


	while (success == 0)

		n := Pow(root, y);
		i := x - n;
		i := i / (a * root);
		i := Trunc(i, 3);
		root := root + i;
		if (i == 0)
			success := 1;
			if ( Pow(root, y) > x )
				root := root - 0.001;
			endif
			if ( Pow((root + 0.001), y) == x)
				root := root + 0.001;
			endif
		endif
	endwhile

	return root;

endfunction


function Mean(arrayx)

	var n, i:=0.000;

	foreach n in arrayx
		i := i + arrayx[n];
	endforeach

	n := CDbl(i / len(arrayx) );

	return n;

endfunction


function StdDev(arrayx)

	var m, n, i := 0.000;
	var arraymean := Mean(arrayx);
	var arraylen := len(arrayx);

	foreach m in arrayx
		n := arrayx[m] - arraymean;
		n := Pow(n, 2);
		i := i + n;
	endforeach

	i := i / (arraylen - 1);
	n := Sqrt(i);

	return n;

endfunction


function ZTest(arrayx, mu, sigma)

	var i, n;
	var z:= 0.000;
	var arraymean := Mean(arrayx);
	var arraylen := len(arrayx);

	i := arraymean - mu;
	n := sigma / Sqrt(arraylen);

	z := i/n;

	return z;

endfunction


function TTest(arrayx, mu)

	var i, n, s;
	var t:= 0.000;
	var arraymean := Mean(arrayx);
	var arraylen := len(arrayx);
	var s := StdDev(arrayx);

	i := arraymean - mu;
	n := s / Sqrt(arraylen);

	t := i/n;

	return t;

endfunction

function BitIn(arrayx)

	var i := 8;
	var n;
	var loop := 1;
	var bitvalue := 0;

	while (i >= 0)

		n := 8 - i;
		if (arrayx[i] == 1)
			bitvalue := bitvalue + Pow( 2, n );
		endif
		i := i - 1;

	endwhile

	return bitvalue;

endfunction


function BitOut(x)

	x := CInt(x);
	var loop := 1;
	var bitvalue := 0;
	var arrayx := {0,0,0,0,0,0,0,0};
	var n;

	while (loop <= 8)

		n := 8 - loop;
		bitvalue := Pow(2, n);
		if ( (x - bitvalue) >= 0)
			arrayx[loop] := 1;
			x := x - bitvalue;
		endif
		loop := loop + 1;

	endwhile

	return arrayx;

endfunction


function BitAnd(x, y)

	var arrayx := BitOut(x);
	var arrayy := BitOut(y);
	var z := 0;
	var loop := 1;
	var n;

	while (loop <= 8)

		n := loop - 1;
		if ( (arrayx[loop] == 1) && (arrayy[loop] == 1) )
			z := z + Pow(2, n);
		endif
		loop := loop + 1;

	endwhile

	return z;

endfunction


function BitOr(x, y)

	var arrayx := BitOut(x);
	var arrayy := BitOut(y);
	var z := 0;
	var loop := 1;
	var n;

	while (loop <= 8)

		n := loop - 1;
		if ( (arrayx[loop] == 1) || (arrayy[loop] == 1) )
			z := z + Pow(2, n);
		endif
		loop := loop + 1;

	endwhile

	return z;

endfunction


function BitXor(x, y)

	var arrayx := BitOut(x);
	var arrayy := BitOut(y);
	var z := 0;
	var loop := 1;
	var n;

	while (loop <= 8)

		n := loop - 1;
		if (arrayx[loop] - arrayy[loop] != 0)
			z := z + Pow(2, n);
		endif
		loop := loop + 1;

	endwhile

	return z;

endfunction


function BitEqv(x, y)

	var arrayx := BitOut(x);
	var arrayy := BitOut(y);
	var z := 0;
	var loop := 1;
	var n;

	while (loop <= 8)

		n := loop - 1;
		if (arrayx[loop] - arrayy[loop] == 0)
			z := z + Pow(2, n);
		endif
		loop := loop + 1;

	endwhile

	return z;

endfunction


function BitImp(x, y)

	var arrayx := BitOut(x);
	var arrayy := BitOut(y);
	var z := 0;
	var loop := 1;
	var n;

	while (loop <= 8)

		n := loop - 1;
		if (arrayx[loop] - arrayy[loop] < 1)
			z := z + Pow(2, n);
		endif
		loop := loop + 1;

	endwhile

	return z;

endfunction


function BitNot(x)

	var arrayx := BitOut(x);
	var z := 0;
	var loop := 1;
	var n;

	while (loop <= 8)

		n := loop - 1;
		if (arrayx[loop] == 0)
			arrayx[loop] := 1;
			z := z + Pow(2, n);
		else
			arrayx[loop] := 0;
		endif

		loop := loop + 1;

	endwhile

	return z;

endfunction


function FlagSet(flag, bit);

	bit := 9 - bit;

	var flagarray := BitOut(flag);
	flagarray[bit] := 1;

	bit := BitIn(flagarray);

	return bit;

endfunction


function FlagReset(flag, bit);

	var x := 1;
	var y := 0;
	var n := 0;

	bit := 9 - bit;

	var flagarray := BitOut(flag);
	flagarray[bit] := 0;

	while (x <= 8)

		if (flagarray[x] == 1)
			n := x - 1;
			y := y + Pow(2, n);
		endif

		x := x + 1;

	endwhile

	return y;

endfunction


function FlagCheck(flag, bit);

	var flagarray := BitOut(flag);
	bit := 9 - bit;

	if (flagarray[bit] == 1)
		return 1;
	else
		return 0;
	endif

endfunction