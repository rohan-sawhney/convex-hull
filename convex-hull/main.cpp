#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include "ConvexHullGenerator.h"

int gridX = 600;
int gridY = 600;
int gridZ = 600;

bool drawOnionPeels = false;
bool draw3dHull = false;
bool draw2dHullTriangulation = false;

ConvexHullGenerator chg;
std::vector<Point*> points;
std::vector<std::vector<Point*>> hullPoints;
std::vector<Face*> faces;

void printInstructions()
{
    std::cout << "spacebar: generates new points and their convex hull\n"
              << "i: draws onion peels\n"
              << "o: draws triangulation of 2d hull\n"
              << "p: draws 3d hull\n"
              << "escape: exit"
              << std::endl;
}

void init()
{
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, gridX, 0, gridY, -gridZ, gridZ);
}

void drawFace(Face *f, const double z1, const double z2, const double z3)
{
    glColor4f(0.0f, 0.0f, 1.0f, 0.6f);
    glBegin(GL_TRIANGLES);
    glVertex3f(f->p1->x, f->p1->y, z1);
    glVertex3f(f->p2->x, f->p2->y, z2);
    glVertex3f(f->p3->x, f->p3->y, z3);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(f->p1->x, f->p1->y, z1);
    glVertex3f(f->p2->x, f->p2->y, z2);
    
    glVertex3f(f->p2->x, f->p2->y, z2);
    glVertex3f(f->p3->x, f->p3->y, z3);
    
    glVertex3f(f->p3->x, f->p3->y, z3);
    glVertex3f(f->p1->x, f->p1->y, z1);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_POINTS);
    glVertex3f(f->p1->x, f->p1->y, z1);
    glVertex3f(f->p2->x, f->p2->y, z2);
    glVertex3f(f->p3->x, f->p3->y, z3);
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glPointSize(3.5);
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_POINTS);
    for (size_t i = 1; i < points.size(); i++) {
        if (draw3dHull) {
            glVertex3f(points[i]->x, points[i]->y, points[i]->z);
                
        } else {
            glVertex3f(points[i]->x, points[i]->y, 0);
        }
    }
    glEnd();
    
    if (draw3dHull) {
        for (size_t i = 0; i < faces.size(); i++) {
            Face *f = faces[i];
            drawFace(f, f->p1->z, f->p2->z, f->p3->z);
        }
    
    } else if (draw2dHullTriangulation) {
        for (size_t i = 0; i < faces.size(); i++) {
            Face *f = faces[i];
            drawFace(f, 0, 0, 0);
        }
        
    } else {
        glColor4f(0.0f, 0.0f, 1.0f, 0.6f);
        for (int i = 0; i < (int)hullPoints.size(); i++) {
            glBegin(GL_LINES);
            int size = (int)hullPoints[i].size();
            for (int j = 0; j < size; j++) {
                int next = (j+1) % size;
                
                glVertex3f(hullPoints[i][j]->x, hullPoints[i][j]->y, 0);
                glVertex3f(hullPoints[i][next]->x, hullPoints[i][next]->y, 0);
            }
            glEnd();
        }
    }
    
    glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
        case 27 :
            exit(0);
        case ' ':
            points = chg.generatePoints(Point(300,300,0), 200, 200);
            if (draw3dHull) faces = chg.generateConvexHull3d();
            else if (draw2dHullTriangulation) faces = chg.generatetriangulatedHull2d();
            else if (drawOnionPeels) hullPoints = chg.generateOnionPeeling2d();
            else hullPoints = chg.generateConvexHull2d();
            break;
            
        case 'i':
            drawOnionPeels = !drawOnionPeels;
            if (!draw3dHull && drawOnionPeels) hullPoints = chg.generateOnionPeeling2d();
            else hullPoints = chg.generateConvexHull2d();
            break;
            
        case 'o':
            draw2dHullTriangulation = !draw2dHullTriangulation;
            if (!draw3dHull && draw2dHullTriangulation) faces = chg.generatetriangulatedHull2d();
            else hullPoints = chg.generateConvexHull2d();
            break;
            
        case 'p':
            draw3dHull = !draw3dHull;
            if (draw3dHull) {
                faces = chg.generateConvexHull3d();
            }
            else hullPoints = chg.generateConvexHull2d();
            break;
    }
    
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    printInstructions();
    points = chg.generatePoints(Point(300,300,0), 200, 200);
    hullPoints = chg.generateConvexHull2d();
    
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(gridX, gridY);
    glutCreateWindow("Convex Hull and Onion Peels");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    
    return 0;
}
