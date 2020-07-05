#include "qui/core/qui_control.h"

#include "qui/core/qui_paint_manager.h"

#include <Windows.h>

QuiControl* QuiControlNameHashFilter::OnFindControl(QuiControl *control)
{
    const QuiString& name = control->GetName();
    if (name.IsEmpty()) return nullptr;
    // Add this control to the hash list
    manager_->control_name_hash_.Set(name, control);
    return nullptr; // Attempt to add all controls
}

QuiControl::QuiControl()
    : paint_manager_(nullptr)
    , parent_(nullptr)
    , update_needed_(false)
    , menu_used_(false)
    , async_notify_(false)
    , visible_(true)
    , intern_visible_(true)
    , focused_(false)
    , enabled_(true)
    , mouse_enabled_(true)
    , keyboard_enabled_(true)
    , float_(false)
    , set_pos_(false)
    , background_color_(QUI_ColorTRANSPARENT)
    , background_color2_(QUI_ColorTRANSPARENT)
    , background_color3_(QUI_ColorTRANSPARENT)
    , border_color_(QUI_ColorTRANSPARENT)
    , border_focus_color_(QUI_ColorTRANSPARENT)
    , tooltip_width_(300)
{
    fixed_size_ = { 0 };
    min_size_ = { 0 };
    max_size_ = { 9999, 9999 };

    padding_ = { 0 };
    item_area_ = { 0 };
    paint_area_ = { 0 };
    border_size_ = { 0 };
}
QuiControl::~QuiControl()
{

}

const wchar_t * QuiControl::GetClass() const
{
    return QUI_CONTROL_CONTROL;
}
void * QuiControl::GetInterface(const wchar_t * interface_name)
{
    if (wcscmp(interface_name, QUI_CONTROL_CONTROL) == 0) {
        return this;
    }
    return nullptr;
}

void QuiControl::SetManager(QuiPaintManager * manager, QuiControl * parent, bool init/* = true*/)
{
    paint_manager_ = manager;
    parent_ = parent;
    if (init && parent_) {
        Init();
    }
}

void QuiControl::SetBkColor(QuiColor bkcolor)
{
    if (bkcolor == background_color_) return;

    background_color_ = bkcolor;
    Invalidate();
}

void QuiControl::SetPos(const QuiRect& area, bool need_invalidate/* = true*/)
{
    QuiRect invalidate_area = item_area_;
    if (false/*float*/) {

    }
    else {
        item_area_ = area;
    }

    if (!paint_manager_) return;

    if (!set_pos_) {
        set_pos_ = true;
        // fire onsize event;
        set_pos_ = false;
    }

    update_needed_ = false;

    if (need_invalidate && IsVisible()) {
        // TODO: area join
        paint_manager_->Invalidate(invalidate_area);
    }
}

QuiControl * QuiControl::FindControl(QuiControlFilter& filter, QuiFindFlagType flags)
{
    if ((flags & QUIFIND_VISIBLE) != 0 && !IsVisible()) return nullptr;
    if ((flags & QUIFIND_ENABLED) != 0 && !IsEnabled()) return nullptr;
    if ((flags & QUIFIND_HITTEST) != 0 && !filter.FindControl(this)) return nullptr;
    if ((flags & QUIFIND_UPDATETEST) != 0 && !filter.FindControl(this)) return nullptr;

    QuiControl * ret = nullptr;
    if ((flags & QUIFIND_ME_FIRST) != 0) {
        if ((flags & QUIFIND_HITTEST) == 0 || IsMouseEnabled()) {
            ret = filter.FindControl(this);
        }
    }
    if (!ret && (flags & QUIFIND_ME_FIRST) == 0) {
        if ((flags & QUIFIND_HITTEST) == 0 || IsMouseEnabled()) {
            ret = filter.FindControl(this);
        }
    }
    return nullptr;
}

void QuiControl::Invalidate()
{
    if (!IsVisible()) return;

    // TODO: improve

    if (paint_manager_) {
        paint_manager_->Invalidate();
    }
}
void QuiControl::NeedUpdate()
{
    if (!IsVisible()) return;
    update_needed_ = true;
    Invalidate();

    if (paint_manager_) {
        paint_manager_->NeedUpdate();
    }
}

void QuiControl::Init()
{
    DoInit();
}
void QuiControl::DoInit()
{

}

bool QuiControl::Paint(QuiCanvas& canvas, const QuiRect& paint_area, QuiControl * stop_control/* = nullptr*/)
{
    // 
    if (false/*has round corner*/) {

    }
    else {
        PaintBkColor(canvas);
    }
    return false;
}
void QuiControl::PaintBkColor(QuiCanvas& canvas)
{
    canvas.DrawColor(item_area_, background_color_);
}
