/*
 This project is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Deviation is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Deviation.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OVERRIDE_PLACEMENT
#include "common.h"
#include "../pages.h"

enum {
    NAME_X = 0,
    NAME_W = 60,
    SAVE_X = LCD_WIDTH - 60,
    SAVE_W = 60,
    UNDERLINE = 1,
    LABEL_X = 0,
    LABEL_W = 74,
    LABEL1_W = 39,
    LABEL2_W = 50,
    TEXTSEL1_X = 39,
    TEXTSEL1_W = 35,
    TEXTSEL2_X = 50,
    TEXTSEL2_W = 24,
    VALUE_X = 0,
    #define VALUE_Y_OFFSET LINE_SPACE
    GRAPH_X = 77,
    #define GRAPH_Y LINE_HEIGHT
    GRAPH_W = 50,
    GRAPH_H = 50,
};
#endif //OVERRIDE_PLACEMENT
#include "../../common/advanced/_mixer_curves.c"

static unsigned action_cb(u32 button, unsigned flags, void *data);

void PAGE_EditCurvesInit(int page)
{
    (void)page;
    struct Curve *curve = edit->curveptr;
    u8 type = CURVE_TYPE(curve);
    PAGE_SetActionCB(action_cb);
    edit->pointnum = 0;
    edit->reverse = MIXER_SRC_IS_INV(pagemem.u.mixer_page.cur_mixer->src);
    if ((type == CURVE_EXPO || type == CURVE_DEADBAND)
        && curve->points[0] == curve->points[1])
    {
        edit->pointnum = -1;
    }
    edit->curve = *curve;

    // GUI_CreateTextSelectPlate(&gui->name, NAME_X, 0, NAME_W, HEADER_HEIGHT, &TEXTSEL_FONT, NULL, set_curvename_cb, NULL);
    GUI_CreateLabelBox(&gui->title, NAME_X, 0 , NAME_W, HEADER_HEIGHT, &LABEL_FONT, GUI_Localize, NULL, _tr(CURVE_GetName(tempstring, curve)));
    GUI_CreateButtonPlateText(&gui->save, SAVE_X, 0, SAVE_W, HEADER_WIDGET_HEIGHT, &BUTTON_FONT , GUI_Localize, okcancel_cb, _tr_noop("Save"));
    // Draw a line
    if (UNDERLINE)
        GUI_CreateRect(&gui->rect, 0, HEADER_WIDGET_HEIGHT, LCD_WIDTH, 1, &DEFAULT_FONT);

    u8 space = LINE_SPACE;
    u8 y = space;

    if (type >= CURVE_3POINT) {
        GUI_CreateLabelBox(&gui->smoothlbl, LABEL_X, y, LABEL1_W, LINE_HEIGHT, &LABEL_FONT, GUI_Localize, NULL, _tr_noop("Smooth"));
        GUI_CreateTextSelectPlate(&gui->smooth, TEXTSEL1_X, y, TEXTSEL1_W, LINE_HEIGHT, &TEXTSEL_FONT, NULL, set_smooth_cb, NULL);
        y += space;
        GUI_CreateLabelBox(&gui->pointlbl, LABEL_X, y , LABEL2_W, LINE_HEIGHT, &LABEL_FONT, GUI_Localize, NULL, _tr_noop("Point"));
        GUI_CreateTextSelectPlate(&gui->point, TEXTSEL2_X, y, TEXTSEL2_W, LINE_HEIGHT, &TEXTSEL_FONT, NULL, set_pointnum_cb, NULL);
    } else if(type == CURVE_DEADBAND || type == CURVE_EXPO) {
        GUI_CreateLabelBox(&gui->pointlbl, LABEL_X, y , LABEL_W, LINE_HEIGHT, &LABEL_FONT, GUI_Localize, NULL, _tr_noop("Pos/Neg"));
        y += space;
        GUI_CreateTextSelectPlate(&gui->point, LABEL_X, y, LABEL_W, LINE_HEIGHT, &TEXTSEL_FONT, NULL, set_expopoint_cb, NULL);
    }

    y += space;
    GUI_CreateLabelBox(&gui->valuelbl, LABEL_X, y , LABEL_W, LINE_HEIGHT, &LABEL_FONT, GUI_Localize, NULL, _tr_noop("Value"));
    y += VALUE_Y_OFFSET;
    GUI_CreateTextSelectPlate(&gui->value, VALUE_X, y, LABEL_W, LINE_HEIGHT, &TEXTSEL_FONT, NULL, set_value_cb, NULL);

    GUI_CreateXYGraph(&gui->graph, GRAPH_X, GRAPH_Y, GRAPH_W, GRAPH_H,
                              CHAN_MIN_VALUE, CHAN_MIN_VALUE,
                              CHAN_MAX_VALUE, CHAN_MAX_VALUE,
                              0, 0, //CHAN_MAX_VALUE / 4, CHAN_MAX_VALUE / 4,
                              show_curve_cb, NULL, touch_cb, &edit->curve);
    GUI_SetSelected((guiObject_t *)&gui->value);
}

static unsigned action_cb(u32 button, unsigned flags, void *data)
{
    (void)data;
    if ((flags & BUTTON_PRESS) || (flags & BUTTON_LONGPRESS) || (flags & BUTTON_RELEASE)) {
        if (CHAN_ButtonIsPressed(button, BUT_EXIT)) {
            if (flags & BUTTON_RELEASE) 
                PAGE_Pop();
        } else if (CHAN_ButtonIsPressed(button, BUT_ENTER) && (flags & BUTTON_LONGPRESS)) {
            // long press enter = save without exiting
            if (edit->pointnum < 0)
                edit->curve.points[1] = edit->curve.points[0];
            *edit->curveptr = edit->curve;
            struct mixer_page * const mp = &pagemem.u.mixer_page;
            PAGE_SaveMixerSetup(mp);
        }
        else {
            // only one callback can handle a button press, so we don't handle BUT_ENTER here, let it handled by press cb
            return 0;
        }
    }
    return 1;
}
