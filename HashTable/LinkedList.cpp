//--------------------------------------------------------------
// Author:     Zachary Langley
// Date:       11/22/2016
// Instructor: Thompson
// Assignment: Dynamic Hashtable Application
// Purpose:    Take in random.txt and organize information
//             into a hashtable
//--------------------------------------------------------------

#include "LinkedList.h"
//Constructor
LinkedList::LinkedList()
{
    head = new Node;
    head -> next = NULL;
    length = 0;
}

//insert node at end of list
void LinkedList::insert(Node * newNode)
{
    // TODO
    if(!head -> next){
       head -> next = newNode;
       length++;
       return;
    }
    Node *temp1 = head;
    Node *temp2 = head;
    while(temp2 != NULL){
       temp1 = temp2;
       temp2 = temp1 -> next;
    }
    temp1 -> next = newNode;
    newNode -> next = NULL;
    length++;
}

//remove node from head of list
bool LinkedList::remove(int NodeKey)
{
    // TODO
    if(!head -> next) return false;
    Node *temp1 = head;
    Node *temp2 = head;
    while(temp2 != NULL){
       if(temp2 -> key == NodeKey){
            temp1 -> next = temp2 -> next;
            delete temp2;
            length--;
            return true;
       }
       temp1 = temp2;
       temp2 = temp1 -> next;
    }
   return false;
}

//finds node of linked list
Node * LinkedList::getNode(int NodeKey)
{
    // TODO
    Node *temp1 = head;
    Node *temp2 = head;
    while(temp2 != NULL){
       temp1 = temp2;
       if ((temp1 != head) && (temp1 -> key == NodeKey))
                return temp1;
       temp2 = temp1 -> next;
    }
    return NULL;
}

// Prints the linked list
void LinkedList::printList()
{
    if (length == 0)
    {
        cout << "\n{ }\n";
        return;
    }
    Node *p = head;
    Node *q = head;
    cout << "\n{ " << endl;
    while (q)
    {
        p = q;
        if (p != head)
        {
            cout  << "\t" << "ID: " << p -> stud_ID << ", GPA: " << p -> stud_GPA << ", MJR: " << p -> stud_MJR;
            if (p->next) cout << endl;
            else cout << " ";
        }
        q = p->next;
    }
    cout << "\n}\n";
}

// Get and Return length of the list
int LinkedList::getLength()
{
    return length;
}

// De-Constructor
LinkedList::~LinkedList()
{
    Node *p = head;
    Node *q = head;
    while (q)
    {
        p = q;
        q = p->next;
        if (q) delete p;
    }
}
