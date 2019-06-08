: concatinate ( adr1 adr2 -- adr3 )
    over count
    over count
    + 1 + ( total length )
    heap-alloc
    dup >r ( saving adr3 )
    dup rot 
    dup count >r ( saving first string length)
    dup >r ( saving adr1 ) 
    string-copy 
    r> heap-free ( free first string )
    r> + swap 
    dup >r ( saving adr2 )
    string-copy 
    r> heap-free ( free second string )
    r>
;
