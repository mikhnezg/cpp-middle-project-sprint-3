#pragma once

#include <format>
#include <stdexcept>
#include <string_view>

namespace bookdb {

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

constexpr std::string_view GenreToString(Genre g) {
    switch (g) {
        case Genre::Fiction:    return "Fiction";
        case Genre::NonFiction: return "NonFiction";
        case Genre::SciFi:      return "SciFi";
        case Genre::Biography:  return "Biography";
        case Genre::Mystery:    return "Mystery";
        case Genre::Unknown:    return "Unknown";
        default:                return "Unknown";
    }
}

constexpr Genre GenreFromString(std::string_view s) {
    if (s == "Fiction")    return Genre::Fiction;
    if (s == "NonFiction") return Genre::NonFiction;
    if (s == "SciFi")      return Genre::SciFi;
    if (s == "Biography")  return Genre::Biography;
    if (s == "Mystery")    return Genre::Mystery;
    return Genre::Unknown;
}

struct Book {
    // string_view для экономии памяти, чтобы ссылаться на оригинальную строку, хранящуюся в другом контейнере
    std::string_view author;
    std::string title;
    int year;
    Genre genre;
    double rating;
    int read_count;

    constexpr Book(std::string_view title_, std::string_view author_, int year_,
                   Genre genre_, double rating_, int read_count_)
        : author(author_), title(title_), year(year_),
          genre(genre_), rating(rating_), read_count(read_count_) {}

    constexpr Book(std::string_view title_, std::string_view author_, int year_,
                   std::string_view genre_str, double rating_, int read_count_)
        : author(author_), title(title_), year(year_),
          genre(GenreFromString(genre_str)), rating(rating_), read_count(read_count_) {}
};

}  // namespace bookdb

namespace std {

template <>
struct formatter<bookdb::Genre, char> {
    template <typename FormatContext>
    auto format(const bookdb::Genre g, FormatContext &fc) const {
        return format_to(fc.out(), "{}", bookdb::GenreToString(g));
    }
    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
};

template <>
struct formatter<bookdb::Book, char> {
    template <typename FormatContext>
    auto format(const bookdb::Book &b, FormatContext &fc) const {
        return format_to(fc.out(),
            "Book{{title: \"{}\", author: \"{}\", year: {}, genre: {}, rating: {:.1f}, reads: {}}}",
            b.title, b.author, b.year, b.genre, b.rating, b.read_count);
    }
    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
};

}  // namespace std
