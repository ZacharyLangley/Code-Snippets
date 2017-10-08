//--------------------------------------------------------------
// Author:     Zachary Langley
// Date:       11/22/2016
// Instructor: Thompson
// Assignment: Dynamic Hashtable Application
// Purpose:    Take in random.txt and organize information
//             into a hashtable
//--------------------------------------------------------------

#include "LinkedList.h"

class HashTable
{
private:

    LinkedList * list; // linked list

    int hashLength; // size of the hash table

    int HashFunction(int hashKey); //returns the hash value

public:

	//Constructor
    HashTable(int hashTableLength = 500); // Set by default
	//Don't forget to update the HashTable value

    void insert(Node *newNode); // hash insert

    bool remove(int hashKey); // hash delete

    Node *lookUp(int hashKey); // hash lookup

    int getLength(); // returns number of locations

    int getNumberOfNodes(); // returns number of nodes in the hash table

    void printBucketValues(); // detailed print of the hash table

    void printHistogram(); // higher-level view of the table

	//De-constructor
    ~HashTable();
};
