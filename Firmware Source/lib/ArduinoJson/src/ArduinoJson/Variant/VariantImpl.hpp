// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Memory/ResourceManager.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename T>
inline void VariantData::setRawString(SerializedValue<T> value,
                                      ResourceManager* resources) {
  ARDUINOJSON_ASSERT(type_ == VariantType::Null);  // must call clear() first
  auto dup = resources->saveString(adaptString(value.data(), value.size()));
  if (dup)
    setRawString(dup);
}

template <typename TAdaptedString>
inline bool VariantData::setString(TAdaptedString value,
                                   ResourceManager* resources) {
  ARDUINOJSON_ASSERT(type_ == VariantType::Null);  // must call clear() first

  if (value.isNull())
    return false;

  if (isTinyString(value, value.size())) {
    setTinyString(value);
    return true;
  }

  auto dup = resources->saveString(value);
  if (dup) {
    setLongString(dup);
    return true;
  }

  return false;
}

inline void VariantData::clear(ResourceManager* resources) {
  if (type_ & VariantTypeBits::OwnedStringBit)
    resources->dereferenceString(content_.asStringNode->data);

#if ARDUINOJSON_USE_8_BYTE_POOL
  if (type_ & VariantTypeBits::EightByteBit)
    resources->freeEightByte(content_.asSlotId);
#endif

  asCollection(resources).clear();

  type_ = VariantType::Null;
}

#if ARDUINOJSON_USE_8_BYTE_POOL
inline const EightByteValue* VariantData::getEightByte(
    const ResourceManager* resources) const {
  return type_ & VariantTypeBits::EightByteBit
             ? resources->getEightByte(content_.asSlotId)
             : 0;
}
#endif

template <typename T>
enable_if_t<sizeof(T) == 8, bool> VariantData::setFloat(
    T value, ResourceManager* resources) {
  ARDUINOJSON_ASSERT(type_ == VariantType::Null);  // must call clear() first
  (void)resources;                                 // silence warning

  float valueAsFloat = static_cast<float>(value);

#if ARDUINOJSON_USE_DOUBLE
  if (value == valueAsFloat) {
    type_ = VariantType::Float;
    content_.asFloat = valueAsFloat;
  } else {
    auto slot = resources->allocEightByte();
    if (!slot)
      return false;
    type_ = VariantType::Double;
    content_.asSlotId = slot.id();
    slot->asDouble = value;
  }
#else
  type_ = VariantType::Float;
  content_.asFloat = valueAsFloat;
#endif
  return true;
}

template <typename T>
enable_if_t<is_signed<T>::value, bool> VariantData::setInteger(
    T value, ResourceManager* resources) {
  ARDUINOJSON_ASSERT(type_ == VariantType::Null);  // must call clear() first
  (void)resources;                                 // silence warning

  if (canConvertNumber<int32_t>(value)) {
    type_ = VariantType::Int32;
    content_.asInt32 = static_cast<int32_t>(value);
  }
#if ARDUINOJSON_USE_LONG_LONG
  else {
    auto slot = resources->allocEightByte();
    if (!slot)
      return false;
    type_ = VariantType::Int64;
    content_.asSlotId = slot.id();
    slot->asInt64 = value;
  }
#endif
  return true;
}

template <typename T>
enable_if_t<is_unsigned<T>::value, bool> VariantData::setInteger(
    T value, ResourceManager* resources) {
  ARDUINOJSON_ASSERT(type_ == VariantType::Null);  // must call clear() first
  (void)resources;                                 // silence warning

  if (canConvertNumber<uint32_t>(value)) {
    type_ = VariantType::Uint32;
    content_.asUint32 = static_cast<uint32_t>(value);
  }
#if ARDUINOJSON_USE_LONG_LONG
  else {
    auto slot = resources->allocEightByte();
    if (!slot)
      return false;
    type_ = VariantType::Uint64;
    content_.asSlotId = slot.id();
    slot->asUint64 = value;
  }
#endif
  return true;
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
