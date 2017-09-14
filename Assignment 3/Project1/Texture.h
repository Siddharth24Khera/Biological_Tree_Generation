# pragma once

#include "Dependencies\freeglut\freeglut.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;

class Texture {
public:
	GLuint loadBMP_custom(const char * imagepath);
};



