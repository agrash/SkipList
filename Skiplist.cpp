#ifndef LVS_SKIPLIST_HPP
#define LVS_SKIPLIST_HPP

#include <vector>
#include <random>
#include <stdexcept>

namespace lsm {

	template<typename key_type, typename val_type>
	class SkipList {
	private:
		struct Node
		{
			key_type key;
			val_type val;

			std::vector<Node*> next;

			Node(key_type key, val_type val, size_t levels) : key(std::move(key)), val(std::move(val)), next(levels + 1, nullptr) {
			}
		};

		const size_t MAX_LVL = 20;
		const double PROBABILITY = 0.5;
		std::mt19937 gen;
		std::geometric_distribution<int> dist;

		Node* header;

		Node* getHeader() {
			return header;
		}
	 
		std::vector<Node*> getPredecessors(key_type key) {
			Node* curr = header;
			int curr_level = MAX_LVL;

			std::vector<Node*> res(MAX_LVL + 1);

			while (curr_level >= 0) {
				if (curr->next[curr_level] && curr->next[curr_level]->key < key) {
					curr = curr->next[curr_level];
				}
				else {
					res[curr_level] = curr;
					--curr_level;
				}
			}

			return res;
		}


		size_t calcNodeLevel() {
			int res = dist(gen);
			if (res < MAX_LVL) {return res;}
			return MAX_LVL;
		}

	public:

		SkipList() : gen(std::random_device{}()), dist(PROBABILITY) {
			header = new Node({}, {}, MAX_LVL);
		}

		class iterator {
		public:
			Node* ptr;

			iterator(Node* ptr) : ptr(ptr) {}

			val_type& operator*() {
				return ptr->val;
			}

	        Node* operator->() {
	            return ptr;
	        }

			iterator operator++() {
				if (!ptr) {throw std::domain_error("Accessing out of bounds memory.");}
				return iterator(ptr->next[0]);
			}

			bool operator==(const iterator& other) const{
				return ptr == other.ptr;
			}

			bool operator!=(const iterator& other) const {
	            return !(*this == other);
	        }

		};

		iterator begin() {
			return iterator(header->next[0]);
		}

		iterator end() {
			return iterator(nullptr);
		}

		iterator search(key_type key) {
			Node* req = getPredecessors(key)[0]->next[0];
			if (req && key == req->key) {
				return iterator(req);
			}

			return end();
		}

		bool remove(key_type key) {
			auto predecessors = getPredecessors(key);
			Node* curr = predecessors[0]->next[0];

			if (!curr || curr->key != key) {
				return false;
			}

			int levels = curr->next.size() - 1;

			for (int i=levels; i>=0; --i) {
				predecessors[i]->next[i] = curr->next[i];
			}

			delete curr;

			return true;
		}

		void insert(key_type key, val_type val) {
			auto predecessors = getPredecessors(key);
			Node* next = predecessors[0]->next[0];

			if (next && key == next->key) {
				next->val = std::move(val);
			}
			else {
				size_t max_level = calcNodeLevel();
				Node* to_add = new Node(key, val, max_level);

				for (size_t i=0; i<=max_level; ++i) {
					to_add->next[i] = predecessors[i]->next[i];
					predecessors[i]->next[i] = to_add;
				}
			}
		}
	};

}

#endif