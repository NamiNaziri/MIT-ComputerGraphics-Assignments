#pragma once

#include "base/Math.hpp"

#include <cassert>
#include <cstdio>

class Camera;
class Light;
class Material;
class Object3D;
class Group;
class Sphere;
class Plane;
class Triangle;
class Transform;

#define MAX_PARSER_TOKEN_LENGTH 100

class SceneParser
{
public:
    SceneParser(const char* filename);
	SceneParser();

    ~SceneParser();

    Camera* getCamera() const {
        return camera;
	}

	void setCamera(Camera* new_camera) {
		camera = new_camera;
	}

    FW::Vec3f getBackgroundColor() const {
        return background_color;
    }

    FW::Vec3f getAmbientLight() const {
        return ambient_light;
    }
    
    int getNumLights() const {
        return num_lights;
    }
    
    Light* getLight( int i ) const {
        assert( i >= 0 && i < num_lights );
        return lights[i];
    }

    int getNumMaterials() const {
        return num_materials;
    }

    Material* getMaterial( int i ) const {
        assert( i >= 0 && i < num_materials );
        return materials[i];
    }

    Group* getGroup() const {
        return group;
    }

private:
    void parseFile();
    void parseOrthographicCamera();
    void parsePerspectiveCamera();
    void parseBackground();
    void parseLights();
    Light* parseDirectionalLight();
    Light* parsePointLight();
    void parseMaterials();
    Material* parsePhongMaterial();
    Material* parseCheckerboard(int count);

    Object3D* parseObject(char token[MAX_PARSER_TOKEN_LENGTH]);
    Group* parseGroup();
    Sphere* parseSphere();
    Plane* parsePlane();
    Triangle* parseTriangle();
    Group* parseTriangleMesh();
    Transform* parseTransform();
    void parseMatrixHelper(FW::Mat4f& matrix, char token[MAX_PARSER_TOKEN_LENGTH]);

    int getToken(char token[MAX_PARSER_TOKEN_LENGTH]);
    FW::Vec3f readVec3f();
    FW::Vec2f readVec2f();
    float readFloat();
    int readInt();

    FILE* file;
    Camera* camera;
    FW::Vec3f background_color;
    FW::Vec3f ambient_light;
    int num_lights;
    Light** lights;
    int num_materials;
    Material** materials;
    Material* current_material;
    Group* group;
};
