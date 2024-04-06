#include "fintamath/core/MathObjectIdStorage.hpp"

namespace fintamath {

size_t MathObjectIdStorage::get(const MathObjectClass objClass) noexcept {
  auto iter = getMap().find(objClass);
  return iter != getMap().end() ? iter->second : 0;
}

void MathObjectIdStorage::add(const MathObjectClass objClass) noexcept {
  getMap().insert_or_assign(objClass, maxId++);
}

MathObjectIdStorage::MathObjectClassToIdMap &MathObjectIdStorage::getMap() noexcept {
  static MathObjectIdStorage::MathObjectClassToIdMap map;
  return map;
}

}
