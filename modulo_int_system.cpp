#include <assert.h>
#include <iostream>
#include "modulo_int_system.hpp"

namespace trlsai {
    namespace lsystem {
        ModuloMaterialiserBase::ModuloMaterialiserBase(int min, int max): min(min), max(max), modulo(1 + max - min) {
            assert(this->min <= this->max);
        }

        int ModuloMaterialiserBase::calculate(const int base, const int interval) {
            int new_val = base + interval;
            if (new_val < this->min) {
                return this->max + (1 + new_val - this->min) % this->modulo;
            } else {
                return (new_val - this->min) % this->modulo + this->min;
            }
        }

        void ModuloMaterialiserBase::set_min(int min) {
            this->min = min;
            this->modulo = (1 + this->max - this->min);
        }

        void ModuloMaterialiserBase::set_max(int max) {
            this->max = max;
            this->modulo = (1 + this->max - this->min);
        }
        
        ModuloIntMaterialiser::ModuloIntMaterialiser(int min, int max): ModuloMaterialiserBase(min, max) { }

        Triplet<int, empty, int> ModuloIntMaterialiser::produce(const int &key, const empty &ruledata, const Triplet<int, empty, int> &parent, unsigned int total_siblings) {
            int final_val = this->calculate(parent.value, key);
            return Triplet<int, empty, int>(final_val, final_val);
        }

        ModuloDurationMaterialiser::ModuloDurationMaterialiser(int min, int max): ModuloMaterialiserBase(min, max) { }

        Triplet<int, Duration, ModuloValue> ModuloDurationMaterialiser::produce(const int &key, const Duration &duration,
                                                                                const Triplet<int, Duration, ModuloValue> &parent, unsigned int total_siblings) {
            int final_val = this->calculate(parent.value.interval, key);
            ModuloValue value;
            value.interval = final_val;
            value.duration.numerator = parent.value.duration.numerator * duration.numerator;
            value.duration.denominator = parent.value.duration.denominator * duration.denominator;
            return Triplet<int, Duration, ModuloValue>(final_val, duration, value);
        }
    }
}

