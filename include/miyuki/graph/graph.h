#ifndef MIYUKI_GRAPH_H
#define MIYUKI_GRAPH_H

#include <miyuki.h>
#include <utils/noncopymovable.hpp>

namespace Miyuki {
	namespace Graph {
		class Graph;
		class Node;
		struct Edge {
			Node* from, * to;
			std::string name;
			Edge(Node* from, Node* to, const std::string& name)
				:from(from), to(to), name(name) {}
		};

		class Node : NonCopyMovable {
			friend class Graph;
			Graph* _graph = nullptr;
			std::vector<Edge> _subnodes;
			std::string _name;
		public:
			Node(const std::string& name, Graph* graph = nullptr) :_graph(graph), _name(name) {}
			Graph* graph()const { return _graph; }
			Edge makeEdge(const std::string& name, Node* to) {
				return Edge(this, to, name);
			}

			const Edge& byName(const std::string& name)const {
				return *std::find_if(_subnodes.begin(), _subnodes.end(),
					[&](const Edge & edge) {return edge.name == name; });
			}

			Edge& byName(const std::string& name) {
				return *std::find_if(_subnodes.begin(), _subnodes.end(),
					[&](const Edge & edge) {return edge.name == name; });
			}

			virtual void serialize(json& j)const;
			virtual const char* type()const = 0;
			virtual bool isLeaf()const { return false; }
			const std::string& name()const { return _name; }
		};

		class IDeserializer {
		public:
			virtual Node* deserialize(const json&, Graph * ) = 0;
		};

		class Graph {
			std::vector<Node*> _nodeList;

			// owns the nodes
			std::unordered_map<std::string, std::unique_ptr<Node>> _allNodes;

			std::unordered_map<std::string, std::unique_ptr<IDeserializer>> _deserializers;
			Graph();
			void deserialize(const json&);
		public:
			bool addNode(const std::string&, std::unique_ptr<Node>);
			void serialize(json&);
			static std::unique_ptr<Graph> CreateGraph(const json&);
			void registerDeserializer(const std::string&, std::unique_ptr<IDeserializer>);
		};
	}
}

#endif