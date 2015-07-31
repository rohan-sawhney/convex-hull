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

bool draw3dHull = false;

ConvexHullGenerator chg;
std::vector<Point*> points;
std::vector<std::vector<Point*>> convexHulls2d;
std::vector<Face*> convexHull3d;

void printInstructions()
{
    std::cout << "spacebar: generates new points and their convex hull\n"
              << "o: draws onion peels\n"
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

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glPointSize(3.5);
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_POINTS);
    for (size_t i = 1; i < points.size(); i++) {
        if (!points[i]->onHull) {
            if (draw3dHull) {
                glVertex3f(points[i]->x, points[i]->y, points[i]->z);
                
            } else {
                glVertex3f(points[i]->x, points[i]->y, 0);
            }
        }
    }
    glEnd();
    
    if (draw3dHull) {
        for (size_t i = 0; i < convexHull3d.size(); i++) {
            Face *f = convexHull3d[i];
            glColor4f(0.0f, 0.0f, 1.0f, 0.6f);
            glBegin(GL_TRIANGLES);
            glVertex3f(f->p1->x, f->p1->y, f->p1->z);
            glVertex3f(f->p2->x, f->p2->y, f->p2->z);
            glVertex3f(f->p3->x, f->p3->y, f->p3->z);
            glEnd();
            
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_LINES);
            glVertex3f(f->p1->x, f->p1->y, f->p1->z);
            glVertex3f(f->p2->x, f->p2->y, f->p2->z);
            
            glVertex3f(f->p2->x, f->p2->y, f->p2->z);
            glVertex3f(f->p3->x, f->p3->y, f->p3->z);
            
            glVertex3f(f->p3->x, f->p3->y, f->p3->z);
            glVertex3f(f->p1->x, f->p1->y, f->p1->z);
            glEnd();
            
            glColor3f(1.0f, 1.0f, 0.0f);
            glBegin(GL_POINTS);
            glVertex3f(f->p1->x, f->p1->y, f->p1->z);
            glVertex3f(f->p2->x, f->p2->y, f->p2->z);
            glVertex3f(f->p3->x, f->p3->y, f->p3->z);
            glEnd();
        }

    } else {
        glColor4f(0.0f, 0.0f, 1.0f, 0.6f);
        for (int i = 0; i < (int)convexHulls2d.size(); i++) {
            glBegin(GL_LINES);
            int size = (int)convexHulls2d[i].size();
            for (int j = 0; j < size; j++) {
                int next = (j+1) % size;
                
                glVertex3f(convexHulls2d[i][j]->x, convexHulls2d[i][j]->y, 0);
                glVertex3f(convexHulls2d[i][next]->x, convexHulls2d[i][next]->y, 0);
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
            points = chg.generatePoints(Point(300,300,300), 200, 200);
            if (draw3dHull) convexHull3d = chg.generateConvexHull3d();
            else convexHulls2d = chg.generateConvexHull2d();
            break;
        case 'o':
            if (!draw3dHull) convexHulls2d = chg.generateOnionPeeling2d();
            break;
        case 'p':
            draw3dHull = !draw3dHull;
            if (draw3dHull) convexHull3d = chg.generateConvexHull3d();
            else convexHulls2d = chg.generateConvexHull2d();
            
            break;
    }
    
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    printInstructions();
    points = chg.generatePoints(Point(300,300,300), 200, 200);
    convexHulls2d = chg.generateConvexHull2d();
    
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
