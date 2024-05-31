/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#define CATCH_CONFIG_MAIN

#include <string>
#include <catch2/catch.hpp>
#include "Tools/Storage_Config.h"
#include "GC/Garbled_Circuit_Storage.h"
#include "GC-Test-Utils.h"
#include "GC/Garbled_Circuit_Serializer.h"
#include <cppconn/resultset.h>

void drop_garbled_circuit(const shared_ptr<sql::Connection> &connection, const int &circuit_number) {
    const char *query = "DELETE from garbled_circuit where CIRCUIT_NUMBER = ?";
    unique_ptr<sql::PreparedStatement> preparedStatement{
            connection->prepareStatement(query)
    };

    preparedStatement->setInt(1, circuit_number);
    preparedStatement->execute();
}

unsigned int get_last_circuit_id_from_file(const string &path)
{
  ifstream file;
  file.open(path, ios_base::app);
  string line;
  string circuit_id;

  while (getline(file, line)) {
      stringstream ss(line);
      getline(ss, circuit_id, ',');
    }

  file.close();
  return stoi(circuit_id);
}

TEST_CASE("Storage config using the filesystem should")
{
    int a_player_number = 1;
    int a_circuit_number = 2;
    auto a_garbled_circuit = create_garbled_circuit();
    auto another_garbled_circuit = create_garbled_circuit();

    auto some_choicebits = Choicebits({1, 1, 0});
    auto gc_storage = make_unique<File_System_Garbled_Circuit_Storage>("Test");
    string path = "Test/Garbled-Circuits-Storage-" + to_string(a_circuit_number) + "-P" + to_string(a_player_number);

    SECTION("store a garbled circuit") {
        gc_storage->save({a_garbled_circuit}, some_choicebits, a_circuit_number, a_player_number);

        string circuit_id, player_number, circuit_number, circuit, choicebits, used;
        ifstream file(path.c_str());
        if (!file) {
            throw file_error(path);
        }
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, circuit_id, ';');
            getline(ss, player_number, ';');
            getline(ss, circuit_number, ';');
            getline(ss, circuit, ';');
            getline(ss, choicebits, ';');
            getline(ss, used, ';');
        }
        file.close();
        remove(path.c_str());

        string circuit_as_json = Garbled_Circuit_Serializer::serialize_to_base64(a_garbled_circuit);
        circuit_as_json.erase(std::remove(circuit_as_json.begin(), circuit_as_json.end(), '\n'),circuit_as_json.end());
        circuit_as_json.erase(std::remove(circuit_as_json.begin(), circuit_as_json.end(), ' '),circuit_as_json.end());

        REQUIRE (a_player_number == stoi(player_number));
        REQUIRE (a_circuit_number == stoi(circuit_number));
        REQUIRE (circuit == circuit_as_json);
        REQUIRE (some_choicebits.to_string() == choicebits);
        REQUIRE (used == "0");
    }

    SECTION("get next available circuit id") {
      gc_storage->save({a_garbled_circuit}, some_choicebits, a_circuit_number, a_player_number);

      unsigned int last_circuit_id = get_last_circuit_id_from_file(path);
      remove(path.c_str());
      REQUIRE(last_circuit_id == 0);
    }

    SECTION("read a garbled circuit") {
        gc_storage->save({a_garbled_circuit}, some_choicebits, a_circuit_number, a_player_number);

        vector<Base_Garbled_Circuit> circuits = gc_storage->read(1, some_choicebits, a_circuit_number, a_player_number);
        remove(path.c_str());

        REQUIRE(circuits.size() == 1);
        REQUIRE(circuits[0] == a_garbled_circuit);
    }

    SECTION("read multiple garbled circuit") {
        gc_storage->save({a_garbled_circuit, another_garbled_circuit}, some_choicebits, a_circuit_number, a_player_number);

        vector<Base_Garbled_Circuit> circuits = gc_storage->read(2, some_choicebits, a_circuit_number, a_player_number);
        remove(path.c_str());

        REQUIRE(circuits.size() == 2);
    }

    SECTION("not read more circuits than stored") {
        CHECK_THROWS_AS(gc_storage->read(1, some_choicebits, a_circuit_number, a_player_number),
                        Not_Enough_Garbled_Circuits);
    }
}

TEST_CASE("Storage config with a mysql database should") {
    int a_player_number = 0;
    int a_circuit_number = 1;
    auto config = Storage_Config(a_player_number);
    int insert_batch_size = 500;
    auto controller = make_shared<MySQL_Controller>(
            config.get_mysql_url(),
            config.get_mysql_user(),
            config.get_mysql_password(),
            config.get_mysql_database()
    );
    drop_garbled_circuit(controller->connection, a_circuit_number);
    Base_Garbled_Circuit a_garbled_circuit = create_garbled_circuit();
    Base_Garbled_Circuit another_garbled_circuit = create_garbled_circuit();
    auto some_choicebits = Choicebits({1, 1, 0});
    auto gc_storage = make_unique<MySQL_Garbled_Circuit_Storage>(controller, insert_batch_size);

    SECTION("store a garbled circuit") {
        gc_storage->save({a_garbled_circuit},
                         some_choicebits,
                         a_circuit_number,
                         a_player_number);

        const char *query = "SELECT PLAYER, CIRCUIT_NUMBER, CIRCUIT_CONTENT, CHOICEBITS from garbled_circuit ORDER BY creation_date DESC LIMIT 1;";
        sql::ResultSet *result = controller->connection->createStatement()->executeQuery(query);
        REQUIRE(result->next());
        REQUIRE(result->getInt(1) == a_player_number);
        REQUIRE(result->getInt(2) == a_circuit_number);
        REQUIRE(result->getString(3) == Garbled_Circuit_Serializer::serialize_to_base64(a_garbled_circuit));
        REQUIRE(result->getString(4) == some_choicebits.to_string());
    }

    SECTION("read a garbled circuit") {
        gc_storage->save({a_garbled_circuit}, some_choicebits, a_circuit_number, a_player_number);

        vector<Base_Garbled_Circuit> circuits = gc_storage->read(1, some_choicebits, a_circuit_number, a_player_number);

        REQUIRE(circuits.size() == 1);
        REQUIRE(circuits[0] == a_garbled_circuit);
    }

    SECTION("read multiple garbled circuit") {
        gc_storage->save({a_garbled_circuit, another_garbled_circuit}, some_choicebits, a_circuit_number, a_player_number);

        vector<Base_Garbled_Circuit> circuits = gc_storage->read(2, some_choicebits, a_circuit_number, a_player_number);

        REQUIRE(circuits.size() == 2);
    }

    SECTION("not read more circuits than stored") {
        CHECK_THROWS_AS(gc_storage->read(1, some_choicebits, a_circuit_number, a_player_number),
                        Not_Enough_Garbled_Circuits);
    }
}