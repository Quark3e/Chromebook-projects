
#include <iostream>
#include <vector>
#include <memory>

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
    private:
        T m_data;
};

typedef std::vector<std::shared_ptr<Property>> property_list_type;




int main(int argc, char** argv) {
    property_list_type testVec; 
    testVec.push_back(std::make_shared<TypedProperty<int>>("int var", 69));
    testVec.push_back(std::make_shared<TypedProperty<std::string>>("int var", "cool"));


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
