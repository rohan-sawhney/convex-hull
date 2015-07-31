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

struct Point
{
    Point() {}
    Point(const double x0, const double y0, const double z0 = 0.0): x(x0), y(y0), z(z0), used(false) {}
    Point(const Point& p): x(p.x), y(p.y), z(p.z), used(p.used) {}
    
    Point& operator=(const Point& p) {
        x = p.x;
        y = p.y;
        z = p.z;
        used = p.used;
        
        return *this;
    }
    
    double x, y, z;
    bool used;
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
    const std::vector<std::vector<Point*>> generateConvexHull3d() const;
    
private:
    // clean points
    void cleanPoints();
    
    // member variables
    std::vector<Point*> points;
};

#endif
