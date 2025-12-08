// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Collection/CollectionData.hpp>
#include <ArduinoJson/Memory/ResourceManager.hpp>
#include <ArduinoJson/Polyfills/alias_cast.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

inline Slot<VariantData> ResourceManager::allocVariant() {
  auto slot = variantPools_.allocSlot(allocator_);
  if (!slot) {
    overflowed_ = true;
    return {};
  }
  new (slot.ptr()) VariantData();
  return slot;
}

inline void ResourceManager::freeVariant(Slot<VariantData> slot) {
  slot->clear(this);
  variantPools_.freeSlot(slot);
}

inline VariantData* ResourceManager::getVariant(SlotId id) const {
  return reinterpret_cast<VariantData*>(variantPools_.getSlot(id));
}

#if ARDUINOJSON_USE_8_BYTE_POOL
inline Slot<EightByteValue> ResourceManager::allocEightByte() {
  auto slot = eightBytePools_.allocSlot(allocator_);
  if (!slot) {
    overflowed_ = true;
    return {};
  }
  return slot;
}

inline void ResourceManager::freeEightByte(SlotId id) {
  auto p = getEightByte(id);
  eightBytePools_.freeSlot({p, id});
}

inline EightByteValue* ResourceManager::getEightByte(SlotId id) const {
  return eightBytePools_.getSlot(id);
}
#endif

ARDUINOJSON_END_PRIVATE_NAMESPACE
