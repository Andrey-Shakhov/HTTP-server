#include "storage/storage.hpp"
#include <limits>
namespace storage {
Item Storage::Create(std::string name, int value) {
  std::lock_guard lock(mutex_);
  Item item{next_id_, name, value, 1};
  items_.emplace(item.id, item);
  next_id_++;
  return item;
}
std::optional<Item> Storage::Get(unsigned int id)const {
  std::lock_guard lock(mutex_);
  auto it = items_.find(id);
  if (it == items_.end()) {
    return std::nullopt;
  }
  return it->second;
}
std::vector<Item> Storage::List(std::size_t offset,
                                std::size_t limit)const {
  std::lock_guard lock(mutex_);
  std::vector<Item> result;
  auto it = items_.begin();
  while (it != items_.end() && offset > 0) {
    it++;
    offset--;
  }
  while (it != items_.end() && result.size() < limit) {
    result.push_back(it->second);
    it++;
  }
  return result;
}
Result Storage::Update(unsigned int id, std::optional<std::string> name,
                       std::optional<unsigned int> value,
                       std::optional<unsigned int> version) {
  std::lock_guard lock(mutex_);
  auto it = items_.find(id);
  if (it == items_.end()) {
    return Result{Status::NotFound};
  }
  if (version && *version != it->second.version) {
    return Result{Status::Conflict};
  }
  Item &item = it->second;
  if (name) {
    item.name = std::move(*name);
  }

  if (value) {
    item.value = *value;
  }
  item.version++;
  return Result{Status::Ok, item};
}
bool Storage::Delete(unsigned int id) {
  std::lock_guard lock(mutex_);
  return items_.erase(id) != 0;
}
Result Storage::Increment(unsigned int id, unsigned int delta) {
  std::lock_guard lock(mutex_);
  auto it = items_.find(id);
  if (it == items_.end()) {
    return Result{Status::NotFound};
  }
  Item &item = it->second;
  if ((delta > 0 &&
       item.value + delta > std::numeric_limits<unsigned int>::max()) ||
      (delta < 0 && item.value + delta < 0)) {
    return Result{Status::Overflow};
  }
item.value += delta;
item.version++;
return Result{Status::Ok, item};
}

} // namespace storage

// namespace storage
