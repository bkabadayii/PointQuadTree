#ifndef PointQuadTree_hpp
#define PointQuadTree_hpp
#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <map>

using namespace std;

struct point {
    int x;
    int y;
    point(int _x, int _y): x(_x), y(_y) {}
    
    //Comparision operator for point
    //Implemented because std::map requires comparision operator to work with custom structs/ classes.
    bool operator <(const point &rhs) const {
        return x < rhs.x;
    }
};

template <class type>
struct PointQuadTreeNode {
    point coordinates;
    type data;
    PointQuadTreeNode *ne;
    PointQuadTreeNode *nw;
    PointQuadTreeNode *se;
    PointQuadTreeNode *sw;
    
    //Constructors:
    PointQuadTreeNode(int _x, int _y, type _data) :
    coordinates(point(_x, _y)), ne(NULL), nw(NULL), se(NULL), sw(NULL), data(_data) {}
    
    PointQuadTreeNode(point _p, string _data) :
    coordinates(_p), ne(NULL), nw(NULL), se(NULL), sw(NULL), data(_data) {}
};

template <class type>
class PointQuadTree {
private:
    //Class members:
    PointQuadTreeNode<type> *root;
    
    //Helper functions:
    void printTreeHelper(PointQuadTreeNode<type> *_root);
    int comparePoints(point &p1, point &p2);
    void insertHelper(point &p, type &_data, PointQuadTreeNode<type>* &_root);
    point normalizePoint(point dataPoint, point &searchCenter, int &directionCode);
    int regionRelativeToSearchPoint(point &searchPoint, point &normalizedDataPoint, int &radius);
    void setSearchMarks(int &directionCode, int &region, vector<bool> &searchMarks);
    void searchAreaHelper(point &searchCenter, int &radius, map<point, type> &pointFoundToData, PointQuadTreeNode<type>* &_root);
    

public:
    PointQuadTree(); //Default constructor.
    void printTree(); //Only works if "cout << data_type;" is defined.
    void insert(point p, type _data); //Inserts a point combined with data to tree.
    void insert(int xCoordinate, int yCoordinate, type _data); //Inserts a point given the x coordinates, y coordinates.
    map<point, type> searchArea(point &searchCenter, int &radius); //Searches a circular area for data given a center point and radius.
};

#include "PointQuadTree.cpp"
#endif /* PointQuadTree_hpp */
