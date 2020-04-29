#include "dui_real_window.h"
#include "dui_window_priv.h"
#include "dui_real_application.h"

const DuiWindowFlags kFullScreenMask = kFullscreen_Window | kFullscreenDesktop_Window;
const DuiWindowFlags kCreateFlags = kChild_Window | kBorderless_Window | kResizable_Window
    | kAlwaysOnTop_Window | kSkipTaskbar_Window | kPopupMenu_Window
    | kUtility_Window | kTooltip_Window | kLayered_Window;


DuiRealWindow::DuiRealWindow(DuiWindow * owner)
    : owner_(owner)
    , hiding_(false)
    , x_(kUndefined_WindowPos)
    , y_(kUndefined_WindowPos)
    , width_(0)
    , height_(0)
    , min_width_(0)
    , min_height_(0)
    , max_width_(0)
    , max_height_(0)
    , flags_(0)
    , prev_(nullptr)
    , next_(nullptr)
{
    
}
DuiRealWindow::~DuiRealWindow()
{
    if (DuiRealApplication::GetWindows() && DuiRealApplication::GetWindows() == this) {
        if (next_) {
            DuiRealApplication::SetWindows(next_);
        }
        else if (prev_) {
            DuiRealApplication::SetWindows(prev_);
        }
    }

    if (prev_) {
        prev_->next_ = next_;
    }
    if (next_) {
        next_->prev_ = prev_;
    }
    next_ = nullptr;
    prev_ = nullptr;
}

DuiWindowHandle DuiRealWindow::GetHandle()
{
    return OnGetHandle();
}

DuiPoint DuiRealWindow::GetLocation() const
{
    return{ x_, y_ };
}
DuiSize DuiRealWindow::GetSize() const
{
    return{ width_, height_ };
}
DuiSize DuiRealWindow::GetMaximumSize() const
{
    return{ max_width_, max_height_ };
}
DuiSize DuiRealWindow::GetMinimumSize() const
{
    return{ min_width_, min_height_ };
}
void DuiRealWindow::SetLocation(const DuiPoint& pt)
{
    int x = pt.x;
    int y = pt.y;
    if (DuiWindowPos_IsCentered(x) || DuiWindowPos_IsCentered(y)) {
        int displayIndex = (x & 0xFFFF);
        DuiRect bounds = { 0 };
        if (displayIndex > DuiRealApplication::GetNumOfDisplays()) {
            displayIndex = 0;
        }

        DuiRealApplication::GetDisplayBounds(displayIndex, &bounds);
        if (DuiWindowPos_IsCentered(x)) {
            x = bounds.x() + (bounds.width() - width_) / 2;
        }
        if (DuiWindowPos_IsCentered(y)) {
            y = bounds.y() + (bounds.height() - height_) / 2;
        }
    }

    if ((flags_ & kFullscreen_Window)) {
        if (!DuiWindowPos_IsUndefined(x)) {
            windowed_.SetX(x);
        }
        if (!DuiWindowPos_IsUndefined(y)) {
            windowed_.SetY(y);
        }
    }
    else {
        if (!DuiWindowPos_IsUndefined(x)) {
            x_ = x;
        }
        if (!DuiWindowPos_IsUndefined(y)) {
            y_ = y;
        }

        OnSetLocation();
    }
}

DuiWindowHandle DuiRealWindow::Create(const wchar_t *title, int x, int y, int w, int h, DuiWindowFlags flags, DuiWindow * parent)
{
    if (!GetHandle()) {
        /* Some platforms can't create zero-sized windows */
        if (w < 1) {
            w = 1;
        }
        if (h < 1) {
            h = 1;
        }
        /* Some platforms blow up if the windows are too large. Raise it later? */
        if ((w > 16384) || (h > 16384)) {
            return nullptr;
        }


        x_ = x;
        y_ = y;
        width_ = w;
        height_ = h;

        if (DuiWindowPos_IsUndefined(x) || DuiWindowPos_IsUndefined(y) ||
            DuiWindowPos_IsCentered(x) || DuiWindowPos_IsCentered(y)) {
            VideoDisplay *display = DuiRealApplication::GetDisplayForWindow(this);
            int displayIndex;
            DuiRect bounds;

            displayIndex = DuiRealApplication::GetIndexOfDisplay(display);
            DuiRealApplication::GetDisplayBounds(displayIndex, &bounds);
            if (DuiWindowPos_IsUndefined(x) || DuiWindowPos_IsCentered(x)) {
                x_ = bounds.x() + (bounds.width() - w) / 2;
            }
            if (DuiWindowPos_IsUndefined(y) || DuiWindowPos_IsCentered(y)) {
                y_ = bounds.y() + (bounds.height() - h) / 2;
            }
        }

        windowed_.SetXYWH(x_, y_, width_, height_);

        if (flags & kFullscreen_Window) {
            VideoDisplay *display = DuiRealApplication::GetDisplayForWindow(this);
            int displayIndex;
            DuiRect bounds;

            displayIndex = DuiRealApplication::GetIndexOfDisplay(display);
            DuiRealApplication::GetDisplayBounds(displayIndex, &bounds);

            x_ = bounds.x();
            y_ = bounds.y();
            width_ = bounds.width();
            height_ = bounds.height();
        }

        flags_ = ((flags & kCreateFlags) | kHidden_Window);
        last_fullscreen_flags_ = flags_;

        if (!owner_->destroy_by_self_) {
            next_ = DuiRealApplication::GetWindows();
            if (DuiRealApplication::GetWindows()) {
                DuiRealApplication::GetWindows()->prev_ = this;
            }
            DuiRealApplication::SetWindows(this);
        }

        DuiRealWindow * real_parent = parent ? parent->real_window_ : nullptr;
        this->OnCreate(real_parent);

        if (title) {
            SetTitle(title);
        }

        FinishCreation(flags);

        /* If the window was created fullscreen, make sure the mode code matches */
        UpdateFullscreenMode(Fullscreen_Visible(this));
    }

    return GetHandle();
}
DuiWindowHandle DuiRealWindow::CreateFrom(DuiWindowHandle extern_handle)
{
    flags_ = kForeign_Window;
    last_fullscreen_flags_ = flags_;
    destroying_ = false;
    next_ = DuiRealApplication::GetWindows();
    if (DuiRealApplication::GetWindows()) {
        DuiRealApplication::GetWindows()->prev_ = this;
    }
    DuiRealApplication::SetWindows(this);

    return OnCreateFrom(extern_handle);
}
void DuiRealWindow::SetTitle(const wchar_t *title)
{
    if ((!title && title_.empty()) || title == title_) {
        return;
    }
    title_ = title ? title : L"";
    OnSetTitle();
}
void DuiRealWindow::Add(DuiWindow * child)
{
    OnAdd(child->real_window_);
}

void DuiRealWindow::FinishCreation(DuiWindowFlags flags)
{
    if (flags & kMaximized_Window) {
        Maximize();
    }
    if (flags & kMinimized_Window) {
        Minimize();
    }
    if (flags & kFullscreen_Window) {
        SetFullscreen(flags);
    }
    //if (flags & SDL_WINDOW_INPUT_GRABBED) {
    //    SDL_SetWindowGrab(window, SDL_TRUE);
    //}
    if ((flags & kLayered_Window) == kLayered_Window) {
        SetLayered(true);
    }
    if (!(flags & kHidden_Window)) {
        Show();
    }
}

void DuiRealWindow::Show()
{
    if (flags_ & kShown_Window) {
        return;
    }
    OnShow();
}
void DuiRealWindow::Hide()
{
    if (!(flags_ & kShown_Window)) {
        return;
    }
    hiding_ = true;
    UpdateFullscreenMode(false);
    OnHide();
    hiding_ = false;
}
void DuiRealWindow::Raise()
{
    if (!(flags_ & kShown_Window)) {
        return;
    }
    OnRaise();
}
void DuiRealWindow::Maximize()
{
    if ((flags_ & kMaximized_Window)) {
        return;
    }
    OnMaximize();
}
void DuiRealWindow::Minimize()
{
    if ((flags_ & kMinimized_Window)) {
        return;
    }
    UpdateFullscreenMode(false);
    OnMinimize();
}
void DuiRealWindow::Restore()
{
    if (!(flags_ & (kMaximized_Window | kMinimized_Window))) {
        return;
    }
    OnRestore();
}
bool DuiRealWindow::SetFullscreen(DuiWindowFlags flags)
{
    DuiWindowFlags oldflags;
    flags &= kFullScreenMask;

    if (flags == (flags_ & kFullScreenMask)) {
        return true;
    }

    /* clear the previous flags and OR in the new ones */
    oldflags = flags_ & kFullScreenMask;
    flags_ &= ~kFullScreenMask;
    flags_ |= flags;

    if (UpdateFullscreenMode(Fullscreen_Visible(this))) {
        return true;
    }

    flags_ &= ~kFullScreenMask;
    flags_ |= oldflags;

    return false;
}

bool DuiRealWindow::UpdateFullscreenMode(bool fullscreen)
{
    VideoDisplay *display;
    DuiRealWindow *other;

    /* if we are in the process of hiding don't go back to fullscreen */
    if (hiding_ && fullscreen) {
        return true;
    }

    display = DuiRealApplication::GetDisplayForWindow(this);

    if (fullscreen) {
        /* Hide any other fullscreen windows */
        if (DuiRealApplication::GetFullscreenWindow(display) &&
            DuiRealApplication::GetFullscreenWindow(display) != this) {
            DuiRealApplication::GetFullscreenWindow(display)->Minimize();
        }
    }

    /* See if anything needs to be done now */
    if ((DuiRealApplication::GetFullscreenWindow(display) == this) == fullscreen) {
        if ((last_fullscreen_flags_ & kFullScreenMask) == (flags_ & kFullScreenMask)) {
            return true;
        }
    }

    /* See if there are any fullscreen windows */
    for (other = DuiRealApplication::GetWindows(); other; other = other->next_) {
        bool display_mode_set = false;

        if (other == this) {
            display_mode_set = fullscreen;
        }
        else if (Fullscreen_Visible(other) &&
            DuiRealApplication::GetDisplayForWindow(other) == display) {
            display_mode_set = true;
        }

        if (display_mode_set) {
            DisplayMode fullscreen_mode;

            if (DuiRealApplication::GetWindowDisplayMode(other, &fullscreen_mode)) {
                bool resized = true;

                if (other->width_ == fullscreen_mode.w && other->height_ == fullscreen_mode.h) {
                    resized = false;
                }

                /* only do the mode change if we want exclusive fullscreen */
                if ((flags_ & kFullscreenDesktop_Window) != kFullscreenDesktop_Window) {
                    if (!DuiRealApplication::SetDisplayModeForDisplay(display, &fullscreen_mode)) {
                        return false;
                    }
                }
                else {
                    if (!DuiRealApplication::SetDisplayModeForDisplay(display, nullptr)) {
                        return false;
                    }
                }


                other->SetFullscreen(display, true);
                
                display->fullscreen_window = other;

                /* Generate a mode change event here */
                if (resized) {
                    DuiWindowEvent evt;
                    DuiWindowEventResult result;
                    evt.type = kResized_WindowEvent;
                    evt.size.set(fullscreen_mode.w, fullscreen_mode.h);
                    other->HandleEvent(&evt, result);
                }
                else {
                    other->OnResized();
                }

                //SDL_RestoreMousePosition(other);

                last_fullscreen_flags_ = flags_;
                return true;
            }
        }
    }

    /* Nope, restore the desktop mode */
    DuiRealApplication::SetDisplayModeForDisplay(display, nullptr);

    SetFullscreen(display, false);
    
    display->fullscreen_window = NULL;

    /* Generate a mode change event here */
    OnResized();

    /* Restore the cursor position */
    //SDL_RestoreMousePosition(window);

    last_fullscreen_flags_ = flags_;
    return true;
}

bool DuiRealWindow::HandleEvent(const DuiWindowEvent * evt, DuiWindowEventResult& result)
{
    switch (evt->type) {
    case kShown_WindowEvent:
        if (flags_ & kShown_Window) {
            result = 0;
            return true;
        }
        flags_ &= ~kHidden_Window;
        flags_ |= kShown_Window;
        OnShown();
        break;
    case kHidden_WindowEvent:
        if (!(flags_ & kShown_Window)) {
            result = 0;
            return true;
        }
        flags_ &= ~kShown_Window;
        flags_ |= kHidden_Window;
        OnHidden();
    case kMoved_WindowEvent:
        if (DuiWindowPos_IsUndefined(evt->location.x) ||
            DuiWindowPos_IsUndefined(evt->location.y)) {
            result = 0;
            return true;
        }
        if (!(flags_ & kFullscreen_Window)) {
            windowed_.left = evt->location.x;
            windowed_.top = evt->location.y;
        }
        if (evt->location.x == x_ && evt->location.y == y_) {
            result = 0;
            return true;
        }
        x_ = evt->location.x;
        y_ = evt->location.y;
        break;
    case kResized_WindowEvent:
    {
        int data1 = evt->size.width;
        int data2 = evt->size.height;
        if (!(flags_ & kFullscreen_Window)) {
            windowed_.right = windowed_.left + data1;
            windowed_.bottom = windowed_.top + data2;
        }
        if (data1 == width_ && data2 == height_) {
            result = 0;
            return true;
        }
        width_ = data1;
        height_ = data2;
        OnResized();
        break;
    }
    case kMinimized_WindowEvent:
        if (flags_ & kMinimized_WindowEvent) {
            return 0;
        }
        flags_ &= ~kMaximized_Window;
        flags_ |= kMinimized_WindowEvent;
        OnMinimized();
        break;
    case kMaximized_WindowEvent:
        if (flags_ & kMaximized_Window) {
            result = 0;
            return true;
        }
        flags_ &= ~kMinimized_Window;
        flags_ |= kMaximized_Window;
        break;
    case kRestored_WindowEvent:
        if (!(flags_ & (kMinimized_Window | kMaximized_Window))) {
            result = 0;
            return true;
        }
        flags_ &= ~(kMinimized_Window | kMaximized_Window);
        OnRestored();
        break;
    }

    if (owner_) {
        return owner_->HandleEvent(evt, result);
    }
    
    return false;
}

void DuiRealWindow::OnShown()
{
    OnRestored();
}
void DuiRealWindow::OnHidden()
{
    UpdateFullscreenMode(false);
}
void DuiRealWindow::OnResized()
{
    Invalidate();
}
void DuiRealWindow::OnMinimized()
{
    UpdateFullscreenMode(false);
}
void DuiRealWindow::OnRestored()
{
    if (Fullscreen_Visible(this)) {
        UpdateFullscreenMode(true);
    }
}

int DuiRealWindow::SetTooltipWidth(int width)
{
    return OnSetTooltipWidth(width);
}
void DuiRealWindow::ShowTooltip()
{
    OnShowTooltip();
}
void DuiRealWindow::HideTooltip()
{
    OnHideTooltip();
}
void DuiRealWindow::MoveTooltip(const DuiPoint& loc)
{
    OnMoveTooltip(loc);
}
int DuiRealWindow::GetTooltipTime()
{
    return OnGetTooltipTime();
}
