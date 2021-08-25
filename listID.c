#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "listID.h"



int search(int id,ListID *lid){
    if(lid == NULL){
        return 0;
    }

    ListID *temp = lid;
    while(temp != NULL){
        if(temp->id == id){
            //puts("Id exists\n");
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}


void append(int id,ListID **lid){

    if(search(id,*lid)){
        return;
    }

    ListID *node = malloc(sizeof(ListID));
    node->id = id;
    node->next = NULL;

    if(*lid == NULL){
        node->previous = NULL;
        *lid = node;
        return;
    }

    ListID  *temp = *lid;
    while(temp->next != NULL){
        temp = temp->next;
    }
    temp->next = node;
    node->previous = temp;
}

void appendString(char *string,int size,int id,ListID **lid){


    ListID *node = malloc(sizeof(ListID));
    node->filename = strdup(string);
    node->next = NULL;
    node->size = size;
    node->id = id;

    if(*lid == NULL){
        node->previous = NULL;
        *lid = node;
        return;
    }

    ListID  *temp = *lid;
    while(temp->next != NULL){
        temp = temp->next;
    }
    temp->next = node;
    node->previous = temp;
}



void print(ListID *lid){
    ListID *temp = lid;
    if(lid == NULL) return;
    while(temp != NULL){
        printf("Id is %d\n",temp->id);
        temp = temp->next;
    }
}

void destroyList(ListID **lid){
    ListID *node = *lid;
    ListID *todel;

    while(node != NULL){
        todel = node->next;
        free(node);
        node = todel;
    }

    *lid = NULL;
}

int countNodes(ListID *lid){
    ListID *temp = lid;
    int count = 0;
    while(temp != NULL){
        count++;
        temp = temp->next;
    }
    return count;
}

void delete(int id,ListID **lid){

    if (!search(id,*lid)) return;

    ListID *temp = *lid;
    ListID *prev = NULL;

    while (temp != NULL){
        if(temp->id == id){
            printf("%d\n",id);
            temp->next->previous = temp->previous;
            temp->previous->next = temp->next;
            free(temp);
            return;
        }
        temp = temp->next;
    }

}

