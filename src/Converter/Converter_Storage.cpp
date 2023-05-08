#include "Converter_Storage.h"
#include "Converter_Exceptions.h"

using namespace std;

shared_ptr<Converter_Storage> Converter_Storage::create_from_config_file(unsigned int player_number)
{
  auto config= Storage_Config(player_number);
  switch (config.get_general_storage_type())
    {
        case General_storage_type::MySQL_Storage_Type: {
          auto controller= make_shared<MySQL_Controller>(
              config.get_mysql_url(),
              config.get_mysql_user(),
              config.get_mysql_password(),
              config.get_mysql_database());
          return make_shared<MySQL_Converter_Storage>(controller);
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