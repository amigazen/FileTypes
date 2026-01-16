MODULE EdgeCases;

(* Edge cases and special syntax *)

IMPORT Out;

CONST
    Zero = 0;
    One = 1;
    Empty = "";

TYPE
    EmptyRecord = RECORD
    END;
    
    Nested = RECORD
        inner: RECORD
            value: INTEGER;
        END;
    END;

VAR
    empty: EmptyRecord;
    nested: Nested;
    numbers: ARRAY 5 OF INTEGER;
    text: ARRAY 10 OF CHAR;

PROCEDURE EmptyProc;
BEGIN
    (* Empty procedure *)
END EmptyProc;

PROCEDURE NestedProc;
    PROCEDURE Inner;
    BEGIN
        Out.String("Inner");
        Out.Ln;
    END Inner;
BEGIN
    Inner;
END NestedProc;

PROCEDURE WithNested;
BEGIN
    WITH nested.inner DO
        value := 42;
    END;
END WithNested;

PROCEDURE LoopVariants;
VAR
    i: INTEGER;
BEGIN
    (* FOR loop with different step sizes *)
    FOR i := 0 TO 10 BY 1 DO
        numbers[i] := i;
    END;
    
    FOR i := 0 TO 20 BY 2 DO
        numbers[i DIV 2] := i;
    END;
    
    (* WHILE loop *)
    i := 0;
    WHILE i < 5 DO
        numbers[i] := i * i;
        INC(i);
    END;
    
    (* REPEAT loop *)
    i := 0;
    REPEAT
        numbers[i] := i MOD 3;
        INC(i);
    UNTIL i >= 5;
    
    (* LOOP with EXIT *)
    i := 0;
    LOOP
        IF i >= 5 THEN
            EXIT;
        END;
        numbers[i] := i;
        INC(i);
    END;
END LoopVariants;

PROCEDURE StringOps;
VAR
    i: INTEGER;
BEGIN
    (* String assignment *)
    text[0] := 'H';
    text[1] := 'e';
    text[2] := 'l';
    text[3] := 'l';
    text[4] := 'o';
    text[5] := 0X;
    
    (* String processing *)
    i := 0;
    WHILE (i < LEN(text)) & (text[i] # 0X) DO
        Out.Char(text[i]);
        INC(i);
    END;
    Out.Ln;
END StringOps;

PROCEDURE Comments;
BEGIN
    (* This is a comment *)
    (* This is also a comment
       that spans multiple lines *)
    (* Nested (* comments *) are allowed *)
END Comments;

BEGIN
    EmptyProc;
    NestedProc;
    WithNested;
    LoopVariants;
    StringOps;
    Comments;
END EdgeCases.
