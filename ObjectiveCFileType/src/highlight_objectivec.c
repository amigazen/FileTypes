#define __NOLIBBASE__
#include <proto/exec.h>
#include <proto/alib.h>
#include <proto/locale.h>

/* Force version 3 API before including filetypes.h */
#undef FILETYPEPLUGIN_VERSION
#define FILETYPEPLUGIN_VERSION 3

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

/* Highlight style definitions for Objective-C */
#define HL_PLAIN 0
#define HL_KEYWORD 1
#define HL_STRING 2
#define HL_COMMENT 3
#define HL_NUMBER 4
#define HL_OPERATOR 5
#define HL_FUNCTION 6
#define HL_CLASS 7
#define HL_PREPROCESSOR 8
#define HL_DIRECTIVE 9
#define HL_CONSTANT 10

static UWORD defaultStyleCodes[11] =
{
    (0<<8)|0, /* plain */
    (1<<8)|TBSTYLE_BOLD, /* keyword (bold for emphasis) */
    (2<<8)|TBSTYLE_SETCOLOR, /* string (color only) */
    (3<<8)|TBSTYLE_ITALIC, /* comment (italic) */
    (4<<8)|TBSTYLE_SETCOLOR, /* number (color only) */
    (5<<8)|TBSTYLE_SETCOLOR, /* operator (color only) */
    (6<<8)|TBSTYLE_SETCOLOR, /* function (color only) */
    (1<<8)|TBSTYLE_BOLD, /* class (bold, reuse keyword color) */
    (7<<8)|TBSTYLE_SETCOLOR, /* preprocessor (distinct color) */
    (8<<8)|TBSTYLE_SETCOLOR, /* directive (distinct color) */
    (4<<8)|TBSTYLE_SETCOLOR /* constant (reuse number color) */
};

static CONST_STRPTR stylename[12];

/* Objective-C keywords and directives */
static CONST_STRPTR objectivec_keywords[] = {
    "@interface", "@implementation", "@end", "@class", "@protocol",
    "@selector", "@synthesize", "@dynamic", "@property", "@autoreleasepool",
    "@try", "@catch", "@finally", "@throw", "@synchronized",
    "@encode", "@defs", "@compatibility_alias", "@private", "@protected",
    "@public", "@package", "@optional", "@required", "@import",
    "self", "super", "id", "nil", "YES", "NO", "BOOL", "SEL", "IMP",
    "Class", "Protocol"
};

/* C keywords */
static CONST_STRPTR c_keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default",
    "do", "double", "else", "enum", "extern", "float", "for", "goto",
    "if", "int", "long", "register", "return", "short", "signed",
    "sizeof", "static", "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while"
};

/* Objective-C built-in types and constants */
static CONST_STRPTR objectivec_types[] = {
    "NSObject", "NSString", "NSArray", "NSDictionary", "NSNumber",
    "NSData", "NSDate", "NSError", "NSException", "NSAutoreleasePool",
    "NSMutableArray", "NSMutableDictionary", "NSMutableString",
    "NSMutableData", "NSMutableSet", "NSSet", "NSOrderedSet",
    "NSMutableOrderedSet", "NSIndexSet", "NSMutableIndexSet",
    "NSRange", "NSPoint", "NSSize", "NSRect", "NSValue", "NSNull"
};

/* Setup Objective-C string highlighting (@"" strings) - based on working Amiga E plugin */
void setupObjectiveCStrings(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    ULONG startstate, state1, state2;
    
    /* Objective-C string literals @"..." */
    startstate = fType->createHighlightKeyword(hook, "@\"", HL_STRING, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_STRING);
    /* Closing quote - applies formatting like Amiga E plugin */
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 34, HL_STRING);
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, startstate, state2); /* Empty string */
    fType->linkHighlightState(hook, state1, state2); /* Content to closing quote */
    /* No explicit exit link - state machine handles it automatically */
}

/* Setup C string highlighting - based on working Amiga E plugin */
void setupCStrings(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    ULONG startstate, state1, state2;
    
    /* C-style double quoted strings */
    startstate = fType->createHighlightKeyword(hook, "\"", HL_STRING, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_STRING);
    /* Closing quote - applies formatting like Amiga E plugin */
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 34, HL_STRING);
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, startstate, state2); /* Empty string */
    fType->linkHighlightState(hook, state1, state2); /* Content to closing quote */
    /* No explicit exit link - state machine handles it automatically */
    
    /* C-style single quoted character literals */
    startstate = fType->createHighlightKeyword(hook, "\'", HL_STRING, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_STRING);
    /* Closing quote - applies formatting like Amiga E plugin */
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 39, HL_STRING);
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, startstate, state2); /* Empty character */
    fType->linkHighlightState(hook, state1, state2);
    /* No explicit exit link - state machine handles it automatically */
}

/* Setup comment highlighting */
void setupComments(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    ULONG startstate, state1, state2, state3;
    
    /* Single line comments starting with // */
    startstate = fType->createHighlightKeyword(hook, "//", HL_COMMENT, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_COMMENT);
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 10, HL_COMMENT); /* Newline */
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, startstate, state2); /* Empty comment */
    fType->linkHighlightState(hook, state1, state1); /* Self-loop for comment content */
    fType->linkHighlightState(hook, state1, state2); /* Comment to newline */
    fType->linkHighlightState(hook, state2, startState); /* Return to initial state after newline */
    
    /* Multiline comments start with slash-star and end with star-slash - based on working Amiga E plugin */
    startstate = fType->createHighlightKeyword(hook, "/*", HL_COMMENT, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SURVIVESNEWLINE|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_COMMENT);
    /* * character state - still applies formatting, can survive newlines */
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SURVIVESNEWLINE|TEHL_STATE_SELFREPEATING, '*', HL_COMMENT);
    /* / character state - closing delimiter, applies formatting */
    state3 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '/', HL_COMMENT);
    fType->linkHighlightState(hook, startstate, state1); /* Link start to any character */
    fType->linkHighlightState(hook, startstate, state2); /* Link start directly to * */
    fType->linkHighlightState(hook, state1, state2); /* Link any character to * */
    fType->linkHighlightState(hook, state2, state3); /* Link * to / */
    fType->linkHighlightState(hook, state2, state1); /* Link * back to any character */
    /* No explicit exit link - state machine handles it automatically */
}

/* Setup preprocessor highlighting */
void setupPreprocessor(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    ULONG startstate, state1, state2;
    
    /* Preprocessor directives starting with # */
    startstate = fType->createHighlightKeyword(hook, "#", HL_PREPROCESSOR, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_PREPROCESSOR);
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 10, HL_PREPROCESSOR); /* Newline */
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, startstate, state2); /* Empty directive */
    fType->linkHighlightState(hook, state1, state1); /* Self-loop for directive content */
    fType->linkHighlightState(hook, state1, state2); /* Directive to newline */
    fType->linkHighlightState(hook, state2, startState); /* Return to initial state after newline */
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

static void __ASM__ init(__REG__(a0, APTR userData))
{
	struct Hook *hook;
    struct Filetype *fType = (struct Filetype *)userData;
	struct HighlighterGUI *hlGUI;

    int i;

    ensureOpenLibs();

	fType->typeName = "ObjectiveC";
	fType->autoFileTypes = "objectivec";
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

	fType->name = "Objective-C";

	/* Initialize style names */
	stylename[0] = "Plain";
	stylename[1] = "Keyword";
	stylename[2] = "String";
	stylename[3] = "Comment";
	stylename[4] = "Number";
	stylename[5] = "Operator";
	stylename[6] = "Function";
	stylename[7] = "Class";
	stylename[8] = "Preprocessor";
	stylename[9] = "Directive";
	stylename[10] = "Constant";
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

	/* Set up Objective-C syntax highlighting - order matters for precedence */
	/* Set up preprocessor directives FIRST (before other patterns) */
	setupPreprocessor(fType, hook, INITIALSTATE);
	
	/* Set up keyword highlighting SECOND (before other patterns) */
	for (i = 0; i < sizeof(objectivec_keywords)/sizeof(objectivec_keywords[0]); i++) {
		fType->createHighlightKeyword(hook, (STRPTR)objectivec_keywords[i], HL_KEYWORD, INITIALSTATE, TRUE);
	}
	
	/* Set up C keywords THIRD */
	for (i = 0; i < sizeof(c_keywords)/sizeof(c_keywords[0]); i++) {
		fType->createHighlightKeyword(hook, (STRPTR)c_keywords[i], HL_KEYWORD, INITIALSTATE, TRUE);
	}
	
	/* Set up type highlighting FOURTH */
	for (i = 0; i < sizeof(objectivec_types)/sizeof(objectivec_types[0]); i++) {
		fType->createHighlightKeyword(hook, (STRPTR)objectivec_types[i], HL_CLASS, INITIALSTATE, TRUE);
	}
	
	/* Setup multiline patterns FIFTH (comments) */
	setupComments(fType, hook, INITIALSTATE);
	
	/* Setup strings SIXTH */
	setupObjectiveCStrings(fType, hook, INITIALSTATE);
	setupCStrings(fType, hook, INITIALSTATE);
	
	/* Setup numbers SEVENTH */
	setupNumbers(fType, hook, INITIALSTATE);
	
	/* Setup operators LAST */
	setupOperators(fType, hook, INITIALSTATE);

	fType->linkHighlightState(hook, DEFAULTSTATE, INITIALSTATE);
}
