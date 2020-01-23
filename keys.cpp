//
// keys.cpp
//
// ITfKeyEventSink implementation.
//

#include "globals.h"
#include "case.h"
#include "editsess.h"

class CKeystrokeEditSession : public CEditSessionBase
{
public:
    CKeystrokeEditSession(ITfContext *pContext, WPARAM wParam) : CEditSessionBase(pContext)
    {
        _wParam = wParam;
    }

    // ITfEditSession
    STDMETHODIMP DoEditSession(TfEditCookie ec);

private:
    WPARAM _wParam;
};


/* 5d6d1b1e-64f2-47cd-9fe1-4e032c2dae77 */
static const GUID GUID_PRESERVEDKEY_FLIPCASE = { 0x5d6d1b1e, 0x64f2, 0x47cd, {0x9f, 0xe1, 0x4e, 0x03, 0x2c, 0x2d, 0xae, 0x77} };
// arbitrary hotkey: ctl-f
static const TF_PRESERVEDKEY c_FlipCaseKey = { 'F', TF_MOD_CONTROL };


//+---------------------------------------------------------------------------
//
// IsKeyEaten
//
//----------------------------------------------------------------------------

inline BOOL IsKeyEaten(BOOL fFlipKeys, WPARAM wParam)
{
    // we're only interested in VK_A - VK_Z, when the "Flip Keys" menu option
    // is on
    return fFlipKeys && (wParam >= 'A') && (wParam <= 'Z');
}

//+---------------------------------------------------------------------------
//
// _InitKeystrokeSink
//
// Advise a keystroke sink.
//----------------------------------------------------------------------------

BOOL CCaseTextService::_InitKeystrokeSink()
{
    ITfKeystrokeMgr *pKeystrokeMgr;
    HRESULT hr;

    if (_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void **)&pKeystrokeMgr) != S_OK)
        return FALSE;

    hr = pKeystrokeMgr->AdviseKeyEventSink(_tfClientId, (ITfKeyEventSink *)this, TRUE);

    pKeystrokeMgr->Release();

    return (hr == S_OK);
}

//+---------------------------------------------------------------------------
//
// _UninitKeystrokeSink
//
// Unadvise a keystroke sink.  Assumes we have advised one already.
//----------------------------------------------------------------------------

void CCaseTextService::_UninitKeystrokeSink()
{
    ITfKeystrokeMgr *pKeystrokeMgr;

    if (_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void **)&pKeystrokeMgr) != S_OK)
        return;

    pKeystrokeMgr->UnadviseKeyEventSink(_tfClientId);

    pKeystrokeMgr->Release();
}

//+---------------------------------------------------------------------------
//
// _InitPreservedKey
//
// Register a hot key.
//----------------------------------------------------------------------------

BOOL CCaseTextService::_InitPreservedKey()
{
    const WCHAR wchToggleCase[] = L"Toggle Case";
    ITfKeystrokeMgr *pKeystrokeMgr;
    HRESULT hr;

    if (_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void **)&pKeystrokeMgr) != S_OK)
        return FALSE;

    hr = pKeystrokeMgr->PreserveKey(_tfClientId, GUID_PRESERVEDKEY_FLIPCASE,
                                    &c_FlipCaseKey, wchToggleCase,
                                    ARRAYSIZE(wchToggleCase)-1);

    pKeystrokeMgr->Release();

    return (hr == S_OK);
}

//+---------------------------------------------------------------------------
//
// _UninitPreservedKey
//
// Uninit a hot key.
//----------------------------------------------------------------------------

void CCaseTextService::_UninitPreservedKey()
{
    ITfKeystrokeMgr *pKeystrokeMgr;

    if (_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void **)&pKeystrokeMgr) != S_OK)
        return;

    pKeystrokeMgr->UnpreserveKey(GUID_PRESERVEDKEY_FLIPCASE, &c_FlipCaseKey);

    pKeystrokeMgr->Release();
}

//+---------------------------------------------------------------------------
//
// OnSetFocus
//
// Called by the system whenever this service gets the keystroke device focus.
//----------------------------------------------------------------------------

STDAPI CCaseTextService::OnSetFocus(BOOL fForeground)
{
    return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnTestKeyDown
//
// Called by the system to query this service wants a potential keystroke.
//----------------------------------------------------------------------------

STDAPI CCaseTextService::OnTestKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
    *pfEaten = IsKeyEaten(_fFlipKeys, wParam);
    return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnKeyDown
//
// Called by the system to offer this service a keystroke.  If *pfEaten == TRUE
// on exit, the application will not handle the keystroke.
//----------------------------------------------------------------------------

STDAPI CCaseTextService::OnKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	HRESULT hr = S_OK;

    *pfEaten = IsKeyEaten(_fFlipKeys, wParam);

    if (*pfEaten)
    {
		hr = StartEditSession(pContext, wParam);
    }

    if (hr != S_OK)
    {
        *pfEaten = FALSE;
    }
    return S_OK;
}

HRESULT CCaseTextService::StartEditSession(ITfContext *pContext, WPARAM wParam) {
	CKeystrokeEditSession *pEditSession;
	HRESULT hr = S_OK;

	// we'll insert a char ourselves in place of this keystroke
	if ((pEditSession = new CKeystrokeEditSession(pContext, wParam)) == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	// we need a lock to do our work
	// nb: this method is one of the few places where it is legal to use
	// the TF_ES_SYNC flag
	if (pContext->RequestEditSession(_tfClientId, pEditSession, TF_ES_SYNC | TF_ES_READWRITE, &hr) != S_OK)
	{
		hr = E_FAIL;
	}

	pEditSession->Release();

Exit:
	return hr;
}

//+---------------------------------------------------------------------------
//
// DoEditSession
//
//----------------------------------------------------------------------------

STDAPI CKeystrokeEditSession::DoEditSession(TfEditCookie ec)
{
    WCHAR wc;

    // we want to insert circled letters

	if (_wParam == 0)
	{
		// preserved key Ctrl+F
		wc = 0x24FF;
	}
	else if (GetKeyState(VK_SHIFT) & 0x8000)
    {
        // shifted A-Z (Caps Lock state ignored)
        wc = (WCHAR)(_wParam - 'A' + 0x24B6);
    }
    else
    {
		// unshifted A-Z
        wc = (WCHAR)(_wParam - 'A' + 0x24D0);
    }

    InsertTextAtSelection(ec, _pContext, &wc, 1);

    return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnTestKeyUp
//
// Called by the system to query this service wants a potential keystroke.
//----------------------------------------------------------------------------

STDAPI CCaseTextService::OnTestKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
    *pfEaten = IsKeyEaten(_fFlipKeys, wParam);
    return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnKeyUp
//
// Called by the system to offer this service a keystroke.  If *pfEaten == TRUE
// on exit, the application will not handle the keystroke.
//----------------------------------------------------------------------------

STDAPI CCaseTextService::OnKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
    *pfEaten = IsKeyEaten(_fFlipKeys, wParam);
    return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnPreservedKey
//
// Called when a hotkey (registered by us, or by the system) is typed.
//----------------------------------------------------------------------------

STDAPI CCaseTextService::OnPreservedKey(ITfContext *pContext, REFGUID rguid, BOOL *pfEaten)
{
    if (IsEqualGUID(rguid, GUID_PRESERVEDKEY_FLIPCASE))
    {
		StartEditSession(pContext, 0);
        *pfEaten = TRUE;
    }
    else
    {
        *pfEaten = FALSE;
    }

    return S_OK;
}


//+---------------------------------------------------------------------------
//
// InsertTextAtSelection
//
//----------------------------------------------------------------------------

void InsertTextAtSelection(TfEditCookie ec, ITfContext *pContext, const WCHAR *pchText, ULONG cchText)
{
	ITfInsertAtSelection *pInsertAtSelection;
	ITfRange *pRange;
	TF_SELECTION tfSelection;

	// we need a special interface to insert text at the selection
	if (pContext->QueryInterface(IID_ITfInsertAtSelection, (void **)&pInsertAtSelection) != S_OK)
		return;

	// insert the text
	if (pInsertAtSelection->InsertTextAtSelection(ec, 0, pchText, cchText, &pRange) != S_OK)
		goto Exit;

	// update the selection, we'll make it an insertion point just past
	// the inserted text.
	pRange->Collapse(ec, TF_ANCHOR_END);

	tfSelection.range = pRange;
	tfSelection.style.ase = TF_AE_NONE;
	tfSelection.style.fInterimChar = FALSE;

	pContext->SetSelection(ec, 1, &tfSelection);

	pRange->Release();

Exit:
	pInsertAtSelection->Release();
}
