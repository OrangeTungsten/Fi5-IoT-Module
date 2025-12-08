// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Collection/CollectionData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

class ObjectImpl : public CollectionImpl {
 public:
  ObjectImpl() {}

  ObjectImpl(CollectionData* data, ResourceManager* resources)
      : CollectionImpl(data, resources) {}

  template <typename TAdaptedString>
  VariantData* addMember(TAdaptedString key);

  VariantData* addPair(VariantData** value);

  template <typename TAdaptedString>
  VariantData* getOrAddMember(TAdaptedString key);

  template <typename TAdaptedString>
  VariantData* getMember(TAdaptedString key) const;

  template <typename TAdaptedString>
  void removeMember(TAdaptedString key);

  void remove(iterator it) {
    CollectionImpl::removePair(it);
  }

  size_t size() const {
    return CollectionImpl::size() / 2;
  }

 private:
  template <typename TAdaptedString>
  iterator findKey(TAdaptedString key) const;
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
