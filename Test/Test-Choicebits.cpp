#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "OT/Choicebits.h"
#include "Exceptions/Exceptions.h"

using namespace std;

void remove_file_content(const string &file_name){
    ofstream output;
    output.open(file_name, ofstream::trunc);
    output.close();
}

TEST_CASE("should fail if choicebits file does not exist"){
    string file_name;
    file_name = "a_file_that_does_not_exists";
    PRNG pseudoRandomNumberGenerator;
    pseudoRandomNumberGenerator.ReSeed(20000);

    CHECK_THROWS_AS(Choicebits::create(file_name, pseudoRandomNumberGenerator), file_error);
}

TEST_CASE("should generate new choicebits if file is empty"){
    string file_name;
    file_name = "Test/sample-files-for-testing/an-empty-file";
    remove_file_content(file_name);
    PRNG pseudoRandomNumberGenerator;
    pseudoRandomNumberGenerator.ReSeed(20000);

    Choicebits::create(file_name, pseudoRandomNumberGenerator);

    REQUIRE(file_is_not_empty(file_name));
}

TEST_CASE("should read content of choicebit config file when not empty"){
    string file_name;
    file_name = "Test/sample-files-for-testing/a-file-with-choicebits";
    PRNG pseudoRandomNumberGenerator;
    pseudoRandomNumberGenerator.ReSeed(20000);

    Choicebits bits = Choicebits::create(file_name, pseudoRandomNumberGenerator);
    vector<int> expected_result{0, 1, 1};

    REQUIRE(bits.to_vector() == expected_result);
}
