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
            Iterator(vector<T> &series): series(series) { }            
            virtual bool has_next() = 0;
            virtual T &next() = 0;

            void update_series(vector<T> &series) {
                this->series = series;
            }
            
        protected:
            vector<T> series;
        };

        template <typename T>
        class IteratorRegistry {
        public:
            virtual void register_it(T &key, shared_ptr<Iterator<T>> it) = 0;
            virtual void unregister_it(T &key, shared_ptr<Iterator<T>> it) = 0;
        };

        template <typename T>
        class VectorIterator : public Iterator<T> {
        public:
            VectorIterator(vector<T> &series): Iterator<T>(series), index(0) { }
    
            bool has_next() override {
                return this->index < this->series.size();
            }

            T &next() override {
                T &retval = this->series[index];
                ++index;
                return retval;        
            }
    
        private:
            size_t index;
        };

        template <typename T>
        struct IteratorRegistration {
            T key;
            shared_ptr<Iterator<T>> iterator;
        };
        
        template <typename T>
        class NestedLazyIterator : public Iterator<T> {
        public:
            NestedLazyIterator(IteratorRegistry<T> *registry,
                               function<shared_ptr<Iterator<T>>(T&)> &&iter_gen,
                               vector<T> &series)
                : Iterator<T>(series), registry(registry), iter_gen(iter_gen),
                  series_index(0), current_it({ T(), nullptr }), _has_next(false) {
            }

            bool has_next() override {
                this->check_next();
                return this->_has_next;
            }

            T &next() override {
                this->check_next();
                this->_has_next = false;
                return this->current_it.iterator->next();
            }
    
        private:
            IteratorRegistry<T> *registry;
            function<shared_ptr<Iterator<T>>(T&)> iter_gen;
            size_t series_index;
            IteratorRegistration<T> current_it;
            bool _has_next;

            void check_next() {
                if (this->_has_next) {
                    return;
                }

                while (true) {
                    if (this->current_it.iterator == nullptr) {
                        if (this->series_index < this->series.size()) {
                            T &series_key = this->series[this->series_index];
                            this->current_it.iterator = this->iter_gen(series_key);
                            this->current_it.key = series_key;
                            this->register_it();
                            ++this->series_index;
                        } else {
                            break;
                        }
                    }

                    if (this->current_it.iterator->has_next()) {
                        this->_has_next = true;
                        break;
                    } else {
                        this->unregister_it();
                        this->current_it.iterator = nullptr;
                    }
                }
            }

            void register_it() {
                this->registry->register_it(this->current_it.key, this->current_it.iterator);
            }

            void unregister_it() {
                this->registry->unregister_it(this->current_it.key, this->current_it.iterator);
            }
        };
    }
}

#endif
