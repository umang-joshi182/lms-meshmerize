# Task 8: Maze-Solving Algorithms

## 1. DFS vs wall-preference rule

| | Left-hand rule (wall preference) | DFS (depth-first search) |
|---|---|---|
| Memory | None. Fixed priority at each junction (left, straight, right, else U-turn). | Visited-junction set plus the path from the start. |
| Loops in the maze | Has no record of where it has been, so it can circle a loop indefinitely. | Never re-enters a visited junction, so loops cannot trap it. |
| Produces a map | No | Yes (nodes and edges are recorded as it goes) |
| Implementation | Very simple, almost no code | More code: junction identification, position tracking, backtracking |
| Finds a route to the goal | Only if the goal is reachable by following the rule | Always, if a route exists (it eventually visits every reachable junction) |

**Choice for an unknown maze: DFS.** The maze is unknown, so the safe choice is the method that is guaranteed to find the goal if one exists and cannot be trapped by loops. DFS also records the map, which the replay phase needs. The wall rule is smaller and faster to implement, but it gives no guarantee on mazes with loops and builds no map. Assumption: the maze is a grid with uniform cell spacing, so the robot can identify a junction by dead reckoning (position and heading).

## 2. Example maze and graph mapping

```
                D  [dead end]
                |
 S ------------ A ------------ B ------------ G  [goal]
 [start]        |              |
                C ------------ E
```

- **Nodes** are the start, the goal, every junction (a point where more than one exit exists) and every dead end: S, A, B, C, D, E, G.
- **Edges** are the line segments between two nodes: S-A, A-D, A-B, A-C, B-G, B-E, C-E.
- The edges A-B, B-E, E-C and C-A form a **loop**. This is exactly the case where a memoryless wall rule can go round in circles and where DFS needs its visited set.
- Corners that have a single continuing exit are not nodes: the robot just keeps following the line.

## 3. Worked DFS traversal

Exit priority at every junction: left, straight, right (the order is arbitrary but fixed). The robot arrives at A heading east.

| Step | Robot at | Exits (L, S, R) | Action |
|---|---|---|---|
| 1 | S | only east | Move to A. Mark S, A visited. |
| 2 | A (heading east) | left = D, straight = B, right = C | Try left first: go to D. |
| 3 | D | none | Dead end: U-turn, backtrack to A. D is marked as a dead end. |
| 4 | A (heading south after return) | next unvisited: B | Go to B. |
| 5 | B (heading east) | straight = G, right = E | Try straight first: go to G. |
| 6 | G | goal | Goal found. Stop. |

Visit order: S, A, D, A, B, G.
The path stack at the moment the goal is found is S, A, B, G (the D detour was popped when the robot backtracked), so the route to replay is: S to A, A to B, B to G.

**Loop handling:** if the goal were not at G, the robot would go back to B and take the right exit to E, then to C. At C the exit leading to A would point to an already visited node, so DFS treats it as a loop edge, does not enter it, and backtracks. That is how DFS avoids going round the A-B-E-C loop.

**Limitation:** DFS returns the first route it finds, not necessarily the shortest. For example, if the goal were C, DFS would reach it through S, A, B, E, C even though S, A, C is shorter. Getting the optimal route needs the full map (see optional extension below).

## 4. Pseudocode for the chosen approach (DFS with backtracking)

```
visited = empty set
route   = empty list        # turns on the current path from the start

function explore(node, heading):
    visited.add(node)
    if is_goal(node):
        return FOUND

    for dir in [LEFT, STRAIGHT, RIGHT]:           # fixed priority
        if exit_exists(node, heading, dir):
            next = neighbour(node, heading, dir)
            if next not in visited:
                record_edge(node, next)
                turn(dir)
                follow_line_to_next_junction()    # PD line follower between junctions
                route.append(dir)
                if explore(next, heading_after(heading, dir)) == FOUND:
                    return FOUND
                # dead end or subtree exhausted: go back
                u_turn()
                follow_line_to(node)
                route.pop()                       # drop this branch from the route
                heading = restore_heading(node)

    return NOT_FOUND
```

## 5. Exploration vs optimal-route replay

- **Exploration (unknown maze):** the robot does not know which exit leads to the goal, so it must decide at every junction, remember visited junctions, handle dead ends and backtrack. It must also run slowly enough to detect junction shapes reliably.
- **Replay (known map):** the route from start to goal is already stored as a list of turns (the `route` list above, with dead-end branches already removed). The robot only has to follow the line and, at the Nth junction, execute the Nth stored turn. There is nothing to search, no visited set, no backtracking and no ambiguity, so the logic is a simple lookup. Because there is no decision-making, the robot can also run at a higher speed.
- **Optional extension:** after exploring the whole maze, run a shortest-path search (BFS for equal edge lengths, Dijkstra if turns or segment lengths have different costs) on the recorded graph, and replay that route instead of the first-found one.

## 6. Assumptions and limitations

- Grid maze with uniform cell spacing; junction identity comes from dead reckoning (cell count and heading), which can drift without encoders.
- The example maze and traversal are worked by hand; DFS has not been run on a physical maze.
- Junction detection (reading the exits from the sensor array) is not implemented in the current sketch; the line follower from Task 9 handles the segments between junctions.