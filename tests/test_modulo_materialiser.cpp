#include "../catch/catch.hpp"
#include "../modulo_int_system.hpp"

using namespace trlsai::lsystem;

TEST_CASE("Can get modulo with negative min", "[modulo_materialiser_base]") {
    ModuloMaterialiserBase materialiser(-4, 4);
    REQUIRE(materialiser.calculate(0, -4) == -4);
    REQUIRE(materialiser.calculate(0, -5) == 4);
    REQUIRE(materialiser.calculate(0, -6) == 3);
    REQUIRE(materialiser.calculate(0, -7) == 2);
    REQUIRE(materialiser.calculate(0, -8) == 1);
    REQUIRE(materialiser.calculate(0, -9) == 0);
    REQUIRE(materialiser.calculate(0, -11) == -2);
    REQUIRE(materialiser.calculate(-1, -11) == -3);
    REQUIRE(materialiser.calculate(-2, -11) == -4);
    REQUIRE(materialiser.calculate(-3, -11) == 4);
    REQUIRE(materialiser.calculate(2, 2) == 4);
    REQUIRE(materialiser.calculate(2, 3) == -4);
    REQUIRE(materialiser.calculate(0, 6) == -3);
}

TEST_CASE("Can get modulo with positive min", "[modulo_materialiser_base]") {
    ModuloMaterialiserBase materialiser(1, 6);
    REQUIRE(materialiser.calculate(0, 0) == 6);
    REQUIRE(materialiser.calculate(0, -1) == 5);
    REQUIRE(materialiser.calculate(0, -3) == 3);
    REQUIRE(materialiser.calculate(0, -5) == 1);
    REQUIRE(materialiser.calculate(0, -6) == 6);

    materialiser = ModuloIntMaterialiser(40, 42);
    REQUIRE(materialiser.calculate(42, 1) == 40);
    REQUIRE(materialiser.calculate(42, 2) == 41);
    REQUIRE(materialiser.calculate(42, 3) == 42);
    REQUIRE(materialiser.calculate(42, 4) == 40);
    REQUIRE(materialiser.calculate(39, 0) == 42);
    REQUIRE(materialiser.calculate(38, 0) == 41);
    REQUIRE(materialiser.calculate(37, 0) == 40);
    REQUIRE(materialiser.calculate(36, 0) == 42);
    REQUIRE(materialiser.calculate(36, -1) == 41);
}
