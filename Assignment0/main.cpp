#include "GL/freeglut.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include "vecmath.h"
using namespace std;

// Globals

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;


// You will need more global variables to implement color and position changes
int i = 0;
float LightVerticalLocation = 1.f;
float LightHorizontalLocation = 1.f;
bool bCanRotate = 0;
GLdouble  Rotation = 90;

// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }




void DrawTries()
{

    for ( unsigned h = 0; h < vecf.size(); h++)
    {
		int c, a, f, d, i, g;
		a = vecf[h][0];
		c = vecf[h][2];
		d = vecf[h][3];
		f = vecf[h][5];
		g = vecf[h][6];
		i = vecf[h][8];
		glBegin(GL_TRIANGLES);
		glNormal3d(vecn[c - 1][0], vecn[c - 1][1], vecn[c - 1][2]);
		glVertex3d(vecv[a - 1][0], vecv[a - 1][1], vecv[a - 1][2]);
		glNormal3d(vecn[f - 1][0], vecn[f - 1][1], vecn[f - 1][2]);
		glVertex3d(vecv[d - 1][0], vecv[d - 1][1], vecv[d - 1][2]);
		glNormal3d(vecn[i - 1][0], vecn[i - 1][1], vecn[i - 1][2]);
		glVertex3d(vecv[g - 1][0], vecv[g - 1][1], vecv[g - 1][2]);
		glEnd();
    }
    
}


// This function is responsible for displaying the object.
void drawScene(void)
{
	

	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Rotate the image
	glMatrixMode(GL_MODELVIEW);  // Current matrix affects objects positions
	glLoadIdentity();              // Initialize to the identity

	// Position the camera at [0,0,5], looking at [0,0,0],
	// with [0,1,0] as the up direction.
	gluLookAt(0.0, 0.0, 5.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);

	// Set material properties of object

	// Here are some colors you might use - feel free to add more
	GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
								 {0.9, 0.5, 0.5, 1.0},
								 {0.5, 0.9, 0.3, 1.0},
								 {0.3, 0.8, 0.9, 1.0} };

	// Here we use the first color entry as the diffuse color
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[i]);

	// Define specular color and shininess
	GLfloat specColor[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat shininess[] = { 100.0 };

	// Note that the specular color and shininess can stay constant
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	// Set light properties

	// Light color (RGBA)
	GLfloat Lt0diff[] = { 1.0,1.0,1.0,1.0 };
	// Light position
	GLfloat Lt0pos[] = { LightHorizontalLocation, LightVerticalLocation, 5.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
	glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);


    glRotatef(Rotation, 0, 1, 0);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	//glutSolidTeapot(1.0);
    DrawTries();
	// Dump the image to the screen.
	glutSwapBuffers();


}



// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
        
        i = (i + 1) % 4;
        
		cout << "color Change key press " << key << "." << endl; 
        break;

    case 'r':
        bCanRotate = bCanRotate ? false : true;
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        // add code to change light position
        LightVerticalLocation += 0.5;
		cout << " key press: up arrow." << endl;
		break;
    case GLUT_KEY_DOWN:
        // add code to change light position
        LightVerticalLocation -= 0.5;
		cout << " key press: down arrow." << endl;
		break;
    case GLUT_KEY_LEFT:
        // add code to change light position
        LightHorizontalLocation -= 0.5f;
		cout << " key press: left arrow." << endl;
		break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
        LightHorizontalLocation += 0.5f;
		cout << " key press: right arrow." << endl;
		break;
    }
    
	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}


// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void calcFaces(vector<unsigned>& faces,string input)
{
	istringstream iss(input);
    string token;

	for (int loop = 0; loop < 3; loop++)
	{
        std::getline(iss, token, '/');

		stringstream num(token);
		int value;
		num >> value;
		faces.push_back(value);
	}
}


void Tick(int in)
{
    if (bCanRotate)
    {	
        Rotation = (Rotation + 2.f) ;
    }

	// this will refresh the screen so that the user sees the light position
	glutPostRedisplay();
    
    glutTimerFunc(1000 / 60, Tick, 0);
}

void loadInput()
{
	// load the OBJ file here
    string buffer; 
    int x = 0;
    
    while (getline(cin,buffer))
    {
		stringstream ss(buffer);
		string mode;
		Vector3f vec3;
		vector<unsigned> faces;
        ss >> mode;
        if (mode == "v" )
        {
            ss >> vec3[0] >> vec3[1] >> vec3[2];
            vecv.push_back(vec3);
        }
        else if (mode == "vn")
        {
			ss >> vec3[0] >> vec3[1] >> vec3[2];
			vecn.push_back(vec3);
        }
        else if (mode == "f")
        {
            
            string abc, def, ghi;
            ss >> abc >> def >> ghi;

            
            calcFaces(faces, abc);
            calcFaces(faces, def);
            calcFaces(faces, ghi);

            vecf.push_back(faces);

        }

      
    }



}




// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();
    
    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Set up Loop Function for animation rotation
	glutTimerFunc(1000 / 60, Tick, 0);




    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
