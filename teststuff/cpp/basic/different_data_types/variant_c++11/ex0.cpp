/**
 * @file ex0.cpp
 * @author u/alfps (https://www.reddit.com/user/alfps/)
 * @brief source: (https://www.reddit.com/r/cpp_questions/comments/zm1j4l/comment/j0gfzxb/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button)
 * @version 0.1
 * @date 2024-06-04
 * 
 * 
 */

#include <new>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

#include <assert.h>     // assert
#include <stdint.h>     // uint8_t

namespace my {
    using   std::string,            // <string>
            std::type_info,         // <typeinfo>
            std::move, std::swap,   // <utility>
            std::vector;            // <vector>

    template< class Type > using in_ = const Type&;

    class Variant final
    {
    public:
        struct Alternative{ enum Enum{ an_int, a_string, a_vector }; };
        
    private:
        using Vec = vector<uint8_t>;
        union
        {
            int             m_int;
            string          m_string;
            Vec             m_vector;
        };
        
        Alternative::Enum   m_tag;

    public:
        ~Variant()
        {
            switch( m_tag ) {
                case Alternative::an_int:       break;
                case Alternative::a_string:     m_string.~string();  break;
                case Alternative::a_vector:     m_vector.~Vec();  break;
            }
        }

        Variant( const int arg ) noexcept:          m_int( arg ), m_tag( Alternative::an_int )              {}
        Variant( string arg ) noexcept:             m_string( move( arg ) ), m_tag( Alternative::a_string ) {}
        Variant( vector<uint8_t> arg ) noexcept:    m_vector( move( arg ) ), m_tag( Alternative::a_vector ) {}
        
        Variant() noexcept: Variant( 0 ) {}
        
        Variant( in_<Variant> other )
        {
            switch( other.m_tag ) {
                case Alternative::an_int:       ::new( &m_int ) int( other.m_int );  break;
                case Alternative::a_string:     ::new( &m_string ) string( other.m_string );  break;
                case Alternative::a_vector:     ::new( &m_vector ) Vec( other.m_vector );  break;
            }
            m_tag = other.m_tag;
        }
        
        Variant( Variant&& other ) noexcept
        {
            switch( other.m_tag ) {
                case Alternative::an_int:       ::new( &m_int ) int( other.m_int );  break;
                case Alternative::a_string:     ::new( &m_string ) string( move( other.m_string ) );  break;
                case Alternative::a_vector:     ::new( &m_vector ) Vec( move( other.m_vector ) );  break;
            }
            m_tag = other.m_tag;
        }
        
        friend void swap( Variant& a, Variant& b ) noexcept
        {
            // This logic does not support derived classes, hence this class is `final`.
            Variant moved_a = move( a );
            a.~Variant();  ::new( &a ) Variant( move( b ) );
            b.~Variant();  ::new( &b ) Variant( move( moved_a ) );
        }

        auto operator=( in_<Variant> other )
            -> Variant&
        {
            Variant temp = other;
            swap( *this, temp );
            return *this;
        }
        
        auto operator=( Variant&& other ) noexcept
            -> Variant&
        {
            // This logic does not support derived classes, hence this class is `final`.
            this->~Variant();  new( this ) Variant( move( other ) );
            return *this;
        }

        auto holds( const Alternative::Enum alternative ) -> bool { return (alternative == m_tag); }

        template< Alternative::Enum alternative >
        auto value() const
            -> const auto&
        {
            assert( alternative == m_tag );
            if constexpr( alternative == Alternative::an_int ) {
                return m_int;
            } else if constexpr( alternative == Alternative::a_string ) {
                return m_string;
            } else if constexpr( alternative == Alternative::a_vector ) {
                return m_vector;
            }
            for( ;; ) {}        // Should never get here.
        }
    };
}  // namespace my

#include <iostream>

using std::cout;

auto main() -> int
{
    my::Variant a = 42;
    my::Variant b = std::string( "Kladask!" );
    swap( a, b );

    using Alt = my::Variant::Alternative;
    cout << a.value<Alt::a_string>() << "\n";
    cout << b.value<Alt::an_int>() << "\n";
}