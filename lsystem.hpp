#ifndef __MODAL_LSYSTEM_LSYSTEM__
#define __MODAL_LSYSTEM_LSYSTEM__

#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include "lazy_iterator.hpp"

using namespace std;

namespace trlsai {
    namespace lsystem {
        template <typename KEY, typename RULEDATA>
        struct RuleNode {
            RuleNode() { }
            RuleNode(KEY key, RULEDATA ruledata): key(key), ruledata(ruledata) { }
            KEY key;
            RULEDATA ruledata;
        };

        struct empty {
        };

        template <typename KEY>
        struct RuleNode<KEY, empty> {
            RuleNode() { }
            RuleNode(KEY key): key(key), ruledata(empty()) { }
            KEY key;
            empty ruledata;
        };

        template <typename KEY, typename VALUE>
        struct RawNode {
            RawNode(KEY key, VALUE value): key(key), value(value) { }
            KEY key;
            VALUE value;
        };

        template <typename KEY, typename RULEDATA, typename VALUE>
        struct Triplet: public RuleNode<KEY, RULEDATA> {
            Triplet() { }
            Triplet(KEY key, RULEDATA ruledata, VALUE value): RuleNode<KEY, RULEDATA>(key, ruledata), value(value) { }
            VALUE value;            
        };

        template <typename KEY, typename VALUE>
        struct Triplet<KEY, empty, VALUE>: public RuleNode<KEY, empty> {
            Triplet() { }
            Triplet(KEY key, VALUE value): RuleNode<KEY, empty>(key), value(value) { }
            VALUE value;
        };

        template <typename KEY, typename RULEDATA, typename VALUE>
        class Materialiser {
        public:
            virtual Triplet<KEY, RULEDATA, VALUE> produce(const KEY &key, const RULEDATA &rule_data, const Triplet<KEY, RULEDATA, VALUE> &parent, unsigned int total_siblings) = 0;
        };

        template <typename T>
        class Context {
        public:
            Context(T element): element(element), iterations(0) { }
            int iterations;
            T element;

            shared_ptr<Context<T>> child(T element) {
                auto child = make_shared<Context<T>>(element);
                child->iterations = this->iterations - 1;
                return child;
            }
        };

        template <typename KEY, typename RULEDATA, typename VALUE>
        class System: public IteratorRegistry<Triplet<KEY, RULEDATA, VALUE>> {
        public:
            using TreeNode = Triplet<KEY, RULEDATA, VALUE>;
            using Rules = map<KEY, vector<RuleNode<KEY, RULEDATA>>>;
            using RegistrationNode = RawNode<TreeNode, shared_ptr<Iterator<TreeNode>>>;
    
            System(shared_ptr<Rules> rules, shared_ptr<Materialiser<KEY, RULEDATA, VALUE>> materialiser):
                rules(rules), materialiser(materialiser) {
            }

            void update_rule(const KEY &key, vector<RuleNode<KEY, RULEDATA>> &value) {
                (*this->rules)[key] = value;

                auto registration = this->registrations.find(key);
                if (registration == this->registrations.end()) {
                    return;
                }
                for (auto it = registration->second.begin(); it != registration->second.end(); ++it) {
                    RegistrationNode &element = *it;
                    vector<TreeNode> expanded;
                    this->expand(element.key, expanded);
                    element.value->update_series(expanded);
                }
            }

            void register_it(TreeNode &key, shared_ptr<Iterator<TreeNode>> it) override {
                auto iter = this->registrations.find(key.key);
                if (iter == this->registrations.end()) {
                    this->registrations[key.key] = vector<RegistrationNode>({ RegistrationNode(key, it) });
                } else {
                    iter->second.push_back(RegistrationNode(key, it));
                }
            }

            void unregister_it(TreeNode &key, shared_ptr<Iterator<TreeNode>> it) override {
                auto iter = this->registrations.find(key.key);
                if (iter == this->registrations.end()) {
                    return;
                }

                vector<RegistrationNode> &elements = iter->second;
                size_t size = elements.size();
                for (int j = size - 1; j >= 0; --j) {
                    if (elements[j].value == it) {
                        elements.erase(elements.begin() + j);
                        break;
                    }
                }
            }

            void expand(TreeNode &original, vector<TreeNode> &result) {
                auto iter = this->rules->find(original.key);
                if (iter == this->rules->end()) {
                    result.push_back(this->materialiser->produce(original.key, original.ruledata, original, 1));
                } else {
                    unsigned int total_siblings = iter->second.size();
                    for (auto keyit = iter->second.begin(); keyit != iter->second.end(); ++keyit) {
                        TreeNode element = this->materialiser->produce(keyit->key, keyit->ruledata, original, total_siblings);
                        result.push_back(element);
                    }
                }
            }

            shared_ptr<Iterator<TreeNode>> lazy_expand(TreeNode &original, int iterations) {
                auto ctx = make_shared<Context<TreeNode>>(original);
                ctx->iterations = iterations;

                auto retval = this->ltree(ctx);
                this->register_it(original, retval);
                // TODO: this one doesn't get unregistered, it's probably better to let
                // iterators register and unregister themselves when they reach the end.
                return retval;
            }

            vector<TreeNode> expand(TreeNode &original, int iterations) {
                if (iterations <= 0) {
                    return { original };
                }
            
                vector<TreeNode> expanded;
                this->expand(original, expanded);
                if (iterations == 1) {
                    return expanded;
                }

                vector<TreeNode> result;
                for (auto n = expanded.begin(); n != expanded.end(); ++n) {
                    auto new_seq = this->expand(*n, iterations - 1);
                    for (auto val = new_seq.begin(); val != new_seq.end(); ++val) {
                        result.push_back(*val);
                    }
                }

                return result;
            }
    
        private:
            shared_ptr<Rules> rules;
            shared_ptr<Materialiser<KEY, RULEDATA, VALUE>> materialiser;
            map<KEY, vector<RegistrationNode>> registrations;

            shared_ptr<Iterator<TreeNode>> ltree(shared_ptr<Context<TreeNode>> ctx) {
                if (ctx->iterations <= 0) {
                    TreeNode element = ctx->element;
                    vector<TreeNode> series({ element });
                    return make_shared<VectorIterator<TreeNode>>(series);
                }

                vector<TreeNode> expanded;
                this->expand(ctx->element, expanded);

                if (ctx->iterations == 1) {
                    return make_shared<VectorIterator<TreeNode>>(expanded);
                }

                return make_shared<NestedLazyIterator<TreeNode>>(this,
                                                                 [this, ctx](TreeNode &node) { return this->ltree(ctx->child(node)); },
                                                                 expanded);
            }
        };
    }
}

#endif
