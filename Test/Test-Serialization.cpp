/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <string>

#include "GC/Garbled_Circuit_Serializer.h"
#include "GC/Garbled_Circuit_Exceptions.h"
#include "Math/gf2n.h"
#include "Tools/MMO.h"
#include "GC-Test-Utils.h"

using namespace std;

TEST_CASE("gf2n should be able to be rebuilt from text")
{
  gf2n value= create_gf2n();
  string value_as_text= Garbled_Circuit_Serializer::gf2n_as_text(value);

  gf2n result= Garbled_Circuit_Serializer::text_to_gf2n(value_as_text);

  REQUIRE(value.equal(result));
}

TEST_CASE("aBitDTO should be able to be converted to an equivalent aBit")
{
  aBit abit= create_aBit();
  aBitDTO abit_dto= aBitDTO::from_domain_model(abit);

  aBit result= aBitDTO::to_domain_model(abit_dto);

  REQUIRE(result.get_raw_value().equal(abit.get_raw_value()));
  REQUIRE_THAT(result.get_MACs(), Catch::Matchers::Equals(abit.get_MACs()));
  REQUIRE_THAT(result.get_Keys(), Catch::Matchers::Equals(abit.get_Keys()));
}

TEST_CASE("Base_Garbled_Circuit_DTO should be able to be converted to an equivalent Base_Garbled_Circuit")
{
  Base_Garbled_Circuit garbled_circuit= create_garbled_circuit();
  Base_Garbled_Circuit_DTO garbled_circuit_dto= Base_Garbled_Circuit_DTO::from_domain_model(garbled_circuit);

  Base_Garbled_Circuit result_garbled_circuit= garbled_circuit_dto.to_domain_model();

  REQUIRE(garbled_circuit == result_garbled_circuit);
}

TEST_CASE("should not be able to deserialize a circuit when no file found")
{
    string file_name = "not_existing_garbled_circuit.json";
    CHECK_THROWS_AS(Garbled_Circuit_Serializer::deserialize_garbled_circuit_from_json_file(file_name), Garbled_circuit_file_not_found);
}

TEST_CASE("should serialize a Garbled Circuit as string"){
    Base_Garbled_Circuit garbled_circuit = create_garbled_circuit();

    string serialized_circuit = Garbled_Circuit_Serializer::serialize_to_base64(garbled_circuit);

    Base_Garbled_Circuit rebuilt_garbled_circuit = Garbled_Circuit_Serializer::deserialize_from_base64(serialized_circuit);
    REQUIRE(rebuilt_garbled_circuit == garbled_circuit);
}