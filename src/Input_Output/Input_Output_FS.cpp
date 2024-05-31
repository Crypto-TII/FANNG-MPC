/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#include "Input_Output_FS.h"
#include "Exceptions/Exceptions.h"

std::map<unsigned int, std::istream *> input_channels;
std::map<unsigned int, std::ostream *> output_channels;
const int DEBUG_CHANNEL = 0;
const string BASE_PATH = "Data/";

long Input_Output_FS::open_channel(unsigned int channel) {
    cout << "Opening channel " << channel << endl;

    if (channel == DEBUG_CHANNEL) {
        create_debug_channel();
        return 0;
    }

    create_custom_channel(channel);
    return 0;
}

void Input_Output_FS::create_debug_channel() {
    input_channels.insert(std::make_pair(DEBUG_CHANNEL, &cin));
    output_channels.insert(std::make_pair(DEBUG_CHANNEL, &cout));
}

Input_Output_FS::Channel_Type Input_Output_FS::get_type() const {
    cout << "What will the channel be used for? (i/o)" << endl;
    string type;
    cin >> type;
    cout << type << endl;

    if (type.compare("o") == 0) {
        return Channel_Type::Output;
    }
    return Input_Output_FS::Channel_Type::Input;
}

string Input_Output_FS::get_filepath() const {
    cout << "Introduce filepath: " << endl;
    string filepath;
    cin >> filepath;
    cout << filepath << endl;
    return filepath;
}

void Input_Output_FS::create_custom_channel(unsigned int channel) {

    Channel_Type channelType = get_type();
    bool isInput = channelType == Channel_Type::Input;

    string filepath = get_filepath();
    if (isInput) {
        auto *ifs = new ifstream(BASE_PATH + filepath);
        input_channels.insert(std::make_pair(channel, ifs));
        return;
    }

    auto *ofs = new ofstream(BASE_PATH + filepath);
    output_channels.insert(std::make_pair(channel, ofs));
}

void Input_Output_FS::close_channel(unsigned int channel) {
    if (is_input_channel_open(channel)) {
        close_input_channel(channel);
        cout << "Channel " << channel << " closed" << endl;
    } else if (is_output_channel_open(channel)) {
        close_output_channel(channel);
        cout << "Channel " << channel << " closed" << endl;
    } else {
        cout << "Channel " << channel << " already closed before" << endl;
    }
}

bool Input_Output_FS::is_input_channel_open(unsigned int channel) {
    auto input_channel = input_channels.find(channel);
    if (input_channel != input_channels.end()) {
        return true;
    }
    return false;
}

bool Input_Output_FS::is_output_channel_open(unsigned int channel) {
    if (output_channels.find(channel) != output_channels.end()) {
        return true;
    }
    return false;
}

void Input_Output_FS::close_input_channel(unsigned int channel) {
    auto input_channel = input_channels.find(channel);
    if (channel != DEBUG_CHANNEL) {
        ((ifstream *) input_channel->second)->close();
    }
    input_channels.erase(input_channel);
}

void Input_Output_FS::close_output_channel(unsigned int channel) {
    auto output_channel = output_channels.find(channel);
    if (channel != DEBUG_CHANNEL) {
        ((ofstream *) output_channel->second)->close();
    }
    output_channels.erase(output_channel);
}

gfp Input_Output_FS::private_input_gfp(unsigned int channel) {
    auto input_channel = input_channels.find(channel);
    istream *is = input_channel->second;
    string value;
    *is >> value;
    gfp y;

    regex rational_number_regex("^(-?[0-9]+)(\\.[0-9]+)?$");
    if (!regex_match (value, rational_number_regex))
        throw Invalid_format_exception();

    bool is_decimal = value.find(".") != std::string::npos;
    if (is_decimal){
        double converted_value = stod(value);
        y.assign(converted_value);
    }
    else{
        int converted_value = stoi(value);
        y.assign(converted_value);
    }
    return y;
}

void Input_Output_FS::private_output_gfp(const gfp &output, unsigned int channel) {
    auto output_channel = output_channels.find(channel);
    ostream *os = output_channel->second;
    output.output(*os, true);
}

gfp Input_Output_FS::public_input_gfp(unsigned int channel) {
    auto input_channel = input_channels.find(channel);
    istream *is = input_channel->second;
    string value;
    *is >> value;
    gfp y;

    regex rational_number_regex("^(-?[0-9]+)(\\.[0-9]+)?$");
    if (!regex_match (value, rational_number_regex))
        throw Invalid_format_exception();

    bool is_decimal = value.find(".") != std::string::npos;

    if (is_decimal){
        double converted_value = stod(value);
        y.assign(converted_value);
    }
    else{
        int converted_value = stoi(value);
        y.assign(converted_value);
    }

    Update_Checker(y, channel);

    return y;
}

void Input_Output_FS::public_output_gfp(const gfp &output, unsigned int channel) {
    auto output_channel = output_channels.find(channel);
    ostream *os = output_channel->second;
    output.output(*os, true);
    *os << std::endl;
}

long Input_Output_FS::public_input_int(unsigned int channel) {
    auto input_channel = input_channels.find(channel);
    long x;
    istream *is = input_channel->second;
    *is >> x;

    // Important to have this call in each version of public_input_gfp
    Update_Checker(x, channel);

    return x;
}

void Input_Output_FS::public_output_int(const long output, unsigned int channel) {
    auto output_channel = output_channels.find(channel);
    ostream *os = output_channel->second;

    *os << "Output channel " << channel << " : " << output;
    *os << " = 0x" << hex << output << dec << endl;
}

void Input_Output_FS::output_share(const Share &S, unsigned int channel) {
    auto output_channel = output_channels.find(channel);
    ostream *os = output_channel->second;

    S.output(*os, human);
}

Share Input_Output_FS::input_share(unsigned int channel) {
    auto input_channel = input_channels.find(channel);
    istream *is = input_channel->second;

    Share S;
    S.input(*is, human);
    return S;
}

void Input_Output_FS::trigger(Schedule &schedule) {
    printf("Restart requested: Enter a number to proceed\n");
    int i;
    cin >> i;

    // Load new schedule file program streams, using the original
    // program name
    //
    // Here you could define programatically what the new
    // programs you want to run are, by directly editing the
    // public variables in the schedule object.
    unsigned int nthreads = schedule.Load_Programs();
    if (schedule.max_n_threads() < nthreads) {
        throw Processor_Error("Restart requires more threads, cannot do this");
    }
}

void Input_Output_FS::debug_output(const stringstream &ss) {
    printf("%s", ss.str().c_str());
    fflush(stdout);
}

void Input_Output_FS::crash(unsigned int PC, unsigned int thread_num) {
    printf("Crashing in thread %d at PC value %d\n", thread_num, PC);
    throw crash_requested();
}
