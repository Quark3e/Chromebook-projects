/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-04
 * @note C++20
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>
#include <any>
#include <string>
#include <vector>

struct Foo {
    int i;
    int j;
};

struct Property {
    std::string m_label;
    std::any m_value;
};

void process_some_type(Property const& property) {
    if (property.m_label == "int")
        std::cout << "received an int with value " << std::any_cast<int>(property.m_value) << '\n';
    else if (property.m_label == "double")
        std::cout << "received a double with value " << std::any_cast<double>(property.m_value) << '\n';
    else
        std::cout << "received something else\n";
}

int main() {
    std::vector<Property> container;
    container.emplace_back("int", 10);
    container.emplace_back("double", 3.14);
    container.emplace_back("string", "hello world");
    container.emplace_back("Foo", Foo{0, 0});

    for (auto const& property : container)
        process_some_type(property);
}