#ifndef __MODAL_LSYSTEM_LAZY_ITERATOR__
#define __MODAL_LSYSTEM_LAZY_ITERATOR__

#include <vector>
#include <memory>
#include <map>
#include <functional>

using namespace std;

namespace trlsai {
    namespace lsystem {
    
        template <typename T>
        class Iterator {
        public:
            virtual bool has_next() = 0;
            virtual T &next() = 0;
        };

        template <typename T>
        class VectorIterator : public Iterator<T> {
        public:
            VectorIterator(vector<T> values): values(values), index(0) { }
    
            bool has_next() override {
                return this->index < values.size();
            }

            T &next() override {
                T &retval = values[index];
                ++index;
                return retval;        
            }
    
        private:
            vector<T> values;
            size_t index;
        };

        template <typename T>
        class NestedLazyIterator : public Iterator<T> {
        public:
            NestedLazyIterator(function<shared_ptr<Iterator<T>>(T&)> &&iter_gen,
                               function<vector<T>()> &&series_gen)
                : iter_gen(iter_gen), series_gen(series_gen),
                  series_fetched(false), series_index(0),
                  current_it(nullptr), _has_next(false) {
            };

            bool has_next() override {
                this->check_next();
                return this->_has_next;
            }

            T &next() override {
                this->check_next();
                this->_has_next = false;
                return this->current_it->next();
            }
    
        private:
            void check_next() {
                if (this->_has_next) {
                    return;
                }

                if (!this->series_fetched) {
                    this->series = this->series_gen();
                    this->series_fetched = true;
                }

                while (true) {
                    if (this->current_it == nullptr) {
                        if (this->series_index < this->series.size()) {
                            this->current_it = this->iter_gen(this->series[this->series_index]);
                            ++this->series_index;
                        } else {
                            break;
                        }
                    }

                    if (this->current_it->has_next()) {
                        this->_has_next = true;
                        break;
                    } else {
                        this->current_it = nullptr;
                    }
                }
            }
    
            function<shared_ptr<Iterator<T>>(T&)> iter_gen;
            function<vector<T>()> series_gen;
            bool series_fetched;
            vector<T> series;
            size_t series_index;
            shared_ptr<Iterator<T>> current_it;
            bool _has_next;
        };
    }
}

#endif
