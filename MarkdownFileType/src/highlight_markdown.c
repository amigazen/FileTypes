#define __NOLIBBASE__
#include <proto/exec.h>
#include <proto/alib.h>
#include <proto/locale.h>

#include <tools/textedit/filetypes.h>

#include <stdlib.h>

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

/* Highlight style definitions */
#define HL_PLAIN 0
#define HL_HEADING 1
#define HL_CODE 2
#define HL_BOLD 3
#define HL_ITALIC 4
#define HL_LINK 5
#define HL_LIST 6
#define HL_BLOCKQUOTE 7
#define HL_CODEBLOCK 8
#define HL_STRIKETHROUGH 9
#define HL_HTML 10
#define HL_IMAGE 11
#define HL_REFERENCE 12
#define HL_AUTOLINK 13
#define HL_BOLDUNDERLINE 14

static UWORD defaultStyleCodes[15] =
{
    (0<<8)|0, // plain
    (1<<8)|TBSTYLE_BOLD, // heading (bold for emphasis)
    (2<<8)|TBSTYLE_SETCOLOR, // code (color only)
    (3<<8)|TBSTYLE_BOLD, // bold text
    (4<<8)|TBSTYLE_ITALIC, // italic text 
    (5<<8)|TBSTYLE_SETCOLOR, // link (color only)
    (6<<8)|TBSTYLE_SETCOLOR, // list (color only)
    (7<<8)|TBSTYLE_SETCOLOR, // blockquote (color only)
    (8<<8)|TBSTYLE_SETCOLOR, // codeblock (color only)
    (9<<8)|TBSTYLE_ITALIC|TBSTYLE_UNDERLINE|TBSTYLE_BOLD, // strikethrough (italic + underline + bold)
    (10<<8)|TBSTYLE_SETCOLOR, // html (color only)
    (11<<8)|TBSTYLE_SETCOLOR, // image (color only)
    (12<<8)|TBSTYLE_SETCOLOR, // reference link (color only)
    (13<<8)|TBSTYLE_SETCOLOR, // auto-link (color only)
    (14<<8)|TBSTYLE_BOLD|TBSTYLE_UNDERLINE // bold+underline for *** and ___ patterns
};

static CONST_STRPTR stylename[15];

/* Setup inline code highlighting (backticks) */
ULONG setupInlineCode(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
	ULONG codestartstate;
	ULONG codewildstate;
	ULONG codeendstate;

	/* Start with backtick (no formatting) */
	codestartstate = fType->createHighlightState(hook, 0, '`', 0);
	fType->linkHighlightState(hook, startState, codestartstate);

	/* Content - use CHAR_NONNEWLINE like working patterns */
	codewildstate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_CODE);
	fType->linkHighlightState(hook, codestartstate, codewildstate);
	
	/* End with backtick (no formatting - delimiters should not be styled) */
	codeendstate = fType->createHighlightState(hook, 0, '`', 0);
	fType->linkHighlightState(hook, codewildstate, codeendstate);
	fType->linkHighlightState(hook, codeendstate, startState);

	return codeendstate;
}

/* Setup bold, italic, and bold+underline highlighting **bold**, *italic*, and ***bold+underline*** */
void setupBoldItalic(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
	ULONG asteriskstart, asterisksecond, asteriskthird;
	ULONG boldunderlinecontent, boldunderlineend1, boldunderlineend2, boldunderlineend3;
	ULONG boldcontent, boldend1, boldend2;
	ULONG italiccontent, italicend;

	/* Context-aware asterisk emphasis - look ahead to determine pattern */
	/* First asterisk - start of any asterisk pattern */
	asteriskstart = fType->createHighlightState(hook, 0, '*', 0);
	fType->linkHighlightState(hook, startState, asteriskstart);
	
	/* Second asterisk - could be ** or *** */
	asterisksecond = fType->createHighlightState(hook, 0, '*', 0);
	fType->linkHighlightState(hook, asteriskstart, asterisksecond);
	
	/* Third asterisk - must be *** */
	asteriskthird = fType->createHighlightState(hook, 0, '*', 0);
	fType->linkHighlightState(hook, asterisksecond, asteriskthird);
	
	/* Content for *** (bold+underline) */
	boldunderlinecontent = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_BOLDUNDERLINE);
	fType->linkHighlightState(hook, asteriskthird, boldunderlinecontent);
	
	/* End states for *** (no formatting - delimiters should not be styled) */
	boldunderlineend1 = fType->createHighlightState(hook, 0, '*', 0);
	fType->linkHighlightState(hook, boldunderlinecontent, boldunderlineend1);
	
	boldunderlineend2 = fType->createHighlightState(hook, 0, '*', 0);
	fType->linkHighlightState(hook, boldunderlineend1, boldunderlineend2);
	
	boldunderlineend3 = fType->createHighlightState(hook, 0, '*', 0);
	fType->linkHighlightState(hook, boldunderlineend2, boldunderlineend3);
	fType->linkHighlightState(hook, boldunderlineend3, startState);
	
	/* Content for ** (bold) - from second asterisk */
	boldcontent = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_BOLD);
	fType->linkHighlightState(hook, asterisksecond, boldcontent);
	
	/* End states for ** (no formatting - delimiters should not be styled) */
	boldend1 = fType->createHighlightState(hook, 0, '*', 0);
	fType->linkHighlightState(hook, boldcontent, boldend1);
	
	boldend2 = fType->createHighlightState(hook, 0, '*', 0);
	fType->linkHighlightState(hook, boldend1, boldend2);
	fType->linkHighlightState(hook, boldend2, startState);
	
	/* Content for * (italic) - from first asterisk */
	italiccontent = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_ITALIC);
	fType->linkHighlightState(hook, asteriskstart, italiccontent);
	
	/* End state for * (no formatting - delimiters should not be styled) */
	italicend = fType->createHighlightState(hook, 0, '*', 0);
	fType->linkHighlightState(hook, italiccontent, italicend);
	fType->linkHighlightState(hook, italicend, startState);
}

/* Setup underscore emphasis (italic and bold) */
void setupUnderscoreEmphasis(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
	ULONG underscorestart, underscoresecond, underscorethird;
	ULONG underscoreboldunderlinecontent, underscoreboldunderlineend1, underscoreboldunderlineend2, underscoreboldunderlineend3;
	ULONG underscoreboldcontent, underscoreboldend1, underscoreboldend2;
	ULONG underscoreitaliccontent, underscoreitalicend;

	/* Context-aware underscore emphasis - look ahead to determine pattern */
	/* First underscore - start of any underscore pattern */
	underscorestart = fType->createHighlightState(hook, 0, '_', 0);
	fType->linkHighlightState(hook, startState, underscorestart);
	
	/* Second underscore - could be __ or ___ */
	underscoresecond = fType->createHighlightState(hook, 0, '_', 0);
	fType->linkHighlightState(hook, underscorestart, underscoresecond);
	
	/* Third underscore - must be ___ */
	underscorethird = fType->createHighlightState(hook, 0, '_', 0);
	fType->linkHighlightState(hook, underscoresecond, underscorethird);
	
	/* Content for ___ (bold+underline) */
	underscoreboldunderlinecontent = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_BOLDUNDERLINE);
	fType->linkHighlightState(hook, underscorethird, underscoreboldunderlinecontent);
	
	/* End states for ___ (no formatting - delimiters should not be styled) */
	underscoreboldunderlineend1 = fType->createHighlightState(hook, 0, '_', 0);
	fType->linkHighlightState(hook, underscoreboldunderlinecontent, underscoreboldunderlineend1);
	
	underscoreboldunderlineend2 = fType->createHighlightState(hook, 0, '_', 0);
	fType->linkHighlightState(hook, underscoreboldunderlineend1, underscoreboldunderlineend2);
	
	underscoreboldunderlineend3 = fType->createHighlightState(hook, 0, '_', 0);
	fType->linkHighlightState(hook, underscoreboldunderlineend2, underscoreboldunderlineend3);
	fType->linkHighlightState(hook, underscoreboldunderlineend3, startState);
	
	/* Content for __ (bold) - from second underscore */
	underscoreboldcontent = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_BOLD);
	fType->linkHighlightState(hook, underscoresecond, underscoreboldcontent);
	
	/* End states for __ (no formatting - delimiters should not be styled) */
	underscoreboldend1 = fType->createHighlightState(hook, 0, '_', 0);
	fType->linkHighlightState(hook, underscoreboldcontent, underscoreboldend1);
	
	underscoreboldend2 = fType->createHighlightState(hook, 0, '_', 0);
	fType->linkHighlightState(hook, underscoreboldend1, underscoreboldend2);
	fType->linkHighlightState(hook, underscoreboldend2, startState);
	
	/* Content for _ (italic) - from first underscore */
	underscoreitaliccontent = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_ITALIC);
	fType->linkHighlightState(hook, underscorestart, underscoreitaliccontent);
	
	/* End state for _ (no formatting - delimiters should not be styled) */
	underscoreitalicend = fType->createHighlightState(hook, 0, '_', 0);
	fType->linkHighlightState(hook, underscoreitaliccontent, underscoreitalicend);
	fType->linkHighlightState(hook, underscoreitalicend, startState);
}

/* Setup strikethrough highlighting ~~text~~ */
void setupStrikethrough(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
	ULONG strikestartstate, strikewildstate, strikeendstate, strikesecond, strikeendsecond;

	/* Start with first tilde (no formatting) */
	strikestartstate = fType->createHighlightState(hook, 0, '~', 0);
	fType->linkHighlightState(hook, startState, strikestartstate);
	
	/* Second tilde (no formatting) */
	strikesecond = fType->createHighlightState(hook, 0, '~', 0);
	fType->linkHighlightState(hook, strikestartstate, strikesecond);
	
	/* Content */
	strikewildstate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_STRIKETHROUGH);
	fType->linkHighlightState(hook, strikesecond, strikewildstate);
	
	/* End with first tilde (no formatting - delimiters should not be styled) */
	strikeendstate = fType->createHighlightState(hook, 0, '~', 0);
	fType->linkHighlightState(hook, strikewildstate, strikeendstate);
	
	/* Second ending tilde (no formatting - delimiters should not be styled) */
	strikeendsecond = fType->createHighlightState(hook, 0, '~', 0);
	fType->linkHighlightState(hook, strikeendstate, strikeendsecond);
	fType->linkHighlightState(hook, strikeendsecond, startState);
}

/* Setup link highlighting [text](url) */
void setupLinks(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
	ULONG linkstartstate, linktextstate, linktextwild, linkurlstart, linkurlwild, linkendstate;

	/* Start with [ */
	linkstartstate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '[', HL_LINK);
	fType->linkHighlightState(hook, startState, linkstartstate);
	
	/* Link text */
	linktextstate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_LINK);
	fType->linkHighlightState(hook, linkstartstate, linktextstate);
	
	/* End of link text with ] */
	linktextwild = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ']', HL_LINK);
	fType->linkHighlightState(hook, linktextstate, linktextwild);
	
	/* Start of URL with ( */
	linkurlstart = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '(', HL_LINK);
	fType->linkHighlightState(hook, linktextwild, linkurlstart);
	
	/* URL content */
	linkurlwild = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_LINK);
	fType->linkHighlightState(hook, linkurlstart, linkurlwild);
	
	/* End of URL with ) */
	linkendstate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ')', HL_LINK);
	fType->linkHighlightState(hook, linkurlwild, linkendstate);
	fType->linkHighlightState(hook, linkendstate, startState);
}

/* Setup image highlighting ![alt](url) */
void setupImages(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
	ULONG imagestartstate, imagealtstart, imagealttext, imagealtend, imageurlstart, imageurlwild, imageendstate;

	/* Start with ! (no formatting) */
	imagestartstate = fType->createHighlightState(hook, 0, '!', 0);
	fType->linkHighlightState(hook, startState, imagestartstate);
	
	/* Start of alt text with [ */
	imagealtstart = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '[', HL_IMAGE);
	fType->linkHighlightState(hook, imagestartstate, imagealtstart);
	
	/* Alt text content */
	imagealttext = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_IMAGE);
	fType->linkHighlightState(hook, imagealtstart, imagealttext);
	
	/* End of alt text with ] */
	imagealtend = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ']', HL_IMAGE);
	fType->linkHighlightState(hook, imagealttext, imagealtend);
	
	/* Start of URL with ( */
	imageurlstart = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '(', HL_IMAGE);
	fType->linkHighlightState(hook, imagealtend, imageurlstart);
	
	/* URL content */
	imageurlwild = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_IMAGE);
	fType->linkHighlightState(hook, imageurlstart, imageurlwild);
	
	/* End of URL with ) */
	imageendstate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ')', HL_IMAGE);
	fType->linkHighlightState(hook, imageurlwild, imageendstate);
	fType->linkHighlightState(hook, imageendstate, startState);
}

/* Setup HTML tag highlighting <tag>content</tag> */
void setupHTMLTags(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
	ULONG tagstartstate, tagnamestate, tagendstate, contentstate, endtagstart, endtagname, endtagend;
	ULONG endtagslash;

	/* Start with < */
	tagstartstate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '<', HL_HTML);
	fType->linkHighlightState(hook, startState, tagstartstate);
	
	/* Tag name */
	tagnamestate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_HTML);
	fType->linkHighlightState(hook, tagstartstate, tagnamestate);
	
	/* End of opening tag with > */
	tagendstate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '>', HL_HTML);
	fType->linkHighlightState(hook, tagnamestate, tagendstate);
	
	/* Content between tags */
	contentstate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_HTML);
	fType->linkHighlightState(hook, tagendstate, contentstate);
	
	/* Start of closing tag with </ */
	endtagstart = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '<', HL_HTML);
	fType->linkHighlightState(hook, contentstate, endtagstart);
	
	endtagslash = fType->createHighlightState(hook, 0, '/', 0);
	fType->linkHighlightState(hook, endtagstart, endtagslash);
	
	/* Closing tag name */
	endtagname = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_HTML);
	fType->linkHighlightState(hook, endtagslash, endtagname);
	
	/* End of closing tag with > */
	endtagend = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '>', HL_HTML);
	fType->linkHighlightState(hook, endtagname, endtagend);
	fType->linkHighlightState(hook, endtagend, startState);
}

/* Setup code block highlighting ``` */
void setupCodeBlocks(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
	ULONG blockstart1, blockstart2, blockstart3, blockcontent, blockend1, blockend2, blockend3;

	/* Start with three backticks (no formatting) */
	blockstart1 = fType->createHighlightState(hook, 0, '`', 0);
	fType->linkHighlightState(hook, startState, blockstart1);
	
	blockstart2 = fType->createHighlightState(hook, 0, '`', 0);
	fType->linkHighlightState(hook, blockstart1, blockstart2);
	
	blockstart3 = fType->createHighlightState(hook, 0, '`', 0);
	fType->linkHighlightState(hook, blockstart2, blockstart3);
	
	/* Content state - match any character including newlines */
	blockcontent = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING|TEHL_STATE_SURVIVESNEWLINE, CHAR_ALWAYS, HL_CODEBLOCK);
	fType->linkHighlightState(hook, blockstart3, blockcontent);
	
	/* End with three backticks (no formatting) */
	blockend1 = fType->createHighlightState(hook, 0, '`', 0);
	fType->linkHighlightState(hook, blockcontent, blockend1);
	
	blockend2 = fType->createHighlightState(hook, 0, '`', 0);
	fType->linkHighlightState(hook, blockend1, blockend2);
	
	blockend3 = fType->createHighlightState(hook, 0, '`', 0);
	fType->linkHighlightState(hook, blockend2, blockend3);
	fType->linkHighlightState(hook, blockend3, startState);
}



/* Setup task lists - [ ] and [x] */
void setupTaskLists(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
	ULONG taskstart, taskbracket, taskspace, taskx, taskcontent;
	ULONG taskspace1, taskclose;

	/* Task list starts with - [ at beginning of line */
	taskstart = fType->createHighlightState(hook, 0, '-', 0);
	fType->linkHighlightState(hook, startState, taskstart);
	
	taskspace1 = fType->createHighlightState(hook, 0, ' ', 0);
	fType->linkHighlightState(hook, taskstart, taskspace1);
	
	taskbracket = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '[', HL_LIST);
	fType->linkHighlightState(hook, taskspace1, taskbracket);
	
	/* Either space (unchecked) or x (checked) */
	taskspace = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ' ', HL_LIST);
	fType->linkHighlightState(hook, taskbracket, taskspace);
	
	taskx = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 'x', HL_LIST);
	fType->linkHighlightState(hook, taskbracket, taskx);
	
	/* Closing bracket */
	taskclose = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ']', HL_LIST);
	fType->linkHighlightState(hook, taskspace, taskclose);
	fType->linkHighlightState(hook, taskx, taskclose);
	
	/* Task content */
	taskcontent = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_LIST);
	fType->linkHighlightState(hook, taskclose, taskcontent);
	fType->linkHighlightState(hook, taskcontent, startState);
}

/* Setup reference links [text][ref] */
void setupReferenceLinks(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
	ULONG refstart, reftext, refclose1, refopen2, refname, refclose2;

	/* Start with [ */
	refstart = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '[', HL_REFERENCE);
	fType->linkHighlightState(hook, startState, refstart);
	
	/* Link text */
	reftext = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_REFERENCE);
	fType->linkHighlightState(hook, refstart, reftext);
	
	/* Close first bracket */
	refclose1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ']', HL_REFERENCE);
	fType->linkHighlightState(hook, reftext, refclose1);
	
	/* Open second bracket */
	refopen2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '[', HL_REFERENCE);
	fType->linkHighlightState(hook, refclose1, refopen2);
	
	/* Reference name */
	refname = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_REFERENCE);
	fType->linkHighlightState(hook, refopen2, refname);
	
	/* Close second bracket */
	refclose2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ']', HL_REFERENCE);
	fType->linkHighlightState(hook, refname, refclose2);
	fType->linkHighlightState(hook, refclose2, startState);
}

/* Setup auto-links <http://example.com> */
void setupAutoLinks(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
	ULONG autostart, autourl, autoend;

	/* Start with < */
	autostart = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '<', HL_AUTOLINK);
	fType->linkHighlightState(hook, startState, autostart);
	
	/* URL content */
	autourl = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_AUTOLINK);
	fType->linkHighlightState(hook, autostart, autourl);
	
	/* End with > */
	autoend = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '>', HL_AUTOLINK);
	fType->linkHighlightState(hook, autourl, autoend);
	fType->linkHighlightState(hook, autoend, startState);
}

/* Setup blockquote highlighting > text */
void setupBlockquotes(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
	ULONG blockquotestate, blockquotewild, blockquotespace;

	/* Blockquotes start with > at beginning of line */
	blockquotestate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_AUTOINITIALSTATE, '>', HL_BLOCKQUOTE);
	fType->linkHighlightState(hook, INITIALSTATE, blockquotestate);
	
	/* After >, expect a space */
	blockquotespace = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ' ', HL_BLOCKQUOTE);
	fType->linkHighlightState(hook, blockquotestate, blockquotespace);
	
	/* Continue with blockquote content to end of line */
	blockquotewild = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_BLOCKQUOTE);
	fType->linkHighlightState(hook, blockquotespace, blockquotewild);
	fType->linkHighlightState(hook, blockquotewild, INITIALSTATE);
}

static void __ASM__ init(__REG__(a0, APTR userData))
{
	struct Hook *hook;
    struct Filetype *fType = (struct Filetype *)userData;
	struct HighlighterGUI *hlGUI;
	ULONG headingstate, headingwild;
	ULONG blockquotestate, blockquotewild, blockquotespace;
	ULONG liststate, listwild, listspace;
	ULONG listplusstate, listpluswild, listplusspace;
	ULONG listasteriskstate, listasteriskwild, listasteriskspace;
	ULONG listnumstate;
	ULONG listnumdotstate, listnumdotwild, listnumwild;
	ULONG listnum0, listnum2, listnum3, listnum4, listnum5, listnum6, listnum7, listnum8, listnum9;
	ULONG headingspace, headingtext;
	ULONG listnumdotspace;
	ULONG headingstartstate, blockquotestartstate, listdashstartstate, listplusstartstate, listasteriskstartstate;
	ULONG listnumstartstate, listnum0startstate, listnum2startstate, listnum3startstate, listnum4startstate;
	ULONG listnum5startstate, listnum6startstate, listnum7startstate, listnum8startstate, listnum9startstate;
	ULONG whitespace1, whitespace2, whitespace3, whitespace4;
	ULONG tabstate;

	if (!fType)
		return;

    ensureOpenLibs();

	fType->typeName = "Markdown";
	fType->autoFileTypes = "markdown,md";
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

	fType->name = "Markdown";

	stylename[0] = "Plain";
	stylename[1] = "Heading";
	stylename[2] = "Code";
	stylename[3] = "Bold";
	stylename[4] = "Italic";
	stylename[5] = "Link";
	stylename[6] = "List";
	stylename[7] = "Blockquote";
	stylename[8] = "CodeBlock";
	stylename[9] = "Strikethrough";
	stylename[10] = "HTML";
	stylename[11] = "Image";
	stylename[12] = "Reference";
	stylename[13] = "AutoLink";
	stylename[14] = "BoldUnderline";

	hlGUI = fType->createHighlightStylesGUI(stylename, defaultStyleCodes);
	fType->pluginData = (ULONG)hlGUI;

	if (!hlGUI)
		return;

	fType->highlighterHook = hook = fType->createHighlighter(2000, 0, hlGUI->inuseStylecodes);

	if (!hook)
		return;

	fType->createHighlightState(hook, 0, CHAR_NONWORD, 0); /* non word wildcard */

	fType->createHighlightState(hook, TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, 0); /* always matches wildcard */

	/* Setup multiline patterns FIRST (code blocks) */
	setupCodeBlocks(fType, hook, INITIALSTATE);
	
	/* Setup headings FIRST to avoid conflicts */
	/* Headings start with # and continue to end of line */
	headingstate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '#', HL_HEADING);
	fType->linkHighlightState(hook, INITIALSTATE, headingstate);
	
	/* Continue with more # characters for deeper headings */
	headingwild = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, '#', HL_HEADING);
	fType->linkHighlightState(hook, headingstate, headingwild);
	
	/* After # characters, expect a space */
	headingspace = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ' ', HL_HEADING);
	fType->linkHighlightState(hook, headingwild, headingspace);
	fType->linkHighlightState(hook, headingstate, headingspace); /* Also allow single # */
	
	/* After space, continue with heading text to end of line */
	headingtext = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_HEADING);
	fType->linkHighlightState(hook, headingspace, headingtext);
	fType->linkHighlightState(hook, headingtext, headingtext); /* Self-loop to continue highlighting */
	fType->linkHighlightState(hook, headingtext, INITIALSTATE);
	
	/* Also allow heading to continue from the wild state (for multiple #) */
	fType->linkHighlightState(hook, headingwild, headingtext);
	
	/* Allow heading to continue directly from single # state (for # without space) */
	fType->linkHighlightState(hook, headingstate, headingtext);
	
	/* Setup lists SECOND to avoid conflicts with inline patterns */
	/* Lists can start from column 1 OR after whitespace */
	/* First create whitespace states that can lead to lists */
	
	/* Single space */
	whitespace1 = fType->createHighlightState(hook, 0, ' ', 0);
	fType->linkHighlightState(hook, INITIALSTATE, whitespace1);
	
	/* Tab character */
	tabstate = fType->createHighlightState(hook, 0, '\t', 0);
	fType->linkHighlightState(hook, INITIALSTATE, tabstate);
	
	/* Multiple spaces (self-repeating) */
	whitespace2 = fType->createHighlightState(hook, 0, ' ', 0);
	fType->linkHighlightState(hook, whitespace1, whitespace2);
	fType->linkHighlightState(hook, whitespace2, whitespace2); /* Self-loop for multiple spaces */
	fType->linkHighlightState(hook, tabstate, whitespace2); /* Tab can be followed by spaces */
	
	/* Unordered lists start with - */
	liststate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '-', HL_LIST);
	fType->linkHighlightState(hook, INITIALSTATE, liststate);
	fType->linkHighlightState(hook, whitespace1, liststate);
	fType->linkHighlightState(hook, whitespace2, liststate);
	fType->linkHighlightState(hook, tabstate, liststate);
	
	/* After -, expect a space for list items */
	listspace = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ' ', HL_LIST);
	fType->linkHighlightState(hook, liststate, listspace);
	
	/* Continue with list content to end of line */
	listwild = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_LIST);
	fType->linkHighlightState(hook, listspace, listwild);
	fType->linkHighlightState(hook, listwild, listwild); /* Self-loop to continue highlighting */
	fType->linkHighlightState(hook, listwild, INITIALSTATE);

	/* Unordered lists start with + */
	listplusstate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '+', HL_LIST);
	fType->linkHighlightState(hook, INITIALSTATE, listplusstate);
	fType->linkHighlightState(hook, whitespace1, listplusstate);
	fType->linkHighlightState(hook, whitespace2, listplusstate);
	fType->linkHighlightState(hook, tabstate, listplusstate);
	
	/* After +, expect a space for list items */
	listplusspace = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ' ', HL_LIST);
	fType->linkHighlightState(hook, listplusstate, listplusspace);
	
	/* Continue with list content to end of line */
	listpluswild = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_LIST);
	fType->linkHighlightState(hook, listplusspace, listpluswild);
	fType->linkHighlightState(hook, listpluswild, listpluswild); /* Self-loop to continue highlighting */
	fType->linkHighlightState(hook, listpluswild, INITIALSTATE);

	/* Unordered lists start with * followed by space */
	/* Make this more specific to avoid conflicts with bold/italic patterns */
	listasteriskstate = fType->createHighlightState(hook, 0, '*', 0);
	fType->linkHighlightState(hook, INITIALSTATE, listasteriskstate);
	fType->linkHighlightState(hook, whitespace1, listasteriskstate);
	fType->linkHighlightState(hook, whitespace2, listasteriskstate);
	fType->linkHighlightState(hook, tabstate, listasteriskstate);
	
	/* After *, expect a space for list items */
	listasteriskspace = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ' ', HL_LIST);
	fType->linkHighlightState(hook, listasteriskstate, listasteriskspace);
	
	/* Continue with list content to end of line */
	listasteriskwild = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_LIST);
	fType->linkHighlightState(hook, listasteriskspace, listasteriskwild);
	fType->linkHighlightState(hook, listasteriskwild, listasteriskwild); /* Self-loop to continue highlighting */
	fType->linkHighlightState(hook, listasteriskwild, INITIALSTATE);

	/* Ordered lists start with numbers */
	listnumstate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '1', HL_LIST);
	fType->linkHighlightState(hook, INITIALSTATE, listnumstate);
	fType->linkHighlightState(hook, whitespace1, listnumstate);
	fType->linkHighlightState(hook, whitespace2, listnumstate);
	fType->linkHighlightState(hook, tabstate, listnumstate);
	
	/* Add states for other digits 0,2-9 */
	listnum0 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '0', HL_LIST);
	fType->linkHighlightState(hook, INITIALSTATE, listnum0);
	fType->linkHighlightState(hook, whitespace1, listnum0);
	fType->linkHighlightState(hook, whitespace2, listnum0);
	fType->linkHighlightState(hook, tabstate, listnum0);
	
	listnum2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '2', HL_LIST);
	fType->linkHighlightState(hook, INITIALSTATE, listnum2);
	fType->linkHighlightState(hook, whitespace1, listnum2);
	fType->linkHighlightState(hook, whitespace2, listnum2);
	fType->linkHighlightState(hook, tabstate, listnum2);
	
	listnum3 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '3', HL_LIST);
	fType->linkHighlightState(hook, INITIALSTATE, listnum3);
	fType->linkHighlightState(hook, whitespace1, listnum3);
	fType->linkHighlightState(hook, whitespace2, listnum3);
	fType->linkHighlightState(hook, tabstate, listnum3);
	
	listnum4 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '4', HL_LIST);
	fType->linkHighlightState(hook, INITIALSTATE, listnum4);
	fType->linkHighlightState(hook, whitespace1, listnum4);
	fType->linkHighlightState(hook, whitespace2, listnum4);
	fType->linkHighlightState(hook, tabstate, listnum4);
	
	listnum5 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '5', HL_LIST);
	fType->linkHighlightState(hook, INITIALSTATE, listnum5);
	fType->linkHighlightState(hook, whitespace1, listnum5);
	fType->linkHighlightState(hook, whitespace2, listnum5);
	fType->linkHighlightState(hook, tabstate, listnum5);
	
	listnum6 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '6', HL_LIST);
	fType->linkHighlightState(hook, INITIALSTATE, listnum6);
	fType->linkHighlightState(hook, whitespace1, listnum6);
	fType->linkHighlightState(hook, whitespace2, listnum6);
	fType->linkHighlightState(hook, tabstate, listnum6);
	
	listnum7 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '7', HL_LIST);
	fType->linkHighlightState(hook, INITIALSTATE, listnum7);
	fType->linkHighlightState(hook, whitespace1, listnum7);
	fType->linkHighlightState(hook, whitespace2, listnum7);
	fType->linkHighlightState(hook, tabstate, listnum7);
	
	listnum8 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '8', HL_LIST);
	fType->linkHighlightState(hook, INITIALSTATE, listnum8);
	fType->linkHighlightState(hook, whitespace1, listnum8);
	fType->linkHighlightState(hook, whitespace2, listnum8);
	fType->linkHighlightState(hook, tabstate, listnum8);
	
	listnum9 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '9', HL_LIST);
	fType->linkHighlightState(hook, INITIALSTATE, listnum9);
	fType->linkHighlightState(hook, whitespace1, listnum9);
	fType->linkHighlightState(hook, whitespace2, listnum9);
	fType->linkHighlightState(hook, tabstate, listnum9);
	
	/* Allow additional digits for multi-digit numbers */
	listnumwild = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_LIST);
	fType->linkHighlightState(hook, listnumstate, listnumwild);
	fType->linkHighlightState(hook, listnum0, listnumwild);
	fType->linkHighlightState(hook, listnum2, listnumwild);
	fType->linkHighlightState(hook, listnum3, listnumwild);
	fType->linkHighlightState(hook, listnum4, listnumwild);
	fType->linkHighlightState(hook, listnum5, listnumwild);
	fType->linkHighlightState(hook, listnum6, listnumwild);
	fType->linkHighlightState(hook, listnum7, listnumwild);
	fType->linkHighlightState(hook, listnum8, listnumwild);
	fType->linkHighlightState(hook, listnum9, listnumwild);
	
	/* After digit(s), expect a dot */
	listnumdotstate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '.', HL_LIST);
	fType->linkHighlightState(hook, listnumstate, listnumdotstate);
	fType->linkHighlightState(hook, listnum0, listnumdotstate);
	fType->linkHighlightState(hook, listnum2, listnumdotstate);
	fType->linkHighlightState(hook, listnum3, listnumdotstate);
	fType->linkHighlightState(hook, listnum4, listnumdotstate);
	fType->linkHighlightState(hook, listnum5, listnumdotstate);
	fType->linkHighlightState(hook, listnum6, listnumdotstate);
	fType->linkHighlightState(hook, listnum7, listnumdotstate);
	fType->linkHighlightState(hook, listnum8, listnumdotstate);
	fType->linkHighlightState(hook, listnum9, listnumdotstate);
	fType->linkHighlightState(hook, listnumwild, listnumdotstate);
	
	/* After dot, expect a space */
	listnumdotspace = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, ' ', HL_LIST);
	fType->linkHighlightState(hook, listnumdotstate, listnumdotspace);
	
	/* After space, continue with list content to end of line */
	listnumdotwild = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_LIST);
	fType->linkHighlightState(hook, listnumdotspace, listnumdotwild);
	fType->linkHighlightState(hook, listnumdotwild, listnumdotwild); /* Self-loop to continue highlighting */
	fType->linkHighlightState(hook, listnumdotwild, INITIALSTATE);

	/* Setup blockquotes THIRD */
	/* Blockquotes can start from column 1 OR after whitespace */
	/* Blockquotes start with > (no formatting) */
	blockquotestate = fType->createHighlightState(hook, 0, '>', 0);
	fType->linkHighlightState(hook, INITIALSTATE, blockquotestate);
	fType->linkHighlightState(hook, whitespace1, blockquotestate);
	fType->linkHighlightState(hook, whitespace2, blockquotestate);
	fType->linkHighlightState(hook, tabstate, blockquotestate);
	
	/* After >, expect a space (no formatting) */
	blockquotespace = fType->createHighlightState(hook, 0, ' ', 0);
	fType->linkHighlightState(hook, blockquotestate, blockquotespace);
	
	/* Continue with blockquote content to end of line */
	blockquotewild = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_NONNEWLINE, HL_BLOCKQUOTE);
	fType->linkHighlightState(hook, blockquotespace, blockquotewild);
	fType->linkHighlightState(hook, blockquotewild, blockquotewild); /* Self-loop to continue highlighting */
	fType->linkHighlightState(hook, blockquotewild, INITIALSTATE);

	/* Setup inline patterns LAST */
	setupInlineCode(fType, hook, DEFAULTSTATE);
	setupInlineCode(fType, hook, INITIALSTATE); /* Also allow inline code from line start */
	setupBoldItalic(fType, hook, DEFAULTSTATE);
	setupBoldItalic(fType, hook, INITIALSTATE); /* Also allow bold/italic from line start */
	setupUnderscoreEmphasis(fType, hook, DEFAULTSTATE);
	setupUnderscoreEmphasis(fType, hook, INITIALSTATE); /* Also allow underscore emphasis from line start */
	setupStrikethrough(fType, hook, DEFAULTSTATE);
	setupStrikethrough(fType, hook, INITIALSTATE); /* Also allow strikethrough from line start */
	setupLinks(fType, hook, DEFAULTSTATE);
	setupImages(fType, hook, DEFAULTSTATE);
	setupHTMLTags(fType, hook, DEFAULTSTATE);
	setupReferenceLinks(fType, hook, DEFAULTSTATE);
	setupAutoLinks(fType, hook, DEFAULTSTATE);
	
	setupTaskLists(fType, hook, DEFAULTSTATE);

	/* Link from line content states to DEFAULTSTATE for inline patterns */
	fType->linkHighlightState(hook, headingtext, DEFAULTSTATE);
	fType->linkHighlightState(hook, listwild, DEFAULTSTATE);
	fType->linkHighlightState(hook, listpluswild, DEFAULTSTATE);
	fType->linkHighlightState(hook, listasteriskwild, DEFAULTSTATE);
	fType->linkHighlightState(hook, listnumdotwild, DEFAULTSTATE);
	fType->linkHighlightState(hook, blockquotewild, DEFAULTSTATE);
	
	/* Also link from the always-matches wildcard to DEFAULTSTATE for inline patterns */
	fType->linkHighlightState(hook, 1, DEFAULTSTATE); /* 1 is the always-matches wildcard state */
	
	/* Note: Removed fallback from list asterisk state to DEFAULTSTATE to avoid conflicts */
	/* The list asterisk pattern should only trigger when followed by a space */
	/* Bold/italic patterns are set up separately and should work independently */
	
	/* Ensure inline patterns work from column 1 by linking whitespace states to DEFAULTSTATE */
	/* This allows inline patterns to work after whitespace */
	fType->linkHighlightState(hook, whitespace1, DEFAULTSTATE);
	fType->linkHighlightState(hook, whitespace2, DEFAULTSTATE);
	fType->linkHighlightState(hook, tabstate, DEFAULTSTATE);
	
	/* Link DEFAULTSTATE back to INITIALSTATE for proper state machine operation */
	fType->linkHighlightState(hook, DEFAULTSTATE, INITIALSTATE);
}

 