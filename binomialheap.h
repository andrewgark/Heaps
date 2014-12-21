#include "leftistorskewheap.h"
#include <iostream>

class BinomialHeap: public IMeldableHeap
{
    struct Node
    {
        int key;
        Node *child, *brother;

        explicit Node(int key) : key(key), child(nullptr), brother(nullptr) {}
        ~Node()
        {
            delete child;
            delete brother;
        }
    };

    std::vector<Node *> trees;
    Node *minTree;
    ui32 minTreeIndex;
    ui32 size;

    explicit BinomialHeap(int key)
    {
        size = 1;
        trees.push_back(new Node(key));
        minTree = trees[0];
    }

    BinomialHeap heapFromTreeWithoutRoot(Node *node, int sizeOfTree)
    {
        BinomialHeap resultHeap;
        resultHeap.size = sizeOfTree - 1;
        resultHeap.minTree = nullptr;
        Node *previous = nullptr;
        for (Node *current = node->child; current; current = current->brother)
        {
            if (previous)
                previous->brother = nullptr;
            resultHeap.trees.push_back(current);
            if (!resultHeap.minTree || current->key < minTree->key)
                resultHeap.minTree = current;
            previous = current;
        }
        std::reverse(resultHeap.trees.begin(), resultHeap.trees.end());
        node->brother = node->child = nullptr;
        delete node;
        return resultHeap;
    }

    Node * meldNodes(Node *firstNode, Node *secondNode)
    {
        if (!firstNode || !secondNode)
            return (firstNode ? firstNode : secondNode);
        if (firstNode->key > secondNode->key)
            std::swap(firstNode, secondNode);
        secondNode->brother = firstNode->child;
        firstNode->child = secondNode;
        return firstNode;
    }

public:

    BinomialHeap(){}
    ~BinomialHeap(){}

    void init(int key)
    {
        size = 0;
        minTree = nullptr;
        insert(key);
    }

    void insert(int key)
    {
        BinomialHeap newBinomialHeap = BinomialHeap(key);
        IMeldableHeap &newHeap = dynamic_cast<IMeldableHeap&>(newBinomialHeap);
        meld(newHeap);
    }

    int extractMin()
    {
        if (minTree)
        {
            int result = minTree->key;
            trees[minTreeIndex] = nullptr;
            ui32 minTreeSize = 1 << minTreeIndex;
            size -= minTreeSize;
            BinomialHeap minTreeBinomialHeap = heapFromTreeWithoutRoot(minTree, minTreeSize);
            IMeldableHeap &minTreeHeap = dynamic_cast<IMeldableHeap&>(minTreeBinomialHeap);
            meld(minTreeHeap);
            return result;
        }
        else
            throw ExtractFromEmptyHeapException();
    }

    void meld(IMeldableHeap &anotherHeap)
    {
        size += anotherHeap.getSize();
        BinomialHeap &binomialHeap = dynamic_cast<BinomialHeap&> (anotherHeap);
        anotherHeap = BinomialHeap();
        trees.resize(std::max(trees.size(), binomialHeap.trees.size()) + 1, nullptr);
        binomialHeap.trees.resize(trees.size(), nullptr);
        Node *extraTree = nullptr;
        for (ui32 i = 0; i < trees.size(); ++i)
            if (trees[i])
            {
                if (binomialHeap.trees[i] && extraTree)
                    extraTree = meldNodes(extraTree, binomialHeap.trees[i]);
                else if (binomialHeap.trees[i] || extraTree)
                {
                    extraTree = meldNodes(trees[i], extraTree ? extraTree : binomialHeap.trees[i]);
                    trees[i] = nullptr;
                }
            }
            else
            {
                trees[i] = meldNodes(extraTree, binomialHeap.trees[i]);
                extraTree = nullptr;
            }
        while (trees.size() && !trees.back())
            trees.pop_back();
        binomialHeap.trees.clear();
        binomialHeap.minTree = nullptr;
        minTree = nullptr;
        for (ui32 i = 0; i < trees.size(); ++i)
            if (trees[i] && (!minTree || (trees[i]->key < minTree->key)))
            {
                minTree = trees[i];
                minTreeIndex = i;
            }
    }

    ui32 getSize() const
    {
        return size;
    }
};
