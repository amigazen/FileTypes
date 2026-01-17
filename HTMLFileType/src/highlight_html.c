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

/* Highlight style definitions for HTML */
#define HL_PLAIN 0
#define HL_TAG 1
#define HL_ATTRIBUTE 2
#define HL_ATTRIBUTEVALUE 3
#define HL_COMMENT 4
#define HL_DOCTYPE 5
#define HL_ENTITY 6
#define HL_SCRIPT 7
#define HL_STYLE 8
#define HL_STRING 9

static UWORD defaultStyleCodes[10] =
{
    (0<<8)|0, // plain
    (1<<8)|TBSTYLE_BOLD, // tag (bold for emphasis)
    (2<<8)|TBSTYLE_SETCOLOR, // attribute (color only)
    (3<<8)|TBSTYLE_SETCOLOR, // attribute value (color only)
    (4<<8)|TBSTYLE_ITALIC, // comment (italic)
    (5<<8)|TBSTYLE_SETCOLOR, // doctype (color only)
    (6<<8)|TBSTYLE_SETCOLOR, // entity (color only)
    (7<<8)|TBSTYLE_SETCOLOR, // script content (color only)
    (8<<8)|TBSTYLE_SETCOLOR, // style content (color only)
    (9<<8)|TBSTYLE_SETCOLOR // string (color only)
};

static CONST_STRPTR stylename[11];

/* HTML4 tag keywords */
static CONST_STRPTR html_tags[] = {
    /* Document structure */
    "html", "head", "body", "title", "meta", "link", "base", "basefont",
    "script", "style", "noscript",
    /* Text structure */
    "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
    "blockquote", "pre", "address", "center",
    /* Text formatting */
    "strong", "em", "b", "i", "u", "s", "strike", "tt", "code", "kbd",
    "samp", "var", "cite", "abbr", "acronym", "dfn", "del", "ins",
    "big", "small", "sub", "sup", "font",
    /* Lists */
    "ul", "ol", "li", "dl", "dt", "dd", "dir", "menu",
    /* Links and images */
    "a", "img", "map", "area",
    /* Tables */
    "table", "caption", "thead", "tbody", "tfoot", "tr", "th", "td",
    "colgroup", "col",
    /* Forms */
    "form", "input", "button", "textarea", "select", "option", "optgroup",
    "label", "fieldset", "legend", "isindex",
    /* Frames */
    "frameset", "frame", "noframes", "iframe",
    /* Embedded objects */
    "object", "param", "embed", "applet",
    /* Other */
    "hr", "br", "wbr",
    NULL
};

/* HTML4 attribute keywords */
static CONST_STRPTR html_attributes[] = {
    /* Core attributes */
    "id", "class", "style", "title",
    /* Language attributes */
    "lang", "dir",
    /* Event handlers */
    "onclick", "ondblclick", "onmousedown", "onmouseup", "onmouseover",
    "onmousemove", "onmouseout", "onkeypress", "onkeydown", "onkeyup",
    "onload", "onunload", "onfocus", "onblur", "onchange", "onsubmit",
    "onreset", "onselect", "onerror",
    /* Link and image attributes */
    "href", "src", "alt", "usemap", "ismap", "shape", "coords", "target",
    "rel", "rev", "hreflang", "type", "media",
    /* Table attributes */
    "colspan", "rowspan", "headers", "scope", "abbr", "axis", "align",
    "valign", "char", "charoff", "width", "height", "border", "cellpadding",
    "cellspacing", "frame", "rules", "summary",
    /* Form attributes */
    "name", "value", "type", "size", "maxlength", "checked", "disabled",
    "readonly", "multiple", "selected", "method", "action", "enctype",
    "accept", "accept-charset", "tabindex", "accesskey",
    /* Object and embed attributes */
    "codebase", "archive", "standby", "declare", "codetype", "data",
    "code", "object", "classid", "width", "height", "hspace", "vspace",
    /* Meta and link attributes */
    "charset", "content", "http-equiv", "scheme", "name",
    /* Frame attributes */
    "frameborder", "marginwidth", "marginheight", "scrolling", "noresize",
    "longdesc", "rows", "cols",
    /* Font and text attributes */
    "face", "size", "color",
    /* Other attributes */
    "bgcolor", "background", "text", "link", "vlink", "alink",
    "clear", "noshade", "compact", "nowrap", "wrap",
    NULL
};

/* Setup HTML comment highlighting <!-- --> - based on working Amiga E plugin */
void setupComments(struct Filetype *fType, struct Hook *hook, ULONG startState)
{
    ULONG startstate, state1, state2;
    
    /* HTML comments starting with <!-- - build state machine manually */
    /* Note: This is a simplified pattern - full HTML comments are <!-- ... --> */
    startstate = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 60, HL_COMMENT);
    fType->linkHighlightState(hook, startState, startstate);
    state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING|TEHL_STATE_SURVIVESNEWLINE, CHAR_ALWAYS, HL_COMMENT);
    fType->linkHighlightState(hook, startstate, state1);
    state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 62, HL_COMMENT);
    fType->linkHighlightState(hook, state1, state2);
    /* No explicit exit link - state machine handles it automatically */
}


static void __ASM__ init(__REG__(a0, APTR userData))
{
	struct Hook *hook;
    struct Filetype *fType = (struct Filetype *)userData;
	struct HighlighterGUI *hlGUI;

    int i;

    ensureOpenLibs();

	fType->typeName = "HTML";
	fType->autoFileTypes = "html,htm";
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

	fType->name = "HTML";

	/* Initialize style names */
	stylename[0] = "Plain";
	stylename[1] = "Tag";
	stylename[2] = "Attribute";
	stylename[3] = "AttributeValue";
	stylename[4] = "Comment";
	stylename[5] = "DOCTYPE";
	stylename[6] = "Entity";
	stylename[7] = "Script";
	stylename[8] = "Style";
	stylename[9] = "String";
	stylename[10] = NULL;

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

	/* Set up HTML syntax highlighting - order matters for precedence */
	/* Setup comments FIRST (highest priority) */
	setupComments(fType, hook, INITIALSTATE);
	
	/* Set up HTML tag keywords */
	for (i = 0; html_tags[i] != NULL; i++) {
		fType->createHighlightKeyword(hook, (STRPTR)html_tags[i], HL_TAG, INITIALSTATE, TRUE);
	}
	
	/* Set up HTML attribute keywords */
	for (i = 0; html_attributes[i] != NULL; i++) {
		fType->createHighlightKeyword(hook, (STRPTR)html_attributes[i], HL_ATTRIBUTE, INITIALSTATE, TRUE);
	}

	fType->linkHighlightState(hook, DEFAULTSTATE, INITIALSTATE);
} 