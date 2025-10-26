#ifndef GRAPH_H
#define GRAPH_H

#define MAXV 256
#define MAX_NAME_LEN 64

typedef struct Edge {
    int to;
    double weight;
    struct Edge *next;
} Edge;

typedef struct Vertex {
    int id;
    char name[MAX_NAME_LEN];
    Edge *adj;
} Vertex;

typedef struct Graph {
    Vertex verts[MAXV];
    int vcount;
} Graph;

void graph_init(Graph *g);
int graph_add_vertex(Graph *g, const char *name);
void graph_add_edge(Graph *g, int u, int v, double w); // undirected
void graph_load_from_file(Graph *g, const char *filename); // file format described in README
void graph_print(Graph *g);
void graph_dijkstra(Graph *g, int src, int dest);

#endif // GRAPH_H
