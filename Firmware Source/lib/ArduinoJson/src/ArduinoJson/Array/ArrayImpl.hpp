// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Array/ArrayData.hpp>
#include <ArduinoJson/Variant/VariantCompare.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

inline ArrayImpl::iterator ArrayImpl::at(size_t index) const {
  auto it = createIterator();
  while (!it.done() && index) {
    it.next(resources_);
    --index;
  }
  return it;
}

inline VariantData* ArrayImpl::addElement() {
  if (!data_)
    return nullptr;
  ARDUINOJSON_ASSERT(resources_ != nullptr);
  auto slot = resources_->allocVariant();
  if (!slot)
    return nullptr;
  CollectionImpl::appendOne(slot);
  return slot.ptr();
}

inline VariantData* ArrayImpl::getOrAddElement(size_t index) {
  auto it = createIterator();
  while (!it.done() && index > 0) {
    it.next(resources_);
    index--;
  }
  if (it.done())
    index++;
  VariantData* element = it.data();
  while (index > 0) {
    element = addElement();
    if (!element)
      return nullptr;
    index--;
  }
  return element;
}

inline VariantData* ArrayImpl::getElement(size_t index) const {
  return at(index).data();
}

inline void ArrayImpl::removeElement(size_t index) {
  remove(at(index));
}

template <typename T>
inline bool ArrayImpl::addValue(const T& value) {
  if (!data_)
    return false;
  ARDUINOJSON_ASSERT(resources_ != nullptr);
  auto slot = resources_->allocVariant();
  if (!slot)
    return false;
  JsonVariant variant(slot.ptr(), resources_);
  if (!variant.set(value)) {
    resources_->freeVariant(slot);
    return false;
  }
  CollectionImpl::appendOne(slot);
  return true;
}

// Returns the size (in bytes) of an array with n elements.
constexpr size_t sizeofArray(size_t n) {
  return n * sizeof(VariantData);
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
