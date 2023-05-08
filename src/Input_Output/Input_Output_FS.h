#ifndef _InputOutputFS
#define _InputOutputFS

#include "Input_Output_Base.h"
#include "IO_exceptions.h"

#include <fstream>
#include <map>
#include <regex>
#include <exception>

using namespace std;


class Input_Output_FS : public Input_Output_Base {

    bool human; // Only affects share output
    std::map<unsigned int, std::istream *> input_channels;
    std::map<unsigned int, std::ostream *> output_channels;

public:
    Input_Output_FS()
            : Input_Output_Base() {
        ;
    }

    void init(bool human_type) {
        human = human_type;
    }

    long open_channel(unsigned int channel) override;

    void close_channel(unsigned int channel) override;

    gfp private_input_gfp(unsigned int channel) override;

    void private_output_gfp(const gfp &output, unsigned int channel) override;

    void public_output_gfp(const gfp &output, unsigned int channel) override;

    gfp public_input_gfp(unsigned int channel) override;

    void public_output_int(const long output, unsigned int channel) override;

    long public_input_int(unsigned int channel) override;

    void output_share(const Share &S, unsigned int channel) override;

    Share input_share(unsigned int channel) override;

    void trigger(Schedule &schedule) override;

    void debug_output(const stringstream &ss) override;

    void crash(unsigned int PC, unsigned int thread_num) override;

private:
    void create_debug_channel();

    void create_custom_channel(unsigned int channel);

    enum class Channel_Type {
        Input, Output
    };

    Channel_Type get_type() const;

    string get_filepath() const;

    bool is_input_channel_open(unsigned int channel);

    bool is_output_channel_open(unsigned int channel);

    void close_input_channel(unsigned int channel);

    void close_output_channel(unsigned int channel);
};

#endif
