#include "util/JsonLite.hpp"

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace xuitch::util {
namespace {

const JsonValue::Object kEmptyObject{};
const JsonValue::Array kEmptyArray{};

std::string escapeString(const std::string& input) {
    std::ostringstream out;
    out << '"';
    for (unsigned char c : input) {
        switch (c) {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\b': out << "\\b"; break;
            case '\f': out << "\\f"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default:
                if (c < 0x20) {
                    out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c)
                        << std::dec << std::setw(0);
                } else {
                    out << static_cast<char>(c);
                }
        }
    }
    out << '"';
    return out.str();
}

class Parser {
public:
    explicit Parser(const std::string& text) : text(text) {}

    bool run(JsonValue& out, std::string* error) {
        skipWs();
        if (!parseValue(out)) return fail(error);
        skipWs();
        if (pos != text.size()) {
            message = "Unexpected trailing JSON data at offset " + std::to_string(pos);
            return fail(error);
        }
        return true;
    }

private:
    const std::string& text;
    std::size_t pos{0};
    std::string message;

    bool fail(std::string* error) {
        if (message.empty()) message = "Invalid JSON at offset " + std::to_string(pos);
        if (error) *error = message;
        return false;
    }

    void skipWs() {
        while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) ++pos;
    }

    bool consume(char c) {
        if (pos < text.size() && text[pos] == c) { ++pos; return true; }
        return false;
    }

    bool parseValue(JsonValue& out) {
        skipWs();
        if (pos >= text.size()) { message = "Unexpected end of JSON"; return false; }
        const char c = text[pos];
        if (c == '{') return parseObject(out);
        if (c == '[') return parseArray(out);
        if (c == '"') { std::string s; if (!parseString(s)) return false; out = JsonValue(std::move(s)); return true; }
        if (c == 't' && text.compare(pos, 4, "true") == 0) { pos += 4; out = JsonValue(true); return true; }
        if (c == 'f' && text.compare(pos, 5, "false") == 0) { pos += 5; out = JsonValue(false); return true; }
        if (c == 'n' && text.compare(pos, 4, "null") == 0) { pos += 4; out = JsonValue(nullptr); return true; }
        if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parseNumber(out);
        message = "Unexpected JSON token at offset " + std::to_string(pos);
        return false;
    }

    bool parseObject(JsonValue& out) {
        consume('{'); skipWs();
        JsonValue::Object obj;
        if (consume('}')) { out = JsonValue(std::move(obj)); return true; }
        while (true) {
            skipWs();
            std::string key;
            if (!parseString(key)) { message = "Expected object key at offset " + std::to_string(pos); return false; }
            skipWs();
            if (!consume(':')) { message = "Expected ':' at offset " + std::to_string(pos); return false; }
            JsonValue value;
            if (!parseValue(value)) return false;
            obj.emplace(std::move(key), std::move(value));
            skipWs();
            if (consume('}')) break;
            if (!consume(',')) { message = "Expected ',' or '}' at offset " + std::to_string(pos); return false; }
        }
        out = JsonValue(std::move(obj));
        return true;
    }

    bool parseArray(JsonValue& out) {
        consume('['); skipWs();
        JsonValue::Array arr;
        if (consume(']')) { out = JsonValue(std::move(arr)); return true; }
        while (true) {
            JsonValue value;
            if (!parseValue(value)) return false;
            arr.emplace_back(std::move(value));
            skipWs();
            if (consume(']')) break;
            if (!consume(',')) { message = "Expected ',' or ']' at offset " + std::to_string(pos); return false; }
        }
        out = JsonValue(std::move(arr));
        return true;
    }

    bool parseString(std::string& out) {
        if (!consume('"')) return false;
        out.clear();
        while (pos < text.size()) {
            char c = text[pos++];
            if (c == '"') return true;
            if (c != '\\') { out.push_back(c); continue; }
            if (pos >= text.size()) return false;
            char e = text[pos++];
            switch (e) {
                case '"': out.push_back('"'); break;
                case '\\': out.push_back('\\'); break;
                case '/': out.push_back('/'); break;
                case 'b': out.push_back('\b'); break;
                case 'f': out.push_back('\f'); break;
                case 'n': out.push_back('\n'); break;
                case 'r': out.push_back('\r'); break;
                case 't': out.push_back('\t'); break;
                case 'u': {
                    if (pos + 4 > text.size()) return false;
                    unsigned code = 0;
                    for (int i = 0; i < 4; ++i) {
                        char h = text[pos++];
                        code <<= 4;
                        if (h >= '0' && h <= '9') code |= h - '0';
                        else if (h >= 'a' && h <= 'f') code |= h - 'a' + 10;
                        else if (h >= 'A' && h <= 'F') code |= h - 'A' + 10;
                        else return false;
                    }
                    if (code <= 0x7F) out.push_back(static_cast<char>(code));
                    else if (code <= 0x7FF) {
                        out.push_back(static_cast<char>(0xC0 | (code >> 6)));
                        out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
                    } else {
                        out.push_back(static_cast<char>(0xE0 | (code >> 12)));
                        out.push_back(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
                        out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
                    }
                    break;
                }
                default: return false;
            }
        }
        return false;
    }

    bool parseNumber(JsonValue& out) {
        const char* start = text.c_str() + pos;
        char* end = nullptr;
        double v = std::strtod(start, &end);
        if (end == start) return false;
        pos += static_cast<std::size_t>(end - start);
        out = JsonValue(v);
        return true;
    }
};

void stringifyInto(const JsonValue& value, std::ostringstream& out) {
    if (value.isNull()) { out << "null"; return; }
    if (value.isBool()) { out << (value.asBool() ? "true" : "false"); return; }
    if (value.isNumber()) {
        const double n = value.asNumber();
        if (std::floor(n) == n) out << static_cast<long long>(n);
        else out << std::setprecision(15) << n;
        return;
    }
    if (value.isString()) { out << escapeString(value.asString()); return; }
    if (value.isArray()) {
        out << '['; bool first = true;
        for (const auto& item : value.asArray()) {
            if (!first) out << ',';
            first = false;
            stringifyInto(item, out);
        }
        out << ']'; return;
    }
    out << '{'; bool first = true;
    for (const auto& [key, item] : value.asObject()) {
        if (!first) out << ',';
        first = false;
        out << escapeString(key) << ':';
        stringifyInto(item, out);
    }
    out << '}';
}

} // namespace

JsonValue::JsonValue() : data(nullptr) {}
JsonValue::JsonValue(std::nullptr_t) : data(nullptr) {}
JsonValue::JsonValue(bool value) : data(value) {}
JsonValue::JsonValue(int value) : data(static_cast<double>(value)) {}
JsonValue::JsonValue(std::int64_t value) : data(static_cast<double>(value)) {}
JsonValue::JsonValue(double value) : data(value) {}
JsonValue::JsonValue(const char* value) : data(std::string(value ? value : "")) {}
JsonValue::JsonValue(std::string value) : data(std::move(value)) {}
JsonValue::JsonValue(Object value) : data(std::move(value)) {}
JsonValue::JsonValue(Array value) : data(std::move(value)) {}

bool JsonValue::isNull() const { return std::holds_alternative<std::nullptr_t>(data); }
bool JsonValue::isBool() const { return std::holds_alternative<bool>(data); }
bool JsonValue::isNumber() const { return std::holds_alternative<double>(data); }
bool JsonValue::isString() const { return std::holds_alternative<std::string>(data); }
bool JsonValue::isObject() const { return std::holds_alternative<Object>(data); }
bool JsonValue::isArray() const { return std::holds_alternative<Array>(data); }
bool JsonValue::asBool(bool fallback) const { return isBool() ? std::get<bool>(data) : fallback; }
double JsonValue::asNumber(double fallback) const { return isNumber() ? std::get<double>(data) : fallback; }
std::string JsonValue::asString(std::string fallback) const { return isString() ? std::get<std::string>(data) : std::move(fallback); }
const JsonValue::Object& JsonValue::asObject() const { return isObject() ? std::get<Object>(data) : kEmptyObject; }
const JsonValue::Array& JsonValue::asArray() const { return isArray() ? std::get<Array>(data) : kEmptyArray; }

const JsonValue* JsonValue::get(const std::string& key) const {
    if (!isObject()) return nullptr;
    const auto& o = std::get<Object>(data);
    auto it = o.find(key);
    return it == o.end() ? nullptr : &it->second;
}
JsonValue* JsonValue::get(const std::string& key) {
    if (!isObject()) return nullptr;
    auto& o = std::get<Object>(data);
    auto it = o.find(key);
    return it == o.end() ? nullptr : &it->second;
}

std::string JsonValue::stringify() const {
    std::ostringstream out;
    stringifyInto(*this, out);
    return out.str();
}

bool JsonValue::parse(const std::string& text, JsonValue& out, std::string* error) {
    return Parser(text).run(out, error);
}

std::string jsonString(const std::string& value) { return escapeString(value); }

} // namespace xuitch::util
