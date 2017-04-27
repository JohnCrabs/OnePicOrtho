/* Copyright (C) Dec 2016 John Crabs <kavouras.ioannis@gmail.com>

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

 */

#include <iostream>
#include <cstdio>
#include <cstring>

#include "imgEdit.h"
#include "ortho.h"

void GPL();

int main(int argc, char** argv)
{
	GPL();
	
	//-ortho
	if(argv[1] != '\0' && strcmp(argv[1], "-ortho") == 0) {
		if (argc != 4) {
			printf("\nERROR: WRONG ARGUMENT CALL!\n");
			printf("%s -ortho <imgPath> <outputName>\n\n", argv[0]);
			exit(0);
		}
		
		ortho img(argv[2], argv[3]);
	}
	
	//error
	else {
		printf("Uknown Command!\nCommand Input List:\n");
		printf("%s -ortho <imgPath> <outputName>\n", argv[0]);
	}
	
	return 0;
}

void GPL()
{
	printf("\n\n");
	printf("Copyright (C) Dec 2016 John Crabs <kavouras.ioannis@gmail.com>\n\n");
	printf("This program is free software: you can redistribute it and/or modify\n");
	printf("it under the terms of the GNU General Public License as published by\n");
	printf("the Free Software Foundation, either version 3 of the License, or\n");
	printf("(at your option) any later version.\n\n");
	printf("This program is distributed in the hope that it will be useful,\n");
	printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
	printf("GNU General Public License for more details.\n\n");
	printf("You should have received a copy of the GNU General Public License\n");
	printf("along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n\n");
	
	printf("Press ENTER to continue...");
	char c[100];
	myGetLine(c);
}

