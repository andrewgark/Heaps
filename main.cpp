#include "tests.h"
#include <iostream>

using namespace std;

int main()
{
    ui32 queriesNumber;
    std::cin >> queriesNumber;
    GlobalTestResult globalTestResult(queriesNumber);
    if (globalTestResult.succeeds)
    {
        std::cout << "Testing succeeds\n" << endl;
        std::cout << "Binomial Heap works " <<  globalTestResult.binomialHeapTime << " seconds" << endl;
        std::cout << "Skew Heap works " <<  globalTestResult.skewHeapTime << " seconds" << endl;
        std::cout << "Leftist Heap works " <<  globalTestResult.leftistHeapTime << " seconds" << endl;
    }
    else
        std::cout << "Testing failed\n" << endl;
    return 0;
}
