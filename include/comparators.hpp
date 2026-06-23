#pragma once

#include "book.hpp"

namespace bookdb::comp {

struct LessByAuthor {
    bool operator()(const Book &a, const Book &b) const noexcept {
        return a.author < b.author;
    }
};

struct LessByTitle {
    bool operator()(const Book &a, const Book &b) const noexcept {
        return a.title < b.title;
    }
};

struct LessByYear {
    bool operator()(const Book &a, const Book &b) const noexcept {
        return a.year < b.year;
    }
};

struct LessByRating {
    bool operator()(const Book &a, const Book &b) const noexcept {
        return a.rating < b.rating;
    }
};

struct LessByPopularity {
    bool operator()(const Book &a, const Book &b) const noexcept {
        return a.read_count < b.read_count;
    }
};

struct LessByGenre {
    bool operator()(const Book &a, const Book &b) const noexcept {
        return a.genre < b.genre;
    }
};

}  // namespace bookdb::comp
