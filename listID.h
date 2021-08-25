//#ifndef PROJECT_LISTID_H
//#define PROJECT_LISTID_H

#include <stdlib.h>
#include <stdio.h>

typedef struct ListID{
    int id;
    char *filename;
    int size;
    struct ListID *next;
    struct ListID *previous;
} ListID;

int search(int ,ListID *);
void append(int ,ListID **);
void print(ListID *);
void delete(int,ListID **);
void destroyList(ListID **);
void appendString(char *,int,int,ListID **);


//#endif //PROJECT_LISTID_H