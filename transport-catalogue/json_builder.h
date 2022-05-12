#include "json.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <string_view>
#include <vector>


namespace json {
    using namespace std::literals;

    class Builder {
    public:         
        class DictItemContext;
        class ArrayItemContext;
        class KeyItemContext;
        class ValueDictItemContext; 
        
        enum Operation {
            StartArrayO,
            EndArrayO,
            StartDictO,
            EndDictO,
            ValueO,
            KeyO
        };

        class ItemContext {
        public:
            ItemContext(Builder& builder)
                : builder_(builder) {}

            virtual ~ItemContext() {} 

            Builder& GetBuilder() {
                return builder_;
            }

            DictItemContext StartDict() {
                return builder_.StartDict();
            }

            Builder& EndDict() {
                return builder_.EndDict();
            }

            ArrayItemContext StartArray() {
                return builder_.StartArray();
            }

            Builder& EndArray() {
                return builder_.EndArray();
            }

            KeyItemContext Key(const std::string& key) {
                return builder_.Key(key);
            }

            Builder& Value(json::Node value) {
                return builder_.Value(value);
            }

            
        private:
            Builder& builder_;
        };

        class DictItemContext final : public ItemContext {
        public:
            DictItemContext(Builder& builder)
                : ItemContext(builder) {}

            KeyItemContext Key(const std::string& key) {
                return GetBuilder().Key(key); 
            }

            Builder& EndDict() {
                return GetBuilder().EndDict(); 
            }   

            Builder& Value(json::Node value) = delete;
            Builder& EndArray() = delete;
            ArrayItemContext StartArray() = delete;
            DictItemContext StartDict() = delete;
        }; 

        class ArrayItemContext final : public ItemContext {
        public:
            ArrayItemContext(Builder& builder)
                : ItemContext(builder) {}

            ArrayItemContext& Value(json::Node value) {
                GetBuilder().Value(value);
                return *this;
            }

            KeyItemContext Key(const std::string& key) = delete;             
            Builder& EndDict() = delete;
        };

        class ValueDictItemContext final : public ItemContext {
        public:
            ValueDictItemContext(Builder& builder)
                : ItemContext(builder) {}

            KeyItemContext Key(const std::string& key) {
                return GetBuilder().Key(key);
            }

            Builder& Value(json::Node value) = delete;
            Builder& EndArray() = delete;
            ArrayItemContext StartArray() = delete;
            ArrayItemContext StartDict() = delete;
        };

        class KeyItemContext final : public ItemContext {
        public:
            KeyItemContext(Builder& builder)
                : ItemContext(builder) {}

            ValueDictItemContext Value(json::Node value) {
                return ValueDictItemContext(GetBuilder().Value(value));
            }

            KeyItemContext Key(const std::string& key) = delete;        
            Builder& EndArray() = delete;
            Builder& EndDict() = delete;
        };        

        Builder(Node root) : root_(root) {}
        Builder() = default;

        json::Node Build();       
        DictItemContext StartDict();
        Builder& EndDict();
        ArrayItemContext StartArray();
        Builder& EndArray();       
        Builder& Key(const std::string& key);
        Builder& Value(json::Node value);
        

    private:
        json::Node root_;
        std::vector<Node*> nodes_stack_;
        std::stack<char> signs_;
        std::vector<std::string> key_;
        std::vector<Operation> operations_;
    };
}