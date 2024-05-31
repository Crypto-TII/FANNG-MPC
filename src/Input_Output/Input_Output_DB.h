/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#ifndef _InputOutputDB
#define _InputOutputDB

#include "Input_Output_Base.h"

#include "Database.h"
#include "IO_exceptions.h"
#include "Reg_int_DTO.h"
#include "Share_DTO.h"
#include "Utils/DTO_Format_Utils.h"
#include <Tools/Storage_Config.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <stdlib.h>

using namespace std;


class Input_Output_DB : public Input_Output_Base {

    bool human; // Only affects share output
    int player_number;
    std::map<unsigned int, unsigned int> last_retrieved_index_per_channel; //Map<channel, index>
    Database database;
    enum class Channel_Type {
        Input, Output
    };

    Channel_Type get_type() const;
    void create_custom_channel(unsigned int channel);
    void open_database_connection();

public:
    Input_Output_DB()
            : Input_Output_Base() {
        ;
    }

    void init(int player_number, bool human_type) {
        this->player_number = player_number;
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

};

#endif
