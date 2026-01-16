MODULE BasicSyntax;

(* Basic Oberon syntax examples *)

IMPORT Out;

CONST
    MaxSize = 100;
    Pi = 3.14159;

TYPE
    Point = RECORD
        x, y: INTEGER;
    END;

VAR
    counter: INTEGER;
    value: REAL;
    flag: BOOLEAN;
    p: Point;

PROCEDURE Add(a, b: INTEGER): INTEGER;
BEGIN
    RETURN a + b;
END Add;

PROCEDURE ProcessData(VAR data: ARRAY OF INTEGER);
VAR
    i: INTEGER;
BEGIN
    FOR i := 0 TO LEN(data) - 1 DO
        data[i] := data[i] * 2;
    END;
END ProcessData;

BEGIN
    counter := 0;
    value := 1.5;
    flag := TRUE;
    
    IF counter < MaxSize THEN
        counter := Add(counter, 1);
    ELSIF counter = MaxSize THEN
        counter := 0;
    ELSE
        (* Do nothing *)
    END;
    
    WHILE counter < 10 DO
        INC(counter);
    END;
    
    REPEAT
        DEC(counter);
    UNTIL counter <= 0;
    
    FOR counter := 1 TO 10 BY 2 DO
        Out.Int(counter, 0);
        Out.Ln;
    END;
    
    CASE counter OF
        0: value := 0.0;
        1: value := 1.0;
    ELSE
        value := 2.0;
    END;
    
    WITH p DO
        x := 10;
        y := 20;
    END;
END BasicSyntax.
