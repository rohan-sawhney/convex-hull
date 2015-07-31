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
std::vector<std::vector<Point*>> convexHulls;

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
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, gridX, 0, gridY, -gridZ, gridZ);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glPointSize(4.0);
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
        // TODO: Implement
        
    } else {
        glColor3f(0.0f, 1.0f, 0.0f);
        for (int i = 0; i < (int)convexHulls.size(); i++) {
            glBegin(GL_LINES);
            int size = (int)convexHulls[i].size();
            for (int j = 0; j < size; j++) {
                int next = (j+1) % size;
                
                glVertex3f(convexHulls[i][j]->x, convexHulls[i][j]->y, 0);
                glVertex3f(convexHulls[i][next]->x, convexHulls[i][next]->y, 0);
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
            if (draw3dHull) convexHulls = chg.generateConvexHull3d();
            else convexHulls = chg.generateConvexHull2d();
            break;
        case 'o':
            if (!draw3dHull) convexHulls = chg.generateOnionPeeling2d();
            break;
        case 'p':
            draw3dHull = !draw3dHull;
            if (draw3dHull) convexHulls = chg.generateConvexHull3d();
            else convexHulls = chg.generateConvexHull2d();
            
            break;
    }
    
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    printInstructions();
    points = chg.generatePoints(Point(300,300,300), 200, 200);
    convexHulls = chg.generateConvexHull2d();
    
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
