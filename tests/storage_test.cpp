#include "storage/storage.hpp"
#include <gtest/gtest.h>

TEST(StorageTest, Create) {
  storage::Storage storage;

  auto item = storage.Create("book", 10);

  EXPECT_EQ(item.id, 1);
  EXPECT_EQ(item.name, "book");
  EXPECT_EQ(item.value, 10);
  EXPECT_EQ(item.version, 1);
}

TEST(StorageTest, Get) {
  storage::Storage storage;

  auto created = storage.Create("book", 10);
  auto item = storage.Get(created.id);

  ASSERT_TRUE(item.has_value());
  EXPECT_EQ(item->value, 10);

  EXPECT_FALSE(storage.Get(999).has_value());
}

TEST(StorageTest, Update) {
  storage::Storage storage;

  auto item = storage.Create("book", 10);
  auto result = storage.Update(item.id, "new_book", 20);

  ASSERT_EQ(result.status, storage::Status::Ok);
  ASSERT_TRUE(result.item.has_value());

  EXPECT_EQ(result.item->name, "new_book");
  EXPECT_EQ(result.item->value, 20);
  EXPECT_EQ(result.item->version, 2);
}

TEST(StorageTest, Increment) {
  storage::Storage storage;

  auto item = storage.Create("counter", 0);

  for (int i = 0; i < 100; ++i) {
    storage.Increment(item.id, 1);
  }

  auto result = storage.Get(item.id);

  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->value, 100);
  EXPECT_EQ(result->version, 101);
}
