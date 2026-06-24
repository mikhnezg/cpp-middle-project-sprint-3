#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookContainerLike = requires(T t, const T ct) {
    typename T::value_type;
    requires std::same_as<typename T::value_type, Book>;
    { t.begin() } -> std::input_or_output_iterator;
    { t.end() };
    { ct.begin() } -> std::input_or_output_iterator;
    { ct.end() };
    { t.push_back(std::declval<Book>()) };
};

template <typename T>
concept BookIterator = std::input_iterator<T> &&
    std::same_as<std::iter_value_t<T>, Book>;

template <typename S, typename I>
concept BookSentinel = BookIterator<I> && std::sentinel_for<S, I>;

template <typename P>
concept BookPredicate = std::predicate<P, const Book &>;

template <typename C>
concept BookComparator = std::strict_weak_order<C, const Book &, const Book &>;

}  // namespace bookdb