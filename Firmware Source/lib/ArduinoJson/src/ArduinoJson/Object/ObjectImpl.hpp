// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Object/ObjectData.hpp>
#include <ArduinoJson/Variant/VariantCompare.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename TAdaptedString>
inline VariantData* ObjectImpl::getMember(TAdaptedString key) const {
  auto it = findKey(key);
  if (it.done())
    return nullptr;
  it.next(resources_);
  return it.data();
}

template <typename TAdaptedString>
VariantData* ObjectImpl::getOrAddMember(TAdaptedString key) {
  auto data = getMember(key);
  if (data)
    return data;
  return addMember(key);
}

template <typename TAdaptedString>
inline ObjectImpl::iterator ObjectImpl::findKey(TAdaptedString key) const {
  if (key.isNull())
    return iterator();
  bool isKey = true;
  for (auto it = createIterator(); !it.done(); it.next(resources_)) {
    if (isKey && stringEquals(key, adaptString(it->asString())))
      return it;
    isKey = !isKey;
  }
  return iterator();
}

template <typename TAdaptedString>
inline void ObjectImpl::removeMember(TAdaptedString key) {
  remove(findKey(key));
}

template <typename TAdaptedString>
inline VariantData* ObjectImpl::addMember(TAdaptedString key) {
  if (!data_)
    return nullptr;
  ARDUINOJSON_ASSERT(resources_ != nullptr);

  auto keySlot = resources_->allocVariant();
  if (!keySlot)
    return nullptr;

  auto valueSlot = resources_->allocVariant();
  if (!valueSlot)
    return nullptr;

  if (!keySlot->setString(key, resources_))
    return nullptr;

  CollectionImpl::appendPair(keySlot, valueSlot);

  return valueSlot.ptr();
}

inline VariantData* ObjectImpl::addPair(VariantData** value) {
  if (!data_)
    return nullptr;
  ARDUINOJSON_ASSERT(resources_ != nullptr);

  auto keySlot = resources_->allocVariant();
  if (!keySlot)
    return nullptr;

  auto valueSlot = resources_->allocVariant();
  if (!valueSlot)
    return nullptr;
  *value = valueSlot.ptr();

  CollectionImpl::appendPair(keySlot, valueSlot);

  return keySlot.ptr();
}

// Returns the size (in bytes) of an object with n members.
constexpr size_t sizeofObject(size_t n) {
  return 2 * n * sizeof(VariantData);
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
