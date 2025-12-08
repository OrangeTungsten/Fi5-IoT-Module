// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Memory/MemoryPool.hpp>
#include <ArduinoJson/Memory/StringNode.hpp>
#include <ArduinoJson/Misc/SerializedValue.hpp>
#include <ArduinoJson/Numbers/convertNumber.hpp>
#include <ArduinoJson/Strings/JsonString.hpp>
#include <ArduinoJson/Strings/StringAdapters.hpp>
#include <ArduinoJson/Variant/VariantContent.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename T>
T parseNumber(const char* s);

template <typename T>
static bool isTinyString(const T& s, size_t n) {
  if (n > tinyStringMaxLength)
    return false;
  bool containsNul = false;
  for (uint8_t i = 0; i < uint8_t(n); i++)
    containsNul |= !s[i];
  return !containsNul;
}

class VariantData {
  VariantContent content_;  // must be first to allow cast from array to variant
  VariantType type_;
  SlotId next_;

 public:
  // Placement new
  static void* operator new(size_t, void* p) noexcept {
    return p;
  }

  static void operator delete(void*, void*) noexcept {}

  VariantData() : type_(VariantType::Null), next_(NULL_SLOT) {}

  SlotId next() const {
    return next_;
  }

  void setNext(SlotId slot) {
    next_ = slot;
  }

  template <typename TVisitor>
  typename TVisitor::result_type accept(TVisitor& visit,
                                        ResourceManager* resources) {
#if ARDUINOJSON_USE_8_BYTE_POOL
    auto eightByteValue = getEightByte(resources);
#else
    (void)resources;  // silence warning
#endif
    switch (type_) {
      case VariantType::Float:
        return visit.visit(content_.asFloat);

#if ARDUINOJSON_USE_DOUBLE
      case VariantType::Double:
        return visit.visit(eightByteValue->asDouble);
#endif

      case VariantType::Array:
        return visit.visit(asArray(resources));

      case VariantType::Object:
        return visit.visit(asObject(resources));

      case VariantType::TinyString:
        return visit.visit(JsonString(content_.asTinyString));

      case VariantType::LongString:
        return visit.visit(JsonString(content_.asStringNode->data,
                                      content_.asStringNode->length));

      case VariantType::RawString:
        return visit.visit(RawString(content_.asStringNode->data,
                                     content_.asStringNode->length));

      case VariantType::Int32:
        return visit.visit(static_cast<JsonInteger>(content_.asInt32));

      case VariantType::Uint32:
        return visit.visit(static_cast<JsonUInt>(content_.asUint32));

#if ARDUINOJSON_USE_LONG_LONG
      case VariantType::Int64:
        return visit.visit(eightByteValue->asInt64);

      case VariantType::Uint64:
        return visit.visit(eightByteValue->asUint64);
#endif

      case VariantType::Boolean:
        return visit.visit(content_.asBoolean != 0);

      default:
        return visit.visit(nullptr);
    }
  }

  template <typename TVisitor>
  static typename TVisitor::result_type accept(VariantData* var,
                                               ResourceManager* resources,
                                               TVisitor& visit) {
    if (var != 0)
      return var->accept(visit, resources);
    else
      return visit.visit(nullptr);
  }

  VariantData* addElement(ResourceManager* resources) {
    auto array = isNull() ? toArray(resources) : asArray(resources);
    return array.addElement();
  }

  static VariantData* addElement(VariantData* var, ResourceManager* resources) {
    if (!var)
      return nullptr;
    return var->addElement(resources);
  }

  template <typename T>
  bool addValue(const T& value, ResourceManager* resources) {
    auto array = isNull() ? toArray(resources) : asArray(resources);
    return array.addValue(value);
  }

  template <typename T>
  static bool addValue(VariantData* var, const T& value,
                       ResourceManager* resources) {
    if (!var)
      return false;
    return var->addValue(value, resources);
  }

  bool asBoolean(const ResourceManager* resources) const {
#if ARDUINOJSON_USE_8_BYTE_POOL
    auto eightByteValue = getEightByte(resources);
#else
    (void)resources;  // silence warning
#endif
    switch (type_) {
      case VariantType::Boolean:
        return content_.asBoolean;
      case VariantType::Uint32:
      case VariantType::Int32:
        return content_.asUint32 != 0;
      case VariantType::Float:
        return content_.asFloat != 0;
#if ARDUINOJSON_USE_DOUBLE
      case VariantType::Double:
        return eightByteValue->asDouble != 0;
#endif
      case VariantType::Null:
        return false;
#if ARDUINOJSON_USE_LONG_LONG
      case VariantType::Uint64:
      case VariantType::Int64:
        return eightByteValue->asUint64 != 0;
#endif
      default:
        return true;
    }
  }

  ArrayImpl asArray(ResourceManager* resources) {
    return ArrayImpl(isArray() ? &content_.asCollection : nullptr, resources);
  }

  static ArrayImpl asArray(VariantData* var, ResourceManager* resources) {
    return ArrayImpl(
        var && var->isArray() ? &var->content_.asCollection : nullptr,
        resources);
  }

  CollectionImpl asCollection(ResourceManager* resources) {
    return CollectionImpl(isCollection() ? &content_.asCollection : nullptr,
                          resources);
  }

  template <typename T>
  T asFloat(const ResourceManager* resources) const {
    static_assert(is_floating_point<T>::value, "T must be a floating point");
#if ARDUINOJSON_USE_8_BYTE_POOL
    auto eightByteValue = getEightByte(resources);
#else
    (void)resources;  // silence warning
#endif
    const char* str = nullptr;
    switch (type_) {
      case VariantType::Boolean:
        return static_cast<T>(content_.asBoolean);
      case VariantType::Uint32:
        return static_cast<T>(content_.asUint32);
      case VariantType::Int32:
        return static_cast<T>(content_.asInt32);
#if ARDUINOJSON_USE_LONG_LONG
      case VariantType::Uint64:
        return static_cast<T>(eightByteValue->asUint64);
      case VariantType::Int64:
        return static_cast<T>(eightByteValue->asInt64);
#endif
      case VariantType::TinyString:
        str = content_.asTinyString;
        break;
      case VariantType::LongString:
        str = content_.asStringNode->data;
        break;
      case VariantType::Float:
        return static_cast<T>(content_.asFloat);
#if ARDUINOJSON_USE_DOUBLE
      case VariantType::Double:
        return static_cast<T>(eightByteValue->asDouble);
#endif
      default:
        return 0.0;
    }

    ARDUINOJSON_ASSERT(str != nullptr);
    return parseNumber<T>(str);
  }

  template <typename T>
  T asIntegral(const ResourceManager* resources) const {
    static_assert(is_integral<T>::value, "T must be an integral type");
#if ARDUINOJSON_USE_8_BYTE_POOL
    auto eightByteValue = getEightByte(resources);
#else
    (void)resources;  // silence warning
#endif
    const char* str = nullptr;
    switch (type_) {
      case VariantType::Boolean:
        return content_.asBoolean;
      case VariantType::Uint32:
        return convertNumber<T>(content_.asUint32);
      case VariantType::Int32:
        return convertNumber<T>(content_.asInt32);
#if ARDUINOJSON_USE_LONG_LONG
      case VariantType::Uint64:
        return convertNumber<T>(eightByteValue->asUint64);
      case VariantType::Int64:
        return convertNumber<T>(eightByteValue->asInt64);
#endif
      case VariantType::TinyString:
        str = content_.asTinyString;
        break;
      case VariantType::LongString:
        str = content_.asStringNode->data;
        break;
      case VariantType::Float:
        return convertNumber<T>(content_.asFloat);
#if ARDUINOJSON_USE_DOUBLE
      case VariantType::Double:
        return convertNumber<T>(eightByteValue->asDouble);
#endif
      default:
        return 0;
    }

    ARDUINOJSON_ASSERT(str != nullptr);
    return parseNumber<T>(str);
  }

  ObjectImpl asObject(ResourceManager* resources) {
    return ObjectImpl(isObject() ? &content_.asCollection : nullptr, resources);
  }

  static ObjectImpl asObject(VariantData* var, ResourceManager* resources) {
    return ObjectImpl(
        var && var->isObject() ? &var->content_.asCollection : nullptr,
        resources);
  }

  JsonString asRawString() const {
    switch (type_) {
      case VariantType::RawString:
        return JsonString(content_.asStringNode->data,
                          content_.asStringNode->length);
      default:
        return JsonString();
    }
  }

  JsonString asString() const {
    switch (type_) {
      case VariantType::TinyString:
        return JsonString(content_.asTinyString);
      case VariantType::LongString:
        return JsonString(content_.asStringNode->data,
                          content_.asStringNode->length);
      default:
        return JsonString();
    }
  }

#if ARDUINOJSON_USE_8_BYTE_POOL
  const EightByteValue* getEightByte(const ResourceManager* resources) const;
#endif

  VariantData* getElement(size_t index, ResourceManager* resources) {
    return asArray(resources).getElement(index);
  }

  static VariantData* getElement(VariantData* var, size_t index,
                                 ResourceManager* resources) {
    if (!var)
      return nullptr;
    return var->asArray(resources).getElement(index);
  }

  template <typename TAdaptedString>
  VariantData* getMember(TAdaptedString key, ResourceManager* resources) {
    return asObject(resources).getMember(key);
  }

  template <typename TAdaptedString>
  static VariantData* getMember(VariantData* var, TAdaptedString key,
                                ResourceManager* resources) {
    if (!var)
      return 0;
    return var->getMember(key, resources);
  }

  VariantData* getOrAddElement(size_t index, ResourceManager* resources) {
    auto array = isNull() ? toArray(resources) : asArray(resources);
    return array.getOrAddElement(index);
  }

  template <typename TAdaptedString>
  VariantData* getOrAddMember(TAdaptedString key, ResourceManager* resources) {
    if (key.isNull())
      return nullptr;
    auto obj = isNull() ? toObject(resources) : asObject(resources);
    return obj.getOrAddMember(key);
  }

  bool isArray() const {
    return type_ == VariantType::Array;
  }

  bool isBoolean() const {
    return type_ == VariantType::Boolean;
  }

  bool isCollection() const {
    return type_ & VariantTypeBits::CollectionMask;
  }

  bool isFloat() const {
    return type_ & VariantTypeBits::NumberBit;
  }

  template <typename T>
  bool isInteger(const ResourceManager* resources) const {
#if ARDUINOJSON_USE_LONG_LONG
    auto eightByteValue = getEightByte(resources);
#else
    (void)resources;  // silence warning
#endif
    switch (type_) {
      case VariantType::Uint32:
        return canConvertNumber<T>(content_.asUint32);

      case VariantType::Int32:
        return canConvertNumber<T>(content_.asInt32);

#if ARDUINOJSON_USE_LONG_LONG
      case VariantType::Uint64:
        return canConvertNumber<T>(eightByteValue->asUint64);

      case VariantType::Int64:
        return canConvertNumber<T>(eightByteValue->asInt64);
#endif

      default:
        return false;
    }
  }

  bool isNull() const {
    return type_ == VariantType::Null;
  }

  static bool isNull(const VariantData* var) {
    if (!var)
      return true;
    return var->isNull();
  }

  bool isObject() const {
    return type_ == VariantType::Object;
  }

  bool isString() const {
    return type_ == VariantType::LongString || type_ == VariantType::TinyString;
  }

  size_t nesting(ResourceManager* resources) {
    return asCollection(resources).nesting();
  }

  static size_t nesting(VariantData* var, ResourceManager* resources) {
    if (!var)
      return 0;
    return var->nesting(resources);
  }

  void removeElement(size_t index, ResourceManager* resources) {
    asArray(resources).removeElement(index);
  }

  static void removeElement(VariantData* var, size_t index,
                            ResourceManager* resources) {
    if (!var)
      return;
    var->removeElement(index, resources);
  }

  template <typename TAdaptedString>
  void removeMember(TAdaptedString key, ResourceManager* resources) {
    asObject(resources).removeMember(key);
  }

  template <typename TAdaptedString>
  static void removeMember(VariantData* var, TAdaptedString key,
                           ResourceManager* resources) {
    if (!var)
      return;
    var->removeMember(key, resources);
  }

  void reset() {  // TODO: remove
    type_ = VariantType::Null;
  }

  void setBoolean(bool value) {
    ARDUINOJSON_ASSERT(type_ == VariantType::Null);  // must call clear() first
    type_ = VariantType::Boolean;
    content_.asBoolean = value;
  }

  template <typename T>
  enable_if_t<sizeof(T) == 4, bool> setFloat(T value, ResourceManager*) {
    ARDUINOJSON_ASSERT(type_ == VariantType::Null);  // must call clear() first
    type_ = VariantType::Float;
    content_.asFloat = value;
    return true;
  }

  template <typename T>
  enable_if_t<sizeof(T) == 8, bool> setFloat(T value, ResourceManager*);

  template <typename T>
  enable_if_t<is_signed<T>::value, bool> setInteger(T value,
                                                    ResourceManager* resources);

  template <typename T>
  enable_if_t<is_unsigned<T>::value, bool> setInteger(
      T value, ResourceManager* resources);

  void setRawString(StringNode* s) {
    ARDUINOJSON_ASSERT(type_ == VariantType::Null);  // must call clear() first
    ARDUINOJSON_ASSERT(s);
    type_ = VariantType::RawString;
    content_.asStringNode = s;
  }

  template <typename T>
  void setRawString(SerializedValue<T> value, ResourceManager* resources);

  template <typename T>
  static void setRawString(VariantData* var, SerializedValue<T> value,
                           ResourceManager* resources) {
    if (!var)
      return;
    var->clear(resources);
    var->setRawString(value, resources);
  }

  template <typename TAdaptedString>
  bool setString(TAdaptedString value, ResourceManager* resources);

  template <typename TAdaptedString>
  static void setString(VariantData* var, TAdaptedString value,
                        ResourceManager* resources) {
    if (!var)
      return;
    var->clear(resources);
    var->setString(value, resources);
  }

  template <typename TAdaptedString>
  void setTinyString(const TAdaptedString& s) {
    ARDUINOJSON_ASSERT(type_ == VariantType::Null);  // must call clear() first
    ARDUINOJSON_ASSERT(s.size() <= tinyStringMaxLength);

    type_ = VariantType::TinyString;

    auto n = uint8_t(s.size());
    for (uint8_t i = 0; i < n; i++) {
      char c = s[i];
      ARDUINOJSON_ASSERT(c != 0);  // no NUL in tiny string
      content_.asTinyString[i] = c;
    }

    content_.asTinyString[n] = 0;
  }

  void setLongString(StringNode* s) {
    ARDUINOJSON_ASSERT(type_ == VariantType::Null);  // must call clear() first
    ARDUINOJSON_ASSERT(s);
    type_ = VariantType::LongString;
    content_.asStringNode = s;
  }

  size_t size(ResourceManager* resources) {
    if (isObject())
      return asObject(resources).size();

    if (isArray())
      return asArray(resources).size();

    return 0;
  }

  static size_t size(VariantData* var, ResourceManager* resources) {
    return var != 0 ? var->size(resources) : 0;
  }

  ArrayImpl toArray(ResourceManager* resources) {
    ARDUINOJSON_ASSERT(type_ == VariantType::Null);  // must call clear() first
    type_ = VariantType::Array;
    return ArrayImpl(new (&content_.asCollection) CollectionData(), resources);
  }

  static ArrayImpl toArray(VariantData* var, ResourceManager* resources) {
    if (!var)
      return ArrayImpl(nullptr, resources);
    var->clear(resources);
    return var->toArray(resources);
  }

  ObjectImpl toObject(ResourceManager* resources) {
    ARDUINOJSON_ASSERT(type_ == VariantType::Null);  // must call clear() first
    type_ = VariantType::Object;
    return ObjectImpl(new (&content_.asCollection) CollectionData(), resources);
  }

  static ObjectImpl toObject(VariantData* var, ResourceManager* resources) {
    if (!var)
      return ObjectImpl();
    var->clear(resources);
    return var->toObject(resources);
  }

  VariantType type() const {
    return type_;
  }

  // Release the resources used by this variant and set it to null.
  void clear(ResourceManager* resources);

  static void clear(VariantData* var, ResourceManager* resources) {
    if (!var)
      return;
    var->clear(resources);
  }
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
