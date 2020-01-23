//
// case.h
//
// CCaseTextService declaration.
//

#ifndef CASE_H
#define CASE_H

class CCaseTextService : public ITfTextInputProcessor,
                         public ITfKeyEventSink
{
public:
    CCaseTextService();
    ~CCaseTextService();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // ITfTextInputProcessor
    STDMETHODIMP Activate(ITfThreadMgr *pThreadMgr, TfClientId tfClientId);
    STDMETHODIMP Deactivate();

    // ITfKeyEventSink
    STDMETHODIMP OnSetFocus(BOOL fForeground);
    STDMETHODIMP OnTestKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnTestKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnPreservedKey(ITfContext *pContext, REFGUID rguid, BOOL *pfEaten);

    // CClassFactory factory callback
    static HRESULT CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj);

    // server registration
    static BOOL RegisterProfiles();
    static void UnregisterProfiles();
    static BOOL RegisterCategories(BOOL fRegister);
    static BOOL RegisterServer();
    static void UnregisterServer();

    ITfThreadMgr *_GetThreadMgr() { return _pThreadMgr; }
    TfClientId _GetClientId() { return _tfClientId; }
    BOOL _IsKeyFlipping() { return _fFlipKeys; }

private:
    // init methods
    BOOL _InitKeystrokeSink();
    BOOL _InitPreservedKey();

    // uninit methods
    void _UninitKeystrokeSink();
    void _UninitPreservedKey();

	HRESULT StartEditSession(ITfContext *pContext, WPARAM wParam);

    //
    // state
    //

    ITfThreadMgr *_pThreadMgr;
    TfClientId _tfClientId;

    BOOL _fFlipKeys;

    LONG _cRef;     // COM ref count
};


#endif // CASE_H
