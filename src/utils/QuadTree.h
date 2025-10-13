#ifndef QUADTREE_H
#define QUADTREE_H
#include <vector>
#include <array>
#include <algorithm>
#include "constants.h"
#include "Vec.h"


/**
 * @brief Axis-Aligned Bounding Box (AABB) for 2D space.
 *
 * Represents a rectangle whose sides are always parallel to the axes.
 * Used for fast spatial queries and collision checks.
 * Members:
 *   - x, y: coordinates of the top-left corner
 *   - w, h: width and height
 */
struct AABB {
	float x, y, w, h;

	/**
	 * @brief Check if a point is inside this AABB.
	 * @param px X coordinate of the point
	 * @param py Y coordinate of the point
	 * @return true if the point is inside the box
	 */
	bool contains(float px, float py) const {
		// Check if px, py is within the bounds of this box
		return px >= x && px < x + w && py >= y && py < y + h;
	}

	/**
	 * @brief Check if this AABB intersects with another AABB.
	 * @param other The other AABB
	 * @return true if the two boxes overlap
	 */
	bool intersects(const AABB& other) const {
		// If one box is to the left/right or above/below the other, they do not intersect
		return !(x + w < other.x || x > other.x + other.w ||
				 y + h < other.y || y > other.y + other.h);
	}
};

/**
 * @brief QuadTree for 2D spatial partitioning (pointer-free, vector-based).
 *
 * Efficiently stores and queries objects in 2D space by recursively subdividing regions.
 * Template parameter T is the type of data stored at each point.
 */
template<typename T>
class QuadTree {
public:
	/**
	 * @brief Item stored in the QuadTree.
	 * Contains a position (x, y) and associated data.
	 */
	struct Item {
		float x, y; ///< Position of the item
		T data;     ///< User data
	};

	/**
	 * @brief Construct a QuadTree covering a given region.
	 * @param boundary The region covered by the root node
	 * @param capacity Max items per node before subdivision
	 * @param maxDepth Max depth of the tree
	 */
	QuadTree(const AABB& boundary, int capacity = 4, int maxDepth = 8)
		: m_boundary(boundary), m_capacity(capacity), m_maxDepth(maxDepth) {
		// Pre-allocate space for nodes for performance
		m_nodes.reserve(1 << (2 * maxDepth));
		// Add root node
		m_nodes.push_back(Node{boundary, {}, {}, false, 0});
	}

	/**
	 * @brief Remove all items from the QuadTree.
	 * Resets to a single root node.
	 */
	void clear() {
		m_nodes.clear();
		m_nodes.push_back(Node{m_boundary, {}, {}, false, 0});
	}

	/**
	 * @brief Insert an item at (x, y) with associated data.
	 * @param x X coordinate
	 * @param y Y coordinate
	 * @param data Data to store
	 */
	void insert(float x, float y, const T& data) {
		insert(0, x, y, data, 0);
	}

	/**
	 * @brief Query all items within a given range.
	 * @param range The AABB to search within
	 * @param found Output vector for found items
	 */
	void query(const AABB& range, std::vector<Item>& found) const {
		query(0, range, found);
	}

private:
	/**
	 * @brief Node in the QuadTree.
	 * Each node covers a region and may have up to 4 children.
	 * Children are referenced by index in m_nodes (not pointers).
	 */
	struct Node {
		AABB boundary;                 ///< Region covered by this node
		std::vector<Item> items;       ///< Items stored in this node
		std::array<int, 4> children = {-1, -1, -1, -1}; ///< Indices of child nodes (-1 if none)
		bool divided = false;          ///< Has this node been subdivided?
		int depth = 0;                 ///< Depth in the tree (root is 0)
	};

	std::vector<Node> m_nodes; ///< All nodes in the tree (flat vector)
	AABB m_boundary;           ///< Boundary of the root node
	int m_capacity;            ///< Max items per node before subdivision
	int m_maxDepth;            ///< Max allowed tree depth

	/**
	 * @brief Subdivide a node into 4 children (NW, NE, SW, SE).
	 * @param idx Index of the node to subdivide
	 */
	void subdivide(int idx) {
		Node& node = m_nodes[idx];
		float x = node.boundary.x, y = node.boundary.y;
		float hw = node.boundary.w / 2.0f, hh = node.boundary.h / 2.0f;
		// Create 4 children: (0: NW, 1: NE, 2: SW, 3: SE)
		for (int i = 0; i < 4; ++i) {
			float nx = x + (i & 1) * hw; // 0 or hw
			float ny = y + (i >> 1) * hh; // 0 or hh
			AABB childAABB{nx, ny, hw, hh};
			// Add new child node to m_nodes
			m_nodes.push_back(Node{childAABB, {}, {-1, -1, -1, -1}, false, node.depth + 1});
			node.children[i] = (int)m_nodes.size() - 1; // Store index of child
		}
		node.divided = true;
	}

	/**
	 * @brief Recursive insert helper.
	 * @param idx Index of current node
	 * @param x X coordinate
	 * @param y Y coordinate
	 * @param data Data to store
	 * @param depth Current depth in the tree
	 */
	void insert(int idx, float x, float y, const T& data, int depth) {
		Node& node = m_nodes[idx];
		// Ignore if point is outside this node's region
		if (!node.boundary.contains(x, y)) return;
		// If there's room or we've hit max depth, store item here
		if (node.items.size() < m_capacity || depth >= m_maxDepth) {
			node.items.push_back(Item{x, y, data});
			return;
		}
		// Otherwise, subdivide if needed and pass item to children
		if (!node.divided) subdivide(idx);
		for (int i = 0; i < 4; ++i) {
			if (node.children[i] != -1)
				insert(node.children[i], x, y, data, depth + 1);
		}
	}

	/**
	 * @brief Recursive query helper.
	 * @param idx Index of current node
	 * @param range Query region
	 * @param found Output vector for found items
	 */
	void query(int idx, const AABB& range, std::vector<Item>& found) const {
		const Node& node = m_nodes[idx];
		// If this node's region doesn't intersect the query, skip
		if (!node.boundary.intersects(range)) return;
		// Check all items in this node
		for (const auto& item : node.items) {
			if (range.contains(item.x, item.y))
				found.push_back(item);
		}
		// Recurse into children if subdivided
		if (node.divided) {
			for (int i = 0; i < 4; ++i) {
				if (node.children[i] != -1)
					query(node.children[i], range, found);
			}
		}
	}
};
#endif // QUADTREE_H
