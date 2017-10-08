//--------------------------------------------------------------
// Author:     Zachary Langley
// Date:       11/22/2016
// Instructor: Thompson
// Assignment: Dynamic Hashtable Application
// Purpose:    Take in random.txt and organize information
//             into a hashtable
//--------------------------------------------------------------

#include "HashTable.h"
int main()
{
    int val_ID = 0;
    double val_GPA = 0;
    char val_MJR[5] = {0};
    FILE * pFile = NULL; // File pointer
    pFile = fopen("input.txt", "r");
    if (pFile == NULL)
      cout << "The file does not exist" << endl;
    else{

	     //Create new (empty) hash table
        HashTable table;
	cout << "Opening file: input.txt" << endl;

        int i = 0;
        //Dynamically allocated memory for endless amounts of lists
        while(fscanf(pFile, "%d, %lf, %s\n", &val_ID, &val_GPA, val_MJR) != EOF){
             Node * Z = new Node; //Z for Zack, cause im concieted
             Z -> key = i;        //I reuse the node declaration to save space, no need for a giant hardcoded array like in Program 1
             Z -> stud_ID = val_ID;
             Z -> stud_GPA = val_GPA;
             Z -> stud_MJR = val_MJR;
             table.insert(Z); //Inserts into Hashtable
             i++;
	}

        cout << "Students found and inserted successfully";

        cout << "Printing Detailed View (Bucket per Bucket) of the Hash Table with insertered values..." << endl;
        //Detailed View
        table.printBucketValues();
        cout << "Printing High-Level View of the Hash Table";
        //High-Level View
        table.printHistogram();

        fclose(pFile); //Closes file
    }

    return 0;
}
