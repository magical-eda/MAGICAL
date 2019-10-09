# distutils: language = c++
import networkx as nx
import math
from libcpp.map cimport map
from libcpp.vector cimport vector
from libcpp cimport bool
import numpy as np
cimport numpy as np

DTYPE = np.double
ctypedef np.double_t DTYPE_t

cpdef pagerank(G, node_list, r=0.5, d_type='b', max_iter=100, useweight=False):
    sim = dict() 
    for n in node_list:
        sim[n] = dict()
        sim[n][n] = 1
        for x in node_list:
            if x is not n:
                sim[n][x] = 0
    c = 0
    node2ind = dict()
    ind2node = dict()
    for u in node_list:
        node2ind[u] = c
        ind2node[c] = u
        c = c + 1
    g = G.subgraph(node_list)
    neighbor = dict()
    for u in node_list:
        neighbor[node2ind[u]] = list()
        for n_u in g.neighbors(u):
            neighbor[node2ind[u]].append(node2ind[n_u])

    cdef map[int, vector[int]] nei = neighbor
    cdef double[:,:] csim
    cdef int iter_num = max_iter
    cdef int count = c
    cdef double rcount = r
    csim = sim_core(nei, count, iter_num, rcount)
    for i in range(c):
        for j in range(c):
            sim[ind2node[i]][ind2node[j]] = csim[i][j]

    dist = dict()
    for u in node_list:
        dist[u] = dict()
        for v in node_list:
            if d_type == 'b':
                dist[u][v] = b_dist(sim[u].values(), sim[v].values())
            else:
                dist[u][v] = h_dist(sim[u].values(), sim[v].values())
    return dist

cdef double[:,:] sim_core(map[int, vector[int]] nei, int c, int iter_num, double r):
    cdef double[:,:] csim = np.identity(c, dtype=DTYPE)
    cdef double[:,:] csim_old = np.zeros((c,c), dtype=DTYPE)
    cdef int iter_ctr, lenu, cu, cv, cn_u, cn_v
    cdef double s_uv
    cdef double begin
    for iter_ctr in range(iter_num):
        if _is_converge(csim, csim_old):
            break
        csim_old[...] = csim
        for cu in range(c):
            lenu = len(nei[cu])
            for cv in range(c):
                s_uv = 0.0
                for cn_u in nei[cu]:
                    s_uv += csim_old[cn_u][cv]
                csim[cu][cv] = r * s_uv / lenu + (1-r) * (cu==cv)
    return csim
 
cdef bool _is_converge(double[:,:] s1, double[:,:] s2, DTYPE_t eps=1e-4):
    cdef int i,j
    for i in range(s1.shape[0]):
        for j in range(s1.shape[0]):
            if abs(s1[i][j] - s2[i][j]) >= eps:
                return False
    return True

def bc_coeff(p, q):
    m = [math.sqrt(p[i]*q[i]) for i in range(len(p))]
    return sum(m)

def b_dist(p, q):
    if bc_coeff(p,q) <= 0.01:
        return -math.log(0.01) 
    return -math.log(bc_coeff(p,q))

def h_dist(p, q):
    coeff = bc_coeff(p,q)
    if coeff >= 1:
        return 0
    return math.sqrt(1-coeff)


