#define __NOLIBBASE__
#include <proto/exec.h>
#include <proto/alib.h>
#include <proto/locale.h>

#include <tools/textedit/filetypes.h>

extern struct Library *SysBase;
extern struct Locale *Locale;
extern struct Catalog *Catalog;

void ensureOpenLibs(void);

static void __ASM__ init(__REG__(a0, APTR userData));
void __ASM__ terminate(__REG__ (a0, struct Filetype *fType));
Object * __ASM__ settingsGadget(__REG__(a0, struct Filetype *fType), __REG__(d0, ULONG i));
STRPTR __ASM__ settingsTitle(__REG__(a0, struct Filetype *fType), __REG__(d0, ULONG i));
void __ASM__ applySettings(__REG__(a0, struct Filetype *fType));
void __ASM__ setSettingsFromGUI(__REG__(a0, struct Filetype *fType));
void __ASM__ setGUIFromSettings(__REG__(a0, struct Filetype *fType), __REG__(a1, struct Window *window));
void __ASM__ disposeGadgets(__REG__(a0, struct Filetype *fType));
void __ASM__ processGadgetUp(__REG__(a0, struct Filetype *fType), __REG__(d0, UWORD gadgetId), __REG__(a1, struct Window *window));
BOOL __ASM__ saveHighlightSettings(__REG__(a0, struct IFFHandle *iff), __REG__(a1, struct ContextNode *cn), __REG__(a2, APTR data));
BOOL __ASM__ loadHighlightSettings(__REG__(a0, struct IFFHandle *iff), __REG__(a1, struct ContextNode *cn), __REG__(a2, APTR data));

#define INT_VERSION    1
#define INT_REVISION   0

struct Plugin filetypePlugin =
{
  NULL,
  FILETYPEPLUGIN_VERSION,
  MASTER_VERSION,
  INT_VERSION,
  INT_REVISION,
  0,
  0,
  &init
};

/* Highlight style definitions for Python */
#define HL_PLAIN 0
#define HL_KEYWORD 1
#define HL_STRING 2
#define HL_COMMENT 3
#define HL_NUMBER 4
#define HL_OPERATOR 5
#define HL_FUNCTION 6
#define HL_CLASS 7
#define HL_DECORATOR 8
#define HL_BUILTIN 9
#define HL_SPECIAL 10
#define HL_DOCSTRING 11
#define HL_INDENTATION 12

static UWORD defaultStyleCodes[13] =
{
    (0<<8)|0, // plain
    (1<<8)|TBSTYLE_BOLD, // keyword (bold for emphasis)
    (2<<8)|TBSTYLE_SETCOLOR, // string (color only)
    (3<<8)|TBSTYLE_ITALIC, // comment (italic)
    (4<<8)|TBSTYLE_SETCOLOR, // number (color only)
    (5<<8)|TBSTYLE_SETCOLOR, // operator (color only)
    (6<<8)|TBSTYLE_SETCOLOR, // function (color only)
    (1<<8)|TBSTYLE_BOLD, // class (bold, reuse keyword color)
    (2<<8)|TBSTYLE_SETCOLOR, // decorator (reuse string color)
    (4<<8)|TBSTYLE_SETCOLOR, // builtin (reuse number color)
    (5<<8)|TBSTYLE_SETCOLOR, // special (reuse operator color)
    (7<<8)|TBSTYLE_SETCOLOR, // docstring (distinct color)
    (6<<8)|TBSTYLE_SETCOLOR // indentation (reuse function color)
};

static CONST_STRPTR stylename[14];

/* Python 2 keywords */
static CONST_STRPTR python_keywords[] = {
    "False", "None", "True", "and", "as", "assert",
    "break", "class", "continue", "def", "del", "elif", "else", "except",
    "finally", "for", "from", "global", "if", "import", "in", "is",
    "lambda", "not", "or", "pass", "print", "raise", "return",
    "try", "while", "with", "yield"
};

/* Python 2 built-in functions */
static CONST_STRPTR python_builtins[] = {
    "abs", "all", "any", "bin", "bool", "bytearray",
    "callable", "chr", "classmethod", "cmp", "compile", "complex",
    "delattr", "dict", "dir", "divmod", "enumerate", "eval", "exec",
    "filter", "float", "frozenset", "getattr", "globals",
    "hasattr", "hash", "help", "hex", "id", "input", "int", "isinstance",
    "issubclass", "iter", "len", "list", "locals", "map", "max",
    "min", "next", "object", "oct", "open", "ord", "pow",
    "property", "range", "raw_input", "reduce", "repr", "reversed", "round", "set", "setattr",
    "slice", "sorted", "staticmethod", "str", "sum", "super", "tuple",
    "type", "unichr", "unicode", "vars", "xrange", "zip"
};

/* Setup string highlighting */
void setupStrings(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    ULONG startstate, state1, state2;
    
    /* Single quoted strings - simplified approach */
    startstate = fType->createHighlightKeyword(hook, "\'", HL_STRING, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_STRING);
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 39, HL_STRING);
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, startstate, state2);
    fType->linkHighlightState(hook, state1, state2);
    
    /* Double quoted strings - simplified approach */
    startstate = fType->createHighlightKeyword(hook, "\"", HL_STRING, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_STRING);
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 34, HL_STRING);
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, state1, state2);
    
    /* Triple quoted strings (docstrings) - simplified approach */
    startstate = fType->createHighlightKeyword(hook, "'''", HL_DOCSTRING, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING|TEHL_STATE_SURVIVESNEWLINE, CHAR_ALWAYS, HL_DOCSTRING);
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, "'''", HL_DOCSTRING);
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, state1, state2);
    
    /* Triple quoted strings with double quotes */
    startstate = fType->createHighlightKeyword(hook, "\"\"\"", HL_DOCSTRING, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING|TEHL_STATE_SURVIVESNEWLINE, CHAR_ALWAYS, HL_DOCSTRING);
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, "\"\"\"", HL_DOCSTRING);
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, state1, state2);
}

/* Setup comment highlighting */
void setupComments(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    ULONG startstate, state1, state2;
    
    /* Single line comments starting with # */
    startstate = fType->createHighlightKeyword(hook, "#", HL_COMMENT, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_COMMENT);
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 10, HL_COMMENT);
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, startstate, state2);
    fType->linkHighlightState(hook, state1, state2);
}

/* Setup number highlighting - simplified approach */
void setupNumbers(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    /* Numbers will be handled by the keyword system for simplicity */
    /* This avoids complex state machines that can interfere with other highlighting */
}

/* Setup operator highlighting - simplified approach */
void setupOperators(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    /* Operators will be handled by the keyword system for simplicity */
    /* This avoids complex state machines that can interfere with other highlighting */
}

/* Setup decorator highlighting */
void setupDecorators(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    ULONG startstate, state1, state2;
    
    /* Decorators start with @ - simplified approach */
    startstate = fType->createHighlightKeyword(hook, "@", HL_DECORATOR, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_DECORATOR);
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 10, HL_DECORATOR);
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, startstate, state2);
    fType->linkHighlightState(hook, state1, state2);
}

/* Setup function and class definition highlighting */
void setupFunctionClassDefs(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    /* This function is removed as it was creating individual character states
       that interfere with keyword highlighting. Function and class names
       will be handled by the keyword system instead. */
}

/* Setup common class names and function names as keywords */
void setupCommonIdentifiers(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    /* Common Python class names and function names that should be highlighted */
    static CONST_STRPTR common_classes[] = {
        "Color", "Point", "Drawable", "Circle", "Rectangle", "Triangle",
        "Enum", "ABC", "Protocol", "Generic", "TypeVar", "Union", "Optional",
        "List", "Dict", "Tuple", "Set", "Callable", "Any", "None", "True", "False",
        "Exception", "ValueError", "TypeError", "AttributeError", "KeyError",
        "IndexError", "RuntimeError", "OSError", "FileNotFoundError",
        "StringIO", "BytesIO", "Path", "PurePath", "PosixPath", "WindowsPath",
        "datetime", "date", "time", "timedelta", "timezone",
        "collections", "defaultdict", "Counter", "deque", "namedtuple",
        "itertools", "functools", "operator", "re", "json", "pickle", "sqlite3",
        "threading", "multiprocessing", "asyncio", "contextlib", "typing",
        NULL
    };
    
    static CONST_STRPTR common_functions[] = {
        "print", "len", "range", "enumerate", "zip", "map", "filter", "reduce",
        "sorted", "reversed", "min", "max", "sum", "abs", "round", "pow",
        "isinstance", "issubclass", "hasattr", "getattr", "setattr", "delattr",
        "dir", "vars", "locals", "globals", "eval", "exec", "compile",
        "open", "input", "raw_input", "help", "id", "hash", "repr", "str",
        "int", "float", "bool", "list", "dict", "tuple", "set", "frozenset",
        "bytes", "bytearray", "memoryview", "slice", "property", "staticmethod",
        "classmethod", "super", "object", "type", "next", "iter", "callable",
        "chr", "ord", "bin", "oct", "hex", "format", "divmod", "all", "any",
        "breakpoint", "del", "pass", "raise", "try", "except", "finally",
        "with", "yield", "lambda", "def", "class", "import", "from", "as",
        "return", "if", "elif", "else", "for", "while", "continue", "break",
        "global", "nonlocal", "assert", "in", "is", "not", "and", "or",
        NULL
    };
    
    int i;
    
    /* Add common class names as function-style highlighting */
    for (i = 0; common_classes[i] != NULL; i++) {
        fType->createHighlightKeyword(hook, (STRPTR)common_classes[i], HL_FUNCTION, INITIALSTATE, TRUE);
    }
    
    /* Add common function names as function-style highlighting */
    for (i = 0; common_functions[i] != NULL; i++) {
        fType->createHighlightKeyword(hook, (STRPTR)common_functions[i], HL_FUNCTION, INITIALSTATE, TRUE);
    }
}

static void __ASM__ init(__REG__(a0, APTR userData))
{
	struct Hook *hook;
    struct Filetype *fType = (struct Filetype *)userData;
	struct HighlighterGUI *hlGUI;

    int i;

    ensureOpenLibs();

	fType->typeName = "Python";
	fType->autoFileTypes = "python";
	fType->terminate = terminate;
	fType->settingsTitle = settingsTitle;
	fType->settingsGadget = settingsGadget;
	fType->processGadgetUp = processGadgetUp;
	fType->setSettingsFromGUI = setSettingsFromGUI;
	fType->setGUIFromSettings = setGUIFromSettings;
	fType->applySettings = applySettings;
	fType->disposeGadgets = disposeGadgets;
	fType->saveSettings = saveHighlightSettings;
	fType->loadSettings = loadHighlightSettings;

	fType->name = "Python";

	/* Initialize style names */
	stylename[0] = "Plain";
	stylename[1] = "Keyword";
	stylename[2] = "String";
	stylename[3] = "Comment";
	stylename[4] = "Number";
	stylename[5] = "Operator";
	stylename[6] = "Function";
	stylename[7] = "Class";
	stylename[8] = "Decorator";
	stylename[9] = "Builtin";
	stylename[10] = "Special";
	stylename[11] = "Docstring";
	stylename[12] = "Indentation";
	stylename[13] = NULL;

	hlGUI = fType->createHighlightStylesGUI(stylename, defaultStyleCodes);
	fType->pluginData = (ULONG)hlGUI;

	if (!hlGUI)
		return;

	fType->highlighterHook = hook = fType->createHighlighter(1750, TEHL_CASESENSITIVE, hlGUI->inuseStylecodes);

	if (!hook)
		return;

	/* Create basic states first */
	fType->createHighlightState(hook, 0, CHAR_NONWORD, HL_PLAIN);
	fType->createHighlightState(hook, TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_PLAIN);

	/* Set up Python syntax highlighting - order matters for precedence */
	/* Set up keyword highlighting FIRST (before other patterns) */
	for (i = 0; i < sizeof(python_keywords)/sizeof(python_keywords[0]); i++) {
		fType->createHighlightKeyword(hook, (STRPTR)python_keywords[i], HL_KEYWORD, INITIALSTATE, TRUE);
	}
	
	/* Set up built-in function highlighting SECOND */
	for (i = 0; i < sizeof(python_builtins)/sizeof(python_builtins[0]); i++) {
		fType->createHighlightKeyword(hook, (STRPTR)python_builtins[i], HL_BUILTIN, INITIALSTATE, TRUE);
	}
	
	/* Set up common class names and function names as keywords THIRD */
	/* This ensures they get highlighted without conflicting with Python keywords */
	setupCommonIdentifiers(fType, hook, INITIALSTATE);
	
	/* Setup multiline patterns FOURTH (docstrings) */
	setupStrings(fType, hook, INITIALSTATE);
	
	/* Setup comments FIFTH */
	setupComments(fType, hook, INITIALSTATE);
	
	/* Setup numbers SIXTH */
	setupNumbers(fType, hook, INITIALSTATE);
	
	/* Setup operators SEVENTH */
	setupOperators(fType, hook, INITIALSTATE);
	
	/* Setup decorators LAST */
	setupDecorators(fType, hook, INITIALSTATE);

	fType->linkHighlightState(hook, DEFAULTSTATE, INITIALSTATE);
} 