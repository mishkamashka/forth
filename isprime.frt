: save ( value -- address )
    1
    allot
    dup
    -rot
    !
;

: is_prime ( value -- result 1-prime, 0-not, -1-neither prime nor not )
    dup 2 < if
	drop drop
	-1 save
	exit
    then
    dup 2 = if
	drop drop
	1 save
	exit
    then
    dup 2 % not if
    	drop drop
	0 save
	exit
    then
    dup 2 / 1 +
    2 do
	r@ % 0 = if
	    drop 0
	    r> drop
	    r@ >r
	else dup then
    loop
    0 = if 0 save else drop 1 save then
;
    
