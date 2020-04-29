#include "dui/dui.h"

#include <Windows.h>

class ChildWindow : public DuiWindow {
public:
protected:
    bool OnHandleEvent(const DuiWindowEvent * evt, DuiWindowEventResult& result) override
    {
        if (evt->type == kPaint_WindowEvent || evt->type == kPrintClient_WindowEvent) {
            DuiCanvas * canvas = evt->canvas;
            DuiSize sz = this->GetSize();
            canvas->DrawColor({ 0, 0, sz.width, sz.height }, DuiColorSetARGB(0xff, 0, 0xff, 0));
            canvas->DrawRect({ 100, 100, 200, 200 }, 5, DUI_ColorBLUE, kDot_PenStyle);
            canvas->DrawLine({ 0, 0 }, { 100, 100 }, 10, DUI_ColorDKGRAY, kSolid_PenStyle);
            return true;
        }
        return false;
    }
};

class FrameWindow : public DuiWindow, public DuiEditClient, public DuiTooltipClient {
public:
    FrameWindow()
        : child_window_(nullptr)
        , edit_(nullptr)
        , tooltip_(nullptr)
    {
        bk_image_ = DuiBitmap::MakeBitmap(300, 300, DUI_ColorDKGRAY);
    }
    ~FrameWindow()
    {
        if (child_window_) {
            delete child_window_;
            child_window_ = nullptr;
        }
        if (bk_image_) {
            delete bk_image_;
            bk_image_ = nullptr;
        }
    }

public:
    DuiWindowFlags GetWindowFlags() 
    { 
        return 0; 
    }
    DuiEditStyle GetEditStyle() 
    { 
        return kLeft_EditStyle; 
    }
    DuiWindow* GetHostWindow() 
    { 
        return this; 
    }
    void* GetNativeFont() 
    { 
        return nullptr; 
    }
    uint32_t GetMaxChar() const 
    { 
        return 200; 
    }
    wchar_t GetPasswordChar() const 
    { 
        return L'*'; 
    }
    bool IsEnabled() const 
    { 
        return true; 
    }
    bool IsReadOnly() const 
    { 
        return false; 
    }
    bool IsAutoSelAll() const 
    { 
        return false; 
    }
    //DuiColor GetNativeEditBkColor() const
    //{
    //    return DUI_ColorDKGRAY;
    //}
    //DuiColor GetTextColor() const
    //{
    //    return DUI_ColorBLUE;
    //}
    void OnEditChanged(const std::wstring& text)
    {

    }

public:
    bool NeedShowTooltip(const DuiPoint& pt)
    {
        DuiRect tooltip_area = { 200, 200, 400, 400 };
        if (tooltip_area.Contains(pt)) {
            return true;
        }
        else {
            return false;
        }
    }

protected:
    bool OnHandleEvent(const DuiWindowEvent * evt, DuiWindowEventResult& result) override
    {
        if (evt->type == kCreate_WindowEvent) {
            /*this->SetLocation({ 10, 10 });
            this->SetMinimumSize({ 300, 200 });
            this->SetMaximumSize({ 1000, 700 });*/
            child_window_ = new ChildWindow();
            child_window_->Create(0, 0, 0, 100, 100, kChild_Window | kShown_Window, this);
            edit_ = new DuiEditWindow();
            edit_->Init(this);
            Add(child_window_);
            return true;
        }
        if (evt->type == kErasebkgnd_WindowEvent) {
            DuiCanvas * canvas = evt->canvas;
            DuiSize sz = this->GetSize();
            //canvas->DrawColor({ 0, 0, sz.width, sz.height }, DUI_ColorWHITE/*DuiColorSetARGB(0xaf, 0xff, 0, 0)*/);
            return true;
        }
        if (evt->type == kPaint_WindowEvent) {
            DuiCanvas * canvas = evt->canvas;
            DuiSize sz = this->GetSize();
            DuiRect repaint_area = evt->paint_area;
            canvas->Save();
            canvas->Clip(repaint_area);
            canvas->DrawColor({ 0, 0, sz.width, sz.height }, DuiColorSetARGB(0xff, 0, 0xff, 0));
            canvas->DrawColor({ 0, 0, sz.width, sz.height }, DuiColorSetARGB(0x8f, 0xff, 0, 0));
            DuiRect dest_area = { 100, 100, 300, 300 };
            canvas->DrawBitmap(bk_image_, dest_area);
            canvas->Restore();
            //canvas->DrawRect({ 100, 100, 200, 200 }, 5, DUI_ColorBLUE, kDot_PenStyle);
            //canvas->DrawLine({ 0, 0 }, { 100, 100 }, 10, DUI_ColorDKGRAY, kSolid_PenStyle);
            return true;
        }
        if (evt->type == kFlush_WindowEvent) {
            DuiCanvas * canvas = evt->canvas;
            DuiSize sz = this->GetSize();
            //canvas->DrawColor({ 0, 0, sz.width, sz.height }, DUI_ColorWHITE);
            //canvas->DrawRect({ 100, 100, 200, 200 }, 5, DUI_ColorBLUE, kDot_PenStyle);
            //canvas->DrawLine({ 0, 0 }, { 100, 100 }, 10, DUI_ColorDKGRAY, kSolid_PenStyle);
            return true;
        }
        if (evt->type == kClose_WindowEvent) {
            if (child_window_) {
                delete child_window_;
                child_window_ = nullptr;
            }
            return false;
        }
        if (evt->type == kMouseHover_WindowEvent) {
            if (tooltip_) {
                if (NeedShowTooltip(evt->location)) {
                    tooltip_->ShowTooltip();
                    tooltip_->MoveTooltip(evt->location);
                }
            }
        }
        if (evt->type == kMouseMove_WindowEvent) {
            if (!tooltip_) {
                tooltip_ = new DuiTooltipWindow();
                tooltip_->Init(this);
                this->SetHoverTime(tooltip_->GetTooltipTime());
                tooltip_->SetTooltipWindowWidth(200);
            }
            if (tooltip_) {
                if (NeedShowTooltip(evt->location)) {
                    tooltip_->ShowTooltip();
                    tooltip_->MoveTooltip(evt->location);
                }
                else {
                    tooltip_->HideTooltip();
                }
            }
            return true;
        }
        if (evt->type == kMouseLeave_WindowEvent) {
            if (tooltip_) {
                tooltip_->HideTooltip();
            }
            return true;
        }
        return false;
    }
    void OnDestoryed() override
    {
        delete this;
    }

private:
    ChildWindow * child_window_;
    DuiBitmap * bk_image_;

    DuiEditWindow * edit_;
    DuiTooltipWindow * tooltip_;
};

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE /*prev_instance*/, LPSTR /*cmd_line*/, int cmd_show)
{
    DuiApplication::Init();

    FrameWindow* frame_window = new FrameWindow();
    if (!frame_window) {
        return 0;
    }
    DuiWindowFlags window_flags = kResizable_Window /*| kLayered_Window*/;
    frame_window->Create(L"TestAppFrameWindow", kCentered_WindowPos, kCentered_WindowPos, 800, 600, window_flags);
    //pFrame->CenterWindow();
    //pFrame->ShowWindow(true);
    DuiApplication::Run();

    
    return 0;
}
