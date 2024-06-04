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
#include <variant>
#include <string>
#include <vector>
#include <type_traits>

struct Foo {
    int i;
    int j;
};

struct Property {
    std::string m_label;
    std::variant<int, double, std::string, Foo> m_value;
};

void process_if_else(Property const& property) {
    if (property.m_label == "int")
        std::cout << "received an int with value " << std::get<int>(property.m_value) << '\n';
    else if (property.m_label == "double")
        std::cout << "received a double with value " << std::get<double>(property.m_value) << '\n';
    //etc
    else
        std::cout << "received something else I was too lazy to write up\n";
}

void process_visit(Property const& property) {
    std::visit([](auto const& value) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, Foo>)
        std::cout << "got an instance of Foo\n";
        else
        std::cout << "got a value of a different type I'm too lazy to properly write up: " << value << '\n';
    }, property.m_value);
}

int main() {
    std::vector<Property> container;
    container.emplace_back("int", 10);
    container.emplace_back("double", 3.14);
    container.emplace_back("string", "hello world");
    container.emplace_back("Foo", Foo{0, 0});

    for (auto const& property : container)
        process_if_else(property);

    for (auto const& property : container)
        process_visit(property);
}