#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfiles/tourist.h"  // Updated path
#include "cfiles/search.h"   // Updated path

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <list|sort_distance|sort_rating|filter|search> [parameter]\n", argv[0]);
        return 1;
    }
    
    char *command = argv[1];
    Place *places = load_places_from_csv("dehradun_data.csv");
    
    if (!places) {
        printf("Failed to load places\n");
        return 1;
    }
    
    if (strcmp(command, "list") == 0) {
        print_places(places);
    } else if (strcmp(command, "sort_distance") == 0) {
        places = sort_places_by_distance(places);
        print_places(places);
    } else if (strcmp(command, "sort_rating") == 0) {
        places = sort_places_by_rating(places);
        print_places(places);
    } else if (strcmp(command, "filter") == 0 && argc == 3) {
        Place *filtered = filter_places_by_type(places, argv[2]);
        if (filtered) {
            print_places(filtered);
            free_places(filtered);
        }
    } else if (strcmp(command, "search") == 0 && argc == 3) {
        BSTNode *bst = build_bst_from_list(places);
        Place *result = bst_search(bst, argv[2]);
        if (result) {
            printf("Found: %s (%s) Rating: %.1f Dist: %.2f km\n", 
                   result->name, result->type, result->rating, result->distance);
        } else {
            printf("Not found\n");
        }
        bst_free(bst);
    } else {
        printf("Invalid command\n");
    }
    
    free_places(places);
    return 0;
}