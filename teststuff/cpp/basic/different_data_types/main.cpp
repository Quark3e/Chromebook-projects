
#include <iostream>
#include <vector>
#include <memory>

#include <useful.hpp>


class Property {
    public:
        Property(const std::string& name) //note: we don't lightly copy strings in C++
        : m_name(name) {}
        virtual ~Property() {}
    private:
        std::string m_name;
};

template<typename T>
class TypedProperty : public Property {
    public:
        TypedProperty(const std::string& name, const T& data) : Property(name), m_data(data) {}
        T getData() { return m_data; }
    private:
        T m_data;
};

typedef std::vector<std::shared_ptr<Property>> property_list_type;
typedef std::vector<Property> property_type;

std::vector<size_t> vecIdx;
std::vector<std::string> vecNames;
// std::vector<std::string>

property_type testType;

template<typename T>
void addToVec(property_list_type& vecRef, std::string key, T value) {
    vecRef.push_back(std::make_shared<TypedProperty<T>>(key, value));

    testType.push_back()

    vecIdx.push_back(vecRef.size()-1);
    vecNames.push_back(key);
}

template<typename T>
void printFromVec(property_list_type& vecRef, std::string key) {
    int pos = findVectorIndex(vecNames, key);
    if(pos==-1) return;
    // std::cout << dynamic_cast<TypedProperty<T>>(vecRef.at(vecIdx.at(pos)))->getData() << std::endl;

}


int main(int argc, char** argv) {
    property_list_type testVec;
    
    addToVec<int>(testVec, "int var", 69);
    addToVec<std::string>(testVec, "str var", "cool");
    for(auto elem: testVec) {
        elem->getData();
    }
    // testVec.push_back(std::make_shared<TypedProperty<int>>("int var", 69));
    // testVec.push_back(std::make_shared<TypedProperty<std::string>>("int var", "cool"));


    return 0;
}


// class Property
// {
//     Property(std::string name);
//     virtual ~Property() {}
//     std::string m_name;
// };
// class PropertyBoolean : Property {
//     PropertyBoolean(std::string name, bool data);
//     bool m_data;
// };
// class PropertyFloat : Property {
//     PropertyFloat(std::string name, float data);
//     float m_data;
// };

// class PropertyVector : Property {
//     PropertyVector(std::string name, std::vector<float> data);
//     std::vector<float> m_data;
// };
// std::vector<Property*> vec;
