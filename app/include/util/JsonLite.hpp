#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace xuitch::util {

class JsonValue {
public:
    using Object = std::map<std::string, JsonValue>;
    using Array = std::vector<JsonValue>;
    using Storage = std::variant<std::nullptr_t, bool, double, std::string, Object, Array>;

    JsonValue();
    JsonValue(std::nullptr_t);
    JsonValue(bool value);
    JsonValue(int value);
    JsonValue(std::int64_t value);
    JsonValue(double value);
    JsonValue(const char* value);
    JsonValue(std::string value);
    JsonValue(Object value);
    JsonValue(Array value);

    bool isNull() const;
    bool isBool() const;
    bool isNumber() const;
    bool isString() const;
    bool isObject() const;
    bool isArray() const;

    bool asBool(bool fallback = false) const;
    double asNumber(double fallback = 0.0) const;
    std::string asString(std::string fallback = {}) const;
    const Object& asObject() const;
    const Array& asArray() const;

    const JsonValue* get(const std::string& key) const;
    JsonValue* get(const std::string& key);

    std::string stringify() const;
    static bool parse(const std::string& text, JsonValue& out, std::string* error = nullptr);

private:
    Storage data;
};

std::string jsonString(const std::string& value);

} // namespace xuitch::util
