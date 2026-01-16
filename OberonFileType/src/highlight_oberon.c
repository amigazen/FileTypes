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

/* Highlight style definitions for Oberon */
#define HL_PLAIN 0
#define HL_KEYWORD 1
#define HL_STRING 2
#define HL_COMMENT 3
#define HL_NUMBER 4
#define HL_OPERATOR 5
#define HL_PROCEDURE 6
#define HL_TYPE 7
#define HL_CONSTANT 8

static UWORD defaultStyleCodes[9] =
{
    (0<<8)|0, /* plain */
    (1<<8)|TBSTYLE_BOLD, /* keyword (bold for emphasis) */
    (2<<8)|TBSTYLE_SETCOLOR, /* string (color only) */
    (3<<8)|TBSTYLE_ITALIC, /* comment (italic) */
    (4<<8)|TBSTYLE_SETCOLOR, /* number (color only) */
    (5<<8)|TBSTYLE_SETCOLOR, /* operator (color only) */
    (6<<8)|TBSTYLE_SETCOLOR, /* procedure (color only) */
    (1<<8)|TBSTYLE_BOLD, /* type (bold, reuse keyword color) */
    (4<<8)|TBSTYLE_SETCOLOR /* constant (reuse number color) */
};

static CONST_STRPTR stylename[10];

/* Oberon keywords */
static CONST_STRPTR oberon_keywords[] = {
    "ARRAY", "BEGIN", "BY", "CASE", "CONST", "DIV", "DO", "ELSE", "ELSIF",
    "END", "EXIT", "FOR", "IF", "IMPORT", "IN", "IS", "LOOP", "MOD",
    "MODULE", "OF", "OR", "PROCEDURE", "RECORD", "REPEAT", "RETURN",
    "THEN", "TO", "TYPE", "UNTIL", "VAR", "WHILE", "WITH"
};

/* Oberon built-in types */
static CONST_STRPTR oberon_types[] = {
    "BOOLEAN", "CHAR", "INTEGER", "LONGINT", "REAL", "LONGREAL",
    "SET", "POINTER", "NIL", "TRUE", "FALSE"
};

/* Oberon built-in procedures */
static CONST_STRPTR oberon_procedures[] = {
    "ABS", "ASH", "ASSERT", "CHR", "COPY", "DEC", "ENTIER", "EXCL",
    "HALT", "INC", "INCL", "LEN", "LONG", "MAX", "MIN", "NEW", "ODD",
    "ORD", "SHORT", "SIZE", "SYSTEM"
};

/* Setup string highlighting - based on working Amiga E plugin */
void setupStrings(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    ULONG startstate, state1, state2;
    
    /* Double quoted strings */
    startstate = fType->createHighlightKeyword(hook, "\"", HL_STRING, INITIALSTATE, FALSE);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_STRING);
    /* Closing quote - applies formatting like Amiga E plugin */
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 34, HL_STRING);
    fType->linkHighlightState(hook, startstate, state1);
    fType->linkHighlightState(hook, startstate, state2); /* Empty string */
    fType->linkHighlightState(hook, state1, state2); /* Content to closing quote */
    /* No explicit exit link - state machine handles it automatically */
}

/* Setup comment highlighting - based on working Amiga E plugin */
void setupComments(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    ULONG startstate, state1, state2, state3;
    
    /* Multiline comments (* ... *) - following Amiga E pattern */
    startstate = fType->createHighlightKeyword(hook, "(*", HL_COMMENT, INITIALSTATE, FALSE);
    /* Comment content - any character with formatting */
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SURVIVESNEWLINE|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_COMMENT);
    /* * character state - still applies formatting, can survive newlines */
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SURVIVESNEWLINE|TEHL_STATE_SELFREPEATING, '*', HL_COMMENT);
    /* ) character state - closing delimiter, applies formatting */
    state3 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ')', HL_COMMENT);
    /* Link start to any character */
    fType->linkHighlightState(hook, startstate, state1);
    /* Link start directly to * (for empty comments or comments starting with *) */
    fType->linkHighlightState(hook, startstate, state2);
    /* Link any character to * (when we see * in comment) */
    fType->linkHighlightState(hook, state1, state2);
    /* Link * to ) (closing delimiter) */
    fType->linkHighlightState(hook, state2, state3);
    /* Link * back to any character (if * is not followed by )) */
    fType->linkHighlightState(hook, state2, state1);
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

	fType->typeName = "Oberon";
	fType->autoFileTypes = "oberon";
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

	fType->name = "Oberon";

	/* Initialize style names */
	stylename[0] = "Plain";
	stylename[1] = "Keyword";
	stylename[2] = "String";
	stylename[3] = "Comment";
	stylename[4] = "Number";
	stylename[5] = "Operator";
	stylename[6] = "Procedure";
	stylename[7] = "Type";
	stylename[8] = "Constant";
	stylename[9] = NULL;

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

	/* Set up Oberon syntax highlighting - order matters for precedence */
	/* Set up keyword highlighting FIRST (before other patterns) */
	for (i = 0; i < sizeof(oberon_keywords)/sizeof(oberon_keywords[0]); i++) {
		fType->createHighlightKeyword(hook, (STRPTR)oberon_keywords[i], HL_KEYWORD, INITIALSTATE, TRUE);
	}
	
	/* Set up type highlighting SECOND */
	for (i = 0; i < sizeof(oberon_types)/sizeof(oberon_types[0]); i++) {
		fType->createHighlightKeyword(hook, (STRPTR)oberon_types[i], HL_TYPE, INITIALSTATE, TRUE);
	}
	
	/* Set up built-in procedure highlighting THIRD */
	for (i = 0; i < sizeof(oberon_procedures)/sizeof(oberon_procedures[0]); i++) {
		fType->createHighlightKeyword(hook, (STRPTR)oberon_procedures[i], HL_PROCEDURE, INITIALSTATE, TRUE);
	}
	
	/* Setup multiline patterns FOURTH (comments) */
	setupComments(fType, hook, INITIALSTATE);
	
	/* Setup strings FIFTH */
	setupStrings(fType, hook, INITIALSTATE);
	
	/* Setup numbers SIXTH */
	setupNumbers(fType, hook, INITIALSTATE);
	
	/* Setup operators LAST */
	setupOperators(fType, hook, INITIALSTATE);

	fType->linkHighlightState(hook, DEFAULTSTATE, INITIALSTATE);
}
