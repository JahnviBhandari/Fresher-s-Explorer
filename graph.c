#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

void graph_in(Graph *g) {
    g->vcount = 0;
    for (int i=0;i<MAXV;i++) {
        g->verts[i].id = -1;
        g->verts[i].adj = NULL;
    }
}

int graph_find(Graph *g, const char *name) {
    for (int i=0;i<g->vcount;i++){ 
        if (strcmp(g->verts[i].name, name)==0) {
            return i;
        }
    }
    return -1;
}

int graph_add_vertex(Graph *g, const char *name) {
    int idx = graph_find(g, name);
    if (idx != -1) return idx;
    if (g->vcount >= MAXV) return -1;
    int id = g->vcount;
    g->verts[id].id = id;
    strncpy(g->verts[id].name, name, sizeof(g->verts[id].name)-1);
    g->verts[id].name[sizeof(g->verts[id].name)-1]=0;
    g->verts[id].adj = NULL;
    g->vcount++;
    return id;
}

void graph_add_edge(Graph *g, int u, int v, double w) {
    if (u<0 || v<0 || u>=g->vcount || v>=g->vcount) return;
    Edge *e = malloc(sizeof(Edge));
    e->to = v; e->weight = w; e->next = g->verts[u].adj;
    g->verts[u].adj = e;
    Edge *e2 = malloc(sizeof(Edge));
    e2->to = u; e2->weight = w; e2->next = g->verts[v].adj;
    g->verts[v].adj = e2;
}


void graph_load_from_file(Graph *g, const char *filename) {
    FILE *f = fopen(filename, "r");
    if(!f){
        return;
    }
    char cmd[64], a[128], b[128];
    double w;
    while (fscanf(f, "%63s", cmd) == 1) {
        if (strcmp(cmd, "VERTEX")==0) {
            if (fscanf(f, " %127[^\n]", a) == 1) {
                char *s = a;
                while (*s==' ') s++;
                graph_add_vertex(g, s);
            }
        } else if (strcmp(cmd, "EDGE")==0) {
            if (fscanf(f, " %127s %127s %lf", a, b, &w) == 3) {
                int u = graph_add_vertex(g, a);
                int v = graph_add_vertex(g, b);
                graph_add_edge(g, u, v, w);
            }
        }
        else {
            char rest[256]; fgets(rest, sizeof(rest), f);
        }
    }
    fclose(f);
}

void graph_print(Graph *g) {
    printf("Graph: %d vertices\n", g->vcount);
    for (int i=0;i<g->vcount;i++) {
        printf("%d: %s ->", i, g->verts[i].name);
        Edge *e = g->verts[i].adj;
        while (e) { printf(" (%d,%.2f)", e->to, e->weight); e=e->next; }
        printf("\n");
    }
}

void graph_dijkstra(Graph *g, int src, int dest) {
    if (src<0 || dest<0 || src>=g->vcount || dest>=g->vcount) {
        printf("Invalid source/destination\n"); return;
    }
    int n = g->vcount;
    double dist[MAXV];
    int prev[MAXV];
    int visited[MAXV];
    for (int i=0;i<n;i++) { dist[i]=DBL_MAX; prev[i]=-1; visited[i]=0; }
    dist[src]=0;
    for (int iter=0; iter<n; ++iter) {
        int u=-1; double best=DBL_MAX;
        for (int i=0;i<n;i++) if (!visited[i] && dist[i]<best) { best=dist[i]; u=i; }
        if (u==-1) break;
        visited[u]=1;
        Edge *e = g->verts[u].adj;
        while (e) {
            if (!visited[e->to] && dist[u] + e->weight < dist[e->to]) {
                dist[e->to] = dist[u] + e->weight;
                prev[e->to] = u;
            }
            e = e->next;
        }
    }
    if (dist[dest]==DBL_MAX) {
        printf("No path found from %s to %s\n", g->verts[src].name, g->verts[dest].name);
        return;
    }
    int path[MAXV]; int pc=0;
    int cur = dest;
    while (cur != -1) { path[pc++]=cur; cur = prev[cur]; }
    printf("Shortest path (distance=%.2f):\n", dist[dest]);
    for (int i=pc-1;i>=0;i--) {
        printf("%s", g->verts[path[i]].name);
        if (i) printf(" -> ");
    }
    printf("\n");
}
