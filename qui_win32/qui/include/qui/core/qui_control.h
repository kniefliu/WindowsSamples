#ifndef QUI_CORE_CONTROL_H_
#define QUI_CORE_CONTROL_H_

#include "qui/core/qui_prerequisites.h"

#include "qui/utils/qui_string.h"
#include "qui/core/qui_rect.h"
#include "qui/core/qui_size.h"
#include "qui/core/qui_point.h"
#include "qui/core/qui_color.h"
#include "qui/core/qui_define.h"
#include "qui/core/qui_canvas.h"

// Flags for FindControl()
#define QUIFIND_ALL           0x00000000
#define QUIFIND_VISIBLE       0x00000001
#define QUIFIND_ENABLED       0x00000002
#define QUIFIND_HITTEST       0x00000004
#define QUIFIND_UPDATETEST    0x00000008
#define QUIFIND_TOP_FIRST     0x00000010
#define QUIFIND_ME_FIRST      0x80000000
typedef uint32_t QuiFindFlagType;

class QuiControl;
class QuiPaintManager;
class QUI_EXPORT QuiControlFilter {
public:
    virtual ~QuiControlFilter() {}
    QuiControl* FindControl(QuiControl *find_start)
    {
        return this->OnFindControl(find_start);
    }
protected:
    virtual QuiControl* OnFindControl(QuiControl *find_start) = 0;
};

class QUI_EXPORT QuiControlNameHashFilter : public QuiControlFilter {
public:
    QuiControlNameHashFilter(QuiPaintManager * manager)
        : manager_(manager)
    {

    }

protected:
    QuiControl* OnFindControl(QuiControl *find_start) override;

private:
    QuiPaintManager * manager_;
};


class QUI_EXPORT QuiControl {
public:
    QuiControl();
    virtual ~QuiControl();

public:
    const QuiString& GetName() const { return name_; }
    //QuiString GetName() const { return name_; }
    void SetName(const wchar_t * name) { name_ = name; }
    virtual const wchar_t * GetClass() const;
    virtual void * GetInterface(const wchar_t * interface_name);

    // 
    QuiPaintManager * GetManager() const { return paint_manager_; }
    virtual void SetManager(QuiPaintManager * manager, QuiControl * parent, bool init = true);
    QuiControl * GetParent() const { return parent_; }

    // 
    QuiColor GetBkColor() const { return background_color_; }
    void SetBkColor(QuiColor bkcolor);

    // 
    virtual void SetPos(const QuiRect& rc, bool need_invalidate = true);

    // some attributes
    bool IsVisible() const { return visible_ && intern_visible_; }
    void SetVisible(bool visible = true) { visible_ = visible; }
    void SetInternVisible(bool visible = true) { intern_visible_ = visible; }
    bool IsEnabled() const { return enabled_; }
    void SetEnabled(bool enable = true) { enabled_ = enable; }
    bool IsMouseEnabled() const { return mouse_enabled_; }

    // 
    virtual QuiControl * FindControl(QuiControlFilter& filter, QuiFindFlagType flags);

    //
    void Invalidate();
    bool IsUpdateNeeded() const { return update_needed_; }
    void NeedUpdate();

    void Init();
    void DoInit();

    bool Paint(QuiCanvas& canvas, const QuiRect& paint_area, QuiControl * stop_control = nullptr);
    virtual void PaintBkColor(QuiCanvas& canvas);

private:
    QuiPaintManager* paint_manager_;
    QuiControl* parent_;
    QuiString name_;
    QuiRect item_area_; 
    QuiRect paint_area_;
    QuiRect padding_;
    QuiRect border_size_;
    QuiSize fixed_size_;
    QuiSize min_size_;
    QuiSize max_size_;
    QuiString text_;
    QuiString tooltip_;
    int tooltip_width_;
    QuiString user_data_;
    QuiColor background_color_;
    QuiColor background_color2_;
    QuiColor background_color3_;
    QuiColor border_color_;
    QuiColor border_focus_color_;
    bool update_needed_ : 1;
    bool menu_used_ : 1;
    bool async_notify_ : 1;
    bool visible_ : 1;
    bool intern_visible_ : 1;
    bool enabled_ : 1;
    bool mouse_enabled_ : 1;
    bool keyboard_enabled_ : 1;
    bool focused_;
    bool float_;
    bool set_pos_; // layouting

};

#endif   // QUI_CORE_CONTROL_H_
