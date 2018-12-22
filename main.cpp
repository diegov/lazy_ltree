#include <iostream>
#include "lsystem.hpp"

using namespace std;
using namespace trlsai::lsystem;

class ModuloIntMaterialiser : public Materialiser<int, int> {    
public:
    ModuloIntMaterialiser(int min, int max): min(min), max(max), modulo(1 + max - min) { }
    
    Node<int, int> produce(int key, int parent) {
        int new_val = parent + key;
        int final_val = (new_val - this->min) % this->modulo + this->min;
        return Node<int, int>(final_val, final_val);
    }

private:
    int min;
    int max;
    unsigned int modulo;
};

void run_system(string msg, System<int, int> &system, Node<int, int> &original, int iterations) {
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

    cout << endl;

}

void benchmark() {
    shared_ptr<map<int, vector<int>>> rules = make_shared<map<int, vector<int>>>();

    (*rules)[1] = { 1, 2, 3 };
    (*rules)[2] = { 3, -1 };
    (*rules)[3] = { 1, 4, 3 };
    (*rules)[-1] = { 3, -3 };
    (*rules)[-3] = { 2, -3 };

    shared_ptr<ModuloIntMaterialiser> materialiser = make_shared<ModuloIntMaterialiser>(-3, 4);

    System<int, int> system(rules, materialiser);

    Node<int, int> original(1, 1);

    int iterations = 30;

    auto it = system.lazy_expand(original, iterations);

    int values = 0;

    while (it->has_next()) {
        Node<int, int> &next = it->next();
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
    shared_ptr<map<int, vector<int>>> rules = make_shared<map<int, vector<int>>>();

    (*rules)[1] = { 1, 2, 3 };
    (*rules)[2] = { 3, -1 };
    (*rules)[3] = { 1, 4, 3 };
    (*rules)[-1] = { 3, -3 };
    (*rules)[-3] = { 2, -3 };

    shared_ptr<ModuloIntMaterialiser> materialiser = make_shared<ModuloIntMaterialiser>(-3, 4);

    System<int, int> system(rules, materialiser);

    Node<int, int> original(1, 1);

    int iterations = 5;

    run_system("Case 1", system, original, iterations);

    rules->clear();
    (*rules)[0] = { 1, 0, 2 };    
    (*rules)[1] = { 1, 3 };
    (*rules)[2] = { 1 };

    materialiser = make_shared<ModuloIntMaterialiser>(0, 4);
    system = System<int, int>(rules, materialiser);

    Node<int, int> original0(0, 0);

    run_system("Case 2", system, original0, iterations);
}

void test_rule_updates() {
    cout << "With rule updates" << endl;
    shared_ptr<map<int, vector<int>>> rules = make_shared<map<int, vector<int>>>();

    (*rules)[0] = { 1, 0, 2 };    
    (*rules)[1] = { 1, 3, -1 };
    (*rules)[2] = { 1, 3, 0, -3 };

    shared_ptr<ModuloIntMaterialiser> materialiser = make_shared<ModuloIntMaterialiser>(-3, 4);

    System<int, int> system(rules, materialiser);

    Node<int, int> original0(0, 0);
    auto it = system.lazy_expand(original0, 20);
    
    unsigned int counter = 0;
    while (it->has_next()) {
        cout << it->next().key << endl;
        ++counter;
        if (counter == 40) {
            int key = 0;
            vector<int> new_value({ 0 });
            system.update_rule(key, new_value);
        }

        if (counter == 50) {
            int key = 1;
            vector<int> new_value({ 1 });
            system.update_rule(key, new_value);
        }

        if (counter == 60) {
            int key = 2;
            vector<int> new_value({ });
            system.update_rule(key, new_value);
        }

        if (counter == 70) {
            int key = 1;
            vector<int> new_value({ });
            system.update_rule(key, new_value);
        }
    }
}

int main() {
    test_output();
    test_rule_updates();
    benchmark();
    return 0;
}

