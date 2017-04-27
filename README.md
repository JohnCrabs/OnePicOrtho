## Synopsis

A tool to create an orthophotography from one image.

## Code Example

//readNumber
void ortho::readNum(FILE* file, double* val)
{
	char c;
	char buf[50];
	int i = 0;
	while(!isdigit(c = fgetc(file)) && c != '+' && c != '-' && c != '.')
		if(c == EOF)
			return ;
	buf[i++] = c;
	while(isdigit(c = fgetc(file)) || c == '+' || c == '-' || c == '.')
		buf[i++] = c;
	buf[i] = '\0';
	
	//printf("Debug: %s", buf);
	
	*val = atof(buf);
}

## Motivation

This software creates an orthophotography using only one image and a DEM in format XYZ. 

## Installation

1)Open a terminal in the folder with the makefile.
2)Run the command make.

## Contributors

Currently only me.

## License
Copyright (C) Dec 2016 John Crabs <kavouras.ioannis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.




