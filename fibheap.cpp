#include<bits/stdc++.h>
using namespace std;
int no_of_nodes=0;
struct node
{
    struct node* parent;
    struct node* child;
    struct node* left;
    struct node* right;
    int degree;
    int value;
    node(int val){
        this->parent=NULL;
        this->child=NULL;
        this->value=val;
        this->left=this;
        this->right=this;
        this->degree=0;
    }
};
bool comparision(struct node *a,struct node *b){
    return a->degree<b->degree;
}
struct node* getMaxPointer(struct node *head){
    if(head==NULL){
        return NULL;
    }
    if(head->left==head->right){
        if(head->value<head->left->value){
            return head->left;
        }
        return head;
    }
    struct node *temp=head->left;int val=head->value;struct node* t=head;
    while(temp!=head){
        if(temp->value>val){
            val=temp->value;
            t=temp;
        }
        temp=temp->left;
    }
    return t;
    
}
struct node* insert(int val,struct node* a){
    no_of_nodes++;
    struct node *b=new node(val);
    if(a==NULL){
        return b;
    }
    b->left=a->left;
    a->left=b;
    b->right=a;
    return getMaxPointer(a);
}
struct node* meld(struct node* a,struct node* b){
    if(a->value<b->value){
        struct node* temp;
        temp=a;
        a=b;
        b=temp;
    }
    a->degree++;
    b->parent=a;
    if(a->child==NULL){
        a->child=b;
    }else{
        b->left=a->child->left;
        a->child->left=b;
        b->right=a->child;
        if(b->value>a->child->value){
            a->child=b;
        }
    }
    return a;
}
struct node* removeMax(struct node* head){
    if(head->left==head){
        return getMaxPointer(head->child);
    }
    vector<struct node*> siblings;
    struct node* temp=head;
    if(head->degree>0){
        struct node *t=head->child;
        siblings.push_back(head->child);
        while(t->left!=head->child){
            siblings.push_back(t->left);t=t->left;
        }
    }
    while(temp->left!=head){
        siblings.push_back(temp->left);
        temp=temp->left;
    }
    cout<<siblings.size();
    sort(siblings.begin(),siblings.end(),comparision);//Sorting the siblings with degree in increasing order
    int max_degree=log(no_of_nodes)/log(2);
    struct node** aux=(struct node**)calloc(sizeof(struct node*),(max_degree+1));
    bool* b=(bool*)calloc(sizeof(bool),max_degree+1);
    vector<struct node*>::iterator itr;
    for(itr=siblings.begin();itr!=siblings.end();itr++){
        cout<<"Size:"<<siblings.size()<<" ";
        cout<<(*itr)->degree<<endl;
        if(!b[(*itr)->degree]){
            aux[(*itr)->degree]=(*itr);
            b[(*itr)->degree]=true;
            cout<<"hello"<<endl;
        }else{
            struct node *x=meld(*itr,aux[(*itr)->degree]);
            siblings.push_back(x);
            b[(*itr)->degree]=false;
            cout<<x->degree<<"dillo"<<endl;
        }
    }

}
int main(){
    struct node *a;
    a=NULL;
    int n;
    cin>>n;
    for(int i=0;i<n;i++){
        int x;cin>>x;
        a=insert(x,a);
    }
    removeMax(a);
}

