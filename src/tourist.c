#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 128
#define MAX_TYPE_LEN 32

typedef struct Place{
    char name[MAX_NAME_LEN];
    char type[MAX_TYPE_LEN];
    double rating;
    double distance_km;
    struct Place* next;
}Place ;

Place* head=NULL;

void print_place(Place* p) {
    printf("\nName: %s\nType: %s\nDistance: %.2f km\nRating: %.1f\n",
           p->name, p->type, p->distance_km, p->rating);
}

// display places
void display(){
    if(head==NULL){
        printf("No places found\n");
        return;
    }
    for(Place* curr = head; curr != NULL; curr = curr->next){
        print_place(curr);
    }
}

// sort by rating and distance
void sort_places(int choice) {
    if (head == NULL || head->next == NULL) {
        printf("No places available to sort.\n");
        return;
    }

    Place *i, *j;
    char tempName[MAX_NAME_LEN], tempType[MAX_TYPE_LEN];
    double tempDist, tempRating;

    for (i = head; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            int shouldSwap = 0;
            // Sort by distance (ascending)
            if (choice == 1 && i->distance_km > j->distance_km) {
                shouldSwap = 1;
            }
            // Sort by rating (descending)
            else if (choice == 2 && i->rating < j->rating) {
                shouldSwap = 1;
            }

            if (shouldSwap) {
                strcpy(tempName, i->name);
                strcpy(tempType, i->type);
                tempDist = i->distance_km;
                tempRating = i->rating;

                strcpy(i->name, j->name);
                strcpy(i->type, j->type);
                i->distance_km = j->distance_km;
                i->rating = j->rating;

                strcpy(j->name, tempName);
                strcpy(j->type, tempType);
                j->distance_km = tempDist;
                j->rating = tempRating;
            }
        }
    }
    if (choice == 1)
        printf("\nSorted by distance (nearest → farthest)\n");
    else if (choice == 2)
        printf("\nSorted by rating (highest → lowest)\n");
    else
        printf("\nInvalid choice.\n");
}

