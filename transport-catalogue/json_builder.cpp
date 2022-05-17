#include "json_builder.h"

namespace json {
	using namespace std::literals;
    
    json::Node Builder::Build() {
        int open_arr = count(operations_.begin(), operations_.end(), Operation::StartArrayO);
        int close_arr = count(operations_.begin(), operations_.end(), Operation::EndArrayO);
        int open_map = count(operations_.begin(), operations_.end(), Operation::StartDictO);
        int close_map = count(operations_.begin(), operations_.end(), Operation::EndDictO);

        if (!signs_.empty() && (open_arr != close_arr || open_map != close_map)) {
            throw std::logic_error("The object is not ready"s);
        }
        if (operations_.empty()) {
            throw std::logic_error("The object is not ready"s);
        }
        return root_;
    }

    Builder::DictItemContext Builder::StartDict() {
        if (nodes_stack_.empty() && !operations_.empty() && operations_.back() == Operation::ValueO) {
            throw std::logic_error("The Value is repeated"s);
        }
        Builder::DictItemContext dict_item_context();
        json::Dict dict;

        if (!nodes_stack_.empty()) {
            if (!signs_.empty() && signs_.top() == '{' && operations_.back() != Operation::KeyO) {

                throw std::logic_error("The Map is not after Key"s);
            }

            Node* temp = nodes_stack_.back();
            Node* pdict = new Node(dict);
            if (temp->IsArray()) {
                const_cast<json::Array&>(temp->AsArray()).push_back(std::move(*pdict));
                pdict = &const_cast<json::Array&>((temp->AsArray()))[(temp->AsArray()).size() - 1];
            }
            if (temp->IsDict()) {
                const_cast<json::Dict&>(temp->AsDict())[key_.back()] = std::move(*pdict);
                pdict = &const_cast<json::Dict&>(temp->AsDict())[key_.back()];
            }
            nodes_stack_.push_back(pdict);
        }
        else {
            root_ = json::Node(dict);
            nodes_stack_.push_back(&root_);
            operations_.push_back(Operation::StartDictO);
            signs_.push('{');
            return *this;
        }
        operations_.push_back(Operation::StartDictO);
        signs_.push('{');
        return *this;
    }

    Builder& Builder::EndDict() {
        if (!signs_.empty() && signs_.top() == '{') {
            if (!operations_.empty() && (operations_.back() != Operation::ValueO && operations_.back() != Operation::StartDictO && operations_.back() != Operation::EndDictO && operations_.back() != Operation::EndArrayO)) {
                throw std::logic_error("The Map is not closed by Value"s);
            }
            signs_.pop();
        }
        else {
            throw std::logic_error("The Map is not closed"s);
        }
        operations_.push_back(Operation::EndDictO);
        nodes_stack_.pop_back();
        return *this;
    }

    Builder::ArrayItemContext Builder::StartArray() {
        if (nodes_stack_.empty() && !operations_.empty() && operations_.back() == Operation::ValueO) {
            throw std::logic_error("The Value is repeated"s);
        }

        json::Array array;

        if (!nodes_stack_.empty()) {
            if (!signs_.empty() && signs_.top() == '{' && operations_.back() != Operation::KeyO) {

                throw std::logic_error("The Array is not after Key"s);
            }

            Node* temp = nodes_stack_.back();
            Node* parr = new Node(array);
            if (temp->IsArray()) {
                const_cast<json::Array&>(temp->AsArray()).push_back(std::move(*parr));
                parr = &const_cast<json::Array&>((temp->AsArray()))[(temp->AsArray()).size() - 1];
            }
            if (temp->IsDict()) {
                const_cast<json::Dict&>(temp->AsDict())[key_.back()] = *parr;
                parr = &const_cast<json::Dict&>(temp->AsDict())[key_.back()];
            }
            nodes_stack_.push_back(parr);
        }
        else {
            root_ = json::Node(array);
            nodes_stack_.push_back(&root_);
            signs_.push('[');
            operations_.push_back(Operation::StartArrayO);
            return *this;
        }

        signs_.push('[');
        return *this;
    }

    Builder& Builder::EndArray() {
        if (!signs_.empty() && signs_.top() == '[') {
            signs_.pop();
            nodes_stack_.pop_back();
        }
        else {
            throw std::logic_error("The Array is not closed"s);
        }
        operations_.push_back(Operation::EndArrayO);
        return *this;
    }

    Builder& Builder::Key(const std::string& key) {
        if (nodes_stack_.empty() || signs_.top() != '{' || operations_.back() == Operation::KeyO) {
            throw std::logic_error("The Map is not started"s);
        }
        Node* temp = nodes_stack_.back();
        if (temp->IsDict()) {
            const_cast<json::Dict&>(temp->AsDict()).insert({ key, json::Node{} });
            key_.push_back(key);
        }
        operations_.push_back(Operation::KeyO);
        return *this;
    }

    Builder& Builder::Value(json::Node value) {
        if (nodes_stack_.empty()) {
            if (!operations_.empty() && operations_.back() == Operation::ValueO) {
                throw std::logic_error("The Value is repeated"s);
            }
            if (signs_.empty() && !operations_.empty()) {
                throw std::logic_error("The Value is wrong"s);
            }
            root_ = value;
            operations_.push_back(Operation::ValueO);
            return *this;
        }

        if (!signs_.empty() && signs_.top() == '{' && operations_.back() != Operation::KeyO) {
            throw std::logic_error("The Value is not after Key"s);
        }

        Node* temp = nodes_stack_.back();
        if (temp->IsArray()) {
            const_cast<json::Array&>(temp->AsArray()).push_back(std::move(value));
        }
        if (temp->IsDict()) {
            const_cast<json::Dict&>(temp->AsDict())[key_.back()] = std::move(value);
        }
        operations_.push_back(Operation::ValueO);
        return *this;
    }

}

 