//--------------------------------------------------------------
// Author:     Zachary Langley
// Date:       11/22/2016
// Instructor: Thompson
// Assignment: Dynamic Hashtable Application
// Purpose:    Take in random.txt and organize information
//             into a hashtable
//--------------------------------------------------------------

#include "HashTable.h"

void HashTable::insert(Node *newNode)
{
    //returns open space for node insert
    list[HashFunction(newNode -> key)].insert(newNode); //adds node to the hashtable
}

int HashTable::HashFunction(int hashKey)
{
	//old Hash function
       return (hashKey) % hashLength; // Resource of hash function used

}

//removes node from hashtable
bool HashTable::remove(int hashKey)
{
    //returns a node from the hashtable by key
    return list[HashFunction(hashKey)].remove(hashKey); //returns true if successful

}


Node * HashTable::lookUp(int hashKey)
{
    // returns a node from the hash table by key
    return list[HashFunction(hashKey)].getNode(hashKey); //if node isnt found, return null pointer


}

// Below are functions I used to print my values to the screen
int HashTable::getNumberOfNodes()
{
    int NodeCount = 0;
    for ( int i = 0; i < hashLength; i++ )
    {
        NodeCount += list[i].getLength();
    }
    return NodeCount;
}

//Detailed View
void HashTable::printBucketValues()
{
    cout << "\n\nHash Table Bucket Values:\n";
    for ( int i = 0; i < hashLength; i++ )
    {
        cout << "Bucket " << i + 1 << ": ";
        list[i].printList();
    }
}

//High-level view
void HashTable::printHistogram()
{
    cout << "\n\nHash Table Contains ";
    cout << getNumberOfNodes() << " Nodes total\n";
    for ( int i = 0; i < hashLength; i++ )
    {
        cout << i + 1 << ":\t";
        for ( int j = 0; j < list[i].getLength(); j++ )
            cout << " O";
        cout << "\n";
    }
}

// Constructor
HashTable::HashTable(int hashTableLength)
{
    if (hashTableLength <= 0)
        hashTableLength = 500;
    list = new LinkedList[hashTableLength];
    hashLength = hashTableLength;
}

// De-constructor
HashTable::~HashTable()
{
    delete [] list;
}
