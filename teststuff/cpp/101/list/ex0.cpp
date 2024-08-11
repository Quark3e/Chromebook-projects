
#include <iostream>
#include <list>
#include <vector>
#include <string>


template<typename _varType> int checkExistence(_varType toFind, const std::vector<_varType>& toSearch);
template<typename _varType> int checkExistence(_varType toFind, const std::list<_varType>& toSearch);
template<typename _varType> int checkExistence(_varType toFind, _varType toSearch[], int arrLen);



std::string nums[10] = {
    "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"
};

template<typename var>
auto itrFunc(const std::list<var>& listRef, size_t idx=0) {
    size_t count = 0;
    for(auto itr = listRef.begin(); itr!=listRef.end(); ++itr) {
        if(count==idx) return itr;
        count+=1;
    }
    return listRef.end();
}



int main(int argc, char** argv) {
    std::list<std::string> temp;
    for(int i=0; i<10; i++) {
        temp.push_back(nums[i]);
    }

    std::cout << "----------" << std::endl;
    
    for(auto it=temp.begin(); it!=temp.end(); ++it) std::cout<< &*it<<"  "<<*it << std::endl;
    std::cout << "----------" << std::endl;

    std::list<std::string>::iterator toRem;
    toRem = temp.begin();
    advance(toRem, 2);
    temp.erase(toRem);

    int count=0;
    for(auto it=temp.begin(); it!=temp.end(); ++it) {
        std::cout<<"["<<count<<"] "<< &*it<<"  "<<*it << std::endl;
        count++;
    }    
    std::cout << "----------" << std::endl;

    int tempVar=2;
    std::cout <<"["<< tempVar << "]" << " "<<*itrFunc<std::string>(temp, static_cast<size_t>(tempVar));

    std::cout << "\nexistence check: \""<<nums[5]<<"\""<<checkExistence<std::string>(nums[5],temp) << std::endl;
    
    std::cout << "----------" << std::endl;
    std::cout << "test:"<<std::endl;

    std::vector<std::list<std::string>::iterator> itrToErase;
    auto itr = temp.begin();
    count = 0;
    for(auto var: temp) {
        if(count%2==0) {
            std::cout << "   " << *itr << std::endl;//var;
            itrToErase.push_back(itr);
            // temp.erase(itr); //need to solve how to erase
            // break;
        }
        count++;
        ++itr;
    }
    for(auto itrs: itrToErase) temp.erase(itrs);
    std::cout << "-----------" << std::endl;

    count=0;
    for(auto it=temp.begin(); it!=temp.end(); ++it) {
        std::cout<<"["<<count<<"] "<< &*it<<"  "<<*it << std::endl;
        count++;
    }    
    std::cout << "----------" << std::endl;

    return 0;
}



template<typename _varType> int checkExistence(_varType toFind, const std::vector<_varType>& toSearch) {
    for(size_t i=0; i<toSearch.size(); i++) {
        if(toSearch.at(i)==toFind) return static_cast<int>(i);
    }
    return -1;
}
template<typename _varType> int checkExistence(_varType toFind, const std::list<_varType>& toSearch) {
    int count=0;
    for(auto itr=toSearch.begin(); itr!=toSearch.end(); ++itr) {
        if(*itr==toFind) return count;

        count++;
    }

    return -1;
}
template<typename _varType> int checkExistence(_varType toFind, _varType toSearch[], int arrLen) {
    for(int i=0; i<arrLen; i++) {
        if(toSearch[i]==toFind) return i;
    }
    return -1;
}