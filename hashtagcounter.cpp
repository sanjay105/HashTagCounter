#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <stack>
#include <math.h>
#include <chrono>
using namespace std;
using namespace std::chrono;
struct fib_Node
{
    bool mark;
    int value, degree;
    struct fib_Node *left, *right, *parent, *child;
    string hashTag;
    fib_Node(int val, string hashTag)
    {
        this->value = val;
        this->degree = 0;
        this->mark = false;
        this->left = this->right = this;
        this->parent = this->child = NULL;
        this->hashTag = hashTag;
    }
};
int no_of_nodes = 0, lines = 0;
struct fib_Node *maxPtr = NULL;
map<string, struct fib_Node *> tags;
void linkSiblings(fib_Node *a, fib_Node *b)
{
    a->right->left = b->left;
    b->left->right = a->right;
    a->right = b;
    b->left = a;
}
fib_Node *maxNode(fib_Node *head)
{
    if (head == NULL)
    {
        return NULL;
    }
    int max = head->value;
    fib_Node *m = head, *temp = head->left;
    while (temp != head)
    {
        if (max < temp->value)
        {
            max = temp->value;
            m = temp;
        }
        temp = temp->left;
    }
    return m;
}
void insert(int val, string tag)
{
    fib_Node *n = new fib_Node(val, tag);
    no_of_nodes++;
    tags[tag] = n;
    if (maxPtr == NULL)
    {
        maxPtr = n;
    }
    else
    {
        linkSiblings(maxPtr, n);
        if (n->value > maxPtr->value)
        {
            maxPtr = n;
        }
    }
}
fib_Node *meld(fib_Node *a, fib_Node *b)
{
    if (a->value < b->value)
    {
        return meld(b, a);
    }
    else
    {
        a->degree++;
        a->parent = NULL;
        b->parent = a;
        if (a->child == NULL)
        {
            a->child = b;
        }
        else
        {
            linkSiblings(a->child, b);
            if (b->value > a->child->value)
            {
                a->child = b;
            }
        }
        return a;
    }
}
void consolidate(vector<fib_Node *> &topNodes)
{
    if (!topNodes.empty())
    {
        vector<fib_Node *> Nodes(30, NULL);
        vector<bool> b(30, false);
        maxPtr = topNodes[0];
        b[topNodes[0]->degree] = true;
        Nodes[topNodes[0]->degree] = topNodes[0];
        for (int i = 1; i < topNodes.size(); i++)
        {
            fib_Node *a = topNodes[i];
            while (b[a->degree])
            {
                b[a->degree] = false;
                a = meld(a, Nodes[a->degree]);
            }
            b[a->degree] = true;
            Nodes[a->degree] = a;
        }
        stack<fib_Node *> stack;
        int max = 0;
        for (int i = 0; i < 30; i++)
        {
            if (b[i])
            {
                stack.push(Nodes[i]);
                if (max < Nodes[i]->value)
                {
                    maxPtr = Nodes[i];
                    max = Nodes[i]->value;
                }
            }
        }
        fib_Node *a = stack.top();
        stack.pop();
        while (!stack.empty())
        {
            fib_Node *x = stack.top();
            stack.pop();
            linkSiblings(a, x);
        }
    }
    else
    {
        maxPtr = NULL;
    }
}
void removeMax()
{
    if (maxPtr == NULL)
    {
        cout << "Heap is empty" << endl;
    }
    else
    {
        tags.erase(maxPtr->hashTag);
        no_of_nodes--;
        vector<fib_Node *> topNodes;
        fib_Node *temp;
        if (maxPtr->child != NULL)
        {
            temp = maxPtr->child;
            topNodes.push_back(temp);
            temp = temp->right;
            while (temp != maxPtr->child)
            {
                topNodes.push_back(temp);
                temp = temp->right;
            }
        }
        temp = maxPtr->left;
        while (temp != maxPtr)
        {
            topNodes.push_back(temp);
            temp = temp->left;
        }
        for (int i = 0; i < topNodes.size(); i++)
        {
            topNodes[i]->left = topNodes[i]->right = topNodes[i];
            topNodes[i]->parent = NULL;
            topNodes[i]->mark = false;
        }
        consolidate(topNodes);
    }
}
string topXNodes(int x)
{
    string res = "";
    vector<pair<string, int>> list;
    for (int i = 0; i < x; i++)
    {
        pair<string, int> p;
        if (maxPtr == NULL)
        {
            break;
        }
        else
        {
            p.first = maxPtr->hashTag;
            p.second = maxPtr->value;
            if (res != "")
            {
                res += ',';
            }
            res += p.first;
            list.push_back(p);
            removeMax();
        }
    }
    for (int i = 0; i < list.size(); i++)
    {
        insert(list[i].second, list[i].first);
    }
    return res;
}
void cut(fib_Node *ch, fib_Node *pa)
{
    pa->degree--;
    if (ch->left == ch)
    {
        pa->child = NULL;
    }
    else
    {
        ch->left->right = ch->right;
        ch->right->left = ch->left;
        if (pa->child == ch)
        {
            pa->child = maxNode(ch->left);
        }
    }
    ch->left = ch->right = ch;
    ch->parent = NULL;
    ch->mark = false;
    linkSiblings(maxPtr, ch);
}
void cascadeCut(fib_Node *pa)
{
    fib_Node *ppa = pa->parent;
    if (ppa != NULL)
    {
        if (!pa->mark)
        {
            pa->mark = true;
        }
        else
        {
            cut(pa, ppa);
            cascadeCut(ppa);
        }
    }
    else
    {
        pa->mark = false;
    }
}
void increaseKey(int val, string tag)
{
    fib_Node *head = tags[tag];
    head->value += val;
    if (head->parent == NULL)
    {
        if (head->value > maxPtr->value)
        {
            maxPtr = head;
        }
    }
    else
    {
        if (head->parent->value > head->value)
        {
            if (head->parent->child->value < head->value)
            {
                head->parent->child = head;
            }
        }
        else
        {
            fib_Node *p = head->parent;
            cut(head, p);
            cascadeCut(p);
        }
    }
    if (head->value > maxPtr->value)
    {
        maxPtr = head;
    }
}

int main(int argc, char **argv)
{
    ifstream input;
    ofstream output;
    string s = "";
    input.open(argv[1]);
    output.open("output_file.txt");
    auto start = high_resolution_clock::now();
    while (input >> s)
    {
        lines++;
        if (s[0] == '#')
        {
            int x;
            input >> x;
            s = s.substr(1, s.length() - 1);
            if (tags[s] != NULL)
            {
                increaseKey(x, s);
            }
            else
            {
                insert(x, s);
            }
        }
        else if (s == "stop")
        {
            input.close();
            output.close();
            break;
        }
        else
        {
            stringstream geek(s);
            int x = 0;
            geek >> x;
            output << topXNodes(x) << endl;
        }
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken for execution: " << duration.count() << " microseconds" << endl;
    cout << "DONE" << endl;
}
