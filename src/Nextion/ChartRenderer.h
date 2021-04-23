// MIT License
//
// Copyright(c) 2021 Giovanni Bertazzoni <nottheworstdev@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef ChartRenderer_h
#define ChartRenderer_h

#include "NextionDisplay.h"
#include <stddef.h>
#include <Arduino.h>


class ChartRenderer {

public:

	struct point_g {
		int value;
		int minutes;
	};

	ChartRenderer(NexDisplay &nextInstance, int x, int y, int width, int height);
	void update(point_g point);

private:
	void render();
	void drawAxes();
	void drawValueLine(int xValue, int yValue, int color);
	void drawLine(int x1, int y1, int x2, int y2, int color);
	void fillArea(int x, int y, int width, int height, int color);

	int _x;
	int _y;
	int _width;
	int _height;
	NexDisplay *_nextInstance;
	int _points[1000];
	int _pointIndex;
};

#endif