#define __NOLIBBASE__
#include <proto/exec.h>

#include <proto/iffparse.h>

#include <tools/textedit/filetypes.h>

extern struct Library *SysBase;
extern struct Library *IFFParseBase;

void ensureOpenLibs(void)
{
	SysBase  = (*((struct Library **) 4));

	if (! (IFFParseBase = OpenLibrary("iffparse.library",45)))
		return;
}

void ensureCloseLibs(void)
{
	if (IFFParseBase)
		CloseLibrary(IFFParseBase);
}

void __ASM__ terminate(__REG__ (a0, struct Filetype *fType))
{
	struct HighlighterGUI *hlGUI;
	
	if (!fType)
		return;
		
	hlGUI = (struct HighlighterGUI *)fType->pluginData;

	if (fType->highlighterHook)
		fType->freeHighlighter(fType->highlighterHook);

	if (hlGUI)
		fType->disposeHighlightStylesGUI(hlGUI);

	ensureCloseLibs();
}


Object * __ASM__ settingsGadget(__REG__(a0, struct Filetype *fType), __REG__(d0, ULONG i))
{
	struct HighlighterGUI *hlGUI;
	
	if (!fType || i != 0)
		return NULL;
		
	hlGUI = (struct HighlighterGUI *)fType->pluginData;

	if (!hlGUI)
		return NULL;

	return hlGUI->gadget;
}

STRPTR __ASM__ settingsTitle(__REG__(a0, struct Filetype *fType), __REG__(d0, ULONG i))
{
	if (i != 0)
		return NULL;
	return "Highlighting";
}

void __ASM__ setSettingsFromGUI(__REG__(a0, struct Filetype *fType))
{
	struct HighlighterGUI *hlGUI;
	
	if (!fType)
		return;
		
	hlGUI = (struct HighlighterGUI *)fType->pluginData;

	if (hlGUI)
		fType->setHighlightStyleSettingsFromGUI(hlGUI);
}

void __ASM__ setGUIFromSettings(__REG__(a0, struct Filetype *fType), __REG__(a1, struct Window *window))
{
	struct HighlighterGUI *hlGUI;
	
	if (!fType)
		return;
		
	hlGUI = (struct HighlighterGUI *)fType->pluginData;

	if (hlGUI)
		fType->setGUIFromHighlightStyleSettings(hlGUI, window);
}

void __ASM__ applySettings(__REG__(a0, struct Filetype *fType))
{
	struct HighlighterGUI *hlGUI;
	
	if (!fType)
		return;
		
	hlGUI = (struct HighlighterGUI *)fType->pluginData;

	if (hlGUI)
		fType->applyHighlightStylesSettings(hlGUI);
}

void __ASM__ disposeGadgets(__REG__(a0, struct Filetype *fType))
{
	// we don't dispose anything as this is handled in terminate once we are fully gone
}

void __ASM__ processGadgetUp(__REG__(a0, struct Filetype *fType), __REG__(d0, UWORD gadgetId), __REG__(a1, struct Window *window))
{
	struct HighlighterGUI *hlGUI;
	
	if (!fType)
		return;
		
	hlGUI = (struct HighlighterGUI *)fType->pluginData;

	if (hlGUI)
		fType->processHighlightStylesGUIGadgetUp(hlGUI, gadgetId, window);
}

BOOL __ASM__ saveHighlightSettings(__REG__(a0, struct IFFHandle *iff), __REG__(a1, struct ContextNode *cn), __REG__(a2, APTR data))
{
	struct Filetype *fType = (struct Filetype *)data;
	struct HighlighterGUI *hlGUI;
	BOOL result = FALSE;

	if (!fType || !iff)
		return FALSE;
		
	hlGUI = (struct HighlighterGUI *)fType->pluginData;

	if (hlGUI && PushChunk(iff, 0, MAKE_ID('T', 'E', 'H', 'L'), hlGUI->numHLType*2+4) == 0)
	{
		WriteChunkBytes(iff, (void *)&hlGUI->numHLType, hlGUI->numHLType*2+4);
		result = !PopChunk(iff);
	}

	return result;
}

BOOL __ASM__ loadHighlightSettings(__REG__(a0, struct IFFHandle *iff), __REG__(a1, struct ContextNode *cn), __REG__(a2, APTR data))
{
	struct Filetype *fType = (struct Filetype *)data;
	struct HighlighterGUI *hlGUI;
	BOOL result = TRUE;

	if (!fType || !iff || !cn)
		return FALSE;
		
	hlGUI = (struct HighlighterGUI *)fType->pluginData;

	if (hlGUI && cn->cn_ID == MAKE_ID('T', 'E', 'H', 'L') && cn->cn_Type == MAKE_ID('T', 'E', 'P', 'R'))
	{
		ULONG preCount = hlGUI->numHLType;
		ReadChunkBytes(iff, (void *)&hlGUI->numHLType, 4);
		ReadChunkBytes(iff, (void *)&hlGUI->inuseStylecodes, 2*hlGUI->numHLType);
		if (hlGUI->numHLType < preCount)
			hlGUI->numHLType = preCount;
	}
	return result;
}
