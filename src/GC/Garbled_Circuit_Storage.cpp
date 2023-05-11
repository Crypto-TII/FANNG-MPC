#include "Garbled_Circuit_Storage.h"

using namespace std;

shared_ptr<Garbled_Circuit_Storage> Garbled_Circuit_Storage::create_from_config_file(unsigned int player_number)
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
          return make_shared<MySQL_Garbled_Circuit_Storage>(controller, insert_batch_size);
        }
      case General_storage_type::File_System_Garbled_Circuit_Storage_Type:
        return make_shared<File_System_Garbled_Circuit_Storage>(config.get_file_system_storage_directory());
      default: 
          return nullptr;
    }
}

void File_System_Garbled_Circuit_Storage::save(const vector<Base_Garbled_Circuit> &garbled_circuits,
                                               const Choicebits &choicebits,
                                               unsigned int circuit_number,
                                               unsigned int player_number)
{
  for (const Base_Garbled_Circuit &circuit : garbled_circuits)
    {
      const string serialized_garbled_circuit= serialize(circuit);
      const string serialized_choicebits= choicebits.to_string();
      unsigned int circuit_id= get_next_circuit_id(player_number, circuit_number);
      write_in_file(circuit_id, player_number, circuit_number, serialized_choicebits, serialized_garbled_circuit);
    }
}

void File_System_Garbled_Circuit_Storage::write_in_file(unsigned int circuit_id,
                                                        unsigned int player_number,
                                                        unsigned int circuit_number,
                                                        const string &serialized_choicebits,
                                                        const string &serialized_garbled_circuit)
{

  string output_path= build_file_path(player_number, circuit_number);
  ofstream file;
  file.open(output_path, ios_base::app);

  if (!file)
    {
      throw file_error(output_path);
    }

  cout << "Writing garbled circuit in " << output_path << endl;
  string circuit_as_json= serialized_garbled_circuit;

  circuit_as_json.erase(std::remove(circuit_as_json.begin(), circuit_as_json.end(), '\n'), circuit_as_json.end());
  circuit_as_json.erase(std::remove(circuit_as_json.begin(), circuit_as_json.end(), ' '), circuit_as_json.end());

  unsigned int used= 0;
  file << (int) circuit_id << ";";
  file << (int) player_number << ";";
  file << (int) circuit_number << ";";
  file << circuit_as_json << ";";
  file << serialized_choicebits << ";";
  file << used << endl;
  file.close();
}

string File_System_Garbled_Circuit_Storage::build_file_path(unsigned int player_number,
                                                            unsigned int circuit_number)
{
  string filename= FILES_BASE_NAME;
  filename.append(to_string(circuit_number));
  filename.append("-P");
  filename.append(to_string(player_number));

  if (directory_name.empty())
    {
      return filename;
    }

  return directory_name + "/" + filename;
}

unsigned int
File_System_Garbled_Circuit_Storage::get_next_circuit_id(unsigned int player_number, unsigned int circuit_number)
{
  string path= build_file_path(player_number, circuit_number);
  ifstream file;
  file.open(path, ios_base::app);

  string line;
  string last_circuit_id;

  while (getline(file, line))
    {
      stringstream ss(line);
      getline(ss, last_circuit_id, ',');
    }

  if (last_circuit_id.empty())
    {
      return 0;
    }

  return stoi(last_circuit_id) + 1;
}

void File_System_Garbled_Circuit_Storage::overwrite_file_with_lines(vector<string> lines, const string &path) const
{
  ofstream output_file(path);
  ostream_iterator<string> output_iterator(output_file, "\n");
  copy(lines.begin(), lines.end(), output_iterator);
}

vector<Base_Garbled_Circuit> File_System_Garbled_Circuit_Storage::read(unsigned int number_of_circuits,
                                                                       const Choicebits &choicebits,
                                                                       unsigned int circuit_number,
                                                                       unsigned int player_number)
{
  string path= build_file_path(player_number, circuit_number);
  ifstream file;
  file.open(path, ios_base::app);

  if (!file)
    {
      throw file_error(path);
    }

  string line;
  string read_circuit_id;
  string read_player_number;
  string read_circuit_number;
  string read_circuit_content;
  string read_choicebits;
  string read_used;

  vector<Base_Garbled_Circuit> circuits;
  vector<string> updated_lines;

  while (getline(file, line))
    {
      stringstream ss(line);
      getline(ss, read_circuit_id, ';');
      getline(ss, read_player_number, ';');
      getline(ss, read_circuit_number, ';');
      getline(ss, read_circuit_content, ';');
      getline(ss, read_choicebits, ';');
      getline(ss, read_used, ';');

      if (read_used == "0" && read_player_number == to_string(player_number) &&
          read_circuit_number == to_string(circuit_number) && read_choicebits == choicebits.to_string() &&
          circuits.size() < number_of_circuits)
        {

          circuits.push_back(deserialize(read_circuit_content));

          string used_circuit= read_circuit_id + ";";
          used_circuit+= read_player_number + ";";
          used_circuit+= read_circuit_number + ";";
          used_circuit+= read_circuit_content + ";";
          used_circuit+= read_choicebits + ";1";
          updated_lines.push_back(used_circuit);
        }
      else
        {
          updated_lines.push_back(line);
        }
    }

  file.close();

  if (circuits.size() < number_of_circuits)
    {
      throw Not_Enough_Garbled_Circuits(circuits.size(), number_of_circuits);
    }

  overwrite_file_with_lines(updated_lines, path);

  return circuits;
}

void MySQL_Garbled_Circuit_Storage::save(const vector<Base_Garbled_Circuit> &garbled_circuits,
                                         const Choicebits &choicebits,
                                         unsigned int circuit_number,
                                         unsigned int player_number)
{

  if (insert_batch_size > 1000)
    {
      throw Batch_insert_size_too_big(insert_batch_size);
    }

  vector<string> serialized_garbled_circuits(garbled_circuits.size());

  const string INSERT_BASE_QUERY= "INSERT INTO garbled_circuit (PLAYER, CIRCUIT_NUMBER, CIRCUIT_CONTENT, CHOICEBITS) VALUES ";
  string query= INSERT_BASE_QUERY;
  int current_batch_size= 0;
  auto garbled_circuit_iterator= garbled_circuits.begin();
  while (is_batch_incomplete(current_batch_size) &&
         are_circuits_to_insert(garbled_circuit_iterator, garbled_circuits.end()))
    {
      query+= format_insert_row(choicebits, circuit_number, player_number,
                                const_cast<Base_Garbled_Circuit &>(*garbled_circuit_iterator));
      ++current_batch_size;
      ++garbled_circuit_iterator;

      bool batch_is_full= current_batch_size == insert_batch_size;
      bool are_no_circuits_to_insert= garbled_circuit_iterator == garbled_circuits.end();
      if (batch_is_full || are_no_circuits_to_insert)
        {
          query[query.size() - 1]= ';';
          controller->connection->createStatement()->execute(query);
          current_batch_size= 0;
          query= INSERT_BASE_QUERY;
        }
    }
}

string MySQL_Garbled_Circuit_Storage::format_insert_row(const Choicebits &choicebits, unsigned int circuit_number,
                                                        unsigned int player_number,
                                                        const Base_Garbled_Circuit &garbled_circuit) const
{
  return "(" + to_string(player_number) + "," + to_string(circuit_number) + ",'" +
         serialize(garbled_circuit) + "','" + choicebits.to_string() + "'),";
}

bool MySQL_Garbled_Circuit_Storage::are_circuits_to_insert(
    const vector<Base_Garbled_Circuit>::const_iterator &current_garbled_circuit_iterator,
    const vector<Base_Garbled_Circuit>::const_iterator &garbled_circuit_last_element) const
{
  return current_garbled_circuit_iterator != garbled_circuit_last_element;
}

bool MySQL_Garbled_Circuit_Storage::is_batch_incomplete(int current_batch_size) const
{
  return current_batch_size < insert_batch_size;
}

vector<Base_Garbled_Circuit> MySQL_Garbled_Circuit_Storage::read(unsigned int number_of_circuits,
                                                                 const Choicebits &choicebits,
                                                                 unsigned int circuit_number,
                                                                 unsigned int player_number)
{
  string select_query= "SELECT id, circuit_content FROM garbled_circuit WHERE player = ? AND circuit_number = ? AND choicebits = ? AND used = 0 ORDER BY id ASC LIMIT ?";
  unique_ptr<sql::PreparedStatement> preparedStatement{
      controller->connection->prepareStatement(select_query)};

  preparedStatement->setInt(1, (int) player_number);
  preparedStatement->setInt(2, (int) circuit_number);
  preparedStatement->setString(3, choicebits.to_string());
  preparedStatement->setInt(4, (int) number_of_circuits);

  unique_ptr<sql::ResultSet> response{
      preparedStatement->executeQuery()};

  vector<string> circuit_ids;
  vector<Base_Garbled_Circuit> circuits;
  while (response->next())
    {
      circuit_ids.push_back(response->getString("id"));
      string serialized_circuit= response->getString("circuit_content");
      Base_Garbled_Circuit circuit= deserialize(serialized_circuit);
      circuits.push_back(circuit);
    }

  if (circuits.size() < number_of_circuits)
    {
      throw Not_Enough_Garbled_Circuits(circuits.size(), number_of_circuits);
    }

  std::thread t(mark_circuits_as_used, circuit_ids, controller->connection);
  t.join();

  return circuits;
}

void MySQL_Garbled_Circuit_Storage::mark_circuits_as_used(const vector<string> &circuit_ids,
                                                          shared_ptr<sql::Connection> connection)
{
  try
    {
      string ids_as_string= boost::algorithm::join(circuit_ids, ", ");
      string mark_as_read_query=
          "LOCK TABLES garbled_circuit WRITE;UPDATE garbled_circuit SET used = 1 WHERE id in (" + ids_as_string +
          ");UNLOCK TABLES;";
      connection->createStatement()->executeUpdate(mark_as_read_query);
    }
  catch (const std::exception &e)
    {
      throw Error_updating_loaded_circuits_exception(e.what());
    }
}
