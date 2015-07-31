//
//  ConvexHullGenerator.cpp
//  convex-hull-2d
//
//  Created by Rohan Sawhney on 7/28/15.
//  Copyright (c) 2015 Rohan Sawhney. All rights reserved.
//

#include "ConvexHullGenerator.h"
#include "math.h"
#include <iostream>

ConvexHullGenerator::ConvexHullGenerator()
{
    
}

ConvexHullGenerator::~ConvexHullGenerator()
{
    cleanPoints();
}

void ConvexHullGenerator::cleanPoints()
{
    if (points.size() > 0) {
        for (int i = 0; i < points.size(); i++) {
            delete points[i];
        }
        points.clear();
    }
}

bool compare(const Point *p1, const Point* p2)
{
    return p1->x < p2->x || (p1->x == p2->x && p1->y < p2->y);
}

const std::vector<Point*> ConvexHullGenerator::generatePoints(const Point& c, const int radius, const int n)
{
    // clean
    cleanPoints();
    
    // distribute points randomly
    int min = c.x - radius;
    int max = c.x + radius;
    double r2 = radius * radius;
    for (int i = 0; i < n; i++) {
        double x, y, z, d;
        do {
            x = min + (rand() % (max - min + 1));
            y = min + (rand() % (max - min + 1));
            z = min + (rand() % (max - min + 1));
            
            d = (x - c.x)*(x - c.x) + (y - c.y)*(y - c.y) + (z - c.z)*(z - c.z);
            
        } while (d > r2);
        
        Point *p = new Point(x, y, z);
        points.push_back(p);
    }
    
    // sort points by lowest x (and y in case of tie)
    std::sort(points.begin(), points.end(), compare);
    
    return points;
}

double cross2d(const Point *o, const Point *p, const Point *q)
{
    return (p->x - o->x) * (q->y - o->y) -
    (p->y - o->y) * (q->x - o->x);
}

const std::vector<Point*> generateConvexHull2d(std::vector<Point*> points)
{
    int n = (int)points.size();
    std::vector<Point*> hullPoints(2*n);
    int k = 0;
    
    // build lower half
    for (int i = 0; i < n; i++) {
        // while there are at least 2 points in the array and the sequence of those points and
        // point[i] does not make a anti-clockwise turn
        while (k >= 2 && cross2d(hullPoints[k-2], hullPoints[k-1], points[i]) <= 0) {
            k--;
        }
        
        hullPoints[k] = points[i];
        k++;
    }
    
    // build upper half
    for (int i = n-2, t = k+1; i >= 0; i--) {
        // while there are at least 2 points in the array and the sequence of those points and
        // point[i] does not make a anti-clockwise turn
        while (k >= t && cross2d(hullPoints[k-2], hullPoints[k-1], points[i]) <= 0) {
            k--;
        }
        
        hullPoints[k] = points[i];
        k++;
    }
    
    hullPoints.resize(k);
    return hullPoints;
}

const std::vector<std::vector<Point*>> ConvexHullGenerator::generateConvexHull2d() const
{
    std::vector<std::vector<Point*>> convexHulls;
    convexHulls.push_back(::generateConvexHull2d(points));
    
    return convexHulls;
}

const std::vector<std::vector<Point*>> ConvexHullGenerator::generateOnionPeeling2d()
{
    std::vector<std::vector<Point*>> convexHulls;
    std::vector<Point*> remainingPoints(points);
    
    while (remainingPoints.size() > 2) {
        convexHulls.push_back(::generateConvexHull2d(remainingPoints));
        
        // set used
        int size = (int)convexHulls.size();
        for (int i = 0; i < (int)convexHulls[size-1].size(); i++) {
            convexHulls[size-1][i]->used = true;
        }
        
        // add unused points
        remainingPoints.clear();
        for (int i = 0; i < (int)points.size(); i++) {
            if (!points[i]->used) {
                remainingPoints.push_back(points[i]);
            }
        }
    }
    
    // reset
    for (int i = 0; i < (int)points.size(); i++) {
        points[i]->used = false;
    }
    
    return convexHulls;
}

const std::vector<std::vector<Point*>> ConvexHullGenerator::generateConvexHull3d() const
{
    std::vector<std::vector<Point*>> convexHulls;
    
    // TODO: Implement
    
    return convexHulls;
}
