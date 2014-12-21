#include "binomialheap.h"
#include <iostream>
#include <sstream>
#include <random>
#include <vector>
#include <algorithm>
#include <ctime>

enum EQueryType
{
    ADD_HEAP,
    INSERT,
    EXTRACT_MIN,
    MELD
};

std::default_random_engine generator;

int getRand(int a, int b)
{
    std::uniform_int_distribution<int> distribution(a, b);
    return distribution(generator);
}

template<typename TypeValue>
std::string convertToString(TypeValue number)
{
   std::stringstream ss;
   ss << number;
   return ss.str();
}

struct Query
{
    EQueryType type;
    ui32 index, secondIndex;
    int key;
};

std::string convertQueryToString(Query query)
{
    switch (query.type)
    {
        case ADD_HEAP:
            return "ADD_HEAP " + convertToString(query.key);
        case INSERT:
            return "INSERT " + convertToString(query.index) + " " + convertToString(query.key);
        case EXTRACT_MIN:
            return "EXTRACT_MIN " + convertToString(query.index);
        case MELD:
            return "MELD " + convertToString(query.index) + " " + convertToString(query.secondIndex);
    }
}


std::vector<Query> generateRandomQueries(ui32 queriesNumber)
{
    std::vector<Query> queries;
    ui32 heapsNumber = 0;
    for (ui32 i = 0; i < queriesNumber; ++i)
    {
        Query currentQuery;
        ui32 currentIndex = getRand(0, heapsNumber);
        if (currentIndex == heapsNumber)
        {
            currentQuery.type = ADD_HEAP;
            currentQuery.key = getRand(INT_MIN, INT_MAX);
            ++heapsNumber;
        }
        else
        {
            currentQuery.index = currentIndex;
            currentQuery.type = (EQueryType)getRand(1, 3);
            switch (currentQuery.type)
            {
                case INSERT:
                {
                    currentQuery.key = getRand(INT_MIN, INT_MAX);
                    break;
                }
                case MELD:
                {
                    currentQuery.secondIndex = getRand(0, heapsNumber - 1);
                    --heapsNumber;
                    break;
                }
            }
        }
        queries.push_back(currentQuery);
    }
    return queries;
}

template<typename HeapType>
int resultOfQuery(std::vector<HeapType> &heaps, Query query)
{
    switch (query.type)
    {
        case ADD_HEAP:
        {
            HeapType newHeap;
            newHeap.init(query.key);
            heaps.push_back(newHeap);
            return 1;
        }
        case INSERT:
            heaps[query.index].insert(query.key);
            return 1;
        case EXTRACT_MIN:
            return heaps[query.index].extractMin();
        case MELD:
            if (query.index != query.secondIndex)
            {
                heaps[query.index].meld(heaps[query.secondIndex]);
                std::swap(heaps.back(), heaps[query.secondIndex]);
                heaps.pop_back();
            }
            return 1;
    }
}

struct TestResult
{
    bool exception;
    int result;
    double time;
};

template<typename HeapType>
TestResult applyQueryToHeaps(std::vector<HeapType> &heaps, Query query)
{
    clock_t beginTime, endTime;
	beginTime = clock();
    bool exception = false;
    int result = 0;
    try {
        result = resultOfQuery<HeapType>(heaps, query);
    } catch (ExtractFromEmptyHeapException exc) {
        exception = true;
    }
    endTime = clock();
    return TestResult{exception, result, ((double)(endTime - beginTime))/CLOCKS_PER_SEC};
}

bool equalResults(TestResult result1, TestResult result2)
{
    return ((result1.exception == result2.exception) && (result1.result == result2.result));
}

class GlobalTestResult
{
public:
    bool succeeds = true;
    double binomialHeapTime = 0, skewHeapTime = 0, leftistHeapTime = 0;
    GlobalTestResult(ui32 queriesNumber)
    {
        std::vector<Query> queries = generateRandomQueries(queriesNumber);
        std::vector<BinomialHeap> binomialHeaps;
        std::vector<SkewHeap> skewHeaps;
        std::vector<LeftistHeap> leftistHeaps;
        for (ui32 i = 0; i < queriesNumber; ++i)
        {
            TestResult binomialHeapResult = applyQueryToHeaps<BinomialHeap>(binomialHeaps, queries[i]);
            TestResult skewHeapResult = applyQueryToHeaps<SkewHeap>(skewHeaps, queries[i]);
            TestResult leftistHeapResult = applyQueryToHeaps<LeftistHeap>(leftistHeaps, queries[i]);
            binomialHeapTime += binomialHeapResult.time;
            skewHeapTime += skewHeapResult.time;
            leftistHeapTime += leftistHeapResult.time;
            if (!equalResults(leftistHeapResult, skewHeapResult) || (!equalResults(leftistHeapResult, binomialHeapResult)))
                succeeds = false;
        }
    }
};
