// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Collection/CollectionData.hpp>
#include <ArduinoJson/Memory/Alignment.hpp>
#include <ArduinoJson/Strings/StringAdapters.hpp>
#include <ArduinoJson/Variant/VariantCompare.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

inline void CollectionIterator::next(const ResourceManager* resources) {
  ARDUINOJSON_ASSERT(slot_);
  auto nextId = slot_->next();
  slot_ = resources->getVariant(nextId);
  currentId_ = nextId;
}

inline CollectionImpl::iterator CollectionImpl::createIterator() const {
  if (!data_)
    return iterator();
  return iterator(resources_->getVariant(data_->head), data_->head);
}

inline void CollectionImpl::appendOne(Slot<VariantData> slot) {
  ARDUINOJSON_ASSERT(data_ != nullptr);
  ARDUINOJSON_ASSERT(resources_ != nullptr);

  if (data_->tail != NULL_SLOT) {
    auto tail = resources_->getVariant(data_->tail);
    tail->setNext(slot.id());
    data_->tail = slot.id();
  } else {
    data_->head = slot.id();
    data_->tail = slot.id();
  }
}

inline void CollectionImpl::appendPair(Slot<VariantData> key,
                                       Slot<VariantData> value) {
  ARDUINOJSON_ASSERT(data_ != nullptr);
  ARDUINOJSON_ASSERT(resources_ != nullptr);

  key->setNext(value.id());

  if (data_->tail != NULL_SLOT) {
    auto tail = resources_->getVariant(data_->tail);
    tail->setNext(key.id());
    data_->tail = value.id();
  } else {
    data_->head = key.id();
    data_->tail = value.id();
  }
}

inline void CollectionImpl::clear() {
  if (!data_)
    return;
  auto next = data_->head;
  while (next != NULL_SLOT) {
    auto currId = next;
    auto slot = resources_->getVariant(next);
    next = slot->next();
    resources_->freeVariant({slot, currId});
  }

  data_->head = NULL_SLOT;
  data_->tail = NULL_SLOT;
}

inline Slot<VariantData> CollectionImpl::getPreviousSlot(
    VariantData* target) const {
  auto prev = Slot<VariantData>();
  auto currentId = data_->head;
  while (currentId != NULL_SLOT) {
    auto currentSlot = resources_->getVariant(currentId);
    if (currentSlot == target)
      break;
    prev = Slot<VariantData>(currentSlot, currentId);
    currentId = currentSlot->next();
  }
  return prev;
}

inline void CollectionImpl::removeOne(iterator it) {
  if (it.done())
    return;
  auto curr = it.slot_;
  auto prev = getPreviousSlot(curr);
  auto next = curr->next();
  if (prev)
    prev->setNext(next);
  else
    data_->head = next;
  if (next == NULL_SLOT)
    data_->tail = prev.id();
  resources_->freeVariant({it.slot_, it.currentId_});
}

inline void CollectionImpl::removePair(ObjectImpl::iterator it) {
  if (it.done())
    return;

  auto keySlot = it.slot_;

  auto valueId = keySlot->next();
  auto valueSlot = resources_->getVariant(valueId);

  // remove value slot
  keySlot->setNext(valueSlot->next());
  resources_->freeVariant({valueSlot, valueId});

  // remove key slot
  removeOne(it);
}

inline size_t CollectionImpl::nesting() const {
  if (!data_)
    return 0;
  size_t maxChildNesting = 0;
  for (auto it = createIterator(); !it.done(); it.next(resources_)) {
    size_t childNesting = it->nesting(resources_);
    if (childNesting > maxChildNesting)
      maxChildNesting = childNesting;
  }
  return maxChildNesting + 1;
}

inline size_t CollectionImpl::size() const {
  size_t count = 0;
  for (auto it = createIterator(); !it.done(); it.next(resources_))
    count++;
  return count;
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
