
#pragma once
#ifndef HPP_DIY_SEARCH_MULTITHREADED
#define HPP_DIY_SEARCH_MULTITHREADED


#include <vector>
#include <list>
#include <iostream>
#include <math.h>
#include <atomic>

#include <thread>
#include <mutex>
#include <cassert>


namespace DIY_SEARCH_MULTITHREAD
{
    /// @brief maximum number of threads allowed to be spawned for `multithread_searchVec` given by 'std::thread::hardware_concurrency 
    inline unsigned int maxThreads = 0;
    /// @brief minimum number of items to search for to allow multiple threads
    inline int minLenThreads = 100;

    /// @brief mutex for inter-thread communication
    inline std::mutex intercom;
    inline std::mutex cout_mtx;
    inline std::atomic<bool> idxFound(false);

    inline std::vector<std::vector<int>> return_idx;
    
    inline void mtx_print(std::string _toPrint, bool _blocking=true, bool _flushEnd=true, std::string _end="\n") {
        std::unique_lock<std::mutex> u_lck_cout(cout_mtx, std::defer_lock);
        if(_blocking) {
            u_lck_cout.lock();
            std::cout << _toPrint << _end;
            if(_flushEnd) std::cout.flush();
            u_lck_cout.unlock();
        }
        else if(u_lck_cout.try_lock()) {
            std::cout << _toPrint << _end;
            if(_flushEnd) std::cout.flush();
            u_lck_cout.unlock();
        }
    }

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

        // mtx_print(std::to_string(id)+": started: ["+std::to_string(startIdx)+","+std::to_string(endIdx)+"]");
        assert(DIY_SEARCH_MULTITHREAD::return_idx.size()>id);
        int checkCount = 0;
        for(int i=startIdx; i<endIdx; i++) {
            if(!allOccurr && checkCount>=checkSpacing) {
                if(idxFound) break;
                checkCount = 0;
            }
            else if(!allOccurr) checkCount++;

            if(vec.at(i)==toFind) {
                tCOM.lock();
                // mtx_print(std::to_string(id)+": FOUND: "+std::to_string(i));
                if(!idxFound) idxFound = true;
                DIY_SEARCH_MULTITHREAD::return_idx.at(id).push_back(i);
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
     * @param checkSpacing the number each thread will wait before locking mutex and checking if a solution has been found.
     * @param verbose whether to print info to `std::cout`
     * be divided into equal parts so each thread search element num <=threadLen (2). If =`-1` then `numThreads` number of threads will spawn. 
     * @return `std::vector<int>` of index/indices(depending on `allOccurrence` param) to `toFind` in `vec`
     * 
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
        // assert(vec.size()>0);
        if(vec.size()<2) {
            if(vec.size()==0) return std::vector<int>{-1};
            if(vec.at(0)==toFind) return std::vector<int>{0};
            else return std::vector<int>{-1};
        }
        DIY_SEARCH_MULTITHREAD::idxFound = false;
        std::string verbose_str = " >> DIY_SEARCH_MULTITHREAD::multithread_searchVec";
        if(verbose) {
            std::cout << verbose_str+" FUNC CALLED."<<std::endl;
            std::cout << verbose_str+" vector size: "<<vec.size()<<std::endl;
        }
        std::vector<int> idx(1, -1);
        maxThreads = std::thread::hardware_concurrency();
        int threadCount = maxThreads; //number of threads to split the search by
        int vecSize = static_cast<int>(vec.size());
        if(numThreads>maxThreads) numThreads = maxThreads;
        if(checkSpacing<1) checkSpacing = 1;

        if(numThreads==-1 && threadLen==-1) {
            if(vecSize < minLenThreads) {
                threadCount = 1;
            }
            else {
                threadCount = maxThreads;
            }
        }
        else if(numThreads!=-1 && threadLen==-1) {
            if(vecSize < minLenThreads) {
                threadCount = 1;
            }
            else {
                threadCount = numThreads;
            }
            
        }
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

        if(verbose) {
            std::cout << verbose_str+" maxThreads  =\t"<<maxThreads<<std::endl;
            std::cout << verbose_str+" threadCount =\t"<<threadCount<<std::endl;
        }
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
        if(verbose) std::cout << verbose_str+" return_idx =\t"<<return_idx.size()<<std::endl;
        std::vector<std::thread> threadObjects;
        std::unique_lock<std::mutex> tCOM(intercom, std::defer_lock);
        int spacing = ceil(static_cast<float>(vecSize)/threadCount);
        if(verbose) {
            std::cout << verbose_str+" spacing : "<<spacing<<std::endl;
            std::cout << verbose_str+" starting: "<<threadCount<<" num threads:"<<std::endl;
        }
        for (int i=1; i < threadCount-1; i++) {
            threadObjects.emplace_back(
                [&, i] {threadFunc(vec, toFind, i, spacing*i, spacing*i+spacing, allOccurrence, checkSpacing);}
            );
        }
        threadObjects.emplace_back(
            [&] {threadFunc(vec, toFind, threadCount-1, spacing*(threadCount-1), vecSize, allOccurrence, checkSpacing);}
        );
        if(verbose) mtx_print(verbose_str+" T0: threads started.");
        
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
                return_idx.at(0).push_back(i);
                tCOM.unlock();
            }
        }

        for(int i=0; i<threadObjects.size(); i++) { threadObjects.at(i).join(); }
        if(verbose) std::cout<<verbose_str+" threads joined." << std::endl;
        if(idxFound) idx.clear();
        for(int id=0; id<threadCount; id++) {
            for(int i=0; i<return_idx.at(id).size(); i++) {
                idx.push_back(return_idx.at(id).at(i));
            }
        }
        return idx;
    }


    /**
     * @brief check if a `std::vector<T>` has repetitions of its elements (if any element is repeated/"exists in more than one element")
     * 
     * @tparam `T` type of the elements and `std::vector<T>`
     * @param vec `std::vector<T>` to search its element
     * @return `true` if any element occurs more than once in the vector
     * @return `false` if otherwise (all elements only occur once in the vector)
     */
    template<typename T>
    bool hasRepetitions(std::vector<T> vec) {
        bool repeated = false;
        for(T elem: vec) {
            std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<T>(vec, elem, -1, -1, true, 0, false);
            if(pos.size()>1) { repeated=true; break; }
        }
        return repeated;
    }

    /**
     * @brief Check if there are repetitions of an element inside a `std::list` container,
     * i.e. check if any element value occurs more than once
     * 
     * @tparam T data type of the values stored inside `std::list`
     * @param lst the `std::list` container to search through
     * @return true if there are occurrences
     * @return false if there aren't any occurrences
     */
    template<typename T>
    bool hasRepetitions(std::list<T> lst) {
        bool repeated = false;
        for(T elem: lst) {
            int foundCount = 0;
            for(T var: lst) {
                if(elem==var) foundCount++;
            }
            if(foundCount>1) return true;
        }
        return repeated;
    }

    /**
     * @brief Check if a value of type `_check` exists in given `std::vector<_check>` container
     * 
     * @tparam _check data type of value and vector of same type to check
     * @param key the value to check if it exists in `vec`
     * @param vec the `std::vector<_check>` to search through
     * @param verbose `DIY_SEARCH_MULTITHREAD::multithread_searchVec`: boolean for whether to print out intermediary statements
     * @param numThreads `DIY_SEARCH_MULTITHREAD::multithread_searchVec`: number of threads to split the search vector in
     * @param threadLen `DIY_SEARCH_MULTITHREAD::multithread_searchVec`: maximum number of elements for a single thread to look through in a vector (is overriden if `numThreads` num is bigger than
     * recommended value by `std::thread::hardware_concurrency();`).
     * @param checkSpacing `DIY_SEARCH_MULTITHREAD::multithread_searchVec`: number of loop iterations/cycles to wait before checking concurrent threads if they've found the value in vector
     * @return if `key` found, returns  `int` position/index value for `key` in `vec`. If not found returns `-1`; 
     */
    template<typename _check>
    int check_existence(
        _check key,
        std::vector<_check> vec,
        bool verbose=false,
        int numThreads=-1,
        int threadLen=-1,
        int checkSpacing=1
    ) {
        if(vec.size()<1) return -1;
        // assert(vec.size()>0);
        std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<_check>(
            vec, key, numThreads, threadLen, false, checkSpacing, verbose);
        return pos.at(0);
    }


    template<typename _check>
    int check_existence(
        _check key,
        std::list<_check> lst
    ) {
        int idx=0;
        for(auto itr=lst.begin(); itr!=lst.end(); ++itr) {
            if(key==*itr) return idx;
            idx++;
        }
        return -1;
    }


    template<typename _check>
    int check_existence(
        _check key,
        const _check arr[],
        int arrLen
    ) {
        for(int i=0; i<arrLen; i++) {
            if(key==arr[i]) return i;
        }
        return -1;
    }

}


#endif