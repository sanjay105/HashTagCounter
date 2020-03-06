#include<bits/stdc++.h>
#include<iostream>
#include<fstream>
using namespace std;
struct fib_Node//fibonacci heap node structure;
{
    struct fib_Node *parent,*child,*left,*right;//parent/child->pointer to parent/child node,left/right->pointer to left/right siblings
    int value,degree;//value-> node value, degree->degree of the node in the heap
    string hash_tag;//hash_tag->hash tag
    bool mark;//mark->is its child cut
    fib_Node(int val,string hash_tag){//initialize the structure with values
        this->value=val;this->hash_tag=hash_tag;
        this->parent=this->child=NULL;
        this->left=this->right=this;
        this->degree=0;this->mark=false;
    }
};
int no_of_nodes=0;//current number of nodes in the heap
struct fib_Node* maxPtr=NULL;//pointer to the maximum valued node in the heap
map<string,struct fib_Node*> tags;//consists of pointer to respective node by hash tag
void printHeap(struct fib_Node* head){
    if (head == NULL)
        return;
    struct fib_Node *t = head;
    do
    {
        if (t->degree == 0){
            cout << "Child" << t->value << " " << t->hash_tag << endl;
        }
        else{
            cout << "Parent:" << t->value << " with degree" << t->degree << " " << t->hash_tag << endl;
            printHeap(t->child);
            cout << "Parent:" << t->value << "End" << endl;
        }
        t = t->left;
    } while (t != head);
}
void insert(int value,string hash_tag){
    struct fib_Node* n=new fib_Node(value,hash_tag);
    no_of_nodes++;
    tags[hash_tag]=n;
    if(maxPtr==NULL){
        maxPtr=n;//if heap is empty make new node as heap and maxptr
    }else{//inserting n to right side of the current max pointer
        maxPtr->right->left=n;
        n->right=maxPtr->right;
        maxPtr->right=n;
        n->left=maxPtr;
        if(maxPtr->value<n->value){//if current max pointer value is less than n then set n as new max Pointer
            maxPtr=n;
        }
    }
}
struct fib_Node* meld(struct fib_Node* a,struct fib_Node* b){//melds the two heaps of same degree into a single heap of degree+1
    //cout<<"Meld: "<<a->value<<" of degree "<<a->degree<<" and "<<b->value<<" of degree "<<b->degree<<endl;
    if(b->value>a->value){//if value of b node is greater than a then swap a and b
        struct fib_Node* temp=a;
        a=b;
        b=temp;
    }
    a->degree++;//increase the degree of a
    b->parent=a;//set b parent pointer to a
    if(a->child==NULL){
        a->child=b;//if a doesnt have a child set a child pointer to b
    }else{//add b as a sibling to a's childs
        a->child->right->left=b;
        b->right=a->child->right;
        a->child->right=b;
        b->left=a->child;
        if(a->child->value<b->value){
            a->child=b;//value of b is greater than a's child then set a's child pointer to b
        }
    }
    //cout<<"Meld done "<<a->value<<" of degree "<<a->degree<<endl;
    return a;
}
void pairWiseCombine(vector<struct fib_Node*> &sib){
    int n=sib.size(),max_degree=(log(no_of_nodes+1)/log(2))+1;
    bool* b=(bool*)calloc(sizeof(bool),max_degree);
    struct fib_Node** aux=(struct fib_Node**)malloc(sizeof(struct fib_Node*)*max_degree);//stores all the top level nodes
    for(int i=0;i<n;i++){
        sib[i]->left=sib[i]->right=sib[i];
        if(!b[sib[i]->degree]){//if there are no heaps of sib[i]->degree
            b[sib[i]->degree]=true;
            aux[sib[i]->degree]=sib[i];
        }else{//if there exsits two heaps of same degree
            b[sib[i]->degree]=false;
            struct fib_Node* x=meld(sib[i],aux[sib[i]->degree]);//preform meld operation
            while(b[x->degree]){//if there exsisrs two heaps of same degree
                b[x->degree]=false;
                x=meld(x,aux[x->degree]);//perform meld operation
            }
            b[x->degree]=true;
            aux[x->degree]=x;
        }
    }
    int max=INT_MIN;
    for(int i=0;i<max_degree;i++){
        if(b[i]){
            if(aux[i]->value>max){
                max=aux[i]->value;maxPtr=aux[i];//maxPtr is pointed to the max valued top level nodes
            }
        }
    }
    for(int i=0;i<max_degree;i++){//top level siblings are joined together by a double linked lists
        if(b[i]&&maxPtr!=aux[i]){
            maxPtr->right->left=aux[i];
            aux[i]->right=maxPtr->right;
            maxPtr->right=aux[i];
            aux[i]->left=maxPtr;
        }
    }

}
void removeMax(){//removes the maxvalued node from the heap
    if(maxPtr==NULL){
        cout<<"Heap is Empty"<<endl;
        return;
    }
    //cout<<"Hash tag "<<maxPtr->hash_tag<<" with value "<<maxPtr->value<<" of degree "<<maxPtr->degree<<" is deleted from heap."<<endl;
    no_of_nodes--;//decrement the total number of nodes by 1
    tags.erase(maxPtr->hash_tag);//remove the hashtag from the tags hashmap
    if(maxPtr->left==maxPtr){//if max valued node doesnt have any siblings then remove max node and make its child as max pointer
        maxPtr=maxPtr->child;
    }else{
        struct fib_Node *temp=maxPtr;
        vector<struct fib_Node*> siblings;//will store the siblings and childs of maxPtr
        while(temp->left!=maxPtr){//get all siblings of maxPtr
            siblings.push_back(temp->left);temp=temp->left;
        }
        if(maxPtr->degree>0){//get all childs of maxPtr
            temp=maxPtr->child;
            siblings.push_back(temp);
            while(temp->left!=maxPtr->child){
                siblings.push_back(temp->left);temp=temp->left;
            }
        }
        pairWiseCombine(siblings);//perform pairwise combine on all siblings array
    }
}
void cut(struct fib_Node* c,struct fib_Node* p){//remove a child from parent and insert the child in the toplevel node
    //cout<<"Cut parent "<<p->value<<" and child "<<c->value<<endl;
    p->degree--;//reducing the degree of the parent node as we are removing a child from the parent
    if(p->child==c){//if parents child pointer is pointing to this child
        if(c->left==c){//if child doesn't have any sibilings
            c->parent->child=NULL;//makes parents child node to null
        }else{//if child has siblings
            c->left->right=c->right;
            c->right->left=c->left;
            struct fib_Node* temp=c->left;
            int max=temp->value;p->child=temp;
            while(temp->left!=c->left){
                if(max<temp->left->value){
                    p->child=temp->left;//set next highest valued sibling as a child to the parent
                    temp=temp->left;
                }
            }
        }
    }else{
        c->left->right=c->right;
        c->right->left=c->left;
    }
    c->parent=NULL;
    maxPtr->right->left=c;
    c->right=maxPtr->right;
    maxPtr->right=c;
    c->left=maxPtr;
    if(maxPtr->value<c->value){
        maxPtr=c;//if the cut child has higher value than maxPtr value then set maxPtr to cut child
    }
    c->mark=true;
    //cout<<"Cut Done"<<endl;
}
void cascadeCut(struct fib_Node* head){
    //cout<<"CascadeCut on "<<head->value<<endl;
    struct fib_Node* temp = head->parent; 
    if (temp != NULL) { //if parent is not null
        if (!head->mark) { //if this parent ever lost a child
            head->mark = true; 
        } 
        else { 
            cut(head, temp); 
            cascadeCut(temp); 
        } 
    }
    //cout<<"cascade cut done on "<<head->value<<endl;
}
void increaseKey(string hash_tag,int increaseby){//increases the hashtag count for a hashtag
    struct fib_Node* head=tags[hash_tag];
    //cout<<"Increase Key of "<<hash_tag<<" from "<<head->value<<" to "<<head->value+increaseby<<endl;
    head->value+=increaseby;
    if(head->parent==NULL){//if the node is at the top level
        if(maxPtr->value<head->value){
            maxPtr=head;//if node value is greater than the current maxPtr value then maxPtr is pointed to the current node
        }
    }else{
        if(head->parent->value>head->value){//if heap condition is satisfied between parent and child even if child value is increased
            if(head->parent->child->value<head->value){//if sibling value is less than the current node value after node value is increased
                head->parent->child=head;//parents child is set to current node
            }
        }else{//if heap condition not satisfied between parent and child after child value is increased
            struct fib_Node* p=head->parent;
            cut(head,p);
            cascadeCut(p);
        }
    }
    //cout<<"Increase Key done"<<endl;
}
string topXNodes(int x){//returns a string with top x hashtags comma seperated
    string res="";
    vector<pair<string,int> > ele;
    for(int i=0;i<x;i++){
        pair<string,int> p;
        p.first=maxPtr->hash_tag;
        p.second=maxPtr->value;
        ele.push_back(p);
        removeMax();//removes the most popular hashtag
    }
    for(int i=0;i<x;i++){
        if(res!=""){
            res+=',';
        }
        res+=ele[i].first;
        insert(ele[i].second,ele[i].first);//inserts all the hashtags into heap after picking top x hashtags
    }
    return res;
}

int main(int argc, char** argv){//driver function
    ifstream input;ofstream output;
    string s="";
    input.open(argv[1]);//opens the file name given as an argument and reads input from the file
    output.open("output_file.txt");//writes output to output_file.txt
	while(input>>s){
		if(s[0]=='#'){//if input starts with # 
			int x;
			input>>x;
            s=s.substr(1,s.length()-1);
			if(tags[s]!=NULL){//if hashtag already exsists in heap then increaseKey function is invoked.
				increaseKey(s,x);
			}else{//if hashtag doesnt exsist in heap insert function is invoked
				insert(x,s);
			}
		}else if(s=="stop"){//if input encounters "stop" program execution is exited.
            input.close();
            output.close();
			return 0;
		}else{//if input is an integer topXNodes function is invoked and returns top X hashtags
            stringstream geek(s);
			int x=0;
            geek>>x;//converts string to integer
			output<<topXNodes(x)<<endl;
            cout<<"***************************************MAX HEAP***********************************************"<<endl;
            printHeap(maxPtr);
            cout<<"***************************************MAX HEAP***********************************************"<<endl;
		}
	}
}
