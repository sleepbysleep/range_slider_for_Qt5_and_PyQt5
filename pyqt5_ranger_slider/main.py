#!/usr/bin/python3
# -*- coding: utf-8 -*-
import sys, os
from PyQt5 import QtCore, QtGui, QtWidgets
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

if __name__ == "__main__":
    main(sys.argv)

