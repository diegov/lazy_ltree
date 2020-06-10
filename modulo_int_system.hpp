#ifndef __MODAL_LSYSTEM_MODULO_INT_LSYSTEM__
#define __MODAL_LSYSTEM_MODULO_INT_LSYSTEM__

#include "lsystem.hpp"

namespace trlsai {
    namespace lsystem {
        class ModuloMaterialiserBase {
        public:
            ModuloMaterialiserBase(int min, int max);
            
            void set_min(int min);
            void set_max(int max);
            int calculate(const int base, const int interval);
            
        private:
            int min;
            int max;
            int modulo;
        };
        
        class ModuloIntMaterialiser : public ModuloMaterialiserBase, public Materialiser<int, empty, int> {
        public:
            ModuloIntMaterialiser(int min, int max);
            Triplet<int, empty, int> produce(const int &key, const empty &ruledata, const Triplet<int, empty, int> &parent, unsigned int total_siblings) override;
            virtual ~ModuloIntMaterialiser() = default;
        };

        using duration_value = unsigned long;

        struct Duration {
            Duration(duration_value numerator, duration_value denominator): numerator(numerator), denominator(denominator) { }
            Duration(duration_value numerator): numerator(numerator), denominator(1) { }
            Duration(): numerator(0), denominator(1) { }
            duration_value numerator;
            duration_value denominator;
        };

        struct ModuloValue {
            ModuloValue() = default;
            ModuloValue(int interval, Duration duration) : interval(interval), duration(duration) { }
            int interval;
            Duration duration;
        };

        class ModuloDurationMaterialiser : public ModuloMaterialiserBase, public Materialiser<int, Duration, ModuloValue> {
        public:
            ModuloDurationMaterialiser(int min, int max);
            Triplet<int, Duration, ModuloValue> produce(const int &key, const Duration &ruledata, const Triplet<int, Duration, ModuloValue> &parent,
                                                              unsigned int total_siblings) override;
            virtual ~ModuloDurationMaterialiser() = default;
        };
    }
}

#endif
