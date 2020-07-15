#include "qui/core/qui_paint_manager.h"

#include "qui/core/qui_control.h"
#include "qui_math_p.h"
#include <Windows.h>

QuiPaintManager::QuiPaintManager()
    : paint_wnd_(nullptr)
    , wnd_canvas_(nullptr)
    , mem_canvas_(nullptr)
    , mem_bitmap_(nullptr)
    , root_(nullptr)
    , first_layout_(true)
    , focus_needed_(false)
    , update_needed_(false)
    , painting_(false)
{

    min_window_size_ = { 0 };
    max_window_size_ = { 0 };
    init_window_size_ = { 0 };
    window_size_box_ = { 0 };
    window_caption_area_ = { 0 };
    last_mouse_pos_.x = last_mouse_pos_.y = -1;
}
QuiPaintManager::~QuiPaintManager()
{
    if (wnd_canvas_) {
        delete wnd_canvas_;
    }
    if (mem_canvas_) {
        delete mem_canvas_;
    }
    if (mem_bitmap_) {
        delete mem_bitmap_;
    }
}

void QuiPaintManager::Init(QuiWindowHandle hwnd, const wchar_t * manager_name/* = nullptr*/)
{
    HWND native_handle = (HWND)hwnd;
    QuiASSERT(::IsWindow(native_handle));

    name_.Empty();
    if (manager_name) {
        name_ = manager_name;
    }

    if (paint_wnd_ != hwnd) {
        paint_wnd_ = hwnd;
        // create canvas
        wnd_canvas_ = QuiCanvas::MakeCanvasFromHWND(hwnd);
    }
}
void QuiPaintManager::Invalidate()
{
    if (true) {
        ::InvalidateRect((HWND)paint_wnd_, nullptr, FALSE);
    }
}
void QuiPaintManager::Invalidate(const QuiRect& area)
{
    RECT item_area = { area.left, area.top, area.right, area.bottom };
    if (item_area.left <= 0) item_area.left = 0;
    if (item_area.top <= 0) item_area.top = 0;
    if (item_area.right <= item_area.left) item_area.right = item_area.left;
    if (item_area.bottom <= item_area.top) item_area.bottom = item_area.top;
    if (true) {
        ::InvalidateRect((HWND)paint_wnd_, &item_area, FALSE);
    }
}

bool QuiPaintManager::AttachDialog(QuiControl * control)
{
    HWND hwnd = (HWND)paint_wnd_;
    QuiASSERT(::IsWindow(hwnd));
    // Reset any previous attachment

    // Remove the existing control-tree
    if (root_) {
        AddDelayedCleanup(root_);
    }
    // Set the dialog root element
    root_ = control;
    // Go ahead ...
    update_needed_ = true;
    first_layout_ = true;
    focus_needed_ = true;
    // Init all controls
    return InitControls(control);
}
bool QuiPaintManager::InitControls(QuiControl * control, QuiControl * parent/* = nullptr*/)
{
    QuiASSERT(control);
    if (!control) return false;
    control->SetManager(this, parent ? parent : control->GetParent(), true);
    QuiControlNameHashFilter filter(this);
    control->FindControl(filter, QUIFIND_ALL);
    return true;
}
void QuiPaintManager::ReapObjects(QuiControl * control)
{
    if (!control) return;
    const QuiString& name = control->GetName();
    if (!name.IsEmpty()) {
        if (control == FindControl(name)) {
            control_name_hash_.Remove(name);
        }
    }
}

void QuiPaintManager::AddDelayedCleanup(QuiControl * control)
{
    if (!control) return;
    control->SetManager(this, nullptr, false);
    // TODO: cleanup
}

QuiControl* QuiPaintManager::FindControl(const wchar_t * name) const
{
    QuiASSERT(root_);
    return static_cast<QuiControl*>(control_name_hash_.Find(name));
}

class AutoSetLResult {
public:
    AutoSetLResult(QuiWindowLResult& l_res)
        : l_res_(l_res)
    {
        res_ = (LRESULT)l_res_;
    }
    ~AutoSetLResult()
    {
        l_res_ = (QuiWindowLResult)res_;
    }
    LRESULT& GetLResult()
    {
        return res_;
    }

private:
    QuiWindowLResult& l_res_;
    LRESULT res_;
};
bool QuiPaintManager::MessageHandler(QuiWindowMessageID message, QuiWindowWParam w_param, QuiWindowLParam l_param, QuiWindowLResult& l_res)
{
    if (!paint_wnd_) return false;

    UINT msg = message;
    //WPARAM wparam = (WPARAM)w_param;
    //LPARAM lparam = (LPARAM)l_param;
    AutoSetLResult auto_set_ret(l_res);
    LRESULT& res = auto_set_ret.GetLResult();
    res = res;
    HWND hwnd = (HWND)paint_wnd_;

    switch (msg) {
    case WM_PAINT:
    {
        if (!root_) {
            PAINTSTRUCT ps = { 0 };
            ::BeginPaint(hwnd, &ps);
            //CRenderEngine::DrawColor(m_hDcPaint, ps.rcPaint, 0xFF000000);
            ::EndPaint(hwnd, &ps);
            return true;
        }
        RECT client_area = { 0 };
        ::GetClientRect(hwnd, &client_area);

        RECT paint_area = { 0 };
        if (false/*layered*/) {

        }
        else {
            if (!::GetUpdateRect(hwnd, &paint_area, FALSE))
                return true;
        }

        // TODO: Set focus to first control?


        SetPainting(true);
        // layout
        if (update_needed_) {
            update_needed_ = false;
            QuiRect root_area = Win32RECTToQuiRect(client_area);
            root_->SetPos(root_area);
        }
        // render screen
        // prepare offscreen
        // Begin Windows paint
        PAINTSTRUCT ps = { 0 };
        ::BeginPaint(hwnd, &ps);
        if (true/*offscreen*/) {
            QuiRect offscreen_area = Win32RECTToQuiRect(client_area);
            if (mem_bitmap_ && (!(offscreen_area.Width() == mem_bitmap_->GetWidth() && offscreen_area.Height() == mem_bitmap_->GetHeight()))) {
                delete mem_bitmap_;
                mem_bitmap_ = nullptr;
                delete mem_canvas_;
                mem_canvas_ = nullptr;
            }
            if (!mem_bitmap_) {
                mem_bitmap_ = QuiBitmap::MakeGDIBitmap(offscreen_area.Width(), offscreen_area.Height(), kRGB8888_BitmapColor, paint_wnd_);
                mem_canvas_ = QuiCanvas::MakeRaster(mem_bitmap_);
            }
            QuiASSERT(mem_canvas_);
            mem_bitmap_->BeginDraw();
            QuiRect dirty_area = { paint_area.left, paint_area.top, paint_area.right, paint_area.bottom };
            root_->Paint(*mem_canvas_, dirty_area);

            QuiRect src = { 0, 0, offscreen_area.Width(), offscreen_area.Height() };
            QuiRect dest = src;
            wnd_canvas_->DrawBitmap(mem_bitmap_, src, dest);
            mem_bitmap_->EndDraw();
        }
        else {
            // A standard paint job
            
            //int save_dc = ::SaveDC(hdc);
            QuiASSERT(wnd_canvas_);
            QuiRect dirty_area = { paint_area.left, paint_area.top, paint_area.right, paint_area.bottom };
            root_->Paint(*wnd_canvas_, dirty_area);
            //::RestoreDC(hdc, save_dc);
        }
        // All Done!
        ::EndPaint(hwnd, &ps);
        SetPainting(false);
        return true;
    }
    break;
    case WM_SIZE:
    {
        if (root_) {
            root_->NeedUpdate();
        }
    }
    break;
    }
    return false;
}
