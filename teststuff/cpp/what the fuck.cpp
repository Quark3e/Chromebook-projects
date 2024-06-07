/**
 * @file what the fuck.cpp
 * @author SlyBach "Toghether C & C++" discord server
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace notstd // yet
{
    template<class T, class U = T>
    constexpr // Since C++20
    T exchange(T& obj, U&& new_value)
        noexcept( // Since C++23
            std::is_nothrow_move_constructible<T>::value &&
            std::is_nothrow_assignable<T&, U>::value
        )
    {
        T old_value = std::move(obj);
        obj = std::forward<U>(new_value);
        return old_value;
    }

    template<class T, class... Args>
    typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
    make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}

namespace details
{
    struct create_value_tag {};
    constexpr create_value_tag create_value{};
}

template <typename T>
class ref_or_unique_ptr
{
    static_assert(!std::is_array<T>::value, "only supports single objects");

    explicit ref_or_unique_ptr(std::reference_wrapper<T> reference) noexcept
        : m_ShouldDelete(false)
        , m_Data(&reference.get())
    {}

    explicit ref_or_unique_ptr(std::unique_ptr<T> unique) noexcept
        : m_ShouldDelete(true)
        , m_Data(unique.release())
    {}

public:

    static ref_or_unique_ptr make_ref(T& value) noexcept
    {
        return ref_or_unique_ptr(std::ref(value));
    }

    template <typename ... ArgsT>
    static ref_or_unique_ptr make_unique(ArgsT&& ... args)
    {
        static_assert(std::is_constructible<T, ArgsT&&...>::value, "T cannot be constructed from the provided arguments");
        return ref_or_unique_ptr(notstd::make_unique<T>(std::forward<ArgsT>(args)...));
    }

    template <typename ... ArgsT>
    ref_or_unique_ptr(details::create_value_tag, ArgsT&&... args)
        : ref_or_unique_ptr(notstd::make_unique<T>(std::forward<ArgsT>(args)...))
    {}

    ~ref_or_unique_ptr()
    {
        if (m_Data && m_ShouldDelete)
            delete m_Data;
    }

    // no copy semantics
    ref_or_unique_ptr(ref_or_unique_ptr const&) = delete;
    ref_or_unique_ptr& operator=(ref_or_unique_ptr const&) = delete;

    // allow move semantics
    ref_or_unique_ptr(ref_or_unique_ptr&& other) noexcept
        : m_ShouldDelete(notstd::exchange(other.m_ShouldDelete, false))
        , m_Data(notstd::exchange(other.m_Data, nullptr))
    {}

    ref_or_unique_ptr& operator=(ref_or_unique_ptr&& other) noexcept
    {
        if (m_Data && m_ShouldDelete)
            delete m_Data;
        m_ShouldDelete = notstd::exchange(other.m_ShouldDelete, false);
        m_Data = notstd::exchange(other.m_Data, nullptr);
        return *this;
    }

    // object access
    T& get_value() const { return *m_Data; }
    T& operator*() const { return get_value(); }

    // ref or unique owner info
    bool is_reference() const { return !m_ShouldDelete; }
    bool is_owner() const { return m_ShouldDelete; }

private:
    bool m_ShouldDelete;
    T* m_Data;  // Note: every constructor requires this to be non-null
};


template <typename T>
class weird_map
{
    std::unordered_map<std::string, ref_or_unique_ptr<T> > m_Map;

public:

    using iterator = typename decltype(m_Map)::iterator;
    using const_iterator = typename decltype(m_Map)::iterator;


    bool insert_reference(std::string const& key, T& ref)
    {
        return m_Map.emplace(key, ref_or_unique_ptr<T>::make_ref(ref)).second;
    }
    bool insert_reference(std::string const& key, T&& ref) = delete;

    bool insert_value(std::string const& key, T const& to_copy)
    {
        return m_Map.emplace(key, ref_or_unique_ptr<T>::make_unique(to_copy)).second;
    }
    bool insert_value(std::string const& key, T&& to_move)
    {
        return m_Map.emplace(key, ref_or_unique_ptr<T>::make_unique(std::move(to_move))).second;
    }
    template <typename ... ArgsT>
    bool emplace_value(std::string const& key, ArgsT&& ... args)
    {
        return m_Map.emplace(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(details::create_value, std::forward<ArgsT>(args)...)).second;
    }


    iterator find(std::string const& key)
    {
        return m_Map.find(key);
    }
    const_iterator find(std::string const& key) const
    {
        return m_Map.find(key);
    }

    iterator begin() { return m_Map.begin(); }
    iterator end() { return m_Map.end(); }
    const_iterator cbegin() const { return m_Map.cbegin(); }
    const_iterator cend() const { return m_Map.cend(); }
    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }
};


//---------------------------------
// test code
#include <iostream>
int main()
{
    weird_map<std::string> m;
    std::string external = "outside";
    m.insert_reference("ref", external);
    //m.insert_reference("foo", "temporary"); //errors, cannot reference a temporary
    m.emplace_value("bar", 5, 'a'); // creates a new string with value "aaaaa"
    m.insert_value("copy", external);   // doesn't reference 'external', makes a copy

    for (auto const& pair : m)
    {
        auto const& ref_or_unique_ptr = pair.second;
        bool const is_ref = ref_or_unique_ptr.is_reference();
        std::cout
            << pair.first << " - " << ref_or_unique_ptr.get_value()
            << (is_ref ? " - is a reference to external object\n" : " - is owned by the map\n")
            ;
    }

    std::cout << "\nchanging value of external string from outside\n-----\n";
    external = "hello world";
    std::cout
        << "the orignal object's value is: " << external << '\n'
        << "the referenced value in the map is: " << m.find("ref")->second.get_value() << '\n'
        << "the copied value in the map is: " << m.find("copy")->second.get_value() << '\n'
        ;
    
    std::cout << "\nchanging value of external string from the map\n-----\n";
    m.find("ref")->second.get_value() = "lorem ipsum";
    std::cout
        << "the orignal object's value is: " << external << '\n'
        << "the referenced value in the map is: " << m.find("ref")->second.get_value() << '\n'
        << "the copied value in the map is: " << m.find("copy")->second.get_value() << '\n'
        ;
    
    std::cout << "\nchanging value of the copied string (from the map)\n-----\n";
    m.find("copy")->second.get_value() = "qwerty";
    std::cout
        << "the orignal object's value is: " << external << '\n'
        << "the referenced value in the map is: " << m.find("ref")->second.get_value() << '\n'
        << "the copied value in the map is: " << m.find("copy")->second.get_value() << '\n'
        ;
}