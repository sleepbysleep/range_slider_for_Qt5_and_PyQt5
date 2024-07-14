/** Copyright Hoyoung Lee (2024-07-14).
 **
 ** hoyoung.yi@gmail.com
 **
 ** This software is a computer program whose purpose is to provide
 ** "Qt widget of range slider with two handles".
 **
 ** This software is governed by the CeCILL-A license under French law
 ** and abiding by the rules of distribution of free software. You can
 ** use, modify and/or redistribute the software under the terms of the
 ** CeCILL-A license as circulated by CEA, CNRS and INRIA at the following
 ** URL: "http://www.cecill.info".
 **
 ** As a counterpart to the access to the source code and rights to copy,
 ** modify and redistribute granted by the license, users are provided
 ** only with a limited warranty and the software's author, the holder of
 ** the economic rights, and the successive licensors have only limited
 ** liability.
 **
 ** In this respect, the user's attention is drawn to the risks associated
 ** with loading, using, modifying and/or developing or reproducing the
 ** software by the user in light of its specific status of free software,
 ** that may mean that it is complicated to manipulate, and that also
 ** therefore means that it is reserved for developers and experienced
 ** professionals having in-depth computer knowledge. Users are therefore
 ** encouraged to load and test the software's suitability as regards
 ** their requirements in conditions enabling the security of their
 ** systems and/or data to be ensured and, more generally, to use and
 ** operate it in the same conditions as regards security.
 **
 ** The fact that you are presently reading this means that you have had
 ** knowledge of the CeCILL-A license and that you accept its terms.
 */

#pragma once

#include <qslider.h>
#include <qstyle.h>
#include <QMouseEvent>
#include <qpainter.h>
#include <qstyleoption.h>
#include <qpalette.h>
#include <qapplication.h>

/*
 * Originated from
 * https://www.mail-archive.com/pyqt@riverbankcomputing.com/msg22889.html
 * Modification refered from
 * https://gist.github.com/Riateche/27e36977f7d5ea72cf4f
 */
class RangeSlider : public QSlider {
    Q_OBJECT
public:
    RangeSlider(Qt::Orientation ot, QWidget *parent=nullptr)
        : QSlider(ot) {
        this->lowLimit = this->minimum();
        this->highLimit = this->maximum();
        this->pressed_control = QStyle::SC_None;
        this->tick_interval = 0;
        this->tick_position = QSlider::NoTicks;
        this->hover_control = QStyle::SC_None;
        this->click_offset = 0;
        this->active_slider = 0;
    }
    int low() { return this->lowLimit; }
    void setLow(int low_limit) { this->lowLimit = low_limit; }
    int high() { return this->highLimit; }
    void setHigh(int high_limit) { this->highLimit = high_limit; }
signals:
    void sliderMoved(int, int);

private:

protected:
    // based on http://qt.gitorious.org/qt/qt/blobs/master/src/gui/widgets/qslider.cpp
    void paintEvent(QPaintEvent *ev) override {
        QPainter painter(this);
        QStyleOptionSlider opt;

        // Draw groove
        this->initStyleOption(&opt);
        opt.sliderValue = 0;
        opt.sliderPosition = 0;
        opt.subControls = QStyle::SC_SliderGroove;
        if (this->tickPosition() != NoTicks) opt.subControls |= QStyle::SC_SliderTickmarks;
        this->style()->drawComplexControl(QStyle::CC_Slider, &opt, &painter, this);
        QRect groove = this->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);

        // Draw span
        this->initStyleOption(&opt);
        opt.subControls = QStyle::SC_SliderGroove;
        opt.sliderValue = 0;
        opt.sliderPosition = this->lowLimit;
        QRect low_rect = this->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
        opt.sliderPosition = this->highLimit;
        QRect high_rect = this->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

        int low_pos = this->pick(low_rect.center());
        int high_pos = this->pick(high_rect.center());
        int min_pos = std::min(low_pos, high_pos);
        int max_pos = std::max(low_pos, high_pos);

        QPoint c = QRect(low_rect.center(), high_rect.center()).center();
        QRect span_rect;
        if (opt.orientation == Qt::Horizontal) {
            span_rect = QRect(QPoint(min_pos, c.y()-2), QPoint(max_pos, c.y()+1));
            groove.adjust(0,0,-1,0);
        } else {
            span_rect = QRect(QPoint(c.x()-2, min_pos), QPoint(c.x()+1, max_pos));
            groove.adjust(0,0,0,-1);
        }

        if (1) {
            QColor highlight = this->palette().color(QPalette::Highlight);
            painter.setBrush(QBrush(highlight));
            painter.setPen(QPen(highlight,0));
            painter.drawRect(span_rect.intersected(groove));
            //.color(QPalette.)
        }

        // FIXED: use self.style(), not QApplication.style() by MaurizioB
//        QStyle* style = QApplication::style();
        QStyle* style = this->style();
        this->initStyleOption(&opt);
        opt.subControls = QStyle::SC_SliderHandle; // | QStyle::SC_SliderGroove;
        if (this->tickPosition() != QSlider::NoTicks) opt.subControls |= QStyle::SC_SliderTickmarks;
        if (this->pressed_control) opt.activeSubControls = this->pressed_control;
        else opt.activeSubControls = this->hover_control;
        opt.sliderPosition = this->lowLimit;
        opt.sliderValue = this->lowLimit;
        style->drawComplexControl(QStyle::CC_Slider, &opt, &painter, this);

        this->initStyleOption(&opt);
        opt.subControls = QStyle::SC_SliderHandle;
        if (this->tickPosition() != QSlider::NoTicks) opt.subControls |= QStyle::SC_SliderTickmarks;
        if (this->pressed_control) opt.activeSubControls = this->pressed_control;
        else opt.activeSubControls = this->hover_control;
        opt.sliderPosition = this->highLimit;
        opt.sliderValue = this->highLimit;
        style->drawComplexControl(QStyle::CC_Slider, &opt, &painter, this);
    }

    void mousePressEvent(QMouseEvent *ev) override {
        /*  # In a normal slider control, when the user clicks on a point in the
            # slider's total range, but not on the slider part of the control the
            # control would jump the slider value to where the user clicked.
            # For this control, clicks which are not direct hits will slide both
            # slider parts
            */
        if (ev->button() == Qt::LeftButton) {
            ev->accept();
            QStyleOptionSlider opt;
            this->initStyleOption(&opt);
            this->active_slider = -1;

            QStyle* style = QApplication::style();
            QStyle::SubControl hit;

            opt.sliderPosition = this->lowLimit;
            hit = style->hitTestComplexControl(QStyle::CC_Slider, &opt, ev->pos(), this);
            if (hit == QStyle::SC_SliderHandle) {
                this->active_slider = 0;
                this->pressed_control = hit;
                this->triggerAction(this->SliderMove);
                this->setRepeatAction(this->SliderNoAction);
                this->setSliderDown(true);
            } else {
                opt.sliderPosition = this->highLimit;
                hit = style->hitTestComplexControl(QStyle::CC_Slider, &opt, ev->pos(), this);
                if (hit == QStyle::SC_SliderHandle) {
                    this->active_slider = 1;
                    this->pressed_control = hit;
                    this->triggerAction(this->SliderMove);
                    this->setRepeatAction(this->SliderNoAction);
                    this->setSliderDown(true);
                }
            }

            if (this->active_slider < 0) {
                this->pressed_control = QStyle::SC_SliderHandle;
                this->click_offset = this->pixelPosToRangeValue(this->pick(ev->pos()));
                this->triggerAction(this->SliderMove);
                this->setRepeatAction(this->SliderNoAction);
            }
        } else {
            ev->ignore();
        }

        QSlider::mousePressEvent(ev);
    }
    //void mouseReleaseEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override {
        if (this->pressed_control != QStyle::SC_SliderHandle) {
            ev->ignore();
            return;
        }

        ev->accept();
        int new_pos = this->pixelPosToRangeValue(this->pick(ev->pos()));
        QStyleOptionSlider opt;
        this->initStyleOption(&opt);

        int offset, diff;
        if (this->active_slider < 0) {
            offset = new_pos - this->click_offset;
            this->highLimit += offset;
            this->lowLimit += offset;
            if (this->lowLimit < this->minimum()) {
                diff = this->minimum() - this->lowLimit;
                this->lowLimit += diff;
                this->highLimit += diff;
            }
            if (this->highLimit > this->maximum()) {
                diff = this->maximum() - this->highLimit;
                this->lowLimit += diff;
                this->highLimit += diff;
            }
        } else if (this->active_slider == 0) {
            if (new_pos >= this->highLimit)
                new_pos = this->highLimit - 1;
            this->lowLimit = new_pos;
        } else {
            if (new_pos <= this->lowLimit)
                new_pos = this->lowLimit + 1;
            this->highLimit = new_pos;
        }

        this->click_offset = new_pos;
        this->update();
        emit this->sliderMoved(this->lowLimit, this->highLimit);
    }

    int lowLimit, highLimit;
    inline int pick(QPoint const& pt) { return this->orientation() == Qt::Horizontal ? pt.x() : pt.y(); }
    int pixelPosToRangeValue(int pos) {
        QStyleOptionSlider opt;
        this->initStyleOption(&opt);
        QStyle* style = QApplication::style();

        QRect gr = style->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
        QRect sr = style->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

        int slider_length, slider_min, slider_max;
        if (this->orientation() == Qt::Horizontal) {
            slider_length = sr.width();
            slider_min = gr.x();
            slider_max = gr.right() - slider_length + 1;
        } else {
            slider_length = sr.height();
            slider_min = gr.y();
            slider_max = gr.bottom() - slider_length + 1;
        }

        return style->sliderValueFromPosition(this->minimum(), this->maximum(), pos-slider_min, slider_max-slider_min, opt.upsideDown);
    }
    QStyle::SubControl pressed_control = QStyle::SC_None;
    int tick_interval = 0;
    QSlider::TickPosition tick_position = QSlider::NoTicks;
    QStyle::SubControl hover_control = QStyle::SC_None;
    int click_offset = 0;
    int active_slider = 0;
};
