//--------------------------------------------------------------
// Author:     Zachary Langley
// Date:       11/22/2016
// Instructor: Thompson
// Assignment: Dynamic Hashtable Application
// Purpose:    Take in random.txt and organize information
//             into a hashtable
//--------------------------------------------------------------

#include <iostream>
#include <string>
#include <stdio.h>

using std::cout;
using std::endl;
using std::string;
//------------------------------------------
//			Student Key (int)
//			Student ID (int)
//			Student GPA (double)
//			Student Major (string)
//			Pointer to the next node
//------------------------------------------
struct Node
{
    int key;
    int stud_ID;
    double stud_GPA;
    string stud_MJR;
    Node *next;
};


class LinkedList
{
private:

    Node *head; // to keep track of the head

    int length; // number of values in the list

public:
	// Constructor
    LinkedList(void);

    void insert(Node *newNode); // insert new node

    bool remove(int NodeKey); // remove a node

    Node *getNode(int NodeKey); // return a node

    int getLength();

    void printList();

    // De-constructor
    ~LinkedList();
};
