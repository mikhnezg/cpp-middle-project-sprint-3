#include <algorithm>
#include <print>

#include "book_database.hpp"
#include "comparators.hpp"
#include "filters.hpp"
#include "statsistics.hpp"

using namespace bookdb;

int main() {
    BookDatabase<std::vector<Book>> db;

    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.0, 190);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 143);
    db.EmplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.5, 120);
    db.EmplaceBack("To Kill a Mockingbird", "Harper Lee", 1960, Genre::Fiction, 4.8, 156);
    db.EmplaceBack("Pride and Prejudice", "Jane Austen", 1813, Genre::Fiction, 4.7, 178);
    db.EmplaceBack("The Catcher in the Rye", "J.D. Salinger", 1951, Genre::Fiction, 4.3, 112);
    db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.5, 98);
    db.EmplaceBack("Jane Eyre", "Charlotte Bronte", 1847, Genre::Fiction, 4.6, 110);
    db.EmplaceBack("The Hobbit", "J.R.R. Tolkien", 1937, Genre::Fiction, 4.9, 203);
    db.EmplaceBack("Lord of the Flies", "William Golding", 1954, Genre::Fiction, 4.2, 89);

    std::print("Books:\n{}\n", db);

    std::sort(db.begin(), db.end(), comp::LessByAuthor{});
    std::print("Books sorted by author:\n{}\n==================\n", db);

    std::sort(db.begin(), db.end(), comp::LessByPopularity{});
    std::print("Books sorted by popularity:\n{}\n==================\n", db);

    auto histogram = buildAuthorHistogramFlat(db);
    std::print("Author histogram:\n");
    for (const auto &[author, count] : histogram) {
        std::print("  {}: {}\n", author, count);
    }

    auto genreRatings = calculateGenreRatings(db.begin(), db.end());
    std::print("\nAverage ratings by genres:\n");
    for (const auto &[genre, rating] : genreRatings) {
        std::print("  {}: {:.2f}\n", genre, rating);
    }

    auto avrRating = calculateAverageRating(db);
    std::print("Average books rating in library: {:.2f}\n", avrRating);

    auto filtered = filterBooks(db.begin(), db.end(),
                                all_of(YearBetween(1900, 1999), RatingAbove(4.5)));
    std::print("\nBooks from the 20th century with rating >= 4.5:\n");
    for (const auto &v : filtered) {
        std::print("  {}\n", v.get());
    }

    auto topBooks = getTopNBy(db, 3, comp::LessByRating{});
    std::print("\nTop 3 books by rating:\n");
    for (const auto &v : topBooks) {
        std::print("  {}\n", v.get());
    }

    auto orwellBookIt = std::find_if(db.begin(), db.end(),
        [](const auto &v) { return v.author == "George Orwell"; });
    if (orwellBookIt != db.end()) {
        std::print("\nTransparent lookup. Found Orwell's book: {}\n", *orwellBookIt);
    }

    return 0;
}
