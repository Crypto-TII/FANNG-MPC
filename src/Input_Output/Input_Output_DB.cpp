#include "Input_Output_DB.h"
#include "Exceptions/Exceptions.h"

const std::map<unsigned int, unsigned int> last_retrieved_index_per_channel;
const int DEFAULT_INDEX= 0;
const int DEBUG_CHANNEL= 0;
const string BASE_PATH= "Data/";

long Input_Output_DB::open_channel(unsigned int channel)
{
  cout << "Opening channel " << channel << endl;

  if (channel != DEBUG_CHANNEL)
    {
      create_custom_channel(channel);
    }

  return 0;
}

Input_Output_DB::Channel_Type Input_Output_DB::get_type() const
{
  cout << "What will the channel be used for? (i/o)" << endl;
  string type;
  cin >> type;
  cout << type << endl;

  if (type.compare("o") == 0)
    {
      return Input_Output_DB::Channel_Type::Output;
    }
  return Input_Output_DB::Channel_Type::Input;
}

void Input_Output_DB::create_custom_channel(unsigned int channel)
{
  open_database_connection();

  Channel_Type channelType= get_type();
  bool isInput= channelType == Channel_Type::Input;
  if (!isInput)
    {
      database.clear_data(channel, this->player_number);
    }

  last_retrieved_index_per_channel.insert(std::make_pair(channel, DEFAULT_INDEX));
}

void Input_Output_DB::open_database_connection()
{

  auto storage_config= Storage_Config(player_number);

  Input_Output_DB::database= Database(
      storage_config.get_mysql_url(),
      storage_config.get_mysql_user(),
      storage_config.get_mysql_password(),
      storage_config.get_mysql_database());
}

void Input_Output_DB::close_channel(unsigned int channel)
{
  last_retrieved_index_per_channel.erase(channel);
  int is_database_channel= channel != DEBUG_CHANNEL;
  if (is_database_channel)
    {
      Input_Output_DB::database.close();
    }
  cout << "Channel " << channel << " closed successfully" << endl;
}

gfp Input_Output_DB::private_input_gfp(unsigned int channel)
{
  string gfp_as_text= "";
  if (channel == DEBUG_CHANNEL)
    {
      cout << "Enter value on channel " << channel << " : ";
      cin >> gfp_as_text;
    }
  else
    {
      int last_retrieved_index= last_retrieved_index_per_channel.find(channel)->second;
      GFP_DTO gfp_dto= database.read_gfp(this->player_number, channel, last_retrieved_index);
      last_retrieved_index_per_channel[channel]= gfp_dto.get_id();
      gfp_as_text= gfp_dto.get_value();
    }

   DTO_Format_Utils::validate_rational_number_as_text(gfp_as_text);

  gfp y;
  DTO_Format_Utils::assign_gfp_from_text(&y, gfp_as_text);

  return y;
}

void Input_Output_DB::private_output_gfp(const gfp &output, unsigned int channel)
{
  if (channel == DEBUG_CHANNEL)
    {
      stringstream ss;
      output.output(ss, true);
      string value= ss.str();
      cout << value << " " << endl;
    }
  else
    {
      stringstream ss;
      output.output(ss, true);
      string value= ss.str();
      database.save_gfp(this->player_number, channel, value);
    }
  cout << "Writing database, channel= " << channel << endl;
}

gfp Input_Output_DB::public_input_gfp(unsigned int channel)
{
  string gfp_as_text= "";
  if (channel == DEBUG_CHANNEL)
    {
      cout << "Enter value on channel " << channel << " : ";
      cin >> gfp_as_text;
    }
  else
    {
      int last_retrieved_index= last_retrieved_index_per_channel.find(channel)->second;
      GFP_DTO gfp_dto= database.read_cint(this->player_number, channel, last_retrieved_index);
      last_retrieved_index_per_channel[channel]= gfp_dto.get_id();
      gfp_as_text= gfp_dto.get_value();
    }

   DTO_Format_Utils::validate_rational_number_as_text(gfp_as_text);

  gfp y;
  DTO_Format_Utils::assign_gfp_from_text(&y, gfp_as_text);
  return y;
}

void Input_Output_DB::public_output_gfp(const gfp &output, unsigned int channel)
{
  if (channel == DEBUG_CHANNEL)
    {
      cout << "Output channel " << channel << " : ";
      output.output(cout, true);
      cout << endl;
    }
  else
    {
      stringstream ss;
      output.output(ss, true);
      string value= ss.str();
      database.save_cint(this->player_number, channel, value);
      cout << "writing database, channel= " << channel << endl;
    }
}

long Input_Output_DB::public_input_int(unsigned int channel)
{
  if (channel == DEBUG_CHANNEL)
    {
      cout << "Enter value on channel " << channel << " : ";
      long x;
      cin >> x;
      Update_Checker(x, channel);

      return x;
    }

  int last_retrieved_index= last_retrieved_index_per_channel.find(channel)->second;
  Reg_int_DTO reg_int= database.read_reg_int(this->player_number, channel, last_retrieved_index);
  last_retrieved_index_per_channel[channel]= reg_int.get_id();

  long value= reg_int.get_value();

  // Important to have this call in each version of public_input_gfp
  Update_Checker(value, channel);

  return value;
}

void Input_Output_DB::public_output_int(const long output, unsigned int channel)
{
  if (channel == DEBUG_CHANNEL)
    {
      cout << "Output channel " << channel << " : " << output;
      cout << " = 0x" << hex << output << dec << endl;
    }
  else
    {
      string value= to_string(output);
      database.save_regint(this->player_number, channel, value);
      cout << "writing database, channel= " << channel << endl;
    }
}

void Input_Output_DB::output_share(const Share &S, unsigned int channel)
{
  if (channel == DEBUG_CHANNEL)
    {
      cout << "Output channel " << channel << " : ";
      S.output(cout, human);
    }
  else
    {
      database.save_share(channel, S);
      cout << "writing database, channel= " << channel << endl;
    }
}

Share Input_Output_DB::input_share(unsigned int channel)
{

  if (channel == DEBUG_CHANNEL)
    {
      cout << "Enter value on channel " << channel << " : ";
      Share S;
      S.input(cin, human);
      return S;
    }
  int last_retrieved_index= last_retrieved_index_per_channel.find(channel)->second;
  Share_DTO share_dto= database.read_share(this->player_number, channel, last_retrieved_index);
  int share_id= share_dto.get_id();
  last_retrieved_index_per_channel[channel]= share_id;
  string formatted_data= DTO_Format_Utils::format_database_output_to_share(share_dto);

  std::stringstream data_as_stream;
  data_as_stream.str(formatted_data);

  Share S;
  S.input(data_as_stream, human);
  return S;
}

void Input_Output_DB::trigger(Schedule &schedule)
{
  printf("Restart requested: Enter a number to proceed\n");
  int i;
  cin >> i;

  // Load new schedule file program streams, using the original
  // program name
  //
  // Here you could define programatically what the new
  // programs you want to run are, by directly editing the
  // public variables in the schedule object.
  unsigned int nthreads= schedule.Load_Programs();
  if (schedule.max_n_threads() < nthreads)
    {
      throw Processor_Error("Restart requires more threads, cannot do this");
    }
}

void Input_Output_DB::debug_output(const stringstream &ss)
{
  printf("%s", ss.str().c_str());
  fflush(stdout);
}

void Input_Output_DB::crash(unsigned int PC, unsigned int thread_num)
{
  printf("Crashing in thread %d at PC value %d\n", thread_num, PC);
  throw crash_requested();
}
