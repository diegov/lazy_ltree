#ifndef __MODAL_LSYSTEM_MODULO_INT_LSYSTEM__
#define __MODAL_LSYSTEM_MODULO_INT_LSYSTEM__

#include "lsystem.hpp"

namespace trlsai {
    namespace lsystem {
        class ModuloMaterialiserBase {
        protected:
            ModuloMaterialiserBase(int min, int max);
            
            int min;
            int max;
            unsigned int modulo;

            int calculate(const int base, const int interval);
        };
        
        class ModuloIntMaterialiser : public ModuloMaterialiserBase, public Materialiser<int, empty, int> {
        public:
            ModuloIntMaterialiser(int min, int max);
            Triplet<int, empty, int> produce(const int &key, const empty &ruledata, const Triplet<int, empty, int> &parent, unsigned int total_siblings) override;
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
            int interval;
            Duration duration;
        };

        class ModuloDurationMaterialiser : public ModuloMaterialiserBase, public Materialiser<int, Duration, ModuloValue> {
        public:
            ModuloDurationMaterialiser(int min, int max);
            Triplet<int, Duration, ModuloValue> produce(const int &key, const Duration &ruledata, const Triplet<int, Duration, ModuloValue> &parent,
                                                              unsigned int total_siblings) override;
        };
    }
}

#endif
