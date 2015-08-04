//
//  ConvexHullGenerator.h
//  convex-hull-2d
//
//  Created by Rohan Sawhney on 7/28/15.
//  Copyright (c) 2015 Rohan Sawhney. All rights reserved.
//

#ifndef CONVEX_HULL_GENERATOR_H
#define CONVEX_HULL_GENERATOR_H

#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <Eigen/Core>
#include <Eigen/Dense>

struct Point
{
    Point() {}
    Point(const double x0, const double y0, const double z0 = 0.0): x(x0), y(y0), z(z0),
                                                                    onHull(false), used(false) {}
    Point(const Point& p): x(p.x), y(p.y), z(p.z), onHull(p.onHull), used(p.used) {}
    
    Point& operator=(const Point& p) {
        x = p.x;
        y = p.y;
        z = p.z;
        onHull = p.onHull;
        used = p.used;
        
        return *this;
    }
    
    Eigen::Vector3d operator-(const Point& p) const {
        
        return Eigen::Vector3d(x-p.x, y-p.y, z-p.z);
    }
    
    double x, y, z;
    bool onHull;
    bool used;
};

struct Face
{
    Face() {}
    Face(Point *p10, Point *p20, Point *p30): p1(p10), p2(p20), p3(p30) {
        p1->onHull = true;
        p2->onHull = true;
        p3->onHull = true;
        compute();
    }
    Face(const Face& f): p1(f.p1), p2(f.p2), p3(f.p3), n(f.n), d(f.d) {}
    ~Face() {
        p1->onHull = false;
        p2->onHull = false;
        p3->onHull = false;
    }
    
    Face& operator=(const Face& f) {
        p1 = f.p1;
        p2 = f.p2;
        p3 = f.p3;
        n = f.n;
        d = f.d;
        
        return *this;
    }
    
    bool isVisible(Point *p) {
        
        return n.x()*p->x + n.y()*p->y + n.z()*p->z + d > 0;
    }
    
    void flip() {
        Point *temp = p1;
        p1 = p2;
        p2 = temp;
        
        compute();
    }
    
    Point *p1, *p2, *p3;
    
private:
    void compute() {
        Eigen::Vector3d u = *p2 - *p1;
        Eigen::Vector3d v = *p3 - *p2;
        
        n = u.cross(v);
        d = -(n.x()*p1->x + n.y()*p1->y + n.z()*p1->z);
    }
    
    Eigen::Vector3d n;
    double d;
};

class ConvexHullGenerator {
public:
    // default constructor
    ConvexHullGenerator();
    
    // destructor
    ~ConvexHullGenerator();
    
    // generates new points inside sphere
    const std::vector<Point*> generatePoints(const Point& c, const int radius, const int num);
    
    // generates convex hull using monotone chain algorithm
    const std::vector<std::vector<Point*>> generateConvexHull2d() const;
    
    // generates onion peels by making use of the convex hull algorithm
    const std::vector<std::vector<Point*>> generateOnionPeeling2d();
    
    // generates 3d convex hull using incremental 3d convex hull algorithm
    const std::vector<Face*> generateConvexHull3d();
    
private:
    // clean points and faces
    void clean();
    
    // member variables
    std::vector<Point*> points;
    std::vector<Face*> faces;
};

#endif
