
#pragma once
#ifndef HPP_DIY_SEARCH_MULTITHREADED
#define HPP_DIY_SEARCH_MULTITHREADED


#include <vector>
#include <iostream>
#include <math.h>
#include <atomic>

#include <thread>
#include <mutex>


namespace DIY_SEARCH_MULTITHREAD
{
    /// @brief maximum number of threads allowed to be spawned for `multithread_searchVec` given by 'std::thread::hardware_concurrency 
    inline unsigned int maxThreads = 0;
    
    /// @brief mutex for inter-thread communication
    inline std::mutex intercom;

    inline std::atomic<bool> idxFound(false);

    inline std::vector<std::vector<int>> return_idx;
    
    template<class T>
    inline void threadFunc(
        std::vector<T>& vec,
        T toFind,
        int id,
        int startIdx,
        int endIdx,
        bool allOccurr = false,
        int checkSpacing = 1
    ) {
        std::unique_lock<std::mutex> tCOM(intercom, std::defer_lock);

        int checkCount = 0;
        for(int i=startIdx; i<endIdx; i++) {
            if(!allOccurr && checkCount>=checkSpacing) {
                if(idxFound) break;
                checkCount = 0;
            }
            else if(!allOccurr) checkCount++;

            if(vec.at(i)==toFind) {
                tCOM.lock();
                if(!idxFound) idxFound = true;
                DIY_SEARCH_MULTITHREAD::return_idx[id].push_back(i);
                tCOM.unlock();
            }
        }
    }

    /**
     * @brief Search and find index, in given vector, for a certain value with multithreaded support by dividing
     * the search to multiple threads running in parallel
     * 
     * @tparam T data type of vector and variable to find
     * @param vec the vector to search through
     * @param toFind the value to find in the vector
     * @param numThreads max number of threads to split the operation into(1), with a new(starting at 1) thread split occuring according to threadLen. If =`-1` then it's set to `std::thread::hardware_concurrency()`.
     * @param threadLen maximum length of elements for each thread to search through. If length of `vec` is bigger than `threadLen` then the search will
     * @param allOccurrence whether to check for every occurence or to just return the first it finds. `true`-find every occurence(NOTE: this will note return the actual first but a random first it find).
     * @param checkSpacing the number each thread will wait before locking mutex and checking if a solution has been found. if
     * @param verbose whether to print info to `std::cout`
     * be divided into equal parts so each thread search element num <=threadLen (2). If =`-1` then `numThreads` number of threads will spawn. 
     * @return `int` type of index to 
     * @note (1) If `numThreads` is bigger than the max thread hint given by `std::thread::hardware_concurrency()` (stored in `maxThreads`) then it'll cap it at the hint num.
     * This is to avoid spawning number of threads more than number of cores recommended by the hint.
     * @note (2) If number of splits ,according to `threadLen` method, is bigger than `numThreads` value then the function will prioritise `numThreads` and unlock `threadLen`.
     * @note tldr:
     * @note [numThreads, threadLen]:
     * @note - [-1, -1] -> threadCount = `maxThreads`
     * @note - [-1, 10] -> threadCount = "num of threads where each threads search length < `threadLen`, capped at `maxThreads` num";
     * @note - [10, -1] -> threadcount = "10 threads unless `maxThreads`, if 10 > `maxThreads` then thread count = `maxThreads`";
     * @note - [10, 10] -> threadCount = "num of threads where each threads search length < `threadLen`, capped at `numThreads`, assuming that's < `maxThreads`""
     */
    template<class T>
    inline std::vector<int> multithread_searchVec(
        std::vector<T> vec,
        T toFind,
        int numThreads = -1,
        int threadLen = -1,
        bool allOccurrence = false,
        int checkSpacing = 10,
        bool verbose = false
    ) {
        std::string verbose_str = " >> DIY_SEARCH_MULTITHREAD::multithread_searchVec";
        std::vector<int> idx(1, -1);
        maxThreads = std::thread::hardware_concurrency();
        int threadCount = maxThreads; //number of threads to split the search by
        int vecSize = static_cast<int>(vec.size());
        if(numThreads>maxThreads) numThreads = maxThreads;


        if(numThreads==-1 && threadLen==-1) threadCount = maxThreads;
        else if(numThreads!=-1 && threadLen==-1) threadCount = numThreads;
        else if((numThreads!=-1 && threadLen!=-1) || (numThreads==-1 && threadLen!=-1)) {
            if((vecSize/maxThreads)<threadLen) {
                for(int i=1; i<numThreads; i++) {
                    if(static_cast<int>(ceil(static_cast<float>(vecSize)/i)) <= threadLen) {
                        threadCount = i;
                        break;
                    }
                }
            }
            else threadCount = maxThreads;
        }
        else if(numThreads<-1 || threadLen<-1) return idx;
        
        if(verbose) std::cout << verbose_str+" maxThreads  =\t"<<maxThreads<<std::endl;
        if(verbose) std::cout << verbose_str+" threadCount =\t"<<threadCount<<std::endl;

        if(threadCount==0 || threadCount==1) {
            for(int i=0; i<vecSize; i++) {
                if(vec.at(i)==toFind) {
                    if(idx[0]==-1) idx.clear();
                    idx.push_back(i);
                    if(!allOccurrence) break;
                }
            }
            return idx;
        }

        return_idx = std::vector<std::vector<int>>(threadCount, std::vector<int>());
        std::vector<std::thread> threadObjects;
        std::unique_lock<std::mutex> tCOM(intercom, std::defer_lock);
        int spacing = ceil(static_cast<float>(vecSize)/threadCount);

        for (int i=1; i < threadCount-1; i++) {
            threadObjects.emplace_back(
                [&, i] {threadFunc(vec, toFind, i, spacing*i, spacing*i+spacing, allOccurrence, checkSpacing);}
            );
        }
        threadObjects.emplace_back(
            [&] {threadFunc(vec, toFind, threadCount-1, spacing*(threadCount-1), vecSize, allOccurrence, checkSpacing);}
        );


        int checkCount = 0;
        for(int i=0; i<spacing; i++) {
            if(!allOccurrence && checkCount>=checkSpacing) {
                if(idxFound) break;
                checkCount = 0;
            }
            else if(!allOccurrence) checkCount++;

            if(vec.at(i)==toFind) {
                tCOM.lock();
                if(!idxFound) idxFound = true;
                return_idx[0].push_back(i);
                tCOM.unlock();
            }
        }

        for(int i=0; i<threadObjects.size(); i++) { threadObjects[i].join(); }
        if(idxFound) idx.clear();
        for(int id=0; id<threadCount; id++) {
            for(int i=0; i<return_idx.at(id).size(); i++) {
                idx.push_back(return_idx.at(id).at(i));
            }
        }
        return idx;
    }
}



#endif