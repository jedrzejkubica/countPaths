# countPaths

problem:

adjacency matrix -> 1 if there is an edge between nodes i and j, 0 otherwise

adjacency matrix ** k -> the number of walks between nodes i and j
walk: a sequence of edges which join the sequence of vertices (edges and vertices can be visited twice)

1-2-4-5-3

i=1, j=5

one walk of d = 3

several walks of d = 5

**path:** a sequence of edges which join the sequence of vertices (edges and vertices cannot be visited twice)

desired outcome:

adjacency matrix ** k -> the number of **paths** between nodes i and j

note: keep paths of length > the shortest path


idea:

start from a node, move to another node, count the number of neighbors excluding the previous node, repeat for all nodes until k==d or convergence (?)
