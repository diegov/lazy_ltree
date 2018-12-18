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
        template <typename KEY, typename VALUE>
        struct Node {
            Node() { }
            Node(KEY first, VALUE second): first(first), second(second) { }
            KEY first;
            VALUE second;
        };
    
        template <typename KEY, typename VALUE>
        class Materialiser {
        public:
            virtual Node<KEY, VALUE> produce(KEY key, VALUE parent) = 0;
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

        template <typename KEY, typename VALUE>
        class System {
        public:
            typedef Node<KEY, VALUE> TreeNode;
    
            System(shared_ptr<map<KEY, vector<KEY>>> rules, shared_ptr<Materialiser<KEY, VALUE>> materialiser) {
                this->rules = rules;
                this->materialiser = materialiser;
            }

            void expand(TreeNode &original, vector<TreeNode> &result) {
                auto iter = this->rules->find(original.first);
                if (iter == this->rules->end()) {
                    result.push_back(this->materialiser->produce(original.first, original.second));
                } else {
                    for (auto keyit = iter->second.begin(); keyit != iter->second.end(); ++keyit) {
                        TreeNode element = this->materialiser->produce(*keyit, original.second);
                        result.push_back(element);
                    }
                }
            }

            shared_ptr<Iterator<TreeNode>> lazy_expand(TreeNode &original, int iterations) {
                auto ctx = make_shared<Context<TreeNode>>(original);
                ctx->iterations = iterations;

                return this->ltree(ctx);
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
            shared_ptr<map<KEY, vector<KEY>>> rules;
            shared_ptr<Materialiser<KEY, VALUE>> materialiser;

            shared_ptr<Iterator<TreeNode>> ltree(shared_ptr<Context<TreeNode>> ctx) {
                if (ctx->iterations <= 0) {
                    return make_shared<VectorIterator<TreeNode>>(vector<TreeNode>({ ctx->element }));
                }

                vector<TreeNode> expanded;
                this->expand(ctx->element, expanded);

                if (ctx->iterations == 1) {
                    return make_shared<VectorIterator<TreeNode>>(expanded);
                }

                return make_shared<NestedLazyIterator<TreeNode>>([this, ctx](TreeNode &node) { return this->ltree(ctx->child(node)); },
                                                                 [expanded]() { return expanded; });
            }
        };
    }
}

#endif
