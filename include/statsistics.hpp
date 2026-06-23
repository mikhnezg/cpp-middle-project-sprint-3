#pragma once

#include <algorithm>
#include <flat_map>
#include <functional>
#include <iterator>
#include <map>
#include <numeric>
#include <random>
#include <vector>

#include "book_database.hpp"

#include <print>

namespace bookdb {

// Гистограмма без flat-контейнеров — для сравнения производительности
template <BookContainerLike T, typename Comparator = TransparentStringLess>
auto buildAuthorHistogram(const BookDatabase<T> &cont, Comparator comp = {}) {
    std::map<std::string, int, Comparator> histogram(comp);
    for (const auto &book : cont) {
        ++histogram[std::string(book.author)];
    }
    return histogram;
}

// Гистограмма количества книг по авторам через std::flat_map
template <BookContainerLike T, typename Comparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T> &cont, Comparator comp = {}) {
    std::flat_map<std::string, int, Comparator> histogram(comp);
    for (const auto &book : cont) {
        ++histogram[std::string(book.author)];
    }
    return histogram;
}

// Средний рейтинг по жанрам через std::flat_map
template <BookIterator It, BookSentinel<It> Sent>
auto calculateGenreRatings(It first, Sent last) {
    std::flat_map<Genre, std::pair<double, int>> accumulator;
    for (auto it = first; it != last; ++it) {
        auto &[sum, count] = accumulator[it->genre];
        sum += it->rating;
        ++count;
    }
    std::flat_map<Genre, double> result;
    for (const auto &[genre, sc] : accumulator) {
        result[genre] = sc.first / sc.second;
    }
    return result;
}

// Средний рейтинг всех книг через std::transform_reduce
template <BookContainerLike T>
double calculateAverageRating(const BookDatabase<T> &books) {
    if (books.empty()) return 0.0;
    double sum = std::transform_reduce(
        books.begin(), books.end(), 0.0,
        std::plus<>{},
        [](const Book &b) { return b.rating; });
    return sum / static_cast<double>(books.size());
}

// Проверка всех книг предикатом через std::all_of
template <BookContainerLike T, typename Pred>
bool allBooksMatch(const BookDatabase<T> &books, Pred pred) {
    return std::all_of(books.begin(), books.end(), pred);
}

// Извлечение рейтингов через std::transform
template <BookContainerLike T>
std::vector<double> extractRatings(const BookDatabase<T> &books) {
    std::vector<double> ratings;
    ratings.reserve(books.size());
    std::transform(books.begin(), books.end(), std::back_inserter(ratings),
                   [](const Book &b) { return b.rating; });
    return ratings;
}

// Случайная выборка книг
template <BookContainerLike T>
auto sampleRandomBooks(const BookDatabase<T> &cont, size_t num) {
    std::vector<std::reference_wrapper<const Book>> pool;
    pool.reserve(cont.size());
    for (const auto &b : cont) pool.emplace_back(std::cref(b));
    if (num >= pool.size()) return pool;

    std::vector<std::reference_wrapper<const Book>> result;
    result.reserve(num);
    std::mt19937 rng(std::random_device{}());
    std::sample(pool.begin(), pool.end(), std::back_inserter(result), num, rng);
    return result;
}

// Топ N книг по компаратору (разрешено изменять контейнер)
template <BookContainerLike T, typename Comparator>
auto getTopNBy(BookDatabase<T> &cont, size_t n, Comparator comp) {
    std::sort(cont.begin(), cont.end(), comp);
    std::vector<std::reference_wrapper<const Book>> result;
    size_t count = std::min(n, cont.size());
    auto it = cont.end();
    for (size_t i = 0; i < count; ++i) {
        --it;
        result.emplace_back(std::cref(*it));
    }
    return result;
}

}  // namespace bookdb
