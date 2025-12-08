// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Array/JsonArray.hpp>
#include <ArduinoJson/Object/JsonObject.hpp>
#include <ArduinoJson/Variant/JsonVariant.hpp>
#include <ArduinoJson/Variant/VariantDataVisitor.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename TResult>
struct JsonVariantVisitor {
  using result_type = TResult;

  template <typename T>
  TResult visit(const T&) {
    return TResult();
  }
};

template <typename TVisitor>
class VisitorAdapter {
 public:
  using result_type = typename TVisitor::result_type;

  VisitorAdapter(TVisitor& visitor) : visitor_(&visitor) {}

  result_type visit(const ArrayImpl& array) {
    return visitor_->visit(JsonArrayConst(array));
  }

  result_type visit(const ObjectImpl& object) {
    return visitor_->visit(JsonObjectConst(object));
  }

  template <typename T>
  result_type visit(const T& value) {
    return visitor_->visit(value);
  }

 private:
  TVisitor* visitor_;
};

template <typename TVisitor>
typename TVisitor::result_type accept(JsonVariantConst variant,
                                      TVisitor& visit) {
  auto data = VariantAttorney::getData(variant);
  if (!data)
    return visit.visit(nullptr);
  auto resources = VariantAttorney::getResourceManager(variant);
  VisitorAdapter<TVisitor> adapter(visit);
  return data->accept(adapter, resources);
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
