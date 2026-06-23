#include <gtest/gtest.h>

#include "book_database.hpp"
#include "comparators.hpp"
#include "filters.hpp"
#include "statsistics.hpp"

using namespace bookdb;

TEST(TestComponentName, SimpleCheck) { EXPECT_EQ(1 + 1, 2); }

// BookDatabase

TEST(BookDatabase, EmptyOnConstruction) {
    BookDatabase<> db;
    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0u);
}

TEST(BookDatabase, PushBackIncreasesSize) {
    BookDatabase<> db;
    db.PushBack(Book{"Title1", "Author A", 2000, Genre::Fiction, 4.0, 10});
    db.PushBack(Book{"Title2", "Author B", 2001, Genre::SciFi, 3.5, 20});
    EXPECT_EQ(db.size(), 2u);
}

TEST(BookDatabase, EmplaceBack) {
    BookDatabase<> db;
    db.EmplaceBack("Dune", "Frank Herbert", 1965, Genre::SciFi, 4.8, 300);
    ASSERT_EQ(db.size(), 1u);
    EXPECT_EQ(db.begin()->title, "Dune");
    EXPECT_EQ(db.begin()->author, "Frank Herbert");
}

TEST(BookDatabase, InitializerListConstructor) {
    BookDatabase<> db = {
        Book{"A", "Alpha", 2000, Genre::Fiction, 4.0, 5},
        Book{"B", "Beta", 2005, Genre::Mystery, 3.0, 2},
    };
    EXPECT_EQ(db.size(), 2u);
}

TEST(BookDatabase, GetBooksReturnsSpan) {
    BookDatabase<> db;
    db.EmplaceBack("X", "Y", 2010, Genre::NonFiction, 3.0, 1);
    auto span = db.GetBooks();
    EXPECT_EQ(span.size(), 1u);
    EXPECT_EQ(span[0].title, "X");
}

TEST(BookDatabase, GetAuthorsContainsUniqueEntries) {
    BookDatabase<> db;
    db.EmplaceBack("Book1", "Same Author", 2000, Genre::Fiction, 4.0, 1);
    db.EmplaceBack("Book2", "Same Author", 2001, Genre::Fiction, 4.2, 2);
    db.EmplaceBack("Book3", "Other Author", 2002, Genre::SciFi, 3.8, 3);
    EXPECT_EQ(db.GetAuthors().size(), 2u);
}

TEST(BookDatabase, ClearEmptiesDatabase) {
    BookDatabase<> db;
    db.EmplaceBack("Title", "Author", 2000, Genre::Fiction, 4.0, 1);
    db.Clear();
    EXPECT_TRUE(db.empty());
    EXPECT_TRUE(db.GetAuthors().empty());
}

TEST(BookDatabase, TypeAliasesExist) {
    static_assert(std::same_as<BookDatabase<>::value_type, Book>);
    static_assert(std::is_same_v<BookDatabase<>::size_type, std::size_t>);
    SUCCEED();
}

TEST(BookDatabase, StringViewRemainsValidAfterManyInserts) {
    BookDatabase<> db;
    for (int i = 0; i < 200; ++i) {
        db.EmplaceBack("Book" + std::to_string(i), "Author" + std::to_string(i % 10),
                       2000 + i, Genre::Fiction, 4.0, i);
    }
    bool allValid = std::all_of(db.begin(), db.end(), [&db](const Book &b) {
        return db.GetAuthors().count(std::string(b.author)) > 0;
    });
    EXPECT_TRUE(allValid);
}

// Filters

TEST(Filters, YearBetweenSelects) {
    BookDatabase<> db;
    db.EmplaceBack("Old", "A", 1850, Genre::Fiction, 4.0, 1);
    db.EmplaceBack("Modern", "B", 1990, Genre::Fiction, 4.0, 1);
    db.EmplaceBack("New", "C", 2020, Genre::Fiction, 4.0, 1);
    auto result = filterBooks(db.begin(), db.end(), YearBetween(1900, 1999));
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].get().title, "Modern");
}

TEST(Filters, RatingAboveSelects) {
    BookDatabase<> db;
    db.EmplaceBack("Low", "A", 2000, Genre::Fiction, 3.0, 1);
    db.EmplaceBack("High", "B", 2000, Genre::Fiction, 4.8, 1);
    auto result = filterBooks(db.begin(), db.end(), RatingAbove(4.0));
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].get().title, "High");
}

TEST(Filters, GenreIsSelects) {
    BookDatabase<> db;
    db.EmplaceBack("Sci1", "A", 2000, Genre::SciFi, 4.0, 1);
    db.EmplaceBack("Fic1", "B", 2000, Genre::Fiction, 4.0, 1);
    auto result = filterBooks(db.begin(), db.end(), GenreIs(Genre::SciFi));
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].get().genre, Genre::SciFi);
}

TEST(Filters, AllOfCombinesPredicates) {
    BookDatabase<> db;
    db.EmplaceBack("A", "X", 1990, Genre::Fiction, 4.8, 1);
    db.EmplaceBack("B", "X", 1990, Genre::Fiction, 3.0, 1);
    db.EmplaceBack("C", "X", 1850, Genre::Fiction, 4.9, 1);
    auto result = filterBooks(db.begin(), db.end(), all_of(YearBetween(1900, 1999), RatingAbove(4.5)));
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].get().title, "A");
}

TEST(Filters, AnyOfCombinesPredicates) {
    BookDatabase<> db;
    db.EmplaceBack("Sci", "A", 2000, Genre::SciFi, 4.0, 1);
    db.EmplaceBack("Bio", "B", 2000, Genre::Biography, 4.0, 1);
    db.EmplaceBack("Mys", "C", 2000, Genre::Mystery, 4.0, 1);
    auto result = filterBooks(db.begin(), db.end(), any_of(GenreIs(Genre::SciFi), GenreIs(Genre::Biography)));
    EXPECT_EQ(result.size(), 2u);
}

TEST(Filters, EmptyDatabaseReturnsEmptyResult) {
    BookDatabase<> db;
    auto result = filterBooks(db.begin(), db.end(), RatingAbove(0.0));
    EXPECT_TRUE(result.empty());
}

// Statistics

TEST(Statistics, CalculateAverageRatingEmpty) {
    BookDatabase<> db;
    EXPECT_DOUBLE_EQ(calculateAverageRating(db), 0.0);
}

TEST(Statistics, CalculateAverageRating) {
    BookDatabase<> db;
    db.EmplaceBack("A", "X", 2000, Genre::Fiction, 4.0, 1);
    db.EmplaceBack("B", "Y", 2000, Genre::Fiction, 5.0, 1);
    EXPECT_DOUBLE_EQ(calculateAverageRating(db), 4.5);
}

TEST(Statistics, BuildAuthorHistogramFlat) {
    BookDatabase<> db;
    db.EmplaceBack("A", "Orwell", 2000, Genre::Fiction, 4.0, 1);
    db.EmplaceBack("B", "Orwell", 2001, Genre::Fiction, 4.2, 2);
    db.EmplaceBack("C", "Tolkien", 2002, Genre::Fiction, 4.9, 3);
    auto hist = buildAuthorHistogramFlat(db);
    EXPECT_EQ(hist.at("Orwell"), 2);
    EXPECT_EQ(hist.at("Tolkien"), 1);
}

TEST(Statistics, CalculateGenreRatings) {
    BookDatabase<> db;
    db.EmplaceBack("A", "X", 2000, Genre::Fiction, 4.0, 1);
    db.EmplaceBack("B", "Y", 2000, Genre::Fiction, 5.0, 1);
    db.EmplaceBack("C", "Z", 2000, Genre::SciFi, 3.0, 1);
    auto ratings = calculateGenreRatings(db.begin(), db.end());
    EXPECT_DOUBLE_EQ(ratings.at(Genre::Fiction), 4.5);
    EXPECT_DOUBLE_EQ(ratings.at(Genre::SciFi), 3.0);
}

TEST(Statistics, GetTopNBy) {
    BookDatabase<> db;
    db.EmplaceBack("Low", "A", 2000, Genre::Fiction, 2.0, 1);
    db.EmplaceBack("Medium", "B", 2000, Genre::Fiction, 3.5, 1);
    db.EmplaceBack("High", "C", 2000, Genre::Fiction, 4.9, 1);
    auto top2 = getTopNBy(db, 2, comp::LessByRating{});
    ASSERT_EQ(top2.size(), 2u);
    EXPECT_DOUBLE_EQ(top2[0].get().rating, 4.9);
    EXPECT_DOUBLE_EQ(top2[1].get().rating, 3.5);
}

TEST(Statistics, SampleRandomBooks) {
    BookDatabase<> db;
    for (int i = 0; i < 10; ++i) {
        db.EmplaceBack("Book" + std::to_string(i), "Author", 2000, Genre::Fiction, 4.0, 1);
    }
    auto sample = sampleRandomBooks(db, 3);
    EXPECT_EQ(sample.size(), 3u);
}

TEST(Statistics, BuildAuthorHistogram) {
    BookDatabase<> db;
    db.EmplaceBack("A", "Orwell", 2000, Genre::Fiction, 4.0, 1);
    db.EmplaceBack("B", "Orwell", 2001, Genre::Fiction, 4.2, 2);
    db.EmplaceBack("C", "Tolkien", 2002, Genre::Fiction, 4.9, 3);
    auto hist = buildAuthorHistogram(db);
    EXPECT_EQ(hist.at("Orwell"), 2);
    EXPECT_EQ(hist.at("Tolkien"), 1);
}

// Genre

TEST(Genre, FromStringAndBack) {
    EXPECT_EQ(GenreFromString("Fiction"), Genre::Fiction);
    EXPECT_EQ(GenreFromString("SciFi"), Genre::SciFi);
    EXPECT_EQ(GenreFromString("NonFiction"), Genre::NonFiction);
    EXPECT_EQ(GenreFromString("Biography"), Genre::Biography);
    EXPECT_EQ(GenreFromString("Mystery"), Genre::Mystery);
    EXPECT_EQ(GenreFromString("Unknown"), Genre::Unknown);
    EXPECT_EQ(GenreFromString("garbage"), Genre::Unknown);
}

TEST(Book, ConstructFromGenreString) {
    Book b{"Dune", "Frank Herbert", 1965, "SciFi", 4.8, 300};
    EXPECT_EQ(b.genre, Genre::SciFi);
}

// Performance

TEST(Performance, LargeDatabaseInsertAndFilter) {
    BookDatabase<> db;
    const int N = 10000;
    for (int i = 0; i < N; ++i) {
        db.EmplaceBack("Book" + std::to_string(i), "Author" + std::to_string(i % 100),
                       1900 + (i % 124), (i % 2 == 0) ? Genre::Fiction : Genre::SciFi,
                       3.0 + (i % 20) * 0.1, i);
    }
    EXPECT_EQ(db.size(), static_cast<size_t>(N));
    EXPECT_EQ(db.GetAuthors().size(), 100u);
    auto fiction = filterBooks(db.begin(), db.end(), GenreIs(Genre::Fiction));
    EXPECT_EQ(fiction.size(), static_cast<size_t>(N / 2));
}
