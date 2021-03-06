#include "SkeletalModel.h"

#include <FL/Fl.H>

using namespace std;

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);

	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

void SkeletalModel::loadSkeleton( const char* filename )
{
	// Load the skeleton from file here.

	string line;
	ifstream myfile(filename);
	Vector3f JointsTranslation;
	int ParrentIndex;
	if (myfile)  // same as: if (myfile.good())
	{
		while (getline(myfile, line))  // same as: while (getline( myfile, line ).good())
		{
			stringstream ss(line);

			ss >> JointsTranslation.x();
			ss >> JointsTranslation.y();
			ss >> JointsTranslation.z();
			ss >> ParrentIndex;

			Joint* joint = new Joint;
			joint->transform = Matrix4f::translation(JointsTranslation);
			m_joints.push_back(joint);
			if (ParrentIndex == -1)
			{
				m_rootJoint = joint;
			}
			else
			{
				m_joints[ParrentIndex]->children.push_back(joint);
			}

		}
		myfile.close();
	}

}

void SkeletalModel::drawJoints( )
{
	// Draw a sphere at each joint. You will need to add a recursive helper function to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call.
	drawJointsHelper(m_rootJoint);
	
}

void SkeletalModel::drawJointsHelper(Joint* joint)
{
	m_matrixStack.push(joint->transform  );
	glLoadMatrixf(m_matrixStack.top());
	glutSolidSphere(0.025f, 12, 12);
	for (auto Child : joint->children)
	{
		drawJointsHelper(Child);
	}

	m_matrixStack.pop();
}

void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy.
	drawSkeletonHelper(m_rootJoint);
}

void SkeletalModel::drawSkeletonHelper(Joint* joint)
{
	m_matrixStack.push(joint->transform);
	Matrix4f m_BoxTransform = Matrix4f::identity();
	for (auto Child : joint->children)
	{
		m_BoxTransform = Matrix4f::identity();
		
		const float DistanceToNextChildJoint = Child->transform.getCol(3).xyz().abs();
		
		

		
		const Vector3f ZRotation = (Child->transform.getCol(3).xyz()).normalized();
		const Vector3f YRotation = Vector3f::cross(ZRotation, Vector3f(0, 0, 1)).normalized();
		const Vector3f XRotation = Vector3f::cross(YRotation, ZRotation).normalized();

		m_BoxTransform = m_BoxTransform * Matrix4f::rotation(Quat4f::fromRotatedBasis(XRotation, YRotation, ZRotation));

		m_BoxTransform = m_BoxTransform * Matrix4f::scaling(0.05, 0.05, DistanceToNextChildJoint);

		m_BoxTransform = m_BoxTransform * Matrix4f::translation(0, 0, 0.5);
		
	
		m_matrixStack.push(m_BoxTransform);
		//m_BoxTransform.print();
		glLoadMatrixf(m_matrixStack.top());
		glutSolidCube(1.0f);
		m_matrixStack.pop();
		drawSkeletonHelper(Child);
	}

	m_matrixStack.pop();
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
	const Matrix3f RotationMat = (Matrix4f::rotateX(rX) * Matrix4f::rotateY(rY) * Matrix4f::rotateZ(rZ)).getSubmatrix3x3(0, 0);
	m_joints[jointIndex]->transform.setSubmatrix3x3(0, 0, RotationMat);

}


void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
	m_matrixStack.clear();
	
	bindWorldToJointTransformsHelper(m_rootJoint);
	
}

void SkeletalModel::bindWorldToJointTransformsHelper(Joint* joint)
{

	
	m_matrixStack.push(joint->transform);
	joint->bindWorldToJointTransform = m_matrixStack.top().inverse();
	for (auto Child : joint->children)
	{
		bindWorldToJointTransformsHelper(Child);
	}

	m_matrixStack.pop();
}

void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
	m_matrixStack.clear();
	updateCurrentJointToWorldTransformsHelper(m_rootJoint);
}

void SkeletalModel::updateCurrentJointToWorldTransformsHelper(Joint* joint)
{
	m_matrixStack.push(joint->transform);
	joint->currentJointToWorldTransform = m_matrixStack.top();
	for (auto Child : joint->children)
	{
		updateCurrentJointToWorldTransformsHelper(Child);
	}

	m_matrixStack.pop();
}

void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.

	for(size_t i = 0 ; i <  m_mesh.currentVertices.size() ; i++) // for each vertex
	{
		//get per bone average
		Vector3f newCurrentVertex = Vector3f::ZERO;

		for(size_t j = 0 ; j < m_mesh.attachments[i].size() ; j++ )//j goes up to joints count
		{
			float weight = m_mesh.attachments[i][j]; //weight of joint j

			Vector4f WorldToBindBoneTransform = m_joints[j]->bindWorldToJointTransform * Vector4f(m_mesh.bindVertices[i], 1.f);
			Vector4f BindBoneTransformToWorld = m_joints[j]->currentJointToWorldTransform * WorldToBindBoneTransform;
			
			newCurrentVertex += (weight * (BindBoneTransformToWorld.xyz()));
		}
		m_mesh.currentVertices[i] = newCurrentVertex;
	}
}

