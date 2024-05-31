/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#ifndef _Garbled_Circuit_Serializer
#define _Garbled_Circuit_Serializer

#include "Garbled.h"
#include "Garbled_Circuit_Exceptions.h"
#include "Math/gf2n.h"
#include "Mod2Engine/Dabit.h"
#include "OT/aBit.h"
#include "Tools/MMO.h"
#include "Utils/DTO_Format_Utils.h"
#include "cereal/archives/binary.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

#include <cereal/archives/json.hpp>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

class aBitDTO
{

  public:
    string x;
    vector<string> M;
    vector<string> K;

    template <class Archive> void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(x), CEREAL_NVP(M), CEREAL_NVP(K));
    }

    static aBitDTO from_domain_model(const aBit &bit);
    static aBit to_domain_model(const aBitDTO &bitDTO);
    aBit to_domain_model() const;
};

class Share2DTO
{
  public:
    string p;
    vector<string> a;

    template <class Archive> void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(p), CEREAL_NVP(a));
    }

    static Share2DTO from_domain_model(const Share2 &bit);
    static Share2 to_domain_model(const Share2DTO &bitDTO);
    Share2 to_domain_model() const;
};

class ShareDabitDTO
{
  public:
    string player;
    string share;
    string mac_share;
    template <class Archive> void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(player), CEREAL_NVP(share), CEREAL_NVP(mac_share));
    }

    static ShareDabitDTO from_domain_model(const Share &share);
    static Share to_domain_model(const ShareDabitDTO &ShareDabitDTO);
    Share to_domain_model() const;
};

template <class SBitDTO> class DabitDTO
{
  public:
    SBitDTO b2_;
    ShareDabitDTO bp_;

    template <class Archive> void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(bp_), CEREAL_NVP(b2_));
    }

    static DabitDTO<aBitDTO> from_domain_model(const Dabit<aBit> &dabit);

    static Dabit<aBit> to_domain_model(const DabitDTO<aBitDTO> &_dabitDTO);

    static DabitDTO<Share2DTO> from_domain_model(const Dabit<Share2> &dabit);

    static Dabit<Share2> to_domain_model(const DabitDTO<Share2DTO> &_dabitDTO);

    template <class SBit>
    Dabit<SBit> to_domain_model() const;
};

class MMODTO
{
  public:
    uint8_t IV[176];

    MMODTO() = default;

    explicit MMODTO(const uint8_t *iv)
    {
        for (int i = 0; i < 176; i++)
        {
            IV[i] = iv[i];
        }
    }

    template <class Archive> void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(IV)); // operator() is the preferred way of interfacing the archive
    }

    static MMODTO from_domain_model(MMO mmo);

    static MMO to_domain_model(MMODTO mmoDTO);
};

class Base_Garbled_Circuit_DTO
{
  public:
    MMODTO mmo_dto;
    vector<aBitDTO> lambda_dto;
    vector<vector<string>> k_dto;
    vector<vector<vector<vector<string>>>> gab_dto;
    vector<string> one_label_dto;

    template <class Archive> void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(mmo_dto), CEREAL_NVP(lambda_dto), CEREAL_NVP(k_dto), CEREAL_NVP(gab_dto),
           CEREAL_NVP(one_label_dto));
    }

    static Base_Garbled_Circuit_DTO from_domain_model(const Base_Garbled_Circuit &garbleCircuit);

    Base_Garbled_Circuit to_domain_model() const;
};

// Define DTOs cereal version
CEREAL_CLASS_VERSION(Base_Garbled_Circuit_DTO, 1)
CEREAL_CLASS_VERSION(aBitDTO, 1)
CEREAL_CLASS_VERSION(DabitDTO<aBitDTO>, 1)
CEREAL_CLASS_VERSION(DabitDTO<Share2DTO>, 1)
CEREAL_CLASS_VERSION(MMODTO, 1)
CEREAL_CLASS_VERSION(ShareDabitDTO, 1)
CEREAL_CLASS_VERSION(Share2DTO, 1)

 template <class SBit>
class Garbled_Circuit_Serializer
{

  public:
    static string gf2n_as_text(const gf2n &value);

    static gf2n text_to_gf2n(const string &text);

    static vector<string> gf2n_vector_as_text_vector(const vector<gf2n> &values);

    static vector<gf2n> text_vector_to_gf2n_vector(const vector<string> &values);

    static vector<vector<string>> gf2n_matrix_as_text_matrix(const vector<vector<gf2n>> &values);

    static vector<vector<gf2n>> text_matrix_as_gf2n_matrix(const vector<vector<string>> &values);

    static vector<vector<vector<vector<gf2n>>>> text_4D_matrix_as_4D_gf2n_matrix(
        const vector<vector<vector<vector<string>>>> &values);

    static vector<vector<vector<vector<string>>>> g2fn_4D_matrix_as_4D_text_matrix(
        const vector<vector<vector<vector<gf2n>>>> &values);

    static void serialize_garbled_circuit_to_json_file(const Base_Garbled_Circuit &garbled_circuit,
                                                       const string &fileName);

    static string serialize_garbled_circuit_to_binary(const Base_Garbled_Circuit &garbled_circuit);

    static string serialize_garbled_circuit_to_json(const Base_Garbled_Circuit &garbled_circuit);

    static string serialize_garbled_circuit_to_base64(const Base_Garbled_Circuit &garbled_circuit);

    static string serialize_dabit_to_binary(const Dabit<SBit> &dabit);

    static string serialize_dabit_to_json(const Dabit<SBit> &dabit);

    static string serialize_dabit_to_base64(const Dabit<SBit> &dabit);

    static Base_Garbled_Circuit deserialize_garbled_circuit_from_json_file(const string &file_name);

    static Base_Garbled_Circuit deserialize_garbled_circuit_from_json(const string &circuit);

    static Base_Garbled_Circuit deserialize_garbled_circuit_from_binary(const string &circuit);

    static Base_Garbled_Circuit deserialize_garbled_circuit_from_base64(const string &circuit);

    static Dabit<SBit> deserialize_dabit_from_json(const string &dabit);

    static Dabit<SBit> deserialize_dabit_from_binary(const string &dabit);

    static Dabit<SBit> deserialize_dabit_from_base64(const string &dabit);
};

class Base64_Encoder
{
  public:
    static std::string encode(const std::string &input)
    {
        string out;
        int val = 0, valb = -6;
        for (unsigned char c : input)
        {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0)
            {
                out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6)
            out.push_back(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
        while (out.size() % 4)
            out.push_back('=');
        
        return out;
    }

    static std::string decode(const std::string &input)
    {
        string out;
        vector<int> T(256, -1);
        for (int i = 0; i < 64; i++)
        {
            T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
        }
        int val = 0, valb = -8;
        for (unsigned char c : input)
        {
            if (T[c] == -1)
                break;
            val = (val << 6) + T[c];
            valb += 6;
            if (valb >= 0)
            {
                out.push_back(char((val >> valb) & 0xFF));
                valb -= 8;
            }
        }
        return out;
    }
};

#endif
