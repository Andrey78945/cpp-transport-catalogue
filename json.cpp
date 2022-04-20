#include "json.h"

#include <iomanip>
#include <iostream>

#include <string.h>
#include <stdio.h>

//using namespace std;
using namespace std::literals;

namespace json {

    namespace {

        Node LoadNode(std::istream& input);

        Node LoadNumber(std::istream& input) {
            using namespace std::literals;
            std::string parsed_num;
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read from stream"s);
                }
            };

            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            if (input.peek() == '0') {
                read_char();
            }
            else {
                read_digits();
            }

            bool is_int = true;
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    try {
                        return Node(std::stoi(parsed_num));
                    }
                    catch (...) {
                    }
                }
                return Node(std::stod(parsed_num));
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node LoadConst(std::istream& input) {
            std::string result;
            while (isalpha(input.peek())) {
                result.push_back(static_cast<char>(input.get()));
            }
            if (result == "true"s) {
                return Node(true);
            }
            else if (result == "false"s) {
                return Node(false);
            }
            else if (result == "null"s) {
                return Node();
            }
            else {
                throw ParsingError("Failed to convert bool or null to Node"s);
            }
        }

        Node LoadString(std::istream& input) {
            std::string json_line;
            for (char c; input.get(c) && c != '\"';) {
                if (c == '\\') {
                    input.get(c);
                    if (!input.eof()) {
                        if (c == 'n') {
                            json_line += '\n';
                            continue;
                        }
                        else if (c == 'r') {
                            json_line += '\r';
                            continue;
                        }
                        else if (c == '\"') {
                            json_line += '\"';
                            continue;
                        }
                        else if (c == 't') {
                            json_line += '\t';
                            continue;
                        }
                        else if (c == '\\') {
                            json_line += '\\';
                            continue;
                        }
                    }
                }
                else {
                    json_line += c;
                }
            }
            if (input.eof())  throw ParsingError("Failed to load String");
            return Node(move(json_line));
        }

        Node LoadArray(std::istream& input) {
            Array result;

            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (!input)  throw ParsingError("Failed to load Array");
            return Node(move(result));
        }

        Node LoadDict(std::istream& input) {
            Dict result;

            for (char c; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }

                std::string key = LoadString(input).AsString();
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }
            if (!input)  throw ParsingError("Failed to load Map");
            return Node(move(result));
        }

        Node LoadNode(std::istream& input) {
            char c;
            input >> c;
            if (c == EOF) {
                throw ParsingError("Failed to load");
            }
            else if (c == '"') {
                return LoadString(input);
            }
            else if (c == '{') {
                input >> c;
                if (input.eof()) throw ParsingError("Failed to load Dict");
                input.unget();
                return LoadDict(input);
            }
            else if (c == '[') {
                input >> c;
                if (input.eof()) throw ParsingError("Failed to load Array");
                input.unget();
                return LoadArray(input);
            }
            else if (c == 't' || c == 'f' || c == 'n') {
                input.unget();
                return LoadConst(input);
            }
            else {
                input.putback(c);
                return LoadNumber(input);
            }
        }
    }  // namespace

    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(*this);
    }

    bool Node::IsString() const {
        return std::holds_alternative<std::string>(*this);
    }

    bool Node::IsBool() const {
        return std::holds_alternative<bool>(*this);
    }

    bool Node::IsInt() const {
        return std::holds_alternative<int>(*this);
    }

    bool Node::IsDouble() const {
        return std::holds_alternative<double>(*this) || IsInt();
    }

    bool Node::IsPureDouble() const {
        bool pure_double = 0;
        if (!IsInt() && IsDouble()) {
            std::string varAsString = std::to_string(AsDouble());
            pure_double = (count(varAsString.begin(), varAsString.end(), 'e') == 0 || count(varAsString.begin(), varAsString.end(), 'E') == 0);
        }
        return IsDouble() && pure_double;
    }

    bool Node::IsArray() const {
        return std::holds_alternative<Array>(*this);
    }

    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(*this);
    }

    const Array& Node::AsArray() const {
        if (!IsArray()) {
            throw std::invalid_argument("Failed to load Array");
        }
        return std::get<Array>(*this);
    }

    const Dict& Node::AsMap() const {
        if (!IsMap()) {
            throw std::invalid_argument("Failed to load Map");
        }
        return std::get<Dict>(*this);
    }

    bool Node::AsBool() const {
        if (!IsBool()) {
            throw std::invalid_argument("Failed to load Bool");
        }
        return std::get<bool>(*this);
    }

    int Node::AsInt() const {
        if (!IsInt()) {
            throw std::invalid_argument("Failed to load Int");
        }
        return std::get<int>(*this);
    }

    double Node::AsDouble() const {
        if (!IsDouble()) {
            throw std::invalid_argument("Failed to load Double");
        }
        double result = 0.0;
        try {
            result = std::get<double>(*this);
        }
        catch (...) {
            result = std::get<int>(*this) * 1.;
        }
        return result;
    }

    const std::string& Node::AsString() const {
        if (!IsString()) {
            throw std::invalid_argument("Failed to load Dict");
        }
        return std::get<std::string>(*this);
    }

    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(std::istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        const auto& node = doc.Document::GetRoot();
        if (node.Node::IsString()) {
            std::string temp = node.AsString();
            static std::unordered_map<char, std::string> dictionary_{ {'\n', "\\n"s}, {'\r', "\\r"s}, {'"', "\\\""s}, {'\t', "\\t"s}, {'\\', "\\\\"s} };
            output << "\""s;
            for (auto i = 0u; i < temp.size(); ++i) {
                if (dictionary_.count(temp[i]) > 0) {
                    output << dictionary_.at(temp[i]);
                }
                else {
                    output << temp[i];
                }
            }
            output << "\""s;
        }
        else if (node.Node::IsInt()) {
            output << node.AsInt();
        }
        else if (node.Node::IsDouble()) {
            output << node.AsDouble();
        }
        else if (node.Node::IsNull()) {
            output << "null"s;
        }
        else if (node.Node::IsBool()) {
            output << std::boolalpha << node.Node::AsBool();
        }
        else if (node.Node::IsArray()) {
            output << '[';
            //         cout << endl << '[';
            bool not_first = false;
            for (const auto& array_member : node.Node::AsArray()) {
                if (not_first) {
                    output << ", "s;
                }
                not_first = true;
                Print(Document(array_member), output);
            }
            output << ']';
        }
        else if (node.Node::IsMap()) {
            output << '{';
            bool not_first = false;
            for (const auto& [key, value] : node.Node::AsMap()) {
                if (not_first) {
                    output << ", "s;
                }
                Print(Document(key), output);
                output << ": "s;
                Print(Document(value), output);

                not_first = true;
            }
            output << '}';
        }
    }
}  // namespace json
