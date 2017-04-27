#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <math.h>

#include "ortho.h"
#include "imgEdit.h"

#define PI 3.141592653589793238462643383279502884197169

ortho::ortho(const char* path, const char* name)
{
	initIMG(path, name);
	runOrtho();
}

ortho::~ortho() 
{
	clearMemory();
}

/******************************************************************************************************/

//Initialize IMG
void ortho::initIMG(const char* path, const char* name)
{
	//Keep the path of the Image
	_imgPath = (char*)malloc((int)strlen(path)*sizeof(char));
	_imgPath[0] = '\0';
	strcpy(_imgPath, path);
	
	//Keep the name of ers file
	_ersFileName = (char*)malloc((int)strlen(name)*sizeof(char));
	_ersFileName[0] = '\0';
	strcat(_ersFileName, name);
	
	_data[0] = '\0';
	strcat(_data, DATA);
	
	//Keep the name of pixel file
	_mainFilePath = (char*)malloc(ARRAY_SIZE*sizeof(char));
	_mainFilePath[0] = '\0';
	strcat(_mainFilePath, _data);
	strcat(_mainFilePath, _ersFileName);
	
	//Create ERS file and read the IMG Size
	GDAL_translateToERS(_imgPath, _ersFileName);
	getImageSize_ERS(_imgHeight, _imgWidth, _imgBands, _ersFileName);
	
	//user connect the procram with an output folder
	_imgOutputFolder = (char* )malloc(ARRAY_SIZE*sizeof(char));
	_imgOutputFolder[0] = '\0';
	strcpy(_imgOutputFolder, OUTPUT_DATA);
	
	//Allocate Memory for pixelTab and Store the pixelValues
	_pixelTab = (unsigned char*)malloc((_imgHeight*_imgWidth*_imgBands)*sizeof(char));
	getPixelValues_IMG(_mainFilePath, _pixelTab, _imgHeight, _imgWidth, _imgBands);
}

//System Core
void ortho::runOrtho()
{
	commandLine();
}

//Command Line
void ortho::commandLine()
{
	bool loop = true;
	while(loop) {
		char dec[100], buf[100];
		choices();
		myGetLine(dec);
		loop = checkChoice(dec);
		
		printf("Press ENTER to continue....");
		myGetLine(buf);
	}
}

//Choices
void ortho::choices()
{
	int i = 0;
	clearScreen();
	printf("%d)Exit program (type: exit)\n", i++);
	printf("%d)Import image parameters (type: importIMGpar)\n", i++);
	printf("%d)Import XYZ *.txt file (type: importXYZ)\n", i++);
	printf("%d)Create ortho with XYZ (type: orthoXYZ)\n", i++);
	printf("\nTYPE: ");
}

//Clear Screen
void ortho::clearScreen()
{
	for(int i = 0; i <= 100; i++)
		printf("\n");
}

//Check Choice and Perform the actions
bool ortho::checkChoice(char* choice)
{
	//Exit the program
	if (strcmp(choice, "exit") == 0)
		exit(0);
	
	//Import affine parameters
	else if(strcmp(choice, "importIMGpar") == 0)
		importIMGpar();
	
	//Import XYZ file
	else if(strcmp(choice, "importXYZ") == 0)
		importXYZ();
	
	//Create ortho with XYZ
	else if(strcmp(choice, "orthoXYZ") == 0)
		orthoXYZ();
	
	//Uknown Command
	else
		printf("Uknown Command!\n\n");
	
	return true;
}

/******************************************************************************************************/

//Import XYZ
void ortho::importXYZ()
{
	char path[200];
	printf("Insert the path of XYZ *.txt file:\npath$: ");
	myGetLine(path);
	
	FILE* openFile;
	openFile = fopen(path, "r");
	if(openFile == NULL) {
		printf("\n\nERROR :: FILE CANNOT OPEN!\n\n");
		return ;
	}
	char c;
	_xyz.size = 0;
	
	while ((c = fgetc(openFile)) != EOF) {
		if(c == '\n')
			_xyz.size++;
	}
	fclose(openFile);
	
	_xyz.X = (double*)malloc(_xyz.size*sizeof(double));
	_xyz.Y = (double*)malloc(_xyz.size*sizeof(double));
	_xyz.Z = (double*)malloc(_xyz.size*sizeof(double));
	
	openFile = fopen(path, "r");
	for (int i = 0; i < _xyz.size; i++) {
		readNum(openFile, &_xyz.X[i]);
		readNum(openFile, &_xyz.Y[i]);
		readNum(openFile, &_xyz.Z[i]);
		
		printf("%lf %lf %lf\n", _xyz.X[i], _xyz.Y[i], _xyz.Z[i]);
	}
	fclose(openFile);
	
	printf("\n\nINFO :: XYZ IMPORTED SUCCESFULLY!\n");
}

//Import c, xo, yo, Xo, Yo, Zo, w, f, k
void ortho::importIMGpar()
{
	char path[200];
	printf("Insert the path of IMG parameter *.txt file:\npath$: ");
	myGetLine(path);
	
	FILE* openFile;
	openFile = fopen(path, "r");
	if(openFile == NULL) {
		printf("\n\nERROR :: FILE CANNOT OPEN!\n\n");
		return ;
	}
	
	_IMGpar.xo = (_imgWidth / 2.0); //xo
	_IMGpar.yo = (_imgHeight / 2.0); //yo
	readNum(openFile, &_IMGpar.c); //c
	
	readNum(openFile, &_IMGpar.Xo); //Xo
	readNum(openFile, &_IMGpar.Yo); //Yo
	readNum(openFile, &_IMGpar.Zo); //Zo
	
	readNum(openFile, &_IMGpar.w); //w
	readNum(openFile, &_IMGpar.f); //f
	readNum(openFile, &_IMGpar.k); //k
	
	fclose(openFile);
	
	printf("\n\n********************");
	printf("\n* IMAGE PARAMETERS *\n");
	printf("********************\n\n");
	printf("c = %lf\nxo = %lf\nyo = %lf\n\n", _IMGpar.c, _IMGpar.xo, _IMGpar.yo);
	printf("Xo = %lf\nYo = %lf\nZo = %lf\n\n", _IMGpar.Xo, _IMGpar.Yo, _IMGpar.Zo);
	printf("w = %lf\nf = %lf\nk = %lf\n\n", _IMGpar.w, _IMGpar.f, _IMGpar.k);
	
	_IMGpar.w *= (PI/180.0);
	_IMGpar.f *= (PI/180.0);
	_IMGpar.k *= (PI/180.0);
	
	R.a11 = cos(_IMGpar.f) * cos(_IMGpar.k); //a11 = cosf*cosk
	R.a12 = cos(_IMGpar.w) * sin(_IMGpar.k) + sin(_IMGpar.w) * sin(_IMGpar.f) * cos(_IMGpar.k); //a12 = cosw*sink + sinw*sinf*cosk 
	R.a13 = sin(_IMGpar.w) * sin(_IMGpar.k) - cos(_IMGpar.w) * sin(_IMGpar.f) * cos(_IMGpar.k); //a13 = sinw*sink - cosw*sinf*cosk
	
	R.a21 = (-cos(_IMGpar.f) * sin(_IMGpar.k)); //a21 = -cosf*sink
	R.a22 =  cos(_IMGpar.w) * cos(_IMGpar.k) - sin(_IMGpar.w) * sin(_IMGpar.f) * sin(_IMGpar.k); //a22 = cosw*cosk - sinw*sinf*sink
	R.a23 =  sin(_IMGpar.w) * cos(_IMGpar.k) + cos(_IMGpar.w) * sin(_IMGpar.f) * sin(_IMGpar.k); //a23 = sinw*cosk + cosw*sinf*sink
	
	R.a31 = sin(_IMGpar.f); //a31 = sinf
	R.a32 = (-sin(_IMGpar.w) * cos(_IMGpar.f)); //a32 = -sinw*cosf
	R.a33 = cos(_IMGpar.w) * cos(_IMGpar.f); //a33 = cosw*cosf
	
	printf("\n\n");
	printf("-----------\n");
	printf("| TABLE R |\n");
	printf("-----------\n\n");
	
	printf("R11 = %lf\n", R.a11);
	printf("R12 = %lf\n", R.a12);
	printf("R13 = %lf\n", R.a13);
	
	printf("R21 = %lf\n", R.a21);
	printf("R22 = %lf\n", R.a22);
	printf("R23 = %lf\n", R.a23);
	
	printf("R31 = %lf\n", R.a31);
	printf("R32 = %lf\n", R.a32);
	printf("R33 = %lf\n\n", R.a33);
	
}

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

//Create ortho with XYZ file
void ortho::orthoXYZ()
{
	double X_min;
	double X_max;
	
	double Y_min;
	double Y_max;
	
	double newPixelSize;
	
	char buff[20];
	printf("Insert DEM's step:\nStep = ");
	myGetLine(buff);
	step = atof(buff);
	
	printf("Insert X_min (m): ");
	myGetLine(buff);
	X_min = atof(buff);
	
	printf("Insert X_max (m): ");
	myGetLine(buff);
	X_max = atof(buff);
	
	printf("Insert Y_min (m): ");
	myGetLine(buff);
	Y_min = atof(buff);
	
	printf("Insert Y_max (m): ");
	myGetLine(buff);
	Y_max = atof(buff);
	
	printf("Insert newPixelSize (m): ");
	myGetLine(buff);
	newPixelSize = atof(buff);
	
	int newWidth;
	int newHeight;
	
	/*************************************************************************/
	
	//Create New Output File
	char  outName[ARRAY_SIZE];
	outName[0] = '\0';
	strcat(outName,_ersFileName);
	strcat(outName,"_ORTHO_XYZ");
	//Create New Path File
	char* pathNew;
	pathNew = (char*)malloc(ARRAY_SIZE*sizeof(char));
	pathNew[0] = '\0';
	strcat(pathNew, _data);
	strcat(pathNew, outName);
	
	//Create Ortho
	short int pixelValue;
	double pX, pY, pZ;
	double x_xo, y_yo;
	int j, i;
	
	newWidth = 0;
	newHeight = 0;
	
	FILE* outFile = fopen(pathNew, "w");
	for(double y = Y_max; y >= Y_min; y-=newPixelSize) {
		for(int bands = 0; bands < _imgBands; bands++) {
			for(double x = X_min; x <= X_max; x+=newPixelSize) {
				pX = x;
				pY = y;
				linearInterpolationXYZ(pX, pY, &pZ);
				collinearity(pX, pY, pZ, &x_xo, &y_yo);
				j = x_xo + _IMGpar.xo;
				i = y_yo + _IMGpar.yo;
				
				pixelValue = (short int)_pixelTab[((i*_imgBands) + bands)*_imgWidth + j];
				fprintf(outFile, "%c", (char)pixelValue);
				
				if(newHeight == 0)
					newWidth++;
			}
			newHeight++;
		}
	}
	fclose(outFile);
	
	
	//create the new ERS file
	createNewFile_ERS(outName, _mainFilePath, _data, _ersFileName, newHeight, newWidth, _imgBands);
	
	char format[10];
	//ask the user for the output image type
	printf("\n\nSave image format as (PNG, JPEG, GTIFF etc):\nFORMAT: ");
	myGetLine(format);
	//create new name
	lowerCase(format);
	
	char* newImageName;
	newImageName = (char*)malloc(ARRAY_SIZE*sizeof(char));
	newImageName[0] = '\0';
	strcat(newImageName, _imgOutputFolder);
	//strcat(newImageName, _ersFileName);
	strcat(newImageName, outName);
	strcat(newImageName, ".");
	strcat(newImageName, format);
	
	//create the path of new ers
	strcat(pathNew, ".ers");
	
	//save image
	upperCase(format);
	GDAL_translateERStoIMG(pathNew, newImageName, format);
	
	free(pathNew);
	free(newImageName);
}

//Find point's Z using linear interpolation
void ortho::linearInterpolationXYZ(double X, double Y, double *Z)
{
	double Sdist, Sz;
	Sdist = 0;
	Sz = 0;
	
	for (int i = 0; i < _xyz.size; i++) {
		if( ( _xyz.X[i] > (X - step/2) && _xyz.X[i] < (X + step/2) )  &&  ( _xyz.Y[i] > (Y - step/2) && _xyz.Y[i] < (Y + step/2) ) ) {
			Sdist += (_xyz.X[i] - X)*(_xyz.X[i] - X) + (_xyz.Y[i] - Y)*(_xyz.Y[i] - Y);
			Sz += _xyz.Z[i] * ((_xyz.X[i] - X)*(_xyz.X[i] - X) + (_xyz.Y[i] - Y)*(_xyz.Y[i] - Y));
		}
	}
	
	*Z = Sz/Sdist;
	
}

//Collinearity condition
void ortho::collinearity(double X, double Y, double Z, double *x, double *y)
{
	double A1;
	double A2;
	double A3;
	
	A1 = (X - _IMGpar.Xo)*R.a11 + (Y - _IMGpar.Yo)*R.a12 + (Z - _IMGpar.Zo)*R.a13;
	A2 = (X - _IMGpar.Xo)*R.a21 + (Y - _IMGpar.Yo)*R.a22 + (Z - _IMGpar.Zo)*R.a23;
	A3 = (X - _IMGpar.Xo)*R.a31 + (Y - _IMGpar.Yo)*R.a32 + (Z - _IMGpar.Zo)*R.a33;
	
	*x = (-_IMGpar.c) * (A1/A3);
	*y = (-_IMGpar.c) * (A2/A3);
	
}

/******************************************************************************************************/

void ortho::clearMemory()
{
	free(_imgPath);
	free(_ersFileName);
	free(_imgOutputFolder);
	
	free(_mainFilePath);
	
	free(_pixelTab);
	
	free(_xyz.X);
	free(_xyz.Y);
	free(_xyz.Z);
}

