// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Memory/MemoryPool.hpp>
#include <ArduinoJson/Namespace.hpp>
#include <ArduinoJson/Polyfills/assert.hpp>

#include <stddef.h>  // size_t

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

class VariantData;
class ResourceManager;

class CollectionIterator {
  friend class CollectionImpl;

 public:
  CollectionIterator() : slot_(nullptr), currentId_(NULL_SLOT) {}

  void next(const ResourceManager* resources);

  bool done() const {
    return slot_ == nullptr;
  }

  bool operator==(const CollectionIterator& other) const {
    return slot_ == other.slot_;
  }

  bool operator!=(const CollectionIterator& other) const {
    return slot_ != other.slot_;
  }

  VariantData* operator->() {
    ARDUINOJSON_ASSERT(slot_ != nullptr);
    return data();
  }

  VariantData& operator*() {
    ARDUINOJSON_ASSERT(slot_ != nullptr);
    return *data();
  }

  const VariantData& operator*() const {
    ARDUINOJSON_ASSERT(slot_ != nullptr);
    return *data();
  }

  VariantData* data() {
    return slot_;
  }

  const VariantData* data() const {
    return slot_;
  }

 private:
  CollectionIterator(VariantData* slot, SlotId slotId)
      : slot_(slot), currentId_(slotId) {}

  VariantData* slot_;
  SlotId currentId_;
};

struct CollectionData {
  SlotId head = NULL_SLOT;
  SlotId tail = NULL_SLOT;

  // Placement new
  static void* operator new(size_t, void* p) noexcept {
    return p;
  }

  static void operator delete(void*, void*) noexcept {}
};

class CollectionImpl {
 protected:
  CollectionData* data_;
  ResourceManager* resources_;

 public:
  using iterator = CollectionIterator;

  CollectionImpl() : data_(nullptr), resources_(nullptr) {}

  CollectionImpl(CollectionData* data, ResourceManager* resources)
      : data_(data), resources_(resources) {}

  explicit operator bool() const {
    return data_ != nullptr;
  }

  bool isNull() const {
    return data_ == nullptr;
  }

  VariantData* getData() const {
    void* data = data_;  // prevent warning cast-align
    return reinterpret_cast<VariantData*>(data);
  }

  ResourceManager* getResourceManager() const {
    return resources_;
  }

  iterator createIterator() const;

  size_t size() const;
  size_t nesting() const;

  void clear();

  SlotId head() const {
    return data_->head;
  }

 protected:
  void appendOne(Slot<VariantData> slot);
  void appendPair(Slot<VariantData> key, Slot<VariantData> value);

  void removeOne(iterator it);
  void removePair(iterator it);

 private:
  Slot<VariantData> getPreviousSlot(VariantData*) const;
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
