#pragma once
#include "storage/item.hpp"
#include <cstddef>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <vector>
namespace storage {
enum class Status { Ok, NotFound, Conflict,Overflow };
struct Result {
  Status status;
  std::optional<Item> item = std::nullopt;
};
class Storage {
public:
  Item Create(std::string name, int value);
  std::optional<Item> Get(unsigned int id) const;
  std::vector<Item> List(std::size_t offset = 0, std::size_t limit = 100) const;
  Result Update(unsigned int id, std::optional<std::string> name,
                std::optional<unsigned int> value,
                std::optional<unsigned int> version = std::nullopt);
  bool Delete(unsigned int id);
  Result Increment(unsigned int id, unsigned int delta);

private:
  mutable std::mutex mutex_;
  std::map<unsigned int, Item> items_;
  unsigned int next_id_ = 1;
};
} // namespace storage
