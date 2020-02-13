# range_slider_for_Qt5_and_PyQt5
Customized Qt slider widget with two handles to be convenient to get the range of lower, and higher values.

# Screen shots
range slider opted of the horizontal.
![Image of Horizontal](https://github.com/sleepbysleep/range_slider_for_Qt5_and_PyQt5/horizontal.png)
range slider opted of the vertical.
![Image of Vertical](https://github.com/sleepbysleep/range_slider_for_Qt5_and_PyQt5/vertical.png)

# Usage
In c++,
```
#include "range_slider.h"

void echo(int low_value, int high_value)
{
    std::cout << low_value << ", " << high_value << "\n";
}
...
    RangeSlider slider(Qt::Horizontal);
    //RangeSlider slider(Qt::Vertical);
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
```

In Python,
```
import range_slider

def echo(low_value, high_value):
    print(low_value, high_value)

def main(argv):
    app = QtWidgets.QApplication(sys.argv)
    slider = range_slider.RangeSlider(QtCore.Qt.Horizontal)
    slider.setMinimumHeight(30)
    slider.setMinimum(0)
    slider.setMaximum(255)
    slider.setLow(15)
    slider.setHigh(35)
    slider.setTickPosition(QtWidgets.QSlider.TicksBelow)
    slider.sliderMoved.connect(echo)
    #QtCore.QObject.connect(slider, QtCore.SIGNAL('sliderMoved(int)'), echo)
    slider.show()
    slider.raise_()
    app.exec_()
```