( 0 variant )
: kollaz ( number -- )
    dup 1 <= if
	.
    else
	repeat
	    dup 2 % not if
		2 /
		dup . ."  "
    	    else
		3 * 1 +
		dup . ."  "
	    then
		dup 1 =
	until
	drop
    then
;
