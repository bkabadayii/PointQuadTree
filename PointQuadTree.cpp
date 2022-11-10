#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include "PointQuadTree.hpp"


using namespace std;
/*
DIRECTIONS ARE DEFINED AS FOLLOWS:
North-East: 0, South-East: 1, South-West: 2, North-West: 3
*/

//Default Constructor:
template<class type>
PointQuadTree<type>::PointQuadTree() {
    root = NULL;
}

//Recursive Helper for printTree.
template<class type>
void PointQuadTree<type>::printTreeHelper(PointQuadTreeNode<type> *_root) {
    if (_root != NULL) {
        cout << _root -> data << endl;
        //Print all subtrees recursively:
        printTreeHelper(_root -> se);
        printTreeHelper(_root -> sw);
        printTreeHelper(_root -> ne);
        printTreeHelper(_root -> nw);
    }
}

//Prints all the data in the tree.
template<class type>
void PointQuadTree<type>::printTree() {
    printTreeHelper(root);
    cout << "\n";
}

//Returns the direction of p1 relative to p2.
template<class type>
int PointQuadTree<type>::comparePoints(point &p1, point &p2) {
    if (p1.x < p2.x) {
        if (p1.y < p2.y) {
            return 2;
        }
        return 3;
    }
    else if (p1.y < p2.y) {
        return 1;
    }
    return 0;
}

//Rotates the dataPoint n * 90 degrees so that the dataPoint's direction relative to searchCenter becomes "NE". Returns rotated point.
template<class type>
point PointQuadTree<type>::normalizePoint(point dataPoint, point &searchCenter, int &directionCode) {
    //Direction code is also counter-clockwise rotation count.
    //Perform rotations 90 degrees counter clockwise <rotateCount> times.
    int x_new = 0, y_new = 0;
    for (int i = 0; i < directionCode; i++) {
        x_new = searchCenter.y - dataPoint.y + searchCenter.x;
        y_new = dataPoint.x - searchCenter.x + searchCenter.y;
        dataPoint.x = x_new;
        dataPoint.y = y_new;
    }
    return dataPoint;
}

//Returns the region of dataPoint relative to searchPoint and radius, region can be 1, 2, 3, 4, 5.
//Region is always set assuming that normalizedDataPoint's direction relative to search center is 0(North-East).
//1 = inside the search circle
//2 = between the search circle and the tangent line square
//3 = until right tangent line.
//4 = until upper tangent line.
//5 = outside.
//Please see the PointQuadTree document for more information.
template<class type>
int PointQuadTree<type>::regionRelativeToSearchPoint(point &searchPoint, point &normalizedDataPoint, int &radius) {
    //Check if region == 1:
    int delta_x = abs(searchPoint.x - normalizedDataPoint.x);
    int delta_y = abs(searchPoint.y - normalizedDataPoint.y);
    double distanceBetween = sqrt(pow(delta_x, 2) + pow(delta_y, 2)); //Distance between the given points.
    if (distanceBetween < radius) {
        return 1;
    }
    //Check if region == 2:
    if(normalizedDataPoint.x <= searchPoint.x + radius && normalizedDataPoint.y <= searchPoint.y + radius) {
        return 2;
    }
    //Check if region == 3:
    if(normalizedDataPoint.x <= searchPoint.x + radius) {
        return 3;
    }
    //Check if region == 4:
    if (normalizedDataPoint.y <= searchPoint.y + radius) {
        return 4;
    }
    return 5;
}

//Sets a boolean array (searchMarks) which includes where to search given a direction and region.
//region = relative region of a city for a given search center.
//searchMarks' default is all true.
//Indexes of where to search "NE" = 0, "SE" = 1, "SW" = 2, "NW" = 3.
//Example: setting searchMarks [True, True, False, True] means search will be continued for directions: "NE", "SE", "NW".
template<class type>
void PointQuadTree<type>::setSearchMarks(int &directionCode, int &region, vector<bool> &searchMarks) {
    //If region = 1 all directions must be searched, do nothing.
    if (region == 1) {
        return;
    }
    if(region == 2) {
        searchMarks[directionCode] = false;
    }
    else if (region == 3) {
        searchMarks[directionCode] = false;
        searchMarks[(directionCode + 3) % 4] = false;
    }
    else if (region == 4) {
        searchMarks[directionCode] = false;
        searchMarks[(directionCode + 1) % 4] = false;
    }
    else {
        for (int i = 0; i < 4; i++) {
            searchMarks[i] = false;
        }
        searchMarks[(directionCode + 2) % 4] = true;
    }
}

//Recursively searches which dataPoints are in search area for a given center point = center and radius = radius.
//Records found data in pointFoundToData.
template<class type>
void PointQuadTree<type>::searchAreaHelper(point &searchCenter, int &radius, map<point, type> &pointFoundToData, PointQuadTreeNode<type>* &_root) {
    if(_root == NULL) return; //If _root is NULL no further search is required.
    
    //Set necessary variables:
    int directionRelativeToCenter = comparePoints(_root -> coordinates, searchCenter);
    point normalizedDataPoint = normalizePoint(_root -> coordinates, searchCenter, directionRelativeToCenter);
    int region = regionRelativeToSearchPoint(searchCenter, normalizedDataPoint, radius);
    vector<bool> searchMarks(4, true);
    setSearchMarks(directionRelativeToCenter, region, searchMarks); //Bool array stores where to search, idx: 0 = ne, 1 = se, 2 = sw, 3 = nw
    
    //Check if city is found:
    if (region == 1) {
        pointFoundToData.insert({_root -> coordinates, _root -> data});
    }

    //Search necessary subtrees recursively:
    if(searchMarks[0]) searchAreaHelper(searchCenter, radius, pointFoundToData, _root -> ne);
    if(searchMarks[1]) searchAreaHelper(searchCenter, radius, pointFoundToData, _root -> se);
    if(searchMarks[2]) searchAreaHelper(searchCenter, radius, pointFoundToData, _root -> sw);
    if(searchMarks[3]) searchAreaHelper(searchCenter, radius, pointFoundToData, _root -> nw);
}

//Public function to search an area and returns a map(point -> type) consisting the points as keys data found as values in the tree.
template<class type>
map<point, type> PointQuadTree<type>::searchArea(point &searchCenter, int &radius) {
    map<point, type> pointFoundToData;
    searchAreaHelper(searchCenter, radius, pointFoundToData, root);
    return pointFoundToData;
}

//Inserts point combined with data into the tree recursively.
template<class type>
void PointQuadTree<type>::insertHelper(point &p, type &_data, PointQuadTreeNode<type>* & _root) {
    if (_root == NULL) {
        _root = new PointQuadTreeNode<type>(p, _data);
    }
    else {
        int desiredDirection = comparePoints(p, _root -> coordinates);
        if (desiredDirection == 0) insertHelper(p, _data, _root -> ne);
        else if (desiredDirection == 1) insertHelper(p, _data, _root -> se);
        else if (desiredDirection == 2) insertHelper(p, _data, _root -> sw);
        else insertHelper(p, _data, _root -> sw);
    }
}

//Public function to insert point combined with data into tree.
template<class type>
void PointQuadTree<type>::insert(point p, type _data) {
    insertHelper(p, _data, root);
}

//Overloaded insert that takes coordinates as integers, then converts to point.
template<class type>
void PointQuadTree<type>::insert(int xCoordinate, int yCoordinate, type _data) {
    point p = point(xCoordinate, yCoordinate);
    insertHelper(p, _data, root);
}
