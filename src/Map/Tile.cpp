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

#include "Tile.h"

MapTile::MapTile(){}

MapTile::MapTile(uint16_t tileId, float north, float east, float south, float west){
	_tileId = tileId;
	_north = north;
	_east = east;
	_south = south;
	_west = west;
}

//Check if the given point is contained in the tile
bool MapTile::contains(float latitude, float longitude){
	return (latitude < _north && latitude > _south
			&& longitude < _east && longitude > _west);
}

void MapTile::print(){
	Serial.print(_north, 5);
	Serial.print("|");
	Serial.print(_east, 5);
	Serial.print("|");
	Serial.print(_south, 5);
	Serial.print("|");
	Serial.print(_west, 5);
	Serial.print("|");
	Serial.println(_tileId);
}
