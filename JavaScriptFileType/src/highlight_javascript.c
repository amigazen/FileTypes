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

/* Highlight style definitions for JavaScript */
#define HL_PLAIN 0
#define HL_KEYWORD 1
#define HL_STRING 2
#define HL_COMMENT 3
#define HL_NUMBER 4
#define HL_OPERATOR 5
#define HL_FUNCTION 6
#define HL_CLASS 7
#define HL_REGEX 8
#define HL_BUILTIN 9
#define HL_TEMPLATE 10

static UWORD defaultStyleCodes[11] =
{
    (0<<8)|0, // plain
    (1<<8)|TBSTYLE_BOLD, // keyword (bold for emphasis)
    (2<<8)|TBSTYLE_SETCOLOR, // string (color only)
    (3<<8)|TBSTYLE_ITALIC, // comment (italic)
    (4<<8)|TBSTYLE_SETCOLOR, // number (color only)
    (5<<8)|TBSTYLE_SETCOLOR, // operator (color only)
    (6<<8)|TBSTYLE_SETCOLOR, // function (color only)
    (1<<8)|TBSTYLE_BOLD, // class (bold, reuse keyword color)
    (8<<8)|TBSTYLE_SETCOLOR, // regex (distinct color)
    (4<<8)|TBSTYLE_SETCOLOR, // builtin (reuse number color)
    (2<<8)|TBSTYLE_SETCOLOR // template literal (reuse string color)
};

static CONST_STRPTR stylename[12];

/* JavaScript keywords */
static CONST_STRPTR javascript_keywords[] = {
    "break", "case", "catch", "class", "const", "continue",
    "debugger", "default", "delete", "do", "else", "export",
    "extends", "finally", "for", "function", "if", "import",
    "in", "instanceof", "let", "new", "return", "super",
    "switch", "this", "throw", "try", "typeof", "var",
    "void", "while", "with", "yield", "async", "await",
    "enum", "implements", "interface", "package", "private",
    "protected", "public", "static", "true", "false", "null",
    "undefined", "NaN", "Infinity"
};

/* JavaScript built-in objects and functions */
static CONST_STRPTR javascript_builtins[] = {
    "Array", "Boolean", "Date", "Error", "Function", "Math",
    "Number", "Object", "RegExp", "String", "JSON", "Promise",
    "Map", "Set", "WeakMap", "WeakSet", "Symbol", "Proxy",
    "Reflect", "console", "document", "window", "navigator",
    "parseInt", "parseFloat", "isNaN", "isFinite", "encodeURI",
    "decodeURI", "encodeURIComponent", "decodeURIComponent",
    "eval", "setTimeout", "setInterval", "clearTimeout", "clearInterval",
    "alert", "confirm", "prompt", "localStorage", "sessionStorage",
    "XMLHttpRequest", "fetch", "Promise", "async", "await",
    NULL
};

/* Setup string highlighting */
void setupStrings(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    ULONG startstate, state1, state2;
    
    /* Single quoted strings - based on working Amiga E plugin */
    startstate = fType->createHighlightKeyword(hook, "\'", HL_STRING, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_STRING);
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 39, HL_STRING);
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, startstate, state2); /* Empty string */
    fType->linkHighlightState(hook, state1, state2);
    /* No explicit exit link - state machine handles it automatically */
    
    /* Double quoted strings - based on working Amiga E plugin */
    startstate = fType->createHighlightKeyword(hook, "\"", HL_STRING, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_STRING);
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 34, HL_STRING);
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, startstate, state2); /* Empty string */
    fType->linkHighlightState(hook, state1, state2);
    /* No explicit exit link - state machine handles it automatically */
    
    /* Template literals - based on working Amiga E plugin */
    startstate = fType->createHighlightKeyword(hook, "`", HL_TEMPLATE, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING|TEHL_STATE_SURVIVESNEWLINE, CHAR_ALWAYS, HL_TEMPLATE);
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 96, HL_TEMPLATE);
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, startstate, state2); /* Empty template */
    fType->linkHighlightState(hook, state1, state2);
    /* No explicit exit link - state machine handles it automatically */
}

/* Setup comment highlighting */
void setupComments(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    ULONG startstate, state1, state2, state2_star, state3_slash;
    
    /* Single line comments starting with // - simplified approach */
    startstate = fType->createHighlightKeyword(hook, "//", HL_COMMENT, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_COMMENT);
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 10, HL_COMMENT);
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, startstate, state2);
    fType->linkHighlightState(hook, state1, state2);
    
    /* Multi-line comments /* ... */ - based on working Amiga E plugin */
    startstate = fType->createHighlightKeyword(hook, "/*", HL_COMMENT, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SURVIVESNEWLINE|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_COMMENT);
    /* * character state - still applies formatting, can survive newlines */
    state2_star = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SURVIVESNEWLINE|TEHL_STATE_SELFREPEATING, '*', HL_COMMENT);
    /* / character state - closing delimiter, applies formatting */
    state3_slash = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '/', HL_COMMENT);
    fType->linkHighlightState(hook, startstate, state1); /* Link start to any character */
    fType->linkHighlightState(hook, startstate, state2_star); /* Link start directly to * */
    fType->linkHighlightState(hook, state1, state2_star); /* Link any character to * */
    fType->linkHighlightState(hook, state2_star, state3_slash); /* Link * to / */
    fType->linkHighlightState(hook, state2_star, state1); /* Link * back to any character */
    /* No explicit exit link - state machine handles it automatically */
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

/* Setup regex highlighting - simplified approach */
void setupRegex(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    /* Regex highlighting is complex due to division operator conflict */
    /* For now, we'll skip regex highlighting to avoid conflicts */
    /* This can be enhanced later with more sophisticated state machine */
}

/* Setup common JavaScript identifiers as keywords */
void setupCommonIdentifiers(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    /* Common JavaScript function names and class names that should be highlighted */
    static CONST_STRPTR common_functions[] = {
        "log", "error", "warn", "info", "debug", "trace",
        "getElementById", "getElementsByTagName", "getElementsByClassName",
        "querySelector", "querySelectorAll", "addEventListener",
        "removeEventListener", "createElement", "appendChild",
        "removeChild", "setAttribute", "getAttribute",
        "push", "pop", "shift", "unshift", "slice", "splice",
        "map", "filter", "reduce", "forEach", "find", "findIndex",
        "some", "every", "includes", "indexOf", "lastIndexOf",
        "sort", "reverse", "join", "split", "concat", "toString",
        "toLowerCase", "toUpperCase", "trim", "substring", "substr",
        "charAt", "charCodeAt", "replace", "match", "search",
        "test", "exec", "compile", "parse", "stringify",
        "getTime", "getDate", "getMonth", "getFullYear",
        "setTime", "setDate", "setMonth", "setFullYear",
        "getHours", "getMinutes", "getSeconds", "getMilliseconds",
        "setHours", "setMinutes", "setSeconds", "setMilliseconds",
        "toISOString", "toLocaleString", "toDateString", "toTimeString",
        NULL
    };
    
    int i;
    
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

	fType->typeName = "JavaScript";
	fType->autoFileTypes = "js,javascript";
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

	fType->name = "JavaScript";

	/* Initialize style names */
	stylename[0] = "Plain";
	stylename[1] = "Keyword";
	stylename[2] = "String";
	stylename[3] = "Comment";
	stylename[4] = "Number";
	stylename[5] = "Operator";
	stylename[6] = "Function";
	stylename[7] = "Class";
	stylename[8] = "Regex";
	stylename[9] = "Builtin";
	stylename[10] = "Template";
	stylename[11] = NULL;

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

	/* Set up JavaScript syntax highlighting - order matters for precedence */
	/* Set up keyword highlighting FIRST (before other patterns) */
	for (i = 0; i < sizeof(javascript_keywords)/sizeof(javascript_keywords[0]); i++) {
		fType->createHighlightKeyword(hook, (STRPTR)javascript_keywords[i], HL_KEYWORD, INITIALSTATE, TRUE);
	}
	
	/* Set up built-in function highlighting SECOND */
	for (i = 0; i < sizeof(javascript_builtins)/sizeof(javascript_builtins[0]); i++) {
		if (javascript_builtins[i] != NULL) {
			fType->createHighlightKeyword(hook, (STRPTR)javascript_builtins[i], HL_BUILTIN, INITIALSTATE, TRUE);
		}
	}
	
	/* Set up common function names as keywords THIRD */
	setupCommonIdentifiers(fType, hook, INITIALSTATE);
	
	/* Setup multiline patterns FOURTH (template literals) */
	setupStrings(fType, hook, INITIALSTATE);
	
	/* Setup comments FIFTH */
	setupComments(fType, hook, INITIALSTATE);
	
	/* Setup regex SIXTH */
	setupRegex(fType, hook, INITIALSTATE);
	
	/* Setup numbers SEVENTH */
	setupNumbers(fType, hook, INITIALSTATE);
	
	/* Setup operators LAST */
	setupOperators(fType, hook, INITIALSTATE);

	fType->linkHighlightState(hook, DEFAULTSTATE, INITIALSTATE);
}
