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

#include "Map.h"

const static int RADIUS = 6371;     //Earth Radius in Km

GlobosatMap::GlobosatMap(){
	_currentTile = 0;
	_lastTile = 65535;
	cb_onMarkerPositionChange = NULL;
}

void GlobosatMap::update(float latitude, float longitude){

	//Gets the map tile where the currents Gps coordinates are included
	_currentTile = getCurrentTile(latitude, longitude);
	if(!isTileIdValid()) return;

	if(_currentTile != _lastTile){
		_lastTile = _currentTile;
		calibrateTile();
	}
	
	point pos = gpsCoordinatesToScreenCoordinates(latitude, longitude);
	if(cb_onMarkerPositionChange)(*cb_onMarkerPositionChange)(pos.x, pos.y, _currentTile);

	#ifdef DEBUG_MAP
		Serial.print("MAP: Tile ");
		Serial.print(_currentTile);
		Serial.print(" - Screen coordinates (X,Y):  ");
		Serial.print((int)pos.x);
		Serial.print(",");
		Serial.println((int)pos.y);
	#endif
}

//Calculates the global X,Y coordinates of top left and bottom right corners of the tile
void GlobosatMap::calibrateTile(){
	//Display and Gps coordinates at the top left corner of the tile image
	topLeft.displayX = 0;
	topLeft.displaY = 0;
	topLeft.latitude = tiles[_currentTile].north();
	topLeft.longitude = tiles[_currentTile].west();
	
	//Display and Gps coordinates at the bottom right corner of the tile image
	bottomRight.displayX = TILE_WIDTH;
	bottomRight.displaY = TILE_HEIGHT;
	bottomRight.latitude = tiles[_currentTile].south();
	bottomRight.longitude = tiles[_currentTile].east();

	//Global X and Y coordinates for top left reference point and bottom right reference point
	topLeft.globalXY = coordinatesToGlobalXY(topLeft.latitude, topLeft.longitude);
	bottomRight.globalXY = coordinatesToGlobalXY(bottomRight.latitude, bottomRight.longitude);
}

//Converts Gps coordinates to GLOBAL X and Y positions
GlobosatMap::point GlobosatMap::coordinatesToGlobalXY(float latitude, float longitude){
	point p;

	//Calculates x based on cos of average of the latitudes
	p.x = RADIUS * longitude * cos((topLeft.latitude + bottomRight.latitude) / 2);

	//Calculates y based on latitude
	p.y = RADIUS * latitude;

	return p;
}

//Converts Gps coordinates to SCREEN X and Y positions
GlobosatMap::point GlobosatMap::gpsCoordinatesToScreenCoordinates(float latitude, float longitude){
	//Calculate global X and Y for projection point
	point pos = coordinatesToGlobalXY(latitude, longitude);

	//Calculate the percentage of Global X position in relation to total global width
	float x = (pos.x - topLeft.globalXY.x) / (bottomRight.globalXY.x - topLeft.globalXY.x);

	//Calculate the percentage of Global Y position in relation to total global height
	float y = (pos.y - topLeft.globalXY.y) / (bottomRight.globalXY.y - topLeft.globalXY.y);

	//Returns the screen position based on reference points
	point p;
	p.x = topLeft.displayX + (bottomRight.displayX - topLeft.displayX) * x;
	p.y = topLeft.displaY + (bottomRight.displaY - topLeft.displaY) * y;
	return p;
}

bool GlobosatMap::isTileIdValid(){
	return (_currentTile >= 0 && _currentTile < (TILES_NUMBER -1));
}

uint16_t GlobosatMap::getCurrentTile(float latitude, float longitude){
	//This can be improved by scanning only the nearby tiles
	for(int i = 0; i < TILES_NUMBER; i++){
		if(tiles[i].contains(latitude, longitude)) return tiles[i].tileId();
	}
	return 0;
}