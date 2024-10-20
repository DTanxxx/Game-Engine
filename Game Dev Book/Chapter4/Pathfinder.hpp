#pragma once

#include <vector>
#include <unordered_map>

// for unweighted graph
struct GraphNode {
	// each node has pointers to adjacent nodes
	std::vector<GraphNode*> mAdjacent;
};

// unweighted graph
struct Graph {
	// use adjacency lists to represent a graph
	// a graph contains nodes
	std::vector<GraphNode*> mNodes;
};

// for weighted graph
struct WeightedEdge {
	// which nodes are connected by this edge?
	struct WeightedGraphNode* mFrom;
	struct WeightedGraphNode* mTo;
	// weight of this edge
	float mWeight;
};

// for weighted graph
struct WeightedGraphNode {
	// each node stores its outgoing edges
	std::vector<WeightedEdge*> mEdges;
};

// weighted graph
struct WeightedGraph {
	std::vector<WeightedGraphNode*> mNodes;
};

// for Greedy Best-First Search
// GBFS uses two sets of nodes during the search: the open set contains the nodes that are under
// consideration; once chosen for evaluation, the node moves into the closed set - nodes in 
// the closed set will not be considered further by the algorithm
// we define here a struct to encapsulate the scratch data for each GBFS node
struct GBFSScratch {
	const WeightedEdge* mParentEdge = nullptr;  // node's incoming edge, used to reconstruct the path from start to goal (if any)
	float mHeuristic = 0.0f;  // node's heuristic value
	bool mInOpenSet = false;  // node's membership in the open set
	bool mInClosedSet = false;  // node's membership in the closed set
};

// for A* Search
// we define a struct that is very similar to GBFSScratch, with an additional field mActualFromStart that
// represents the path-cost value from start to the corresponding node
struct AStarScratch {
	const WeightedEdge* mParentEdge = nullptr;
	float mHeuristic = 0.0f;
	float mActualFromStart = 0.0f;  // path-cost value from start node to this node
	bool mInOpenSet = false;
	bool mInClosedSet = false;
};

// for Dijkstra's Search
// we define a struct that is very similar to AStarScratch, without the mHeuristic field
struct DijkstraScratch {
	const WeightedEdge* mParentEdge = nullptr;
	float mActualFromStart = 0.0f;
	bool mInOpenSet = false;
	bool mInClosedSet = false;
};


// define a type of map where both the key and value are GraphNode pointers
// this is used in BFS search to avoid adding the same node multiple times to the queue (by
// checking this parent map) - a node's parent is null only if the node hasn't been enqueued before
// or it's the start node
using NodeToParentMap = std::unordered_map<const GraphNode*, const GraphNode*>;

// define a type of map where the key is a pointer to the node and the value is an instance of GBFSScratch
using GBFSMap = std::unordered_map<const WeightedGraphNode*, GBFSScratch>;

// define a type of map similar to GBFSMap, but for A* Search
using AStarMap = std::unordered_map<const WeightedGraphNode*, AStarScratch>;

// define a type of map similar to AStarMap, but for Dijkstra's Search
using DijkstraMap = std::unordered_map<const WeightedGraphNode*, DijkstraScratch>;

class Pathfinder {
public:
	Pathfinder();

	// find the shortest path from start node to goal node, if any, in an unweighted graph
	// the path is recorded in outMap parameter
	bool BFS(const Graph& graph, const GraphNode* start, const GraphNode* goal, NodeToParentMap& outMap);

	// search for a path from start node to goal node, guided by the heuristic function only
	// in other words, GBFS finds the path that has the smallest heuristic value sum
	// GBFS does not guarantee a minimal path, since heuristics are only estimates
	bool GBFS(const WeightedGraph& g, const WeightedGraphNode* start, const WeightedGraphNode* goal, GBFSMap& outMap);

	bool AStarSearch(const WeightedGraph& g, const WeightedGraphNode* start, const WeightedGraphNode* goal, AStarMap& outMap);
	
	void DijkstraSearch(const WeightedGraph& g, const WeightedGraphNode* start, DijkstraMap& outMap);

	void TestBFS();
	void TestHeuristic(bool useAStar);

private:
	float ComputeHeuristic(const WeightedGraphNode* node, const WeightedGraphNode* goal);
};