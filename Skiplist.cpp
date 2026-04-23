template<typename key_type, typename val_type>
class SkipList {
private:
	struct Node
	{
		key_type key;
		val_type val;

		Node* next;
		Node* prev;

		Node* up;
		Node* down;

		bool header;
		bool sentinel;

		Node(key_type key, val_type val) : key(std::move(key)), val(std::move(val)), next(nullptr), prev(nullptr), up(nullptr), down(nullptr), header(false), sentinel(false) {}
		Node(bool header, bool sentinel) : header(header), sentinel(sentinel), next(nullptr), prev(nullptr), up(nullptr), down(nullptr) {}

		/*bool operator==(const Node& other) const {
			return key == other.key;
		}
		bool operator<(const Node& other) const {
			if (header) {return true;}
			if (sentinel) {return false;}

			return key < other.key;
		}
		bool operator<(const Node* other) const {
			if (header) {return true;}
			if (sentinel) {return false;}

			return key < other->key;
		}*/
	};

	std::vector<Node*> headers;
	Node* sentinel;

	Node* getHeader() {
		return headers.back();
	}

	void addHeader() {
		Node* new_header = new Node(true, false);
		new_header->next = sentinel;
		new_header->down = headers.back();

		headers.back()->up = new_header;
		headers.push_back(new_header);
	}

	Node* lower_bound(key_type key) {
		Node* curr = &headers.back();
		Node* prev;
		while (curr) {
			prev = curr;
			if (equals(key, curr)) {
				while (curr->down) {curr = curr->down;}
				return curr;
			}
			else if (lt(key, curr->next)) {
				curr = curr->down;
			}
			else {curr = curr->next;}
		}

		return prev;
	}

	bool equals(key_type key, const Node* other) const {
		if (other->header || other->sentinel) {return false;}
		return key == other->key;
	}

	bool lt(key_type key, const Node* other) const {
		if (other->header) {return false;}
		if (other->sentinel) {return true;}

		return key < other->key;
	}

	bool flippedHead() {
		return false;
	}

public:

	SkipList() {
		sentinel = new Node(false, true);

		Node* header = new Node(true, false);
		header->next = sentinel;
		headers.push_back(header);
	}

	val_type search(key_type key) {
		Node* req = lower_bound(key);
		if (equals(key, req)) {
			return req->val;
		}
		return {};
	}

	bool remove(key_type key) {
		Node* curr = lower_bound(key);
		if (equals(key, curr)) {
			while (curr) {
				Node* up = curr->up;
				curr->prev->next = curr->next;
				curr->next->prev = curr->prev;
				delete curr;
				curr = up;
			}
			return true;
		}
		return false;
	}

	void insert(key_type key, val_type val) {
		Node* predecessor = lower_bound(key);
		if (equals(key, predecessor)) {
			predecessor->val = std::move(val);
		}
		else {
			Node* to_add = new Node(key, val);
			to_add->next = predecessor->next;
			predecessor->next = to_add;

			to_add->prev = predecessor;
			if (!to_add->next->sentinel) {to_add->next->prev = to_add;}

			Node* prev_level_node = to_add;
			while (flippedHead()) {
				while(!predecessor->up && !predecessor->header) {
					predecessor = predecessor->prev;
				}
				if (predecessor->header && !predecessor->up) {
					addHeader();
					predecessor = getHeader();
				}
				else {
					predecessor = predecessor->up;
				}

				Node* next_level_node = new Node(key, val);
				next_level_node->next = predecessor->next;
				predecessor->next     = next_level_node;

				next_level_node->prev = predecessor;
				if (!next_level_node->next->sentinel) {next_level_node->next->prev = next_level_node;}

				prev_level_node->up   = next_level_node;
				next_level_node->down = prev_level_node;
				prev_level_node       = next_level_node;

				predecessor = next_level_node->prev;
			}
		}
	}

	/*void print() {
        for (int i=headers.size() - 1; i>= 0; --i) {
            Node* curr = headers[i]->next;
            while (!curr->isSentinel()) {
                cout<<'('<<curr->key<<','<<curr->val<<") ";
                curr = curr->next;
            }
            cout<<endl;
        }
    }*/
};