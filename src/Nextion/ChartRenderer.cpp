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

#include "ChartRenderer.h"

#define COLOR_VALUE 31		//Blue

ChartRenderer::ChartRenderer(NexDisplay &nextInstance, int x, int y, int width, int height) {
	_x = x;
	_y = y;
	_width = width;
	_height = height;
	_nextInstance = &nextInstance;
	_pointIndex = 0;
	//drawAxes();
}


void ChartRenderer::update(point_g point) {
	//height * point.value / y axe max value
	//int result = ((point.value + 5) / 5) * 5;

	//Y max value
	int yMax = 200;

	int pointHeight = (int)(_height * point.value / yMax);
	_points[_pointIndex] = pointHeight;
	_pointIndex++;

	drawValueLine(_pointIndex, pointHeight, COLOR_VALUE);
}


void ChartRenderer::drawValueLine(int xValue, int yValue, int color) {

	drawLine(_x + 1 + xValue, _height + _y - 1, _x + xValue, _height + _y - 1 - yValue, color);
	fillArea(_x + 1 + xValue - 1, _height + _y - 1 - yValue - 1, 1, 1, 65535);
}

void ChartRenderer::render() {
	
}


void ChartRenderer::drawAxes() {

	//X
	drawLine(_x, _y, _x, _y + _height, 65535);
	//Y
	drawLine(_x, _y + _height, _x + _width, _y + _height, 65535);
}


void ChartRenderer::drawLine(int x1, int y1, int x2, int y2, int color) {
	char data[27];
	sprintf(data, "line %d,%d,%d,%d,%d", x1, y1, x2, y2, color);
	_nextInstance->sendCommand(data);
}

void ChartRenderer::fillArea(int x, int y, int width, int height, int color) {
	char data[27];
	sprintf(data, "fill %d,%d,%d,%d,%d", x, y, width, height, color);
	_nextInstance->sendCommand(data);
}