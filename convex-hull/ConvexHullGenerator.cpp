//
//  ConvexHullGenerator.cpp
//  convex-hull-2d
//
//  Created by Rohan Sawhney on 7/28/15.
//  Copyright (c) 2015 Rohan Sawhney. All rights reserved.
//

#include "ConvexHullGenerator.h"
#include "math.h"
#include <unordered_map>
#include <list>

ConvexHullGenerator::ConvexHullGenerator()
{
    
}

ConvexHullGenerator::~ConvexHullGenerator()
{
    clearPoints();
    clearFaces();
}

void ConvexHullGenerator::clearPoints()
{
    // clear points
    if (points.size() > 0) {
        for (int i = 0; i < points.size(); i++) {
            delete points[i];
        }
        points.clear();
    }
}

void ConvexHullGenerator::clearFaces()
{
    // clear faces
    if (faces.size() > 0) {
        for (int i = 0; i < faces.size(); i++) {
            delete faces[i];
        }
        faces.clear();
    }
}

bool triContainsPoint(Point *t1, Point *t2,
                      Point *t3, Point *p)
{
    // compute vectors
    Eigen::Vector3d v0 = *t2 - *t1;
    Eigen::Vector3d v1 = *t3 - *t1;
    Eigen::Vector3d v2 = *p - *t1;
    
    // compute dot products
    double dot00 = v0.x()*v0.x() + v0.y()*v0.y();
    double dot01 = v0.x()*v1.x() + v0.y()*v1.y();
    double dot02 = v0.x()*v2.x() + v0.y()*v2.y();
    double dot11 = v1.x()*v1.x() + v1.y()*v1.y();
    double dot12 = v1.x()*v2.x() + v1.y()*v2.y();
    
    // compute barycentric coordinates
    double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
    
    // check if point is in triangle
    return (u >= 0) && (v >= 0) && (u + v <= 1);
}

bool comparePoints(const Point *p1, const Point* p2)
{
    return p1->x < p2->x || (p1->x == p2->x && p1->y < p2->y);
}

bool compareFaces(const Face *f1, const Face* f2)
{
    double z1 = (f1->p1->z + f1->p2->z + f1->p3->z) / 3;
    double z2 = (f2->p1->z + f2->p2->z + f2->p3->z) / 3;
    return z1 > z2;
}

const std::vector<Point*> ConvexHullGenerator::generatePoints(const Point& c, const int radius, const int n)
{
    clearPoints();
    
    // distribute points randomly
    int minX = c.x - radius;
    int maxX = c.x + radius;
    int minY = c.y - radius;
    int maxY = c.y + radius;
    int minZ = c.z - radius;
    int maxZ = c.z + radius;
    double r2 = radius * radius;
    for (int i = 0; i < n; i++) {
        double x, y, z, d;
        do {
            x = minX + (rand() % (maxX - minX + 1));
            y = minY + (rand() % (maxY - minY + 1));
            z = minZ + (rand() % (maxZ - minZ + 1));
            
            d = (x - c.x)*(x - c.x) + (y - c.y)*(y - c.y) + (z - c.z)*(z - c.z);
            
        } while (d > r2);
        
        Point *p = new Point(x, y, z);
        points.push_back(p);
    }
    
    // sort points by lowest x (and y in case of tie)
    std::sort(points.begin(), points.end(), comparePoints);
    
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
    
    for (int i = 0; i < (int)hullPoints.size(); i++) {
        hullPoints[i]->onHull = true;
    }
    
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

const std::vector<Face*> ConvexHullGenerator::generatetriangulatedHull2d()
{
    clearFaces();
    
    std::vector<Point*> hullPoints = ::generateConvexHull2d(points);
    
    std::list<Face*> faceList;
    for (int i = 1; i < hullPoints.size()-1; i++) {
        faceList.push_back(new Face(hullPoints[0], hullPoints[i], hullPoints[i+1]));
    }
   
    for (int i = 0; i < (int)points.size(); i++) {
        if (!points[i]->onHull) {
            Face *f = NULL;
            for (std::list<Face*>::iterator it = faceList.begin(); it != faceList.end(); it++) {
                if (triContainsPoint((*it)->p1, (*it)->p2, (*it)->p3, points[i])) {
                    f = *it;
                    faceList.erase(it);
                    break;
                }
            }
            
            if (f) {
                faceList.push_back(new Face(f->p1, f->p2, points[i]));
                faceList.push_back(new Face(f->p2, f->p3, points[i]));
                faceList.push_back(new Face(f->p3, f->p1, points[i]));
                
                delete f;
            }
        }
    }
   
    for (std::list<Face*>::iterator it = faceList.begin(); it != faceList.end(); it++) {
        faces.push_back(*it);
    }
    
    return faces;
}

Point *centroid(Point *p1, Point *p2, Point *p3, Point *p4)
{
    double x = (p1->x + p2->x + p3->x + p4->x) / 4;
    double y = (p1->y + p2->y + p3->y + p4->y) / 4;
    double z = (p1->z + p2->z + p3->z + p4->z) / 4;
    
    return new Point(x, y, z);
}

std::string edgeRep(Point *p1, Point *p2)
{
    Point mid = Point((p1->x + p2->x)/2, (p1->y + p2->y)/2);
    std::string rep = std::to_string(mid.x) + " " + std::to_string(mid.y);
    
    return rep;
}

struct Edge {
    Edge() {}
    Edge(Point *p10, Point *p20): p1(p10), p2(p20) {
        refCount = 1;
    }
    Edge(const Edge& e): p1(e.p1), p2(e.p2), refCount(e.refCount) {}
    
    Edge& operator=(const Edge& e) {
        p1 = e.p1;
        p2 = e.p2;
        refCount = e.refCount;
        
        return *this;
    }
    
    Point *p1, *p2;
    int refCount;
};

void incrementEdgeRef(std::unordered_map<std::string, Edge*>& edgeMap, Point *p1, Point *p2)
{
    std::string rep = edgeRep(p1, p2);
    std::unordered_map<std::string, Edge*>::iterator it = edgeMap.find(rep);
    if (it == edgeMap.end()) {
        edgeMap[rep] = new Edge(p1, p2);
        
    } else {
        edgeMap[rep]->refCount++;
    }
}

const std::vector<Face*> ConvexHullGenerator::generateConvexHull3d()
{
    clearFaces();
    
    // create tetrahedron
    Point *c = centroid(points[0], points[1], points[2], points[3]);
    
    // none of the faces should face the centroid
    Face *face1 = new Face(points[0], points[1], points[2]);
    if (face1->isVisible(c)) face1->flip();
    faces.push_back(face1);
    
    Face *face2 = new Face(points[3], points[1], points[2]);
    if (face2->isVisible(c)) face2->flip();
    faces.push_back(face2);
    
    Face *face3 = new Face(points[0], points[3], points[2]);
    if (face3->isVisible(c)) face3->flip();
    faces.push_back(face3);
    
    Face *face4 = new Face(points[0], points[1], points[3]);
    if (face4->isVisible(c)) face4->flip();
    faces.push_back(face4);
    
    // for the remaining points
    for (size_t i = 4; i < points.size(); i++) {
        Point *p = points[i];
        
        // isolate the visible faces
        // also count the number of times edges recur in the visible faces mesh
        std::vector<Face*> visibleFaces;
        std::vector<Face*> newFaces;
        std::unordered_map<std::string, Edge*> edgeMap;
        for (size_t j = 0; j < faces.size(); j++) {
            if (faces[j]->isVisible(p)) {
                visibleFaces.push_back(faces[j]);
                
                incrementEdgeRef(edgeMap, faces[j]->p1, faces[j]->p2);
                incrementEdgeRef(edgeMap, faces[j]->p2, faces[j]->p3);
                incrementEdgeRef(edgeMap, faces[j]->p3, faces[j]->p1);
            
            } else {
                newFaces.push_back(faces[j]);
            }
        }
        
        // if no faces are visible, point is inside the set of facets
        if (visibleFaces.size() == 0) continue;
        
        // determine horizon edges (these edges have refCount 1 in edgeMap)
        for (auto kv : edgeMap) {
            if (kv.second->refCount == 1) {
                // create a face and add it to newFaces
                Face *face = new Face(kv.second->p1, kv.second->p2, p);
                if (face->isVisible(c)) face->flip();
                newFaces.push_back(face);
            }
            // delete edge
            delete kv.second;
        }
        
        // delete visible faces
        for (size_t j = 0; j < visibleFaces.size(); j++) {
            delete visibleFaces[j];
        }
        faces.clear();
        for (int i = 0; i < (int)newFaces.size(); i++) {
            faces.push_back(newFaces[i]);
        }
    }
    
    delete c;
    
    // sort faces by z value
    std::sort(faces.begin(), faces.end(), compareFaces);
    
    return faces;
}
