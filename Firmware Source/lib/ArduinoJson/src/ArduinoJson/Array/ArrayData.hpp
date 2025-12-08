// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Collection/CollectionData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

class ArrayImpl : public CollectionImpl {
 public:
  ArrayImpl() {}

  ArrayImpl(CollectionData* data, ResourceManager* resources)
      : CollectionImpl(data, resources) {}

  VariantData* addElement();

  template <typename T>
  bool addValue(const T& value);

  VariantData* getOrAddElement(size_t index);

  VariantData* getElement(size_t index) const;

  void removeElement(size_t index);

  void remove(iterator it) {
    CollectionImpl::removeOne(it);
  }

 private:
  iterator at(size_t index) const;
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
