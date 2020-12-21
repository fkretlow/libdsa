/*************************************************************************************************
 *
 * graph.c
 *
 * Implementation of the generic graph data structure declared in graph.h.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

/* edgenode *edgenode_new(const graph *G, unsigned t, const void *v)
 * Create a new edge with the target vertex t and the value v (if given) on the heap and return a
 * pointer to it, or NULL on error. Enough memory is requested to store the edge header and zero
 * or one value objects according to the type interfaces stored in G. */
edgenode *edgenode_new(const graph *G, unsigned t, const void *v)
{
    assert(G);
    assert(!v || G->edge_data_type);

    edgenode *e = calloc(1, edgenode_size(G));
    check_alloc(e);

    e->vertex = t;

    if (v) {
        t_copy(G->edge_data_type, edgenode_data(G, e), v);
        e->has_data = true;
    }

    return e;
error:
    return NULL;
}

/* void edgenode_delete     (graph *G, edgenode *e)
 * void edgenode_delete_rec (graph *G, edgenode *e)
 * Delete the edge e, freeing any associated memory. Use the recursive version to destroy the
 * whole list. */
void edgenode_delete(graph *G, edgenode *e)
{
    assert(G);

    if (e->has_data && G->edge_data_type)
        t_destroy(G->edge_data_type, edgenode_data(G, e));
    }
    free(e);
}

void edgenode_delete_rec(graph *G, edgenode *e)
{
    if (e->next) edgenode_delete_rec(G, e->next);
    edgenode_delete(G, e);
}

int graph_initialize(graph *G, bool directed, t_intf *edt, t_intf *vdt)
{
    check_ptr(G);

    int rc;
    rc = vector_initialize(G->edges, );
    /* TODO: how to hande vector of edge nodes? destructor? */

    if (vdt) {
        rc = vector_initialize(G->vertices, vdt);
        check_rc(rc, "vector_initialize");
    }

    return 0;
error:
    return -1;
}
