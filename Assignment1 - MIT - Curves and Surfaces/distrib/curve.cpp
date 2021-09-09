#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <math.h>
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx(const Vector3f& lhs, const Vector3f& rhs)
    {
        const float eps = 1e-8f;
        return (lhs - rhs).absSquared() < eps;
    }


}


Curve evalBezier(const vector< Vector3f >& P, unsigned steps)
{
    // Check
    if (P.size() < 4 || P.size() % 3 != 1)
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit(0);
    }

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    Curve R;

    Matrix4f BezierBases
    (
        1, -3, 3, -1,
        0, 3, -6, 3,
        0, 0, 3, -3,
        0, 0, 0, 1
    );


    Matrix4f BezierBasesFirstDrivation
    (
        1, -3, 3, -1,
        0, 3, -6, 3,
        0, 0, 3, -3,
        0, 0, 0, 1
    );



    for (int j = 0; j < P.size() - 1; j += 3)
    {

        // Fill it in counterclockwise
        for (unsigned i = 0; i <= steps; ++i)
        {

            float t = float(i) / steps;
            CurvePoint temp;
            // Initialize position
            // We're pivoting counterclockwise around the y-axis
            Vector3f v = (pow((1 - t), 3)) * P[j] + (3 * t * (pow(1 - t, 2))) * P[j + 1] + (3 * pow(t, 2) * (1 - t)) * P[j + 2] + (pow(t, 3)) * P[j + 3];
            temp.V = v;



            // Tangent vector is first derivative=
            Vector3f tt = (-3 * pow(1 - t, 2) * P[j]) + ((3 * (pow(1 - t, 2))) - (6 * t * (1 - t))) * P[j + 1] + ((6 * t * (1 - t)) - (3 * (pow(t, 2)))) * P[j + 2] + ((3 * pow(t, 2)) * P[j + 3]);

            temp.T = tt.normalized();
            Vector3f  B;
            Vector3f n;
            if (j == 0 && i == 0)
            {

                // arbitrarily set B to the forward vector for the initial step
                // of the first Bezier segment.
                B = Vector3f::FORWARD * -1;
                if (Vector3f::cross(temp.T, B) == Vector3f::ZERO) {
                    // change B if first selection was parallel to T.
                    B = Vector3f(0.f, 0.f, 0.9f);
                }


                n = Vector3f::cross(B, temp.T).normalized();

            }
            else
            {
                n = Vector3f::cross(R[R.size() - 1].B, temp.T).normalized();
            }

            temp.N = n;
            temp.B = Vector3f::cross(tt, n).normalized();




            R.push_back(temp);
        }
    }


    Vector3f NRotationalDifference = R[0].N - R[R.size() - 1].N;
    Vector3f BRotationalDifference = R[0].B - R[R.size() - 1].B;

    if (approx(R[0].V, R[R.size() - 1].V) && approx(R[0].T, R[R.size() - 1].T)) // if it was a closed curve
    {
        for (int i = R.size() - 1; i >= 0; i--)
        {

            R[i].N += Vector3f::lerp(Vector3f::ZERO, NRotationalDifference, float(i) / (R.size() - 1));
            R[i].N = R[i].N.normalized();

            R[i].B += Vector3f::lerp(Vector3f::ZERO, BRotationalDifference, float(i) / (R.size() - 1));
            R[i].B = R[i].B.normalized();
        }
    }







    /* for (int i = 0; i < steps; i++)
     {

     }
     cerr << "\t>>> evalBezier has been called with the following input:" << endl;

     cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
     for( unsigned i = 0; i < P.size(); ++i )
     {
         cerr << "\t>>> " << P[i] << endl;
     }

     cerr << "\t>>> Steps (type steps): " << steps << endl;
     cerr << "\t>>> Returning empty curve." << endl;

     // Right now this will just return this empty curve.*/
    return R;
}

Curve evalBspline(const vector< Vector3f >& P, unsigned steps)
{
    // Check
    if (P.size() < 4)
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit(0);
    }

    Curve R;

    Matrix4f BezierBases
    (
        1, -3, 3, -1,
        0, 3, -6, 3,
        0, 0, 3, -3,
        0, 0, 0, 1
    );

    Matrix4f BSplineBases(
        (1.f / 6.f) * 1, (1.f / 6.f) * -3, (1.f / 6.f) * 3, (1.f / 6.f) * -1,
        (1.f / 6.f) * 4, (1.f / 6.f) * 0, (1.f / 6.f) * -6, (1.f / 6.f) * 3,
        (1.f / 6.f) * 1, (1.f / 6.f) * 3, (1.f / 6.f) * 3, (1.f / 6.f) * -3,
        (1.f / 6.f) * 0, (1.f / 6.f) * 0, (1.f / 6.f) * 0, (1.f / 6.f) * 1
    );
    Matrix4f InverseBezierBases = BezierBases.inverse();

    vector<Vector3f> out;

    for (unsigned i = 0; i < P.size() - 3; i++)
    {

        Matrix4f Mattmp(Vector4f(P[i], 0), Vector4f(P[i + 1], 0), Vector4f(P[i + 2], 0), Vector4f(P[i + 3], 0), 1);
        Mattmp = Mattmp * BSplineBases * InverseBezierBases;

        if (out.size() == 0)
        {
            out.push_back(Mattmp.getCol(0).xyz());
        }

        out.push_back(Mattmp.getCol(1).xyz());
        out.push_back(Mattmp.getCol(2).xyz());
        out.push_back(Mattmp.getCol(3).xyz());

    }

    R = evalBezier(out, steps);




    return R;


}

Curve evalCircle(float radius, unsigned steps)
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).

    // Preallocate a curve with steps+1 CurvePoints
    Curve R(steps + 1);

    // Fill it in counterclockwise
    for (unsigned i = 0; i <= steps; ++i)
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float(i) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = R[i].V = Matrix3f::rotateZ(t) * Vector3f(radius, 0, 0) /* Vector3f( cos(t), sin(t), 0 )*/;


        // Tangent vector is first derivative
        R[i].T = Vector3f(-sin(t), cos(t), 0);

        // Normal vector is second derivative
        R[i].N = Vector3f(-cos(t), -sin(t), 0);

        // Finally, binormal is facing up.
        R[i].B = Vector3f(0, 0, 1);
    }

    return R;
}

void drawCurve(const Curve& curve, float framesize)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // Setup for line drawing
    glDisable(GL_LIGHTING);
    glColor4f(1, 1, 1, 1);
    glLineWidth(1);

    // Draw curve
    glBegin(GL_LINE_STRIP);
    for (unsigned i = 0; i < curve.size(); ++i)
    {
        glVertex(curve[i].V);
    }
    glEnd();

    glLineWidth(1);

    // Draw coordinate frames if framesize nonzero
    if (framesize != 0.0f)
    {
        Matrix4f M;

        for (unsigned i = 0; i < curve.size(); ++i)
        {
            M.setCol(0, Vector4f(curve[i].N, 0));
            M.setCol(1, Vector4f(curve[i].B, 0));
            M.setCol(2, Vector4f(curve[i].T, 0));
            M.setCol(3, Vector4f(curve[i].V, 1));

            glPushMatrix();
            glMultMatrixf(M);
            glScaled(framesize, framesize, framesize);
            glBegin(GL_LINES);
            glColor3f(1, 0, 0); glVertex3d(0, 0, 0); glVertex3d(1, 0, 0);
            glColor3f(0, 1, 0); glVertex3d(0, 0, 0); glVertex3d(0, 1, 0);
            glColor3f(0, 0, 1); glVertex3d(0, 0, 0); glVertex3d(0, 0, 1);
            glEnd();
            glPopMatrix();
        }
    }

    // Pop state
    glPopAttrib();
}


