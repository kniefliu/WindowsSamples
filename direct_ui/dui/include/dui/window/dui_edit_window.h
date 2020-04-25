#ifndef DUI_WINDOW_EDIT_WINDOW_H_
#define DUI_WINDOW_EDIT_WINDOW_H_

#include "dui/core/dui_window.h"

#include <string>

enum DuiEditStyle {
    kLeft_EditStyle         = 0x00000000,
    kCenter_EditStyle       = 0x00000001,
    kRight_EditStyle        = 0x00000002,
    kMultiline_EditStyle    = 0x00000004,
    kUppercase_EditStyle    = 0x00000008,
    kLowercase_EditStyle    = 0x00000010,
    kPassword_EditStyle     = 0x00000020,
    kAutoVscroll_EditStyle  = 0x00000040,
    kAutoHscroll_EditStyle  = 0x00000080,
    kNoHideSel_EditStyle    = 0x00000100,
    kOemConvert_EditStyle   = 0x00000400,
    kReadonly_EditStyle     = 0x00000800,
    kWantReturn_EditStyle   = 0x00001000,
    kNumber_EditStyle       = 0x00002000,
};

class DUI_API DuiEditClient {
public:
    virtual DuiWindowFlags GetWindowFlags() { return 0; }
    virtual DuiEditStyle GetEditStyle() { return kLeft_EditStyle; }
    virtual DuiWindow* GetHostWindow() { return nullptr; }
    virtual void* GetNativeFont() { return nullptr; }
    virtual uint32_t GetMaxChar() const { return 200; }
    virtual wchar_t GetPasswordChar() const { return L'*'; }
    virtual bool IsEnabled() const { return true; }
    virtual bool IsReadOnly() const { return false; }
    virtual bool IsAutoSelAll() const { return false; }
    virtual DuiColor GetNativeEditBkColor() const { return DUI_ColorWHITE; }
    virtual DuiColor GetTextColor() const { return DUI_ColorBLACK; }
    virtual void OnEditChanged(const std::wstring& text) { }
};

class DUI_API DuiEditWindow : public DuiWindow {
public:
    explicit DuiEditWindow(bool destroy_by_self = true);

    void Init(DuiEditClient * client);
    DuiEditClient * GetClient();
    
protected:
    DuiWindowHandle Create(const wchar_t *title, int x, int y, int w, int h,
        DuiWindowFlags flags, DuiWindow * parent = nullptr);
    DuiWindowHandle CreateFrom(DuiWindowHandle extern_handle) { return nullptr; }

protected:
    void OnDestoryed() override;

private:
    DuiRealWindow * OnCreate() override;

private:
    DuiEditClient * client_;
};

#endif//DUI_WINDOW_EDIT_WINDOW_H_
