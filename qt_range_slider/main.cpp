#include "dialog.h"
#include "range_slider.h"

#include <QApplication>
#include <iostream>

void echo(int low_value, int high_value)
{
    std::cout << low_value << ", " << high_value << "\n";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Dialog w;
    //w.show();
    //RangeSlider slider(Qt::Horizontal);
    RangeSlider slider(Qt::Vertical);
    slider.setMinimumHeight(30);
    slider.setMinimum(0);
    slider.setMaximum(255);
    slider.setLow(15);
    slider.setHigh(35);
    slider.setTickPosition(QSlider::TicksBelow);
    QObject::connect(&slider, &RangeSlider::sliderMoved, echo);
    //slider.connect(slider.sliderMoved, echo);
    slider.show();
    slider.raise();

    return a.exec();
}
