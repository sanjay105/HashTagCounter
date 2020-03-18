#include<iostream>
#include<fstream>
#include<map>
#include<vector>
#include<string>
#include<sstream>
#include<stack>
#include<math.h>
#include<time.h>
using namespace std;
struct fib_Node {
	bool mark;//to keep track the current node have lost a child or not
	int value, degree;//keeps track of node value and degree
	struct fib_Node* left, * right, * parent, * child;//keeps track of left/rigth sibling and parent/child node
	string hashTag;//stores the hashtag of respective node
	fib_Node(int val,string hashTag) {//to initialize the node
		this->value = val; this->degree = 0;
		this->mark = false;
		this->left = this->right = this;
		this->parent = this->child = NULL;
		this->hashTag = hashTag;
	}
};
int no_of_nodes = 0,lines=0;bool fline=true;
struct fib_Node* maxPtr = NULL;//keeps track of maximum valued node in the fibonacci heap
map<string, struct fib_Node*> tags;//stores the pointer of respective hashtag
void linkSiblings(fib_Node* a, fib_Node* b) {//merges two circular linked lists to one circular linked list
	a->right->left = b->left;
	b->left->right = a->right;
	a->right = b;
	b->left = a;
}
fib_Node* maxNode(fib_Node* head) {//returns the maximum valued node int the given sub tree
	if (head == NULL) {
		return NULL;
	}
	int max = head->value; fib_Node* m = head,*temp=head->left;
	while (temp != head) {
		if (max < temp->value) {
			max = temp->value;
			m = temp; 
		}
		temp = temp->left;
	}
	return m;
}
void insert(int val, string tag) {//inserts the new hashtag into heap
	fib_Node* n = new fib_Node(val, tag);
	no_of_nodes++;
	tags[tag] = n;//maps the newly created node to the respective hashtag
	if (maxPtr == NULL) {
		maxPtr = n;
	}
	else {
		linkSiblings(maxPtr, n);
		if (n->value > maxPtr->value) {//if new node value is greater than current maximum node value, maxPtr is updated with new node
			maxPtr = n;
		}
	}
}
fib_Node* meld(fib_Node* a, fib_Node* b) {//melds two heaps of same degree into one heap of degree+1
	if (a->value < b->value) {
		return meld(b, a);
	}
	else {
		a->degree++; a->parent = NULL; b->parent = a;
		if (a->child == NULL) {
			a->child = b;
		}
		else {
			linkSiblings(a->child, b);
			if (b->value > a->child->value) {
				a->child = b;
			}
		}
		return a;
	}
}
void consolidate(vector<fib_Node*>& topNodes) {//consolidates all the top level nodes 
	if (!topNodes.empty()) {
		vector<fib_Node*> Nodes(100, NULL);
		vector<bool> b(100, false);
		maxPtr = topNodes[0];
		b[topNodes[0]->degree] = true; Nodes[topNodes[0]->degree] = topNodes[0];
		for (int i = 1; i < topNodes.size(); i++) {
			fib_Node* a = topNodes[i];
			while (b[a->degree]) {
				b[a->degree] = false;
				a = meld(a, Nodes[a->degree]);//meld function is called to merge two nodes of same degree to single node
			}
			b[a->degree] = true;
			Nodes[a->degree] = a;
		}
		stack<fib_Node*> stack;//stores all the top level nodes that cannot be melded further
		int max = 0;
		for (int i = 0; i < 100; i++) {
			if (b[i]) {
				stack.push(Nodes[i]);
				if (max < Nodes[i]->value) {
					maxPtr = Nodes[i];//modifiying the maxPtr to the maximum valued node
					max = Nodes[i]->value;
				}
			}
		}
		fib_Node* a = stack.top(); stack.pop();
		while (!stack.empty()) {
			fib_Node* x = stack.top(); stack.pop();
			linkSiblings(a, x);
		}
	}
	else {
		maxPtr = NULL;//if there are no nodes in heap then maxPtr is set to null;
	}
}
void removeMax() {//removes the maximum valued node from the heap and maxPtr is pointed to next maximum valued node
	if (maxPtr == NULL) {//if heap is already empty
		cout << "Heap is empty" << endl;
	}
	else {
		tags.erase(maxPtr->hashTag);//removes the mapping of maximum valued node with respective hashtag
		no_of_nodes--;
		vector<fib_Node*> topNodes;//stores the top levelnodes after maxNode removal
		fib_Node* temp;
		if (maxPtr->child != NULL) {
			temp = maxPtr->child;
			topNodes.push_back(temp); temp = temp->right;
			while (temp != maxPtr->child) {
				topNodes.push_back(temp); temp = temp->right;
			}
		}
		temp = maxPtr->left;
		while (temp != maxPtr) {
			topNodes.push_back(temp); temp = temp->left;
		}
		for (int i = 0; i < topNodes.size(); i++) {
			topNodes[i]->left = topNodes[i]->right = topNodes[i];
			topNodes[i]->parent = NULL; topNodes[i]->mark = false;
		}
		consolidate(topNodes);//consolidate function is called to rebuild the heap with maxPtr pointed to new maximum valued node
	}
}
string topXNodes(int x) {//returns top X valued hashtags (comma seperated)
	string res = "";
	vector<pair<string, int> > list;//stores top X node pointers
	for (int i = 0; i < x; i++) {//removes the top X nodes from the heap
		pair<string, int> p;
		if (maxPtr == NULL) {
			break;
		}
		else {
			p.first = maxPtr->hashTag;
			p.second = maxPtr->value;
			if (res != "") {
				res += ',';
			}
			res += p.first;
			list.push_back(p);
			removeMax();
		}
	}
	for (int i = 0; i < list.size(); i++) {
		insert(list[i].second, list[i].first);//inserts back all the removed nodes above
	}
	return res;//returns the comma seperated top x hashtags
}
void cut(fib_Node* ch, fib_Node* pa) {//cuts the child parent linkage if the max heap condition is violated and inserts the child node in top level
	pa->degree--;//reduces the degree of parent by 1
	if (ch->left == ch) {
		pa->child = NULL;
	}
	else {
		ch->left->right = ch->right;
		ch->right->left = ch->left;
		if (pa->child == ch) {
			pa->child=maxNode(ch->left);
		}
	}
	ch->left = ch->right = ch;
	ch->parent = NULL; ch->mark = false;
	linkSiblings(maxPtr, ch);//inserting child back to toplevel node
}
void cascadeCut(fib_Node* pa) {//performs cascading cut until the parent haven't lost any child earlier
	fib_Node* ppa = pa->parent;
	if (ppa != NULL) {
		if (!pa->mark) {//if parent haven't lost child earlier
			pa->mark = true;//parents mark is set to true 
		}
		else {//if parent lost a child earlier
			cut(pa, ppa);//cut operation is performed between parent and its parent
			cascadeCut(ppa);//cascadecut is performed on parent's parent
		}
	}
	else {
		pa->mark = false;
	}
}
void increaseKey(int val, string tag) {//increases the value of the respective hashtag
	fib_Node* head = tags[tag];//gets the node of respective hashtag
	head->value += val;
	if (head->parent == NULL) {//if the node is in the top level
		if (head->value > maxPtr->value) {//if node value is greater than maxptr value
			maxPtr = head;//maxptr is pointed to the node
		}
	}
	else {
		if (head->parent->value > head->value) {//if node doesnt violates the maxheap condition after increasing the value
			if (head->parent->child->value < head->value) {
				head->parent->child = head;
			}
		}
		else {//if node violates the maxheap condition after increasing the value
			fib_Node* p = head->parent;
			cut(head, p);//cut is performed on node and its parent
			cascadeCut(p);//cascadecut is performed on node's parent
		}
	}
	if (head->value > maxPtr->value) {
		maxPtr = head;
	}
}

int main(int argc, char** argv) {//driver function
	ifstream input; ofstream output;
	string s = "";
	input.open(argv[1]);//file pointer to the input file given via command line argument
	//input.open("input_1000.txt");
	output.open("output_file.txt");//file pointer to the output_file.txt
	clock_t start, end;
	start=clock();
	while (input >> s) {//reads the input from the file continously
		lines++;
		if (s[0] == '#') {//if the input line starts with #
			int x;
			input >> x;
			s = s.substr(1, s.length() - 1);
			if (tags[s] != NULL) {//if the hash tag already exsists in the heap increasekey is called
				increaseKey(x, s);
			}
			else {//insert is called if hashtag doesnt exisit in the heap
				insert(x,s);
			}
		}
		else if (s == "stop") {//teriminates the program if the input line encounters stop
			input.close();
			output.close();
			break;
		}
		else {//if input line contains integer top x nodes function is invoked
			stringstream geek(s);
			int x = 0;
			geek >> x;
			if(fline){output << topXNodes(x);fline=false;}
			else{output <<endl<< topXNodes(x);}	
		}
	}
	end=clock();
	double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
	cout << "Time taken by program is : " << time_taken <<"sec"<<endl;
}
