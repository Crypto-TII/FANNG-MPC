/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#include "Database.h"
#include "Utils/DTO_Format_Utils.h"
Database::Database() {}

Database::Database(const string &connection_url, const string &database_user, const string &database_password,
                   const string &database_name)
{
  Database::driver= get_driver_instance();
  Database::connection= Database::driver->connect(connection_url, database_user, database_password);
  Database::connection->setSchema(database_name);
}

void Database::save_share(int channel, const Share &share)
{
  vector<string> share_value= DTO_Format_Utils::split_share(share);

  unique_ptr<sql::PreparedStatement> preparedStatement{
      Database::connection->prepareStatement("INSERT INTO share(PLAYER, CHANNEL, SHARE, MAC_SHARE) VALUES (?, ?, ?, ?)")};

  preparedStatement->setInt(1, stoi(share_value[0]));
  preparedStatement->setInt(2, channel);
  preparedStatement->setString(3, share_value[1]);
  preparedStatement->setString(4, share_value[2]);

  preparedStatement->execute();
}

void Database::save_gfp(int player_number, int channel, const string &value)
{
  string query= "INSERT INTO gfp (PLAYER, CHANNEL, VALUE) VALUES (?, ?, ?)";
  save(player_number, channel, value, query);
}

void Database::save_regint(int player_number, int channel, const string &value)
{
  string query= "INSERT INTO regint (PLAYER, CHANNEL, VALUE) VALUES (?, ?, ?)";
  save(player_number, channel, value, query);
}

void Database::save_cint(int player_number, int channel, const string &value)
{
  string query= "INSERT INTO cint (PLAYER, CHANNEL, VALUE) VALUES (?, ?, ?)";
  save(player_number, channel, value, query);
}

void Database::save(int player_number, int channel, const string &value, const string &query)
{
  unique_ptr<sql::PreparedStatement> preparedStatement{
      Database::connection->prepareStatement(query)};

  preparedStatement->setInt(1, player_number);
  preparedStatement->setInt(2, channel);
  preparedStatement->setString(3, value);

  preparedStatement->execute();
}

Share_DTO Database::read_share(int player_number, int channel, int last_retrieved_index)
{
  unique_ptr<sql::PreparedStatement> preparedStatement{
      Database::connection->prepareStatement("SELECT id, player, share, mac_share FROM share WHERE player = ? AND channel = ? AND id > ? ORDER BY id ASC LIMIT 1;")};

  preparedStatement->setInt(1, player_number);
  preparedStatement->setInt(2, channel);
  preparedStatement->setInt(3, last_retrieved_index);

  unique_ptr<sql::ResultSet> response{
      preparedStatement->executeQuery()};

  if (!(response->next()))
    {
      throw No_share_found_exception();
    }

  int id= response->getInt("id");
  int player= response->getInt("player");
  string share= response->getString("share");
  string mac_share= response->getString("mac_share");
  auto share_dto= Share_DTO(id, player, share, mac_share);

  return share_dto;
}

Reg_int_DTO Database::read_reg_int(int player_number, int channel, int last_retrieved_index)
{

  unique_ptr<sql::PreparedStatement> preparedStatement{
      Database::connection->prepareStatement("SELECT id, value FROM regint WHERE player = ? AND channel = ? AND id > ? ORDER BY id ASC LIMIT 1;")};

  preparedStatement->setInt(1, player_number);
  preparedStatement->setInt(2, channel);
  preparedStatement->setInt(3, last_retrieved_index);

  unique_ptr<sql::ResultSet> response{
      preparedStatement->executeQuery()};

  if (!(response->next()))
    {
      throw No_reg_int_found_exception();
    }

  int id= response->getInt("id");
  string value_as_string= response->getString("value");
  long value= stol(value_as_string);

  auto reg_int= Reg_int_DTO(id, value);

  return reg_int;
}

sql::ResultSet *Database::read(int player_number, int channel, int last_retrieved_index, const string &query)
{
  unique_ptr<sql::PreparedStatement> preparedStatement{
      Database::connection->prepareStatement(query)};

  preparedStatement->setInt(1, player_number);
  preparedStatement->setInt(2, channel);
  preparedStatement->setInt(3, last_retrieved_index);

  sql::ResultSet *response;
  response= preparedStatement->executeQuery();

  return response;
}

GFP_DTO Database::read_gfp(int player_number, int channel, int last_retrieved_index)
{
  string query= "SELECT id, value FROM gfp WHERE player = ? AND channel = ? AND id > ? ORDER BY id ASC LIMIT 1;";

  unique_ptr<sql::ResultSet> response{
      read(player_number, channel, last_retrieved_index, query)};

  if (!(response->next()))
    {
      throw No_gfp_found_exception();
    }

  int id= response->getInt("id");
  string value_as_string= response->getString("value");

  auto gfp_dto= GFP_DTO(id, value_as_string);

  return gfp_dto;
}

GFP_DTO Database::read_cint(int player_number, int channel, int last_retrieved_index)
{
  string query= "SELECT id, value FROM cint WHERE player = ? AND channel = ? AND id > ? ORDER BY id ASC LIMIT 1;";

  unique_ptr<sql::ResultSet> response{
      read(player_number, channel, last_retrieved_index, query)};

  if (!(response->next()))
    {
      throw No_gfp_found_exception();
    }

  int id= response->getInt("id");
  string value_as_string= response->getString("value");

  auto gfp_dto= GFP_DTO(id, value_as_string);

  return gfp_dto;
}

void Database::close()
{
  delete Database::connection;
  cout << "database closed " << endl;
}

void Database::clear_data(int channel, int player_number)
{
  clear_data("cint", channel, player_number);
  clear_data("gfp", channel, player_number);
  clear_data("regint", channel, player_number);
  clear_data("share", channel, player_number);
}

void Database::clear_data(const string &tableName, int channel, int player_number)
{
  string query= "DELETE FROM " + tableName + " WHERE channel = ? AND player = ?";

  unique_ptr<sql::PreparedStatement> preparedStatement{
      Database::connection->prepareStatement(query)};

  preparedStatement->setInt(1, channel);
  preparedStatement->setInt(2, player_number);
  preparedStatement->executeQuery();
}
