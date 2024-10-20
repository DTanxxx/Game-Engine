#include "Pathfinder.hpp"
#include <queue>
#include <iostream>
#include <algorithm>
#include <limits>

Pathfinder::Pathfinder() {
	// assume we have a Graph g, we can run BFS between two GraphNodes in the graph
	//NodeToParentMap map;
	//bool found = BFS(g, g.mNodes[0], g.mNodes[9], map);
	// if BFS succeeds, we can reconstruct the path by using the parent pointers in the outMap
	// this would give a path in the direction from goal to start; since we want the opposite
	// direction (from start to goal), we can instead run BFS passing in the goal node as the 
	// "start" parameter and the start node as the "goal" parameter
}

bool Pathfinder::BFS(const Graph& graph, const GraphNode* start, const GraphNode* goal, NodeToParentMap& outMap) {
	// whether we found a path
	bool pathFound = false;

	// nodes to consider
	std::queue<const GraphNode*> q;

	// enqueue the first node
	q.emplace(start);

	while (!q.empty()) {
		// dequeue a node
		const GraphNode* current = q.front();
		q.pop();
		if (current == goal) {
			pathFound = true;
			break;
		}

		// enqueue adjacent nodes that aren't already in the queue
		for (const GraphNode* node : current->mAdjacent) {
			// if the parent is null, it hasn't been enqueued (except for the start node)
			const GraphNode* parent = outMap[node];
			if (parent == nullptr && node != start) {
				// enqueue this node, setting its parent
				outMap[node] = current;
				q.emplace(node);
			}
		}
	}

	return pathFound;
}

bool Pathfinder::GBFS(const WeightedGraph& g, const WeightedGraphNode* start, const WeightedGraphNode* goal, GBFSMap& outMap) {
	// define a vector for the open set
	std::vector<const WeightedGraphNode*> openSet;

	// set current node as the start node
	const WeightedGraphNode* current = start;
	// add current node to the closed set by marking it as closed in the scratch data map
	outMap[current].mInClosedSet = true;
	
	// main loop
	do {
		// add adjacent nodes to open set
		for (const WeightedEdge* edge : current->mEdges) {
			// get scratch data for this node
			GBFSScratch& data = outMap[edge->mTo];

			// consider it only if it's not in the closed set
			if (!data.mInClosedSet) {
				// set the adjacent node's parent edge
				data.mParentEdge = edge;
				if (!data.mInOpenSet) {
					// compute the heuristic (from node to goal) for this node, and add to open set
					data.mHeuristic = ComputeHeuristic(edge->mTo, goal);
					data.mInOpenSet = true;
					openSet.emplace_back(edge->mTo);
				}
			}
		}

		// check if the open set is empty; if so then there are no nodes left to evaluate
		// this happens only if there is no path from start to goal
		if (openSet.empty()) {
			break;  // break out of outer loop
		}

		// there are still nodes in the open set, find the node in there with the lowest heuristic cost and 
		// move it to the closed set - this then becomes the new current node
		auto iter = std::min_element(openSet.begin(), openSet.end(),
			[&outMap](const WeightedGraphNode* a, const WeightedGraphNode* b) {
				return outMap[a].mHeuristic < outMap[b].mHeuristic;
			});

		// set to current and move from open to closed
		current = *iter;
		openSet.erase(iter);
		outMap[current].mInOpenSet = false;
		outMap[current].mInClosedSet = true;
	} while (current != goal);  // loop terminates either when the while condition fails or when the open set is empty

	// GBFS only found a path if the current node equals the goal node
	return (current == goal) ? true : false;
}

bool Pathfinder::AStarSearch(const WeightedGraph& g, const WeightedGraphNode* start, const WeightedGraphNode* goal, AStarMap& outMap) {
	// define a vector for the open set
	std::vector<const WeightedGraphNode*> openSet;

	// set current node as the start node
	const WeightedGraphNode* current = start;
	// add current node to the closed set by marking it as closed in the scratch data map
	outMap[current].mInClosedSet = true;

	// main loop
	do {
		// add adjacent nodes to open set
		for (const WeightedEdge* edge : current->mEdges) {
			const WeightedGraphNode* neighbor = edge->mTo;
			
			// get scratch data for this node
			AStarScratch& data = outMap[neighbor];

			// only check nodes that aren't in the closed set
			if (!data.mInClosedSet) {
				if (!data.mInOpenSet) {
					// not in the open set, so parent must be current
					data.mParentEdge = edge;
					data.mHeuristic = ComputeHeuristic(neighbor, goal);
					// path-cost is the parent's plus cost of traversing edge
					data.mActualFromStart = outMap[current].mActualFromStart + edge->mWeight;
					data.mInOpenSet = true;
					openSet.emplace_back(neighbor);
				}
				else {
					// compute what new path-cost is if current becomes parent
					float newG = outMap[current].mActualFromStart + edge->mWeight;
					if (newG < data.mActualFromStart) {
						// current should adopt this node
						data.mParentEdge = edge;
						data.mActualFromStart = newG;
					}
				}
			}
		}

		// check if the open set is empty; if so then there are no nodes left to evaluate
		// this happens only if there is no path from start to goal
		if (openSet.empty()) {
			break;  // break out of outer loop
		}

		// there are still nodes in the open set, find the node in there with the lowest cost (h + g) and 
		// move it to the closed set - this then becomes the new current node
		auto iter = std::min_element(openSet.begin(), openSet.end(),
			[&outMap](const WeightedGraphNode* a, const WeightedGraphNode* b) {
				return outMap[a].mHeuristic + outMap[a].mActualFromStart < outMap[b].mHeuristic + outMap[b].mActualFromStart;
			});

		// set to current and move from open to closed
		current = *iter;
		openSet.erase(iter);
		outMap[current].mInOpenSet = false;
		outMap[current].mInClosedSet = true;
	} while (current != goal);  // loop terminates either when the while condition fails or when the open set is empty

	// A* only found a path if the current node equals the goal node
	return (current == goal) ? true : false;
}

void Pathfinder::DijkstraSearch(const WeightedGraph& g, const WeightedGraphNode* start, DijkstraMap& outMap) {
	// define a vector for the open set
	std::vector<const WeightedGraphNode*> openSet;

	// set current node as the start node
	const WeightedGraphNode* current = start;
	// add current node to the closed set by marking it as closed in the scratch data map
	outMap[current].mInClosedSet = true;

	// main loop
	do {
		// add adjacent nodes to open set
		for (const WeightedEdge* edge : current->mEdges) {
			const WeightedGraphNode* neighbor = edge->mTo;

			// get scratch data for this node
			DijkstraScratch& data = outMap[neighbor];

			// only check nodes that aren't in the closed set
			if (!data.mInClosedSet) {
				if (!data.mInOpenSet) {
					// not in the open set, so parent must be current
					data.mParentEdge = edge;
					// path-cost is the parent's plus cost of traversing edge
					data.mActualFromStart = outMap[current].mActualFromStart + edge->mWeight;
					data.mInOpenSet = true;
					openSet.emplace_back(neighbor);
				}
				else {
					// compute what new path-cost is if current becomes parent
					float newG = outMap[current].mActualFromStart + edge->mWeight;
					if (newG < data.mActualFromStart) {
						// current should adopt this node
						data.mParentEdge = edge;
						data.mActualFromStart = newG;
					}
				}
			}
		}

		// check if the open set is empty; if so then there are no nodes left to evaluate
		// this happens only if there is no path from start to goal
		if (openSet.empty()) {
			break;  // break out of outer loop
		}

		// there are still nodes in the open set, find the node in there with the lowest path-cost and 
		// move it to the closed set - this then becomes the new current node
		auto iter = std::min_element(openSet.begin(), openSet.end(),
			[&outMap](const WeightedGraphNode* a, const WeightedGraphNode* b) {
				return outMap[a].mActualFromStart < outMap[b].mActualFromStart;
			});

		// set to current and move from open to closed
		current = *iter;
		openSet.erase(iter);
		outMap[current].mInOpenSet = false;
		outMap[current].mInClosedSet = true;
	} while (true);  // loop terminates only when the open set is empty
}

float Pathfinder::ComputeHeuristic(const WeightedGraphNode* node, const WeightedGraphNode* goal) {
	// placeholder heuristic, in practice this will be the Manhattan or Euclidean distance
	return 0.0f;
}

void Pathfinder::TestBFS() {
	Graph g;
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {
			GraphNode* node = new GraphNode;
			g.mNodes.emplace_back(node);
		}
	}

	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {
			GraphNode* node = g.mNodes[i * 5 + j];
			if (i > 0) {
				node->mAdjacent.emplace_back(g.mNodes[(i - 1) * 5 + j]);
			}
			if (i < 4) {
				node->mAdjacent.emplace_back(g.mNodes[(i + 1) * 5 + j]);
			}
			if (j > 0) {
				node->mAdjacent.emplace_back(g.mNodes[i * 5 + j - 1]);
			}
			if (j < 4) {
				node->mAdjacent.emplace_back(g.mNodes[i * 5 + j + 1]);
			}
		}
	}

	NodeToParentMap map;
	bool found = BFS(g, g.mNodes[0], g.mNodes[9], map);
	std::cout << found << '\n';
}

void Pathfinder::TestHeuristic(bool useAStar) {
	WeightedGraph g;
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {
			WeightedGraphNode* node = new WeightedGraphNode;
			g.mNodes.emplace_back(node);
		}
	}

	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {
			WeightedGraphNode* node = g.mNodes[i * 5 + j];
			if (i > 0) {
				WeightedEdge* e = new WeightedEdge;
				e->mFrom = node;
				e->mTo = g.mNodes[(i - 1) * 5 + j];
				e->mWeight = 1.0f;
				node->mEdges.emplace_back(e);
			}
			if (i < 4) {
				WeightedEdge* e = new WeightedEdge;
				e->mFrom = node;
				e->mTo = g.mNodes[(i + 1) * 5 + j];
				e->mWeight = 1.0f;
				node->mEdges.emplace_back(e);
			}
			if (j > 0) {
				WeightedEdge* e = new WeightedEdge;
				e->mFrom = node;
				e->mTo = g.mNodes[i * 5 + j - 1];
				e->mWeight = 1.0f;
				node->mEdges.emplace_back(e);
			}
			if (j < 4) {
				WeightedEdge* e = new WeightedEdge;
				e->mFrom = node;
				e->mTo = g.mNodes[i * 5 + j + 1];
				e->mWeight = 1.0f;
				node->mEdges.emplace_back(e);
			}
		}
	}

	bool found = false;
	if (useAStar) {
		AStarMap map;
		found = AStarSearch(g, g.mNodes[0], g.mNodes[9], map);
	}
	else {
		GBFSMap map;
		found = GBFS(g, g.mNodes[0], g.mNodes[9], map);
	}
	std::cout << found << '\n';
}
