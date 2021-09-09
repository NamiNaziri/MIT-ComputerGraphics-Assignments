#include "SceneParser.h"

#include "VecUtils.h"
#include "Camera.h" 
#include "lights.hpp"
#include "material.hpp"
#include "objects.hpp"
#include "utility.hpp"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <direct.h>	// for getcwd

#define DegreesToRadians(x) ((FW_PI * x) / 180.0f)

using namespace std;
using namespace FW;

SceneParser::SceneParser( const char* filename )
{
	// initialize some reasonable default values
	group = nullptr;
	camera = nullptr;
	background_color = Vec3f(0.5,0.5,0.5);
	ambient_light = Vec3f(0,0,0);
	num_lights = 0;
	lights = nullptr;
	num_materials = 0;
	materials = nullptr;
	current_material = nullptr;

	// Jaakko: added support for loading from different directories
	char acDir[_MAX_DIR];
	char acCWD[_MAX_DIR];
	_getcwd( acCWD, sizeof(acCWD) );
	_splitpath( filename, 0, acDir, 0, 0 );

	// parse the file
	assert(filename != nullptr);
	file = fopen(filename,"r");
	if ( file == 0 )
	{
		::printf( "FATAL: Could not open %s!\n", filename );
		exit(0);
	}

	// change into directory in order to access files relative to it.
	_chdir( acDir );

	parseFile();
	fclose(file); 
	file = 0;

	// .. and change back.
	_chdir( acCWD );

	// if no lights are specified, set ambient light to white
	// (do solid color ray casting)
	if (num_lights == 0)
	{
		::printf( "WARNING: No lights specified.  Setting ambient light to (1,1,1)\n");
		ambient_light = Vec3f(1,1,1);
	}
}

SceneParser::SceneParser()
{
	group = nullptr;
	camera = nullptr;
	background_color = Vec3f(0.5, 0.5, 0.5);
	ambient_light = Vec3f(0, 0, 0);
	num_lights = 0;
	lights = nullptr;
	num_materials = 0;
	materials = nullptr;
	current_material = nullptr;
}

SceneParser::~SceneParser()
{
	if (group != nullptr) 
		delete group;
	if (camera != nullptr) 
		delete camera;
	int i;
	for (i = 0; i < num_materials; i++) {
		delete materials[i]; }
	delete [] materials;
	for (i = 0; i < num_lights; i++) {
		delete lights[i]; }
	delete [] lights;
}

// ====================================================================
// ====================================================================

void SceneParser::parseFile() {
	//
	// at the top level, the scene can have a camera, 
	// background color and a group of objects
	// (we add lights and other things in future assignments)
	//
	char token[MAX_PARSER_TOKEN_LENGTH];        
	while (getToken( token )) { 
		if (!strcmp(token, "OrthographicCamera")) {
			parseOrthographicCamera();
		} else if (!strcmp(token, "PerspectiveCamera")) {
			parsePerspectiveCamera();
		} else if (!strcmp(token, "Background")) {
			parseBackground();
		} else if (!strcmp(token, "Lights")) {
			parseLights();
		} else if (!strcmp(token, "Materials")) {
			parseMaterials();
		} else if (!strcmp(token, "Group")) {
			group = parseGroup();
		} else {
			::printf ("Unknown token in parseFile: '%s'\n", token);
			exit(0);
		}
	}
}

void SceneParser::parseOrthographicCamera()
{
	char token[MAX_PARSER_TOKEN_LENGTH];
	// read in the camera parameters
	getToken( token ); assert (!strcmp(token, "{"));
	getToken( token ); assert (!strcmp(token, "center"));
	Vec3f center = readVec3f();
	getToken( token ); assert (!strcmp(token, "direction"));
	Vec3f direction = readVec3f();
	getToken( token ); assert (!strcmp(token, "up"));
	Vec3f up = readVec3f();
	getToken( token ); assert (!strcmp(token, "size"));
	float size = readFloat();
	getToken( token ); assert (!strcmp(token, "}"));
	camera = new OrthographicCamera(center,direction,up,size);
}


void SceneParser::parsePerspectiveCamera() {
	char token[MAX_PARSER_TOKEN_LENGTH];
	// read in the camera parameters
	getToken( token ); assert (!strcmp(token, "{"));
	getToken( token ); assert (!strcmp(token, "center"));
	Vec3f center = readVec3f();
	getToken( token ); assert (!strcmp(token, "direction"));
	Vec3f direction = readVec3f();
	getToken( token ); assert (!strcmp(token, "up"));
	Vec3f up = readVec3f();
	getToken( token ); assert (!strcmp(token, "angle"));
	float angle_degrees = readFloat();
	float angle_radians = DegreesToRadians(angle_degrees);
	getToken( token ); assert (!strcmp(token, "}"));
	camera = new PerspectiveCamera(center,direction,up,angle_radians);
}

void SceneParser::parseBackground() {
	char token[MAX_PARSER_TOKEN_LENGTH];
	// read in the background color
	getToken( token ); assert (!strcmp(token, "{"));    
	while (1) {
		getToken( token ); 
		if (!strcmp(token, "}")) { 
			break;    
		} else if (!strcmp(token, "color")) {
			background_color = readVec3f();
		} else if (!strcmp(token, "ambientLight")) {
			ambient_light = readVec3f();
		} else {
			::printf ("Unknown token in parseBackground: '%s'\n", token);
			assert(0);
		}
	}
}

// ====================================================================
// ====================================================================

void SceneParser::parseLights()
{
	char token[ MAX_PARSER_TOKEN_LENGTH ];
	getToken( token );
	assert( !strcmp( token, "{" ) );

	// read in the number of objects
	getToken( token );
	assert( !strcmp( token, "numLights" ) );
	num_lights = readInt();
	lights = new Light*[ num_lights ];

	// read in the objects
	int count = 0;
	while( num_lights > count )
	{
		getToken( token ); 
		if( !strcmp( token, "DirectionalLight" ) )
		{
			lights[ count ] = parseDirectionalLight();
		}
		else if( !strcmp( token, "PointLight" ) )
		{
			lights[ count ] = parsePointLight();
		}
		else
		{
			::printf( "Unknown token in parseLight: '%s'\n", token ); 
			exit( 0 );    
		}     
		count++;
	}
	getToken( token );
	assert( !strcmp( token, "}" ) );
}


Light* SceneParser::parseDirectionalLight() {
	char token[MAX_PARSER_TOKEN_LENGTH];
	getToken( token ); assert (!strcmp(token, "{"));
	getToken( token ); assert (!strcmp(token, "direction"));
	Vec3f direction = readVec3f();
	getToken( token ); assert (!strcmp(token, "color"));
	Vec3f color = readVec3f();
	getToken( token ); assert (!strcmp(token, "}"));
	return new DirectionalLight(direction,color);
}

Light* SceneParser::parsePointLight()
{
	char token[ MAX_PARSER_TOKEN_LENGTH ];
	getToken( token );
	assert( !strcmp( token, "{" ) );
	getToken( token );
	assert( !strcmp( token, "position" ) );
	Vec3f position = readVec3f();
	getToken( token );
	assert( !strcmp( token, "color" ) );
	Vec3f color = readVec3f();
	Vec3f att( 1, 0, 0 );
	getToken( token ); 
	if( !strcmp( token, "attenuation" ) )
	{
		att[0] = readFloat();
		att[1] = readFloat();
		att[2] = readFloat();
		getToken(token); 
	} 
	assert( !strcmp( token, "}" ) );

	return new PointLight( position, color, att[0], att[1], att[2] );
}

void SceneParser::parseMaterials() {
	char token[MAX_PARSER_TOKEN_LENGTH];
	getToken( token ); assert (!strcmp(token, "{"));
	// read in the number of objects
	getToken( token ); assert (!strcmp(token, "numMaterials"));
	num_materials = readInt();
	materials = new Material*[num_materials];
	// read in the objects
	int count = 0;
	while (num_materials > count) {
		getToken( token ); 
		if( !strcmp( token, "Material" ) ||
			!strcmp( token, "PhongMaterial" ) )
		{
			materials[count] = parsePhongMaterial();
		}
		else if( !strcmp( token, "Checkerboard" ) )
		{
			materials[count] = parseCheckerboard(count);
		}
		else
		{
			::printf ("Unknown token in parseMaterial: '%s'\n", token); 
			exit(0);
		}
		count++;
	}
	getToken( token ); assert (!strcmp(token, "}"));
}    


Material* SceneParser::parsePhongMaterial() {

	char token[MAX_PARSER_TOKEN_LENGTH];
	Vec3f diffuseColor(1,1,1);
	Vec3f specularColor(0,0,0);
	float exponent = 1;
	Vec3f reflectiveColor(0,0,0);
	Vec3f transparentColor(0,0,0);
	float indexOfRefraction = 1;
	char* texture = nullptr;
	int mipmap = 0;
	int linearInterp = 0;
	getToken(token); assert (!strcmp(token, "{"));
	while (1) {
		getToken(token); 
		if (!strcmp(token, "diffuseColor")) {
			diffuseColor = readVec3f();
		} else if (!strcmp(token, "specularColor")) {
			specularColor = readVec3f();
		} else if  (!strcmp(token, "exponent")) {
			exponent = readFloat();
		} else if (!strcmp(token, "reflectiveColor")) {
			reflectiveColor = readVec3f();
		} else if (!strcmp(token, "transparentColor")) {
			transparentColor = readVec3f();
		} else if (!strcmp(token, "indexOfRefraction")) {
			indexOfRefraction = readFloat();
		} else if (!strcmp(token, "texture")) {
			getToken(token); 
			texture = new char[strlen(token)+2];
			strcpy(texture,token);
		} else if (!strcmp(token, "mipmap")) {
			mipmap = 1;
		} else if (!strcmp(token, "linearInterpolation")) {
			linearInterp = 1;
			::printf ("LINEAR\n");
		} else {
			assert (!strcmp(token, "}"));
			break;
		}
	}
	Material* answer = new PhongMaterial
		(diffuseColor,specularColor,exponent,
		 reflectiveColor,transparentColor,indexOfRefraction,texture);

	// jiawen: in case you want to support texture filtering modes
	// you can take advantage of these
	// if (mipmap) answer->setDoMipMap();
	// if (linearInterp) answer->setDoLinearInterp();
	return answer;
}

Material *SceneParser::parseCheckerboard(int count)
{
	// jiawen: why are we passing a Mat4f by pointer?
	//
	char token[MAX_PARSER_TOKEN_LENGTH];
	getToken(token); assert (!strcmp(token, "{"));
	Mat4f matrix;
	matrix.setIdentity();
	getToken(token); 
	if (!strcmp(token, "Transform")) {
		getToken(token); assert (!strcmp(token, "{"));
		parseMatrixHelper(matrix,token);
		assert (!strcmp(token, "}"));
		getToken(token); 
	}
	assert (!strcmp(token, "materialIndex"));
	int m1 = readInt();
	assert (m1 >= 0 && m1 < count);
	getToken(token); assert (!strcmp(token, "materialIndex"));
	int m2 = readInt();
	assert (m2 >= 0 && m2 < count);
	getToken(token); assert (!strcmp(token, "}"));
	return new Checkerboard(matrix, materials[m1],materials[m2]);
}

Object3D* SceneParser::parseObject(char token[MAX_PARSER_TOKEN_LENGTH]) {
	Object3D *answer = nullptr;
	if (!strcmp(token, "Group")) {            
		answer = (Object3D*)parseGroup();
	} else if (!strcmp(token, "Sphere")) {            
		answer = (Object3D*)parseSphere();
	} else if (!strcmp(token, "Plane")) {            
		answer = (Object3D*)parsePlane();
	} else if (!strcmp(token, "Triangle")) {            
		answer = (Object3D*)parseTriangle();
	} else if (!strcmp(token, "TriangleMesh")) {            
		answer = (Object3D*)parseTriangleMesh();
	} else if (!strcmp(token, "Transform")) {            
		answer = (Object3D*)parseTransform();
	} else {
		::printf ("Unknown token in parseObject: '%s'\n", token);
		exit(0);
	}
	return answer;
}

Group* SceneParser::parseGroup() {
	//
	// each group starts with an integer that specifies
	// the number of objects in the group
	//
	// the material index sets the material of all objects which follow,
	// until the next material index (scoping for the materials is very
	// simple, and essentially ignores any tree hierarchy)
	//
	char token[MAX_PARSER_TOKEN_LENGTH];
	getToken( token ); assert (!strcmp(token, "{"));

	// read in the number of objects
	getToken( token ); assert (!strcmp(token, "numObjects"));
	int num_objects = readInt();

	Group *answer = new Group();

	// read in the objects
	int count = 0;
	while (num_objects > count) {
		getToken( token ); 
		if (!strcmp(token, "MaterialIndex")) {
			// change the current material
			int index = readInt();
			assert (index >= 0 && index <= getNumMaterials());
			current_material = getMaterial(index);
		} else {
			auto object = parseObject(token);
			assert(object);
			answer->insert(object);
			count++;
		}
	}
	getToken( token ); assert (!strcmp(token, "}"));
	
	// return the group
	return answer;
}

// ====================================================================
// ====================================================================

Sphere* SceneParser::parseSphere() {
	char token[MAX_PARSER_TOKEN_LENGTH];
	getToken( token ); assert (!strcmp(token, "{"));
	getToken( token ); assert (!strcmp(token, "center"));
	Vec3f center = readVec3f();
	getToken( token ); assert (!strcmp(token, "radius"));
	float radius = readFloat();
	getToken( token ); assert (!strcmp(token, "}"));
	assert (current_material != nullptr);
	return new Sphere(center,radius,current_material);
}


Plane* SceneParser::parsePlane() {
	char token[MAX_PARSER_TOKEN_LENGTH];
	getToken( token ); assert (!strcmp(token, "{"));
	getToken( token ); assert (!strcmp(token, "normal"));
	Vec3f normal = readVec3f();
	getToken( token ); assert (!strcmp(token, "offset"));
	float offset = readFloat();
	getToken( token ); assert (!strcmp(token, "}"));
	assert (current_material != nullptr);
	return new Plane(normal,offset,current_material);
}

#if 0
Triangle* SceneParser::parseTriangle() {
	char token[MAX_PARSER_TOKEN_LENGTH];
	getToken(token); assert (!strcmp(token, "{"));
	getToken(token); 
	assert (!strcmp(token, "vertex0"));
	Vec3f v0 = readVec3f();
	getToken(token); 
	assert (!strcmp(token, "vertex1"));
	Vec3f v1 = readVec3f();
	getToken(token); 
	assert (!strcmp(token, "vertex2"));
	Vec3f v2 = readVec3f();
	getToken(token); assert (!strcmp(token, "}"));
	assert (current_material != nullptr);
	return new Triangle(v0,v1,v2,current_material);
}
#endif

Triangle* SceneParser::parseTriangle() {
	char token[MAX_PARSER_TOKEN_LENGTH];
	getToken(token); assert (!strcmp(token, "{"));
	Vec2f t0( 0, 0 );
	Vec2f t1( 0, 0 );
	Vec2f t2( 0, 0 );
	getToken(token); 
	if (!strcmp(token, "textCoord0")) { t0 = readVec2f(); getToken(token); }
	assert (!strcmp(token, "vertex0"));
	Vec3f v0 = readVec3f();
	getToken(token); 
	if (!strcmp(token, "textCoord1")) { t1 = readVec2f(); getToken(token); }
	assert (!strcmp(token, "vertex1"));
	Vec3f v1 = readVec3f();
	getToken(token); 
	if (!strcmp(token, "textCoord2")) { t2 = readVec2f(); getToken(token); }
	assert (!strcmp(token, "vertex2"));
	Vec3f v2 = readVec3f();
	getToken(token); assert(!strcmp(token, "}"));
	assert (current_material != nullptr);
	return new Triangle(v0,v1,v2,current_material);

	// jiawen:
	// removed texture mapping since it is no longer required 
	// return new Triangle(v0,v1,v2,current_material,t0,t1,t2);
}

Group* SceneParser::parseTriangleMesh() {
	char token[MAX_PARSER_TOKEN_LENGTH];
	char filename[MAX_PARSER_TOKEN_LENGTH];
	// get the filename
	getToken( token ); assert (!strcmp(token, "{"));
	getToken( token ); assert (!strcmp(token, "obj_file"));
	getToken(filename); 
	getToken( token ); assert (!strcmp(token, "}"));
	const char *ext = &filename[strlen(filename)-4];
	assert(!strcmp(ext,".obj"));
	// read it once, get counts
	FILE *mesh_file = fopen(filename,"r");
	assert (mesh_file != nullptr);
	int vcount = 0; int fcount = 0;
	while (1) {
		int c = fgetc(mesh_file);
		if (c == EOF) { break;
		} else if (c == 'v' && fgetc(mesh_file)!='n') {
			assert(fcount == 0); float v0,v1,v2;
			fscanf (mesh_file,"%f %f %f",&v0,&v1,&v2);
			vcount++; 
		}else if (c == 'f') {
			int f0, f1, f2, t1, t2, t3;
			fscanf(mesh_file, "%d//%d %d//%d %d//%d", &f0, &t1, &f1, &t2, &f2, &t3);
			fcount++; 
		} // otherwise, must be whitespace
	}
	fclose(mesh_file);
	// make arrays
	Vec3f *verts = new Vec3f[vcount];

	// load the whole model as a single preview model instead of dealing with each triangle separately
	Group *answer = new Group(current_material, (FW::Mesh<FW::VertexPNT>*)FW::importMesh(filename));
	
	// read it again, save it
	mesh_file = fopen(filename,"r");
	assert (mesh_file != nullptr);
	int new_vcount = 0; int new_fcount = 0;
	while (1) {
		int c = fgetc(mesh_file);
		if (c == EOF) { break;
		}else if (c == 'v' && fgetc(mesh_file) != 'n') {
			assert(new_fcount == 0); float v0,v1,v2;
			fscanf (mesh_file,"%f %f %f",&v0,&v1,&v2);
			verts[new_vcount] = Vec3f(v0,v1,v2);
			new_vcount++; 
		} else if (c == 'f') {
			assert (vcount == new_vcount);
			int f0,f1,f2, t1, t2, t3;
			fscanf (mesh_file,"%d//%d %d//%d %d//%d",&f0,&t1,&f1,&t2,&f2,&t3);
			// indexed starting at 1...
			assert (f0 > 0 && f0 <= vcount);
			assert (f1 > 0 && f1 <= vcount);
			assert (f2 > 0 && f2 <= vcount);
			assert (current_material != nullptr);
			auto t = new Triangle(verts[f0-1], verts[f1-1], verts[f2-1], current_material, false);
			answer->insert(t);
			new_fcount++; 
		} // otherwise, must be whitespace
	}
	delete [] verts;
	assert (fcount == new_fcount);
	assert (vcount == new_vcount);
	fclose(mesh_file);
	return answer;
}

Transform* SceneParser::parseTransform() {
  char token[MAX_PARSER_TOKEN_LENGTH];
  Mat4f matrix;
  matrix.setIdentity();
  // opening brace
  getToken(token); assert (!strcmp(token, "{"));
  // the matrix
  parseMatrixHelper(matrix,token);
  // the Object3D
  Object3D *object = parseObject(token);
  assert(object != nullptr);
  // closing brace
  getToken(token); assert (!strcmp(token, "}"));
  return new Transform(matrix, object);
}

void SceneParser::parseMatrixHelper( Mat4f& matrix, char token[ MAX_PARSER_TOKEN_LENGTH ] )
{
	while( true )
	{
		getToken( token );
		if( !strcmp( token, "Scale" ) )
		{
			Vec3f s = readVec3f();
			matrix = matrix * Mat4f::scale( Vec3f( s[0], s[1], s[2] ) );
		}
		else if( !strcmp( token, "UniformScale" ) )
		{
			float s = readFloat();
			matrix = matrix * Mat4f::scale( Vec3f( s, s, s ) );
		}
		else if( !strcmp( token, "Translate" ) )
		{
			matrix = matrix * Mat4f::translate( readVec3f() );
		}
		else if( !strcmp( token,"XRotate" ) )
		{
			matrix = matrix * VecUtils::rotate( Vec3f( 1.0f, 0.0f, 0.0f ), DegreesToRadians( readFloat() ) );
		}
		else if( !strcmp( token,"YRotate" ) )
		{
			matrix = matrix * VecUtils::rotate( Vec3f( 0.0f, 1.0f, 0.0f ), DegreesToRadians( readFloat() ) );
		}
		else if( !strcmp( token,"ZRotate") )
		{
			matrix = matrix * VecUtils::rotate( Vec3f( 0.0f, 0.0f, 1.0f ), DegreesToRadians( readFloat() ) );
		}
		else if( !strcmp( token,"Rotate" ) )
		{
			getToken( token );
			assert( !strcmp( token, "{" ) );
			Vec3f axis = readVec3f();
			float degrees = readFloat();
			float radians = DegreesToRadians(degrees);
			matrix = matrix * VecUtils::rotate(axis,radians);
			getToken( token );
			assert( !strcmp( token, "}" ) );
		}
		else if( !strcmp( token, "Matrix" ) )
		{
			Mat4f matrix2;
			matrix2.setIdentity();
			getToken( token );
			assert( !strcmp( token, "{" ) );

			for (int j = 0; j < 4; j++)
			{
				for (int i = 0; i < 4; i++)
				{
					float v = readFloat();
					matrix2( i, j ) = v; 
				} 
			}

			getToken( token );
			assert (!strcmp(token, "}"));

			matrix = matrix2 * matrix;
		}
		else
		{
			// otherwise this must be the thing to transform
			break;
		}
	}
}

int SceneParser::getToken(char token[MAX_PARSER_TOKEN_LENGTH]) {
	// for simplicity, tokens must be separated by whitespace
	assert (file != nullptr);
	int success = fscanf(file,"%s ",token);
	if (success == EOF) {
		token[0] = '\0';
		return 0;
	}
	return 1;
}


Vec3f SceneParser::readVec3f() {
	float x,y,z;
	int count = fscanf(file,"%f %f %f",&x,&y,&z);
	if (count != 3) {
		::printf ("Error trying to read 3 floats to make a Vec3f\n");
		assert (0);
	}
	return Vec3f(x,y,z);
}


Vec2f SceneParser::readVec2f() {
	float u,v;
	int count = fscanf(file,"%f %f",&u,&v);
	if (count != 2) {
		::printf ("Error trying to read 2 floats to make a Vec2f\n");
		assert (0);
	}
	return Vec2f(u,v);
}


float SceneParser::readFloat() {
	float answer;
	int count = fscanf(file,"%f",&answer);
	if (count != 1) {
		::printf ("Error trying to read 1 float\n");
		assert (0);
	}
	return answer;
}


int SceneParser::readInt() {
	int answer;
	int count = fscanf(file,"%d",&answer);
	if (count != 1) {
		::printf ("Error trying to read 1 int\n");
		assert (0);
	}
	return answer;
}
