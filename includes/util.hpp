#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstdlib>
#include <cassert>
#include <limits>
#include <type_traits>
#include <utility>

// array elements
// ~~~~~~~~~~~~~~
template <typename T, std::size_t Size>
constexpr std::size_t array_elements (T (&)[Size]) noexcept {
    return Size;
}

// aligned
// ~~~~~~~
template <typename IntType, typename = typename std::enable_if_t<std::is_unsigned<IntType>::value>>
constexpr IntType aligned (IntType v, unsigned align) noexcept {
    return (v + align - 1U) & static_cast<IntType> (~(align - 1U));
}

// type max
// ~~~~~~~~
template <typename T>
constexpr T type_max (T) noexcept {
    return std::numeric_limits<T>::max ();
}
template <typename T>
constexpr T type_max () noexcept {
    return std::numeric_limits<T>::max ();
}

// type min
// ~~~~~~~~
template <typename T>
constexpr T type_min () noexcept {
    return std::numeric_limits<T>::min ();
}

// narrow cast
// ~~~~~~~~~~~
/// A searchable way to do narrowing casts of values
template <class T, class U>
constexpr T narrow_cast (U && u) noexcept {
    assert (u <= type_max<T> ());
    bool const a =
        (std::is_unsigned<T>::value == std::is_unsigned<U>::value && u >= type_min<T> ());
    bool const b = (!std::is_unsigned<T>::value || (std::is_unsigned<T>::value && u >= 0));
    assert (a || b);
    return static_cast<T> (std::forward<U> (u));
}

template <typename T>
class not_null {
    static_assert (std::is_assignable<T &, std::nullptr_t>::value, "T cannot be assigned nullptr.");

public:
    constexpr not_null (T t) noexcept
            : ptr_ (t) {
        ensure_invariant ();
    }
    not_null & operator= (T const & t) noexcept {
        ptr_ = t;
        ensure_invariant ();
        return *this;
    }

    not_null (not_null const & other) = default;
    not_null & operator= (not_null const & other) = default;

    // prevents compilation when someone attempts to assign a nullptr
    not_null (std::nullptr_t) = delete;
    not_null (int) = delete;
    not_null<T> & operator= (std::nullptr_t) = delete;
    not_null<T> & operator= (int) = delete;

    constexpr T get () const noexcept { return ptr_; }

    constexpr operator T () const noexcept { return get (); }
    constexpr T operator-> () const noexcept { return get (); }

    constexpr bool operator== (T const & rhs) const noexcept { return ptr_ == rhs; }
    constexpr bool operator!= (T const & rhs) const noexcept { return !(*this == rhs); }

private:
    T ptr_;

    // we assume that the compiler can hoist/prove away most of the checks inlined from this
    // function if not, we could make them optional via conditional compilation
    constexpr void ensure_invariant () const noexcept { assert (ptr_ != nullptr); }

    // unwanted operators...pointers only point to single objects!
    // TODO ensure all arithmetic ops on this type are unavailable
    not_null<T> & operator++ () = delete;
    not_null<T> & operator-- () = delete;
    not_null<T> operator++ (int) = delete;
    not_null<T> operator-- (int) = delete;
    not_null<T> & operator+ (size_t) = delete;
    not_null<T> & operator+= (size_t) = delete;
    not_null<T> & operator- (size_t) = delete;
    not_null<T> & operator-= (size_t) = delete;
};

template <typename Function>
class scope_guard {
public:
    template <typename OtherFunction>
    explicit scope_guard (OtherFunction && other)
            : exit_function_{std::forward<OtherFunction> (other)} {}

    scope_guard (scope_guard && rhs)
            : execute_on_destruction_{rhs.execute_on_destruction_}
            , exit_function_{std::move (rhs.exit_function_)} {
        rhs.release ();
    }

    scope_guard (scope_guard const &) = delete;
    scope_guard & operator= (scope_guard const &) = delete;
    scope_guard & operator= (scope_guard &&) = delete;

    ~scope_guard () noexcept {
        if (execute_on_destruction_) {
            try {
                exit_function_ ();
            } catch (...) {
            }
        }
    }
    void release () noexcept { execute_on_destruction_ = false; }

private:
    bool execute_on_destruction_ = true;
    Function exit_function_;
};

template <typename Function>
scope_guard<typename std::decay_t<Function>> make_scope_guard (Function && f) {
    return scope_guard<std::decay_t<Function>> (std::forward<Function> (f));
}



/// \returns True if the input value is a power of 2.
template <typename Ty, typename = typename std::enable_if_t<std::is_unsigned<Ty>::value>>
constexpr bool is_power_of_two (Ty n) noexcept {
    //  if a number n is a power of 2 then bitwise & of n and n-1 will be zero.
    return n > 0U && !(n & (n - 1U));
}

/// Calculate the value that must be added to p v in order that it has the alignment
/// given by \p align.
///
/// \param v      The value to be aligned.
/// \param align  The alignment required for \p v.
/// \returns  The value that must be added to \p v in order that it has the alignment given by
/// \p align.
template <typename Ty>
constexpr Ty calc_alignment (Ty v, std::size_t align) noexcept {
    assert (is_power_of_two (align));
    return (align == 0U) ? 0U : ((v + align - 1U) & ~(align - 1U)) - v;
}

/// Calculate the value that must be added to \p v in order for it to have the alignment
/// required by type Ty.
///
/// \param v  The value to be aligned.
/// \returns  The value that must be added to \p v in order that it has the alignment required
/// by type Ty.
template <typename Ty>
constexpr Ty calc_alignment (Ty v) noexcept {
    return calc_alignment (v, alignof (Ty));
}

#endif // UTIL_HPP
