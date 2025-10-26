#ifndef TOURIST_H
#define TOURIST_H

typedef struct Place {
    int id;
    char name[128];
    char type[64];
    double rating;
    double distance; // km
    struct Place *next;
} Place;

Place* load_places_from_csv(const char *csvfile);
void free_places(Place *head);
void print_places(Place *head);
Place* sort_places_by_distance(Place *head);
Place* sort_places_by_rating(Place *head);
Place* filter_places_by_type(Place *head, const char *type);

#endif // TOURIST_H
