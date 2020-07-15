#ifndef QUI_CORE_PAINT_MANAGER_H_
#define QUI_CORE_PAINT_MANAGER_H_

#include "qui/core/qui_prerequisites.h"

#include "qui/utils/qui_string.h"
#include "qui/core/qui_window_types.h"
#include "qui/core/qui_size.h"
#include "qui/core/qui_point.h"
#include "qui/core/qui_rect.h"
#include "qui/core/qui_canvas.h"
#include "qui/core/qui_bitmap.h"

class QuiControl;
class QuiControlNameHashFilter;
class QUI_EXPORT QuiPaintManager {
public:
    QuiPaintManager();
    ~QuiPaintManager();

public:
    void Init(QuiWindowHandle hwnd, const wchar_t * manager_name = nullptr);
    bool IsUpdateNeeded() const { return update_needed_; }
    void NeedUpdate() { update_needed_ = true; }
    void Invalidate();
    void Invalidate(const QuiRect& item_area);

    // 
    bool AttachDialog(QuiControl * control);
    bool InitControls(QuiControl * control, QuiControl * parent = nullptr);
    void ReapObjects(QuiControl * control);

    // 
    bool IsPainting() { return painting_; }
    void SetPainting(bool painting) { painting_ = painting; }

    // 
    void AddDelayedCleanup(QuiControl * control);

    // 
    QuiControl* GetRoot() const { return root_; }
    QuiControl* FindControl(const wchar_t * name) const;

    // 
    bool MessageHandler(QuiWindowMessageID uMsg, QuiWindowWParam w_param, QuiWindowLParam l_param, QuiWindowLResult& l_res);

private:
    QuiString name_;
    QuiWindowHandle paint_wnd_;
    QuiCanvas* wnd_canvas_;
    QuiCanvas* mem_canvas_;
    QuiBitmap* mem_bitmap_;
    //
    QuiControl* root_;
    // 
    QuiPoint last_mouse_pos_;
    QuiSize min_window_size_;
    QuiSize max_window_size_;
    QuiSize init_window_size_;
    QuiRect window_size_box_;
    QuiSize window_round_corner_;
    QuiSize window_caption_area_;
    // 
    bool first_layout_ : 1;
    bool update_needed_ : 1;
    bool focus_needed_ : 1;
    bool painting_ : 1;

    // 
    QuiStringPtrMap control_name_hash_;

private:
    friend class QuiControlNameHashFilter;
};

#endif   // QUI_CORE_PAINT_MANAGER_H_
