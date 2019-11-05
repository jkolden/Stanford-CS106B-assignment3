/*
John Kolden, SCPD
Standford University CS106B
Filename: ngrams.cpp
Assignment 3
April 24, 2015
Section Leader: Sarah Spikes

Purpose of Program:
This program contains source code for the execution of several algorithms that make use of recursion.
*/

#include "recursionproblems.h"
#include <cmath>
#include <iostream>
#include "hashmap.h"
#include "map.h"
#include "random.h"
#include "string.h"
#include "windows.h"
#include "gtypes.h"
#include "strlib.h"
#include "simpio.h"
#include "filelib.h"
#include "random.h"
#include "set.h"

using namespace std;

//function prototypes:
int floodFillHelper(GBufferedImage&, int, int, int, int, int&);
int stringToIntHelper(string);
bool symbolHasChildRules(string, Map<string, Vector<string> >&);
void generateLanguageHelper (string&, Map<string, Vector<string> >&, string&);

int countKarelPaths(int street, int avenue) {

    // optional - not included

    return 0;

}

//This function accepts a text string of numbers and converts them into an integer:
int convertStringToInteger(string exp) {

    if (exp.find_first_not_of("0123456789-") != string::npos) {
        throw "Invalid integer string! (threw an exception)";
    }

    if (exp.substr(0,1) == "-") {//test if string is negative
        return -stringToIntHelper(exp.substr(1, exp.length() - 1));
    } else {
        return stringToIntHelper(exp);
    }
    return 0;
}

//This function accepts a string of parens, curly braces, brackets and angle braces and
//determines if the set is balanced.
bool isBalanced(string exp) {

    if (exp == "") {
        return true;
    } else {
        int found = exp.find("()");
        if (found != -1) {
            exp.erase(found, 2);
            return isBalanced(exp);
        } else {
            found = exp.find("[]");
            if (found != -1) {
                exp.erase(found, 2);
                return isBalanced(exp);
            } else {
                found = exp.find("{}");
                if (found != -1) {
                    exp.erase(found, 2);
                    return isBalanced(exp);
                } else {
                    found = exp.find("<>");
                    if (found != -1) {
                        exp.erase(found, 2);
                        return isBalanced(exp);
                    }
                }
            }

        }
    }

    return false;

}

//This function accepts a multi-dimensional vector of weights the row/column
//position of each weight and determines the weight the each position in the
//pyramid must bear.
double weightOnKnees(int row, int col, Vector<Vector<double> >& weights) {

    //If the row/column passed is outside the bounds of the vector, return 0.
    if (row > weights.size() - 1 || col > weights[row].size() - 1) {
        return 0;
    }

    double myWeight = weights[row][col];

    if (row == 0) {//base case. if there is no one above just return the weight
        return myWeight;
    } else {

        if (col == 0) {//if there is someone on left shoulder only:
            return myWeight + weightOnKnees(row - 1, col, weights)/2;
        } else {
            if (col == weights[row].size() - 1) {//if there is someone on right shoulder only:
                return myWeight + weightOnKnees(row - 1, col - 1, weights)/2;
            } else {//there is someone on both shoulders
                return myWeight + weightOnKnees(row - 1, col, weights)/2
                        + weightOnKnees(row - 1, col - 1, weights)/2;
            }
        }
    }

    return 0.0;

}

//This function accepts a reference to a graphical window and beginning x,y coordinates,
//the triangle max size and the Sierpinski order and draws a Sierpinki pattern.
void drawSierpinskiTriangle(GWindow& gw, double x, double y, int size, int order) {

    if (order < 0 || x < 0 || y < 0 || size < 0) {
        throw "Invalid parameters passed! (threw an exception)";
    }

    double height = sqrt(3)*(size/2);
    double length = size + x; //i.e. if size is 400 but it starts at 150, triangle should go from x150 to x550

    if (order == 0) {//base case, draw one triangle
        gw.drawLine(x,
                    y,
                    length,
                    y);

        gw.drawLine(x,
                    y,
                    (x+length)/2,
                    height + y);

        gw.drawLine((x+length)/2,
                    height + y,
                    length,
                    y);

    } else {//Recursive case: Draw three triangles to form a Siepinski four-triangle pattern:
        drawSierpinskiTriangle(gw, //upper left
                               x,
                               y,
                               size/2,
                               order-1 );

        drawSierpinskiTriangle(gw, //upper right
                               (size/2)+x,
                               y,
                               size/2,
                               order-1 );

        drawSierpinskiTriangle(gw, //bottom center
                               (size/4)+x,
                               (height/2)+y,
                               size/2,
                               order-1 );
    }
}

//This function accepts a reference to an image, x/y coordinates and a target color
//and allows the user to fill each selected shape with the target color.
int floodFill(GBufferedImage& image, int x, int y, int color) {

    if (x < 0 || y < 0 ) {
        throw "Invalid parameters passed! (threw an exception)";
    }

    int targetColor = image.getRGB(x,y);
    int counter = floodFillHelper(image, x, y, color, targetColor,counter);
    return counter;
}

//This function accepts an open file, a grammer language symbol and an integer
//for the number of sentences and then generate random sentences uing the rules
//of the grammar file.
Vector<string> grammarGenerate(istream& input, string symbol, int times) {

    string line;
    Map<string, Vector<string> > map;
    Vector<string> v;
    Set<string> keysAlreadyRead;

    while (!input.eof()) {
        getline(input,line);
        Vector<string> fileText = stringSplit( line, "::=" );
        Vector<string> values = stringSplit( fileText[1], "|" );


        if (!keysAlreadyRead.contains(fileText[0])) {
            map.put(fileText[0], values);
        } else {
            throw "invalid input";
        }

        keysAlreadyRead.add(fileText[0]);

    }


    for (int j = 0; j < times; j++) {
        string sentence;

        if (map.containsKey(symbol)) {//if symbol has a map key, then it has children so call the recursion function.
            generateLanguageHelper(symbol, map, sentence);
            v.add(trim(sentence)) ;
        } else {//else just assume it's a terminal and return the symbol.
            v.add(symbol);
        }
    }

    return v;
}

//This is flood fill helper that tracks the color of the current cell so that it can be compared to
//the replacement color.
int floodFillHelper(GBufferedImage& image, int x, int y, int replacementColor, int targetColor, int& counter ) {

    //algorithm from http://en.wikipedia.org/wiki/Flood_fill#Stack-based_recursive_implementation_.28Four-way.29

    int colorOfCurrentfNode = image.getRGB(x,y);

    if (targetColor == replacementColor) {
        return 0;
    } else {
        if (colorOfCurrentfNode != targetColor) { //base case; cell is already target color so do nothing
            return 0;
        } else {
            image.setRGB(x,y,replacementColor);//Recursive case: Set this node color to our replacement value
            counter += 1;

            if (image.inBounds(x-1,y)) {//one step to the east
                floodFillHelper(image, x-1, y, replacementColor, targetColor, counter);
            }


            if (image.inBounds(x+1,y)) {//one step to the west
                floodFillHelper(image, x+1, y, replacementColor, targetColor,  counter);
            }

            if (image.inBounds(x,y-1)) {//one step to the north
                floodFillHelper(image, x, y-1, replacementColor, targetColor, counter);
            }


            if (image.inBounds(x,y+1)) {//one step to the south
                floodFillHelper(image, x, y+1, replacementColor, targetColor, counter);
            }
        }
    }
    return counter;
}

//This function is a language helper that uses recursion to build a random sentence based on
//rules contained in the grammar file.
void generateLanguageHelper (string& symbol, Map<string, Vector<string> >& map, string& sentence) {

    Vector<string> mapValue = map[symbol];//get the values for this symbol

    if (!symbolHasChildRules(symbol, map)) { //base case, symbol has only terminals
        sentence += mapValue[randomInteger(0, mapValue.size() -1 )] + " ";

    } else { //Recursive case. (mapValue may have multiple rules separated by spaces
        //create a vector of all these and for each one recursively call this function)

        Vector<string> allValues = stringSplit(mapValue[randomInteger(0, mapValue.size() -1 )], " ");

        for (int i = 0; i < allValues.size(); i++) {
            generateLanguageHelper( allValues[i], map, sentence);
        }
    }
}

//This function determines if a grammar rule has child rules or if the children are
//terminals. It returns true if child rules exist.
bool symbolHasChildRules(string symbol, Map<string, Vector<string> >& map )  {

    Vector<string> value = map[symbol]; //get map values for this symbol
    string valueToTest = value[0];//just get first value to test

    //check if string value has multiple elements:
    Vector<string> childrenOfSymbol = stringSplit(valueToTest, " ");

    //test first value:
    string symbolToTest = childrenOfSymbol[0];
    return map.containsKey(symbolToTest); //symbol does not have any child rules
}

//This is helper function that generates an integer based on a string of numbers.
int stringToIntHelper(string exp) {

    if (exp.length() > 0){//recursive case

        char rightCharacter = stringToChar(exp.substr(exp.length()-1, 1));
        return (rightCharacter - '0') +
                (stringToIntHelper(exp.substr(0, exp.length()-1)) * 10);
    }

    return 0;
}









