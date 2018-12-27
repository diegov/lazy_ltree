#include <iostream>
#include "lsystem.hpp"
#include "modulo_int_system.hpp"

using namespace std;
using namespace trlsai::lsystem;

using IntSystem = System<int, empty, int>;
using IntDurationSystem = System<int, Duration, ModuloValue>;

void run_system(string msg, shared_ptr<IntSystem::Rules> rules,
                shared_ptr<ModuloIntMaterialiser>  materialiser,
                IntSystem::TreeNode &original, int iterations) {
    IntSystem system(rules, materialiser);
    auto result = system.expand(original, iterations);

    cout << msg << endl;

    cout << "As vector" << endl;

    for (auto it = result.begin(); it != result.end(); ++it) {
        cout << it->key << endl;
    }

    cout << "As iterator" << endl;

    auto it = system.lazy_expand(original, iterations);
    while (it->has_next()) {
        cout << it->next().key << endl;
    }

    cout << "Rules initialised lazily" << endl;

    auto empty_rules = make_shared<IntSystem::Rules>();
    System<int, empty, int> lazy_system(empty_rules, materialiser);
    it = lazy_system.lazy_expand(original, iterations);

    for (auto it = rules->begin(); it != rules->end(); ++it) {
        lazy_system.update_rule(it->first, it->second);
    }

    while (it->has_next()) {
        cout << it->next().key << endl;
    }

    cout << endl;

}

void benchmark() {
    auto rules = make_shared<IntSystem::Rules>();

    (*rules)[1] = { 1, 2, 3 };
    (*rules)[2] = { 3, -1 };
    (*rules)[3] = { 1, 4, 3 };
    (*rules)[-1] = { 3, -3 };
    (*rules)[-3] = { 2, -3 };

    shared_ptr<ModuloIntMaterialiser> materialiser = make_shared<ModuloIntMaterialiser>(-3, 4);

    System<int, empty, int> system(rules, materialiser);

    IntSystem::TreeNode original(1, 1);

    int iterations = 30;

    auto it = system.lazy_expand(original, iterations);

    int values = 0;

    while (it->has_next()) {
        IntSystem::TreeNode &next = it->next();
        ++values;
        if (next.value > 21) {
            cout << "More than 21 " << endl;
        }

        if (values % 1000000 == 0) {
            cout << "Processed " << values << " values." << endl;
        }
    }

    cout << "Done, " << values << " values." << endl;
}

void test_output() {
    auto rules = make_shared<IntSystem::Rules>();

    (*rules)[1] = { 1, 2, 3 };
    (*rules)[2] = { 3, -1 };
    (*rules)[3] = { 1, 4, 3 };
    (*rules)[-1] = { 3, -3 };
    (*rules)[-3] = { 2, -3 };

    shared_ptr<ModuloIntMaterialiser> materialiser = make_shared<ModuloIntMaterialiser>(-3, 4);

    IntSystem::TreeNode original(1, 1);

    int iterations = 5;

    run_system("Case 1", rules, materialiser, original, iterations);

    rules->clear();
    (*rules)[0] = { 1, 0, 2 };    
    (*rules)[1] = { 1, 3 };
    (*rules)[2] = { 1 };

    materialiser = make_shared<ModuloIntMaterialiser>(0, 4);

    IntSystem::TreeNode original0(0, 0);

    run_system("Case 2", rules, materialiser, original0, iterations);
}

void test_rule_updates() {
    cout << "With rule updates" << endl;
    auto rules = make_shared<IntSystem::Rules>();

    (*rules)[0] = { 1, 0, 2 };    
    (*rules)[1] = { 1, 3, -1 };
    (*rules)[2] = { 1, 3, 0, -3 };

    shared_ptr<ModuloIntMaterialiser> materialiser = make_shared<ModuloIntMaterialiser>(-3, 4);

    System<int, empty, int> system(rules, materialiser);

    IntSystem::TreeNode original0(0, 0);
    auto it = system.lazy_expand(original0, 20);
    
    unsigned int counter = 0;
    while (it->has_next()) {
        cout << it->next().key << endl;
        ++counter;
        if (counter == 40) {
            int key = 0;
            vector<RuleNode<int, empty>> new_value({ 0 });
            system.update_rule(key, new_value);
        }

        if (counter == 50) {
            int key = 1;
            vector<RuleNode<int, empty>> new_value({ 1 });
            system.update_rule(key, new_value);
        }

        if (counter == 60) {
            int key = 2;
            vector<RuleNode<int, empty>> new_value({ });
            system.update_rule(key, new_value);
        }

        if (counter == 70) {
            int key = 1;
            vector<RuleNode<int, empty>> new_value({ });
            system.update_rule(key, new_value);
        }
    }
}

void test_modulo_with_duration() {
    auto rules = make_shared<IntDurationSystem::Rules>();

    using R = RuleNode<int, Duration>;

    (*rules)[1] = { R(1, 1), R(2, 1), R(3, Duration(1, 2)) };
    (*rules)[2] = { R(3, Duration(1, 5)), R(-1, 1) };
    (*rules)[3] = { R(1, 1), R(4, 2), R(3, 2) };
    (*rules)[-1] = { R(3, 4), R(-3, 1) };
    (*rules)[-3] = { R(2, 5), R(-3, 2) };

    auto materialiser = make_shared<ModuloDurationMaterialiser>(-3, 4);

    IntDurationSystem system(rules, materialiser);

    Duration duration = 1;
    ModuloValue value;
    value.duration = duration;
    value.interval = 1;

    IntDurationSystem::TreeNode original(1, duration, value);

    auto it = system.lazy_expand(original, 20);
    
    unsigned long count;
    while (it->has_next()) {
        if (count == 100) {
            vector<RuleNode<int, Duration>> updated;
            system.update_rule(0, updated);
        } else if (count == 150) {
            vector<RuleNode<int, Duration>> updated;
            for (int j = -5; j <= 5; ++j) {
                system.update_rule(j, updated);
            }
        }
        
        IntDurationSystem::TreeNode &next = it->next();
        cout << "INTERVAL: " << next.value.interval << " NUMERATOR: " << next.value.duration.numerator <<
            " DENOMINATOR " << next.value.duration.denominator << endl;
        ++count;
    }

    cout << "Produced " << count << " values." << endl;
    
}

int main() {
    test_output();
    test_rule_updates();
    test_modulo_with_duration();
    //benchmark();
    return 0;
}

