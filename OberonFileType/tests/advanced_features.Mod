MODULE AdvancedFeatures;

(* Advanced Oberon language features *)

IMPORT Out, SYSTEM;

TYPE
    Node = POINTER TO NodeDesc;
    NodeDesc = RECORD
        value: INTEGER;
        next: Node;
    END;
    
    Matrix = ARRAY 10, 10 OF REAL;
    
    Color = (Red, Green, Blue);
    
    String = ARRAY 32 OF CHAR;

VAR
    list: Node;
    matrix: Matrix;
    color: Color;
    name: String;

PROCEDURE NewNode(value: INTEGER): Node;
VAR
    n: Node;
BEGIN
    NEW(n);
    n.value := value;
    n.next := NIL;
    RETURN n;
END NewNode;

PROCEDURE Insert(VAR list: Node; value: INTEGER);
VAR
    n: Node;
BEGIN
    n := NewNode(value);
    n.next := list;
    list := n;
END Insert;

PROCEDURE Search(list: Node; value: INTEGER): BOOLEAN;
BEGIN
    WHILE list # NIL DO
        IF list.value = value THEN
            RETURN TRUE;
        END;
        list := list.next;
    END;
    RETURN FALSE;
END Search;

PROCEDURE ProcessMatrix(VAR m: Matrix);
VAR
    i, j: INTEGER;
BEGIN
    FOR i := 0 TO 9 DO
        FOR j := 0 TO 9 DO
            m[i, j] := i * j;
        END;
    END;
END ProcessMatrix;

PROCEDURE CopyString(VAR dest: ARRAY OF CHAR; src: ARRAY OF CHAR);
VAR
    i: INTEGER;
BEGIN
    i := 0;
    WHILE (i < LEN(dest)) & (i < LEN(src)) & (src[i] # 0X) DO
        dest[i] := src[i];
        INC(i);
    END;
    IF i < LEN(dest) THEN
        dest[i] := 0X;
    END;
END CopyString;

BEGIN
    list := NIL;
    Insert(list, 10);
    Insert(list, 20);
    Insert(list, 30);
    
    IF Search(list, 20) THEN
        Out.String("Found 20");
        Out.Ln;
    END;
    
    ProcessMatrix(matrix);
    
    color := Red;
    CASE color OF
        Red: Out.String("Red");
        Green: Out.String("Green");
        Blue: Out.String("Blue");
    END;
    Out.Ln;
    
    CopyString(name, "Oberon");
    Out.String(name);
    Out.Ln;
END AdvancedFeatures.
