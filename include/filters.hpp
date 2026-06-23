#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"

namespace bookdb {

constexpr auto YearBetween(int start, int end) {
    return [start, end](const Book &b) noexcept {
        return b.year >= start && b.year <= end;
    };
}

constexpr auto RatingAbove(double min_rating) {
    return [min_rating](const Book &b) noexcept {
        return b.rating >= min_rating;
    };
}

constexpr auto GenreIs(Genre genre) {
    return [genre](const Book &b) noexcept {
        return b.genre == genre;
    };
}

// all_of: returns predicate that is true when ALL predicates match (fold expression)
template <BookPredicate... Preds>
constexpr auto all_of(Preds... preds) {
    return [preds...](const Book &b) noexcept {
        return (... && preds(b));
    };
}

// any_of: returns predicate that is true when ANY predicate matches (fold expression)
template <BookPredicate... Preds>
constexpr auto any_of(Preds... preds) {
    return [preds...](const Book &b) noexcept {
        return (... || preds(b));
    };
}

template <BookIterator It, BookSentinel<It> Sent, BookPredicate Pred>
auto filterBooks(It first, Sent last, Pred pred) {
    std::vector<std::reference_wrapper<const Book>> result;
    for (auto it = first; it != last; ++it) {
        if (pred(*it)) {
            result.emplace_back(std::cref(*it));
        }
    }
    return result;
}

}  // namespace bookdb
