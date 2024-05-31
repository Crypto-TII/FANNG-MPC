/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#include "Converter_Storage.h"
#include "Converter_Exceptions.h"
#include "Utils/DTO_Format_Utils.h"

using namespace std;

shared_ptr<Converter_Storage> Converter_Storage::create_from_config_file(unsigned int player_number)
{
  auto config= Storage_Config(player_number);
  switch (config.get_general_storage_type())
    {
        case General_storage_type::MySQL_Storage_Type: {
          int insert_batch_size= config.get_mysql_insert_batch_size();
          auto controller= make_shared<MySQL_Controller>(
              config.get_mysql_url(),
              config.get_mysql_user(),
              config.get_mysql_password(),
              config.get_mysql_database());
          return make_shared<MySQL_Converter_Storage>(controller, insert_batch_size);
        }
      default:
        return nullptr;
    }
}

vector<Share_DTO> MySQL_Converter_Storage::read(unsigned int number_of_elements,
                                                const Triple_Type_DTO &triple_type_dto, unsigned int element_id,
                                                unsigned int player_number)
{

  string select_query= "SELECT id, player, share, mac_share FROM share WHERE player = ? AND channel = ? AND  id > ? ORDER BY id ASC LIMIT ?;";
  unique_ptr<sql::PreparedStatement> preparedStatement{
      controller->connection->prepareStatement(select_query)};

  preparedStatement->setInt(1, (int) player_number);
  preparedStatement->setInt(2, triple_type_dto.get_id());
  preparedStatement->setInt(3, (int) element_id);
  preparedStatement->setInt(4, (int) number_of_elements);

  unique_ptr<sql::ResultSet> response{
      preparedStatement->executeQuery()};

  vector<Share_DTO> shares;

  while (response->next())
    {
      int id= response->getInt("id");
      int player= response->getInt("player");
      string share= response->getString("share");
      string mac_share= response->getString("mac_share");
      auto share_dto= Share_DTO(id, player, share, mac_share);
      shares.push_back(share_dto);
    }

  if (shares.size() < number_of_elements)
    {
      throw Not_Enough_Matrix_Triple_Elements(shares.size(), number_of_elements);
    }

  return shares;
}

vector<Share_DTO> MySQL_Converter_Storage::read_then_delete(unsigned int number_of_elements,
                                                            const Triple_Type_DTO &triple_type_dto, unsigned int element_id,
                                                            unsigned int player_number)
{

  string select_query= "SELECT id, player, share, mac_share FROM share WHERE player = ? AND channel = ? AND  id > ? ORDER BY id ASC LIMIT ?;";
  unique_ptr<sql::PreparedStatement> preparedStatement{
      controller->connection->prepareStatement(select_query)};

  preparedStatement->setInt(1, (int) player_number);
  preparedStatement->setInt(2, triple_type_dto.get_id());
  preparedStatement->setInt(3, (int) element_id);
  preparedStatement->setInt(4, (int) number_of_elements);

  unique_ptr<sql::ResultSet> response{
      preparedStatement->executeQuery()};

  vector<Share_DTO> shares;

  while (response->next())
    {
      int id= response->getInt("id");
      int player= response->getInt("player");
      string share= response->getString("share");
      string mac_share= response->getString("mac_share");
      auto share_dto= Share_DTO(id, player, share, mac_share);
      shares.push_back(share_dto);
    }

  if (shares.size() < number_of_elements)
    {
      throw Not_Enough_Matrix_Triple_Elements(shares.size(), number_of_elements);
    }
  else
    {
      string delete_query= "DELETE FROM share WHERE PLAYER = ? AND CHANNEL = ? AND  ID > ? ORDER BY ID ASC LIMIT ?;";
      unique_ptr<sql::PreparedStatement> deletePreparedStatement{
          controller->connection->prepareStatement(delete_query)};

      deletePreparedStatement->setInt(1, (int) player_number);
      deletePreparedStatement->setInt(2, triple_type_dto.get_id());
      deletePreparedStatement->setInt(3, (int) element_id);
      deletePreparedStatement->setInt(4, (int) number_of_elements);

      deletePreparedStatement->executeQuery();
    }

  return shares;
}

// Share Functionality
void MySQL_Converter_Storage::save(const Share &share,
                                   const unsigned int &type_id,
                                   [[maybe_unused]] const unsigned int &player_number)
{
  vector<string> share_value= DTO_Format_Utils::split_share(share);
  string insert_query= "INSERT INTO share(PLAYER, CHANNEL, SHARE, MAC_SHARE) VALUES (?, ?, ?, ?)";
  unique_ptr<sql::PreparedStatement> preparedStatement{controller->connection->prepareStatement(insert_query)};
  preparedStatement->setInt(1, stoi(share_value[0]));
  preparedStatement->setInt(2, type_id);
  preparedStatement->setString(3, share_value[1]);
  preparedStatement->setString(4, share_value[2]);

  preparedStatement->execute();
}

void MySQL_Converter_Storage::save_batch(const vector<Share> &shares,
                                         const unsigned int &type_id,
                                         const unsigned int &player_number)
{

  string insert_query= "CALL InsertBatchData(?,?,?,?)";
  unsigned int insert_batch_size_l= insert_batch_size;
  unsigned int left_over= shares.size();
  unsigned int count= 0;

  while (left_over > 0)
    {
      unsigned int batch_l= ((int) (insert_batch_size_l > left_over)) * (left_over - insert_batch_size_l) + insert_batch_size_l;
      std::stringstream oss;
      for (unsigned int i= 0; i < batch_l; i++)
        {
          std::ostringstream oss_l;
          oss << DTO_Format_Utils::reduced_format_share_output(shares[i + count]);
        }

      count+= batch_l;
      string share_batch= oss.str();

      if (!share_batch.empty())
        {
          share_batch.erase(share_batch.size() - 1);
        }

      left_over= left_over - batch_l;

      unique_ptr<sql::PreparedStatement> preparedStatement{controller->connection->prepareStatement(insert_query)};

      preparedStatement->setInt(1, batch_l);
      preparedStatement->setInt(2, type_id);
      preparedStatement->setInt(3, player_number);
      preparedStatement->setString(4, share_batch);
      preparedStatement->execute();
    }
}

vector<Share_DTO> MySQL_Converter_Storage::read(const unsigned int &number_of_elements,
                                                const unsigned int &type_id,
                                                const unsigned int element_id, const unsigned int player_number)
{

  string select_query= "SELECT ID, PLAYER, SHARE, MAC_SHARE FROM share WHERE PLAYER = ? AND CHANNEL = ? AND  ID > ? ORDER BY ID ASC LIMIT ?;";
  unique_ptr<sql::PreparedStatement> preparedStatement{
      controller->connection->prepareStatement(select_query)};

  preparedStatement->setInt(1, (int) player_number);
  preparedStatement->setInt(2, type_id);
  preparedStatement->setInt(3, (int) element_id);
  preparedStatement->setInt(4, (int) number_of_elements);

  unique_ptr<sql::ResultSet> response{
      preparedStatement->executeQuery()};

  vector<Share_DTO> shares;

  while (response->next())
    {
      int id= response->getInt("ID");
      int player= response->getInt("PLAYER");
      string share= response->getString("SHARE");
      string mac_share= response->getString("MAC_SHARE");
      auto share_dto= Share_DTO(id, player, share, mac_share);
      shares.push_back(share_dto);
    }

  if (shares.size() < number_of_elements)
    {
      throw Not_Enough_Preprocessed_Elements(shares.size(), number_of_elements);
    }

  return shares;
}

vector<Share_DTO> MySQL_Converter_Storage::read_then_delete(const unsigned int &number_of_elements,
                                                            const unsigned int &type_id,
                                                            const unsigned int element_id, const unsigned int player_number)
{
  string select_query= "SELECT ID, PLAYER, SHARE, MAC_SHARE FROM share WHERE PLAYER = ? AND CHANNEL = ? AND  ID > ? ORDER BY ID ASC LIMIT ?;";
  unique_ptr<sql::PreparedStatement> preparedStatement{
      controller->connection->prepareStatement(select_query)};

  preparedStatement->setInt(1, (int) player_number);
  preparedStatement->setInt(2, type_id);
  preparedStatement->setInt(3, (int) element_id);
  preparedStatement->setInt(4, (int) number_of_elements);

  unique_ptr<sql::ResultSet> response{
      preparedStatement->executeQuery()};

  vector<Share_DTO> shares;

  while (response->next())
    {
      int id= response->getInt("ID");
      int player= response->getInt("PLAYER");
      string share= response->getString("SHARE");
      string mac_share= response->getString("MAC_SHARE");
      auto share_dto= Share_DTO(id, player, share, mac_share);
      shares.push_back(share_dto);
    }

  if (shares.size() < number_of_elements)
    {
      throw Not_Enough_Preprocessed_Elements(shares.size(), number_of_elements);
    }
  else
    {
      string delete_query= "DELETE FROM share WHERE PLAYER = ? AND CHANNEL = ? AND  ID > ? ORDER BY ID ASC LIMIT ?;";
      unique_ptr<sql::PreparedStatement> deletePreparedStatement{
          controller->connection->prepareStatement(delete_query)};

      deletePreparedStatement->setInt(1, (int) player_number);
      deletePreparedStatement->setInt(2, type_id);
      deletePreparedStatement->setInt(3, (int) element_id);
      deletePreparedStatement->setInt(4, (int) number_of_elements);

      deletePreparedStatement->executeQuery();
    }
  return shares;
}