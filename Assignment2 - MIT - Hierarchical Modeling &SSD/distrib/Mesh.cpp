#include "Mesh.h"

using namespace std;

void Mesh::load( const char* filename )
{
	// 2.1.1. load() should populate bindVertices, currentVertices, and faces

	// Add your code here.

    std::ifstream inFile(filename);
	
    string buffer;
    int x = 0;

    while (getline(inFile, buffer))
    {
        stringstream ss(buffer);
        string mode;
        ss >> mode;
        if (mode == "v")
        {
            Vector3f vertexPos;
            ss >> vertexPos[0] >> vertexPos[1] >> vertexPos[2];
            bindVertices.push_back(vertexPos);
        }
        else if (mode == "f")
        {
            Tuple3u facePos;
            ss >> facePos[0] >> facePos[1] >> facePos[2];
            faces.push_back(facePos);
        }


    }


	// make a copy of the bind vertices as the current vertices
	currentVertices = bindVertices;
}

void Mesh::draw()
{
	// Since these meshes don't have normals
	// be sure to generate a normal per triangle.
	// Notice that since we have per-triangle normals
	// rather than the analytical normals from
	// assignment 1, the appearance is "faceted".

	for(auto v : faces)
	{
        Vector3f v1 = currentVertices[ v[0] - 1 ];
        Vector3f v2 = currentVertices[ v[1] - 1 ];
        Vector3f v3 = currentVertices[ v[2] - 1 ];

		// calculate normal of the face
        Vector3f v1v2 = v2 - v1;
        Vector3f v2v3 = v3 - v2;

        Vector3f faceNormal1 = Vector3f::cross(v1v2, v2v3).normalized();

		
		

        glBegin(GL_TRIANGLES);
        glNormal3d(faceNormal1[0], faceNormal1[1], faceNormal1[2]);
        glVertex3d(v1[0], v1[1], v1[2]);
        glNormal3d(faceNormal1[0], faceNormal1[1], faceNormal1[2]);
        glVertex3d(v2[0], v2[1], v2[2]);
        glNormal3d(faceNormal1[0], faceNormal1[1], faceNormal1[2]);
        glVertex3d(v3[0], v3[1], v3[2]);
        glEnd();
		
		
	}
}

void Mesh::loadAttachments( const char* filename, int numJoints )
{
	// 2.2. Implement this method to load the per-vertex attachment weights
	// this method should update m_mesh.attachments


    std::ifstream inFile(filename);

    string buffer;
    int x = 0;

    while (getline(inFile, buffer))
    {
        stringstream ss(buffer);
        vector<float> weightPerJoint;
        weightPerJoint.push_back(0);//the weight of root joint is zero
    	for(int i = 0 ; i < 17 ; i++)
    	{
            float weight;
            ss >> weight;
            weightPerJoint.push_back(weight);
    	}


        attachments.push_back(weightPerJoint);
    }
}
