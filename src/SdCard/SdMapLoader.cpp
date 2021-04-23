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

#include "SdMapLoader.h"

MapLoader::MapLoader(){

}

void MapLoader::load(char *filepath, MapTile *tiles){
	if(_sdCard.begin(SdioConfig(FIFO_SDIO)) == false) return;
	if(_sdCard.exists(filepath) == false) return;
	if(_file.open(filepath, O_READ) == false) return;
	_isFileHeader = true;
	_endOfFile = false;
	_tileCounter = 0;

	while (!_endOfFile){
		readLine(tiles);
	}
	
	/*
	for(int i = 0; i < _tileCounter; i++){
		tiles[i].print();
	}
	*/
}

void MapLoader::readLine(MapTile *tiles){
	
	char textBuffer[255];
	char delimiter[] = "\n";

	//Reads one line
	int len = _file.fgets(textBuffer, sizeof(textBuffer), delimiter);

	if(len > 0){
		
		//Skips the file header
		if(_isFileHeader){
			_isFileHeader = false;
			readLine(tiles);
			return;
		}

		parseLine(textBuffer, tiles);
	}
	else if(len == 0){
		_file.close();
		_endOfFile = true;
	}
}

void MapLoader::parseLine(char *buffer, MapTile *tiles){
	char *p = buffer;
	char *str;
	uint8_t valueIndex = 0;
	float north = 0;
	float east = 0;
	float south = 0;
	float west = 0;
	int tileid = 0;

	while ((str = strtok_r(p, MAP_DATA_DELIMITER, &p)) != NULL){

		switch(valueIndex){
			case 0:
				north = atof(str);
			break;
			case 1:
				east = atof(str);
			break;
			case 2:
				south = atof(str);
			break;
			case 3:
				west = atof(str);
			break;
			case 4:
				tileid = atof(str);
			break;
		}

		valueIndex++;
	}

	tiles[_tileCounter] = MapTile(tileid, north, east, south, west);
	_tileCounter++;
}