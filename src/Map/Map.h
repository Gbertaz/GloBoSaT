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

#ifndef __MAP__
#define __MAP__

#include <Arduino.h>
#include "../Map/Tile.h"
#include "../Config.h"

class GlobosatMap{

public:

	struct displaySize {
		uint16_t width;
		uint16_t height;
	};

	struct point{
		float x;
		float y;
	};

	struct referencePoint{
		int displayX;
        int displaY;
        float latitude;
        float longitude;
        point globalXY;
	};

	GlobosatMap();
	void init(displaySize size);
	void update(float latitude, float longitude);
	void onMarkerPositionChange(void (*callback)(float x, float y, uint16_t tileId)) {
		cb_onMarkerPositionChange = callback;
	}
	MapTile tiles[TILES_NUMBER];

private:

	uint16_t _currentTile;
	uint16_t _lastTile;

	referencePoint topLeft;
	referencePoint bottomRight;

	GlobosatMap::point coordinatesToGlobalXY(float latitude, float longitude);
	GlobosatMap::point gpsCoordinatesToScreenCoordinates(float latitude, float longitude);
	uint16_t getCurrentTile(float latitude, float longitude);
	bool isTileIdValid();
	void calibrateTile();
	void (*cb_onMarkerPositionChange)(float x, float y, uint16_t tileId);
};

#endif
