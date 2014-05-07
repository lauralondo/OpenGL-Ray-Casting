/* raycast.c
 *
 * Laura Londo
 * CS350 Graphics
 * HW #9
 * 6 May 2014
 *
 * Ray casting of a shaded hemishpere with one colored, diffuse light.
 * recieves a light position and color from the user as well as which side of
 * the sphere to render and the radius of the sphere.
 */


#include <GL/glut.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define screenWidth 700.0	//initial screem width
#define screenHeight 700.0	//initial screen height
#define orthoXMin -2.0      //minimum orthographic x dimention
#define orthoXMax  2.0      //maximum orthographic x dimention
#define orthoYMin -2.0      //minimum orthographic y dimention
#define orthoYMax  2.0      //maximum orthographic y dimention


//circle coordinates
float circleX = 0;
float circleY = 0;
float circleZ = 0;


float circleR = 1.5;            //circle radius
int hemSide = 0;                //which side of the hemiphere (top/bottom)
float lightX, lightY, lightZ;   //light position
float lightR, lightG, lightB;   //light color values
float kd = 2.0;



// gets the parameters from the command line or inputs default values if
// incorrect number of arguments are provided
void getParameters(int argc, char *argv[]) {
	if (argc != 9) { //if incorrect number fo arguments provided,
		printf("\ninsufficient command-line arguments.\n\n Please provide:\n");
		printf("   radius,\n");
		printf("   0 or 1 for top or bottom hemisphere,\n");
		printf("   x position of the light,\n");
		printf("   y position of the light,\n");
		printf("   z position of the light,\n");
		printf("   red color value of the light,\n");
		printf("   green color value of the light,\n");
		printf("   blue color value of the light\n\n");
		printf("Now using default values:  1.3  1  0.6  1.0  2.2  0.5  1  0\n\n");

		//set default values
		circleR = 1.3;
		hemSide = 1;
		lightX = 0.6;
		lightY = 1.0;
		lightZ = 2.2;
		lightR = 0.5;
		lightG = 1;
		lightB = 0;
	}
	else {  //save arguments to the correct parameters
		circleR = atof(argv[1]); //radius
		hemSide = atof(argv[2]); //side of the sphere
		lightX = atof(argv[3]);  //x light position
		lightY = atof(argv[4]);  //y light position
		lightZ = atof(argv[5]);  //z light position
		lightR = atof(argv[6]);  //light red value
		lightG = atof(argv[7]);  //light green value
		lightB = atof(argv[8]);  //light blue value
	}
} // end getParameters



// display callback. Uses ray casting to render a shaded hemisphere
void display(void) {

	for(int winY=0; winY < screenHeight; winY++) {   //each row in the window
		for(int winX=0; winX < screenWidth; winX++) {//each column in the window

			//equation of the eye
			float eyeX = 0;
			float eyeY = 0;
			float eyeZ = 2;

			//find mapping from window to viewing plane
			float viewX = ((float)winX/screenWidth)*4.0 - 2.0;
			float viewY = ((float)winY/screenHeight)*4.0 - 2.0;
			float viewZ = 1.0;

			// <EV>  vector of the viewing plane to eye
			float evX = viewX - eyeX;
			float evY = viewY - eyeY;
			float evZ = viewZ - eyeZ;

			//find t using the shere equation
			//  X^2 + Y^2 + Z^2 = R^2
			//float A = viewX*viewX + viewY*viewY + viewZ*viewZ;
			float A = viewX*viewX + viewY*viewY + 1;
			//float B = -2*circleX*viewX - 2*circleY*viewY + 2*circleZ - 4;
			float B =  -4;
			//float C = circleX*circleX + circleY*circleY + circleZ*circleZ - 4*circleZ + 4 - circleR*circleR;
			float C = 4 - circleR*circleR;

			//find the descriminant
			float D = (B*B) - 4*A*C;

			//decide if there is an intersection
			float t;
			if(D < 0) { //no intersection
				//color with background color
				glBegin(GL_POINTS);
				glColor3f(0,0,0);
				glVertex3f(viewX, viewY, viewZ);
				glEnd();
				continue; //continue to the next pixel
			}
			else if(D == 0.0) { //ray is tangeant to sphere (one intersection)
				t = (-1*B + sqrt(D)) / (2*A);
			}
			else { //ray intersects sphere (two intersections)
				float t1 = (-1*B + sqrt(D)) / (2*A);
				float t2 = (-1*B - sqrt(D)) / (2*A);

				//get the smallest t to find the closest intersection
				if (t1 < t2) t = t1;
				else t = t2;
			}

			//get the point of intersection with the sphere
			float interX = eyeX + evX*t;
			float interY = eyeY + evY*t;
			float interZ = eyeZ + evZ*t;

			//if intersection is on the wrong side of the sphere, color it black
			if ((interY<0  &&  hemSide==0)  ||  (interY>0  &&  hemSide==1)) {
				glBegin(GL_POINTS);
				glColor3f(0,0,0);
				glVertex3f(viewX, viewY, viewZ);
				glEnd();
				continue; //continue to the next pixel
			}

			//get the normal vector to the sphere at this intersection
			float normX = interX - circleX;
			float normY = interY - circleY;
			float normZ = interZ - circleZ;

			//get the magnitude of the vector
			float normMag = sqrt(normX*normX + normY*normY + normZ*normZ);

			//normalize the vector
			normX = normX/normMag;
			normY = normY/normMag;
			normZ = normZ/normMag;

			//vector from intersection to the light
			float lightNormX = lightX - interX;
			float lightNormY = lightY - interY;
			float lightNormZ = lightZ - interZ;

			//get the magnitude of the vector
			float lightNormMag = sqrt(lightNormX*lightNormX
									  + lightNormY*lightNormY
									  + lightNormZ*lightNormZ);

			//normalize the light vector
			lightNormX = lightNormX/lightNormMag;
			lightNormY = lightNormY/lightNormMag;
			lightNormZ = lightNormZ/lightNormMag;

			//compute dot product
			float dot = normX*lightNormX + normY*lightNormY + normZ*lightNormZ;

			//for attenuation, use the square of the distance
			float atten = (interX-lightX)*(interX-lightX)
						+ (interY-lightY)*(interY-lightY)
						+ (interZ-lightZ)*(interZ-lightZ);

			//compute the illumination
			float ldR = kd * lightR * dot / atten;
			float ldG = kd * lightG * dot / atten;
			float ldB = kd * lightB * dot / atten;

			//draw pixel
			glColor3f(ldR,ldG,ldB);
			glBegin(GL_POINTS);
			glVertex3f(viewX,viewY,viewZ);
			glEnd();
		} //end for column
	} //end for row

	glutSwapBuffers();
}

//keyboard callback
void keyboard(unsigned char key, int x, int y) {
   	if((int)key == 27) 	//exit program
   		exit(0);
} //end keyboard


//main method
int main(int argc, char *argv[]) {
	glutInit(&argc, argv);

	getParameters(argc, argv);

 	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 	glutInitWindowSize(screenWidth, screenHeight);
 	glutCreateWindow("Ray Casting a Sphere");
 	glClearColor(0,0,0,0);

 	//The four following statements set up the viewing rectangle
	glMatrixMode(GL_PROJECTION);        // use proj. matrix
	glLoadIdentity();                   // load identity matrix
	gluOrtho2D(-2, 2, -2, 2);           // set orthogr. proj.
	glMatrixMode(GL_MODELVIEW);         // back to modelview m.

	//callbacks
 	glutDisplayFunc(display);           //display
 	glutKeyboardFunc(keyboard);         //keyboard

 	glutMainLoop();
	return 0;
} //end main
