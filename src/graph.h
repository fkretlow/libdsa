/*************************************************************************************************
 *
 * graph.h
 *
 * Interface for a generic graph data structure that supports different payloads for vertices and
 * edges by way of type interface structs.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#ifndef _graph_h
#define _graph_h

#include <stdbool.h>

#include "type_interface.h"
#include "vector.h"

struct edgenode;
typedef struct edgenode {
    struct edgenode *next;
    unsigned vertex;
    bool has_data;
} edgenode;

typedef struct graph {
    vector edges;
    vector vertices;
    bool directed;
    t_intf *edge_data_type;
    t_intf *vertex_data_type;
} graph;

#define edgenode_data_size(G)   ((G)->edge_data_type ? t_size((G)->edge_data_type) : 0)
#define edgenode_size(G)        (sizeof(edgenode) + edgenode_data_size(G))
#define edgenode_data(G, e)     (((char*)(e)) + sizeof(edgenode))

int     graph_initialize    (graph *G, bool directed, t_intf *edt, t_intf *vdt);
int     graph_new           (          bool directed, t_intf *edt, t_intf *vdt);
void    graph_destroy       (graph *G);
void    graph_delete        (graph *G);

int     graph_insert_vertex (graph *G,                             const void *v);
int     graph_insert_edge   (graph *G, unsigned from, unsigned to, const void *v);

void    graph_bfs           (graph *G, unsigned start,
                             void (*process_vertex_early)(void*),
                             void (*process_vertex_late) (void*),
                             void (*process_edge)        (edgenode*));

void    graph_dfs           (graph *G, unsigned start,
                             void (*process_vertex_early)(void*),
                             void (*process_vertex_late) (void*),
                             void (*process_edge)        (edgenode*))

#endif /* _graph_h */
