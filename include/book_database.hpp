#pragma once

#include <concepts>
#include <format>
#include <initializer_list>
#include <print>
#include <set>
#include <span>
#include <string>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    // Type aliases for standard container compatibility
    using value_type      = Book;
    using reference       = Book &;
    using const_reference = const Book &;
    using iterator        = typename BookContainer::iterator;
    using const_iterator  = typename BookContainer::const_iterator;
    using size_type       = typename BookContainer::size_type;
    using difference_type = typename BookContainer::difference_type;

    // std::set guarantees stable references on insert — string_view in Book stays valid.
    // TransparentStringLess::is_transparent enables heterogeneous lookup (find by string_view).
    using AuthorContainer = std::set<std::string, TransparentStringLess>;

    BookDatabase() = default;

    BookDatabase(std::initializer_list<Book> books) {
        for (const auto &b : books) {
            PushBack(b);
        }
    }

    void Clear() {
        books_.clear();
        authors_.clear();
    }

    // Standard container interface
    iterator       begin()        { return books_.begin(); }
    iterator       end()          { return books_.end(); }
    const_iterator begin()  const { return books_.begin(); }
    const_iterator end()    const { return books_.end(); }
    const_iterator cbegin() const { return books_.cbegin(); }
    const_iterator cend()   const { return books_.cend(); }
    size_type      size()   const { return books_.size(); }
    bool           empty()  const { return books_.empty(); }

    void PushBack(const Book &book) {
        auto it = authors_.emplace(book.author).first;
        books_.push_back(book);
        books_.back().author = *it;
    }

    void PushBack(Book &&book) {
        auto it = authors_.emplace(book.author).first;
        book.author = *it;
        books_.push_back(std::move(book));
    }

    template <typename... Args>
        requires std::constructible_from<Book, Args...>
    void EmplaceBack(Args &&...args) {
        Book b(std::forward<Args>(args)...);
        PushBack(std::move(b));
    }

    std::span<const Book> GetBooks() const { return books_; }
    std::span<Book>       GetBooks()       { return books_; }

    const AuthorContainer &GetAuthors()       { return authors_; }
    const AuthorContainer &GetAuthors() const { return authors_; }

private:
    BookContainer   books_;
    AuthorContainer authors_;
};

}  // namespace bookdb

// Formatter for BookDatabase
template <bookdb::BookContainerLike T>
struct std::formatter<bookdb::BookDatabase<T>, char> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<T> &db, FormatContext &fc) const {
        auto out = fc.out();
        out = std::format_to(out, "BookDatabase[{}]{{\n", db.size());
        for (const auto &b : db) {
            out = std::format_to(out, "  {}\n", b);
        }
        return std::format_to(out, "}}");
    }
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }
};
