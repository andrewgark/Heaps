#include <algorithm>
#include <iostream>
typedef unsigned int ui32;

class IMeldableHeap
{
public:
    virtual void init(int key) = 0;
    virtual void insert(int key) = 0;
    virtual int extractMin() = 0;
    virtual void meld(IMeldableHeap &anotherHeap) = 0;
    virtual ui32 getSize() const = 0;
    ~IMeldableHeap(){};
};


class ExtractFromEmptyHeapException: public std::exception
{
    std::string message;
public:
    explicit ExtractFromEmptyHeapException()
    {
        message = "You tried to extract from empty heap";
    }
    virtual const char* what() const throw()
    {
        return message.c_str();
    }
};

class LeftistOrSkewHeap: public IMeldableHeap
{
    class Node
    {
    public:
        int key;
        ui32 rank;
        Node *left, *right;

        explicit Node(int key) : key(key), rank(1), left(NULL), right(NULL){}

        ~Node()
        {
            delete left;
            delete right;
        }

        void swapChildren()
        {
            std::swap(left, right);
        }

        void updateRank()
        {
            if (left && right)
                rank = 1 + std::min(left->rank, right->rank);
            else
                rank = 1;
        }
    };

    Node *root;
    int size;
    virtual bool heapIsLeftist() = 0;

    int getRank(Node *node)
    {
        return (node ? node->rank : 0);
    }

    Node * meldedNode(Node *firstNode, Node *secondNode)
    {
        if (!firstNode || !secondNode)
            return (firstNode ? firstNode : secondNode);
        if (firstNode->key > secondNode->key)
            std::swap(firstNode, secondNode);
        firstNode->right = meldedNode(firstNode->right, secondNode);
        if (!heapIsLeftist() || (getRank(firstNode->left) < getRank(firstNode->right)))
            firstNode->swapChildren();
        if (heapIsLeftist() && firstNode)
            firstNode->updateRank();
        secondNode = NULL;
        return firstNode;
    }
public:
    LeftistOrSkewHeap(): root(NULL), size(0)
    {
    }

    ~LeftistOrSkewHeap(){}

    void insert(int key)
    {
        root = meldedNode(root, new Node(key));
        ++size;
    }

    void init(int key)
    {
        insert(key);
    }

    int extractMin()
    {
        if (!root)
            throw ExtractFromEmptyHeapException();
        int min = root->key;
        Node *newRoot = meldedNode(root->left, root->right);
        root->left = NULL;
        root->right = NULL;
        delete root;
        root = newRoot;
        --size;
        return min;
    }

    void meld(IMeldableHeap &anotherHeap)
    {
        size += anotherHeap.getSize();
        LeftistOrSkewHeap &anotherLeftistOrSkewHeap = dynamic_cast<LeftistOrSkewHeap&>(anotherHeap);
        root = meldedNode(root, anotherLeftistOrSkewHeap.root);
    }

    ui32 getSize() const
    {
        return size;
    }
};

class LeftistHeap: public LeftistOrSkewHeap
{
    bool heapIsLeftist()
    {
        return true;
    }
};

class SkewHeap: public LeftistOrSkewHeap
{
    bool heapIsLeftist()
    {
        return false;
    }
};
