/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#include "Garbled_Circuit_Serializer.h"

using namespace std;

// aBitDTO Conversions
aBitDTO aBitDTO::from_domain_model(const aBit &bit)
{

    string x_text = Garbled_Circuit_Serializer<aBit>::gf2n_as_text(bit.get_raw_value());

    vector<string> m_text_vector = Garbled_Circuit_Serializer<aBit>::gf2n_vector_as_text_vector(bit.get_MACs());
    vector<string> k_text_vector = Garbled_Circuit_Serializer<aBit>::gf2n_vector_as_text_vector(bit.get_Keys());
    return aBitDTO({x_text, m_text_vector, k_text_vector});
}

aBit aBitDTO::to_domain_model(const aBitDTO &bitDTO)
{
    aBit bit;
    bit.set_X(Garbled_Circuit_Serializer<aBit>::text_to_gf2n(bitDTO.x));
    bit.set_MACs(Garbled_Circuit_Serializer<aBit>::text_vector_to_gf2n_vector(bitDTO.M));
    bit.set_Keys(Garbled_Circuit_Serializer<aBit>::text_vector_to_gf2n_vector(bitDTO.K));

    return bit;
}

aBit aBitDTO::to_domain_model() const
{
    return aBitDTO::to_domain_model(*this);
}

// Share2DTO conversions
Share2DTO Share2DTO::from_domain_model(const Share2 &bit)
{

    string p = to_string(bit.get_player());
    vector<string> a = DTO_Format_Utils::word_vector_to_string_vector(bit.get_shares());

    return Share2DTO({p, a});
}

Share2 Share2DTO::to_domain_model(const Share2DTO &bitDTO)
{
    int p = std::stoi(bitDTO.p);
    vector<word> a = DTO_Format_Utils::string_vector_to_word_vector(bitDTO.a);
    Share2 share2;
    share2.assign(p, a);
    return share2;
}

Share2 Share2DTO::to_domain_model() const
{
    return Share2DTO::to_domain_model(*this);
}

// ShareDabitDTO conversions
ShareDabitDTO ShareDabitDTO::from_domain_model(const Share &bit)
{

    vector<string> share_str = DTO_Format_Utils::split_share(bit);

    string player = share_str[0];
    string share = share_str[1];
    string mac_share = share_str[2];

    return ShareDabitDTO({player, share, mac_share});
}

Share ShareDabitDTO::to_domain_model(const ShareDabitDTO &bitDTO)
{
    Share bp = DTO_Format_Utils::inputs_to_share(bitDTO.player, bitDTO.share, bitDTO.mac_share);
    return bp;
}

Share ShareDabitDTO::to_domain_model() const
{
    return ShareDabitDTO::to_domain_model(*this);
}

// dabitdto conversions
template <> DabitDTO<Share2DTO> DabitDTO<Share2DTO>::from_domain_model(const Dabit<Share2> &dabit)
{

    Share2DTO b2_;
    b2_ = Share2DTO::from_domain_model(dabit.b2_);

    ShareDabitDTO bp_;
    bp_ = ShareDabitDTO::from_domain_model(dabit.bp_);

    return DabitDTO<Share2DTO>({b2_, bp_});
}

template <> Dabit<Share2> DabitDTO<Share2DTO>::to_domain_model(const DabitDTO<Share2DTO> &dabitDTO)
{
    Share bp = ShareDabitDTO::to_domain_model(dabitDTO.bp_);
    Share2 b2 = Share2DTO::to_domain_model(dabitDTO.b2_);
    Dabit<Share2> dabit(bp, b2);
    stringstream output;
    dabit.bp_.output(output, true);

    return dabit;
}

// aBit implementations for DabitDTO
template <> DabitDTO<aBitDTO> DabitDTO<aBitDTO>::from_domain_model(const Dabit<aBit> &dabit)
{

    aBitDTO b2_;
    b2_ = aBitDTO::from_domain_model(dabit.b2_);

    ShareDabitDTO bp_;
    bp_ = ShareDabitDTO::from_domain_model(dabit.bp_);

    return DabitDTO<aBitDTO>({b2_, bp_});
}

template <> Dabit<aBit> DabitDTO<aBitDTO>::to_domain_model(const DabitDTO<aBitDTO> &dabitDTO)
{

    Share bp = DTO_Format_Utils::inputs_to_share(dabitDTO.bp_.player, dabitDTO.bp_.share, dabitDTO.bp_.mac_share);

    aBit b2;
    b2.set_X(Garbled_Circuit_Serializer<aBit>::text_to_gf2n(dabitDTO.b2_.x));
    b2.set_MACs(Garbled_Circuit_Serializer<aBit>::text_vector_to_gf2n_vector(dabitDTO.b2_.M));
    b2.set_Keys(Garbled_Circuit_Serializer<aBit>::text_vector_to_gf2n_vector(dabitDTO.b2_.K));

    Dabit<aBit> dabit(bp, b2);

    return dabit;
}

template <class SBitDTO> template <class SBit> Dabit<SBit> DabitDTO<SBitDTO>::to_domain_model() const
{

    return DabitDTO<SBit>::to_domain_model(*this);
}

MMODTO MMODTO::from_domain_model(MMO mmo)
{
    return MMODTO(mmo.getIV());
}

MMO MMODTO::to_domain_model(MMODTO mmoDTO)
{
    return MMO(mmoDTO.IV);
}

Base_Garbled_Circuit_DTO Base_Garbled_Circuit_DTO::from_domain_model(const Base_Garbled_Circuit &garbled_circuit)
{
    vector<aBit> lambda = garbled_circuit.get_lambda();
    auto lambda_dto = vector<aBitDTO>(lambda.size());

    for (unsigned int i = 0; i < lambda.size(); i++)
    {
        lambda_dto[i] = aBitDTO::from_domain_model(lambda[i]);
    }

    vector<vector<string>> k_dto =
        Garbled_Circuit_Serializer<aBit>::gf2n_matrix_as_text_matrix(garbled_circuit.get_k());
    vector<string> one_label_dto =
        Garbled_Circuit_Serializer<aBit>::gf2n_vector_as_text_vector(garbled_circuit.get_one_label());
    vector<vector<vector<vector<string>>>> gab_dto =
        Garbled_Circuit_Serializer<aBit>::g2fn_4D_matrix_as_4D_text_matrix(garbled_circuit.get_gab());

    return Base_Garbled_Circuit_DTO({
        MMODTO::from_domain_model(garbled_circuit.get_mmo()),
        lambda_dto,
        k_dto,
        gab_dto,
        one_label_dto,
    });
}

Base_Garbled_Circuit Base_Garbled_Circuit_DTO::to_domain_model() const
{
    MMO mmo = MMODTO::to_domain_model(mmo_dto);
    auto lambda = vector<aBit>(lambda_dto.size());
    for (unsigned int i = 0; i < lambda_dto.size(); i++)
    {
        lambda[i] = aBitDTO::to_domain_model(lambda_dto[i]);
    }

    vector<vector<gf2n>> k2 = Garbled_Circuit_Serializer<aBit>::text_matrix_as_gf2n_matrix(k_dto);
    vector<gf2n> one_label = Garbled_Circuit_Serializer<aBit>::text_vector_to_gf2n_vector(one_label_dto);
    vector<vector<vector<vector<gf2n>>>> gab =
        Garbled_Circuit_Serializer<aBit>::text_4D_matrix_as_4D_gf2n_matrix(gab_dto);

    Base_Garbled_Circuit garbled_circuit;
    garbled_circuit.set_mmo(mmo);
    garbled_circuit.set_lambda(lambda);
    garbled_circuit.set_k(k2);
    garbled_circuit.set_gab(gab);
    garbled_circuit.set_one_label(one_label);

    return garbled_circuit;
}
template <class SBit> string Garbled_Circuit_Serializer<SBit>::gf2n_as_text(const gf2n &value)
{
    stringstream stream;
    value.output(stream, true);
    return stream.str();
}

template <class SBit> gf2n Garbled_Circuit_Serializer<SBit>::text_to_gf2n(const string &text)
{
    stringstream stream;
    stream << text;
    gf2n value;
    value.input(stream, true);
    return value;
}

template <class SBit>
vector<string> Garbled_Circuit_Serializer<SBit>::gf2n_vector_as_text_vector(const vector<gf2n> &values)
{
    unsigned long size = values.size();
    auto values_as_text = vector<string>(size);
    for (unsigned long i = 0; i < size; i++)
    {
        values_as_text[i] = gf2n_as_text(values[i]);
    }

    return values_as_text;
}

template <class SBit>
vector<gf2n> Garbled_Circuit_Serializer<SBit>::text_vector_to_gf2n_vector(const vector<string> &values)
{
    unsigned long size = values.size();
    auto values_as_gf2n = vector<gf2n>(size);
    for (unsigned long i = 0; i < size; i++)
    {
        values_as_gf2n[i] = text_to_gf2n(values[i]);
    }

    return values_as_gf2n;
}

template <class SBit>
vector<vector<string>> Garbled_Circuit_Serializer<SBit>::gf2n_matrix_as_text_matrix(const vector<vector<gf2n>> &values)
{
    unsigned long rows = values.size();
    unsigned long columns = values[0].size();
    auto matrix_as_text = vector<vector<string>>(rows, vector<string>(columns));

    for (unsigned long i = 0; i < rows; i++)
    {
        matrix_as_text[i] = gf2n_vector_as_text_vector(values[i]);
    }

    return matrix_as_text;
}

template <class SBit>
vector<vector<gf2n>> Garbled_Circuit_Serializer<SBit>::text_matrix_as_gf2n_matrix(const vector<vector<string>> &values)
{
    unsigned long rows = values.size();
    unsigned long columns = values[0].size();
    auto matrix_as_gf2n = vector<vector<gf2n>>(rows, vector<gf2n>(columns));

    for (unsigned long i = 0; i < rows; i++)
    {
        matrix_as_gf2n[i] = text_vector_to_gf2n_vector(values[i]);
    }

    return matrix_as_gf2n;
}

template <class SBit>
vector<vector<vector<vector<string>>>> Garbled_Circuit_Serializer<SBit>::g2fn_4D_matrix_as_4D_text_matrix(
    const vector<vector<vector<vector<gf2n>>>> &values)
{
    unsigned long rows = values[0][0].size();
    unsigned long columns = values[0][0][0].size();
    unsigned long depth = values[0].size();
    unsigned long fourth_dimension = values.size();

    auto text_4D_matrix = vector<vector<vector<vector<string>>>>(
        fourth_dimension, vector<vector<vector<string>>>(depth, vector<vector<string>>(columns, vector<string>(rows))));

    for (unsigned long i = 0; i < fourth_dimension; i++)
    {
        for (unsigned long j = 0; j < depth; j++)
        {
            text_4D_matrix[i][j] = gf2n_matrix_as_text_matrix(values[i][j]);
        }
    }

    return text_4D_matrix;
}

template <class SBit>
vector<vector<vector<vector<gf2n>>>> Garbled_Circuit_Serializer<SBit>::text_4D_matrix_as_4D_gf2n_matrix(
    const vector<vector<vector<vector<string>>>> &values)
{
    unsigned long rows = values[0][0].size();
    unsigned long columns = values[0][0][0].size();
    unsigned long depth = values[0].size();
    unsigned long fourth_dimension = values.size();

    auto gf2n_4D_matrix = vector<vector<vector<vector<gf2n>>>>(
        fourth_dimension, vector<vector<vector<gf2n>>>(depth, vector<vector<gf2n>>(columns, vector<gf2n>(rows))));

    for (unsigned long i = 0; i < fourth_dimension; i++)
    {
        for (unsigned long j = 0; j < depth; j++)
        {
            gf2n_4D_matrix[i][j] = text_matrix_as_gf2n_matrix(values[i][j]);
        }
    }

    return gf2n_4D_matrix;
}

template <class SBit>
void Garbled_Circuit_Serializer<SBit>::serialize_garbled_circuit_to_json_file(
    const Base_Garbled_Circuit &garbled_circuit, const string &fileName)
{
    Base_Garbled_Circuit_DTO garbled_circuit_dto = Base_Garbled_Circuit_DTO::from_domain_model(garbled_circuit);
    ofstream output(fileName);
    {
        cereal::JSONOutputArchive ar(output);
        ar(garbled_circuit_dto);
    }
}

template <class SBit>
string Garbled_Circuit_Serializer<SBit>::serialize_garbled_circuit_to_json(const Base_Garbled_Circuit &garbled_circuit)
{
    Base_Garbled_Circuit_DTO garbled_circuit_dto = Base_Garbled_Circuit_DTO::from_domain_model(garbled_circuit);
    stringstream output;
    {
        cereal::JSONOutputArchive ar(output);
        ar(garbled_circuit_dto);
    }

    return output.str();
}

template <class SBit>
string Garbled_Circuit_Serializer<SBit>::serialize_garbled_circuit_to_binary(
    const Base_Garbled_Circuit &garbled_circuit)
{
    Base_Garbled_Circuit_DTO garbled_circuit_dto = Base_Garbled_Circuit_DTO::from_domain_model(garbled_circuit);
    stringstream output;
    {
        cereal::BinaryOutputArchive ar(output);
        ar(garbled_circuit_dto);
    }

    return output.str();
}

template <class SBit>
Base_Garbled_Circuit Garbled_Circuit_Serializer<SBit>::deserialize_garbled_circuit_from_json_file(
    const string &file_name)
{
    ifstream input_garble(file_name);
    Base_Garbled_Circuit_DTO garbled_circuit_to_rebuild;
    try
    {
        cereal::JSONInputArchive ar(input_garble);
        ar(garbled_circuit_to_rebuild);
    }
    catch (const cereal::RapidJSONException &e)
    {
        throw Garbled_circuit_file_not_found(file_name + " error message: " +  e.what());
    }

    return garbled_circuit_to_rebuild.to_domain_model();
}

template <class SBit>
Base_Garbled_Circuit Garbled_Circuit_Serializer<SBit>::deserialize_garbled_circuit_from_json(const string &circuit)
{
    stringstream input_garbled(circuit);
    Base_Garbled_Circuit_DTO garbled_circuit_to_rebuild;
    {
        cereal::JSONInputArchive ar(input_garbled);
        ar(garbled_circuit_to_rebuild);
    }
    return garbled_circuit_to_rebuild.to_domain_model();
}

template <class SBit>
Base_Garbled_Circuit Garbled_Circuit_Serializer<SBit>::deserialize_garbled_circuit_from_binary(const string &circuit)
{
    stringstream input_garbled(circuit);
    Base_Garbled_Circuit_DTO garbled_circuit_to_rebuild;
    {
        cereal::BinaryInputArchive ar(input_garbled);
        ar(garbled_circuit_to_rebuild);
    }
    return garbled_circuit_to_rebuild.to_domain_model();
}

template <class SBit>
string Garbled_Circuit_Serializer<SBit>::serialize_garbled_circuit_to_base64(
    const Base_Garbled_Circuit &garbled_circuit)
{
    return Base64_Encoder::encode(serialize_garbled_circuit_to_binary(garbled_circuit));
}

template <class SBit>
Base_Garbled_Circuit Garbled_Circuit_Serializer<SBit>::deserialize_garbled_circuit_from_base64(const string &circuit)
{
    return deserialize_garbled_circuit_from_binary(Base64_Encoder::decode(circuit));
}

// dabit process
template <> string Garbled_Circuit_Serializer<aBit>::serialize_dabit_to_json(const Dabit<aBit> &dabit)
{
    DabitDTO<aBitDTO> dabit_dto = DabitDTO<aBitDTO>::from_domain_model(dabit);
    stringstream output;
    {
        cereal::JSONOutputArchive ar(output);
        ar(dabit_dto);
    }

    return output.str();
}

template <> string Garbled_Circuit_Serializer<Share2>::serialize_dabit_to_json(const Dabit<Share2> &dabit)
{
    DabitDTO<Share2DTO> dabit_dto = DabitDTO<Share2DTO>::from_domain_model(dabit);
    stringstream output;
    {
        cereal::JSONOutputArchive ar(output);
        ar(dabit_dto);
    }

    return output.str();
}

template <> Dabit<aBit> Garbled_Circuit_Serializer<aBit>::deserialize_dabit_from_json(const string &dabit)
{
    stringstream input_dabit(dabit);
    DabitDTO<aBitDTO> dabit_to_rebuild;
    {
        cereal::JSONInputArchive ar(input_dabit);
        ar(dabit_to_rebuild);
    }
    return DabitDTO<aBitDTO>::to_domain_model(dabit_to_rebuild);
}

template <> Dabit<Share2> Garbled_Circuit_Serializer<Share2>::deserialize_dabit_from_json(const string &dabit)
{
    stringstream input_dabit(dabit);
    DabitDTO<Share2DTO> dabit_to_rebuild;
    {
        cereal::JSONInputArchive ar(input_dabit);
        ar(dabit_to_rebuild);
    }
    return DabitDTO<Share2DTO>::to_domain_model(dabit_to_rebuild);
}

template <> string Garbled_Circuit_Serializer<aBit>::serialize_dabit_to_binary(const Dabit<aBit> &dabit)
{
    DabitDTO<aBitDTO> dabit_dto = DabitDTO<aBitDTO>::from_domain_model(dabit);

    stringstream output;
    {
        cereal::BinaryOutputArchive ar(output);
        ar(dabit_dto);
    }
    return output.str();
}

template <> string Garbled_Circuit_Serializer<Share2>::serialize_dabit_to_binary(const Dabit<Share2> &dabit)
{
    DabitDTO<Share2DTO> dabit_dto = DabitDTO<Share2DTO>::from_domain_model(dabit);
    stringstream output;
    {
        cereal::BinaryOutputArchive ar(output);
        ar(dabit_dto);
    }

    return output.str();
}

template <> Dabit<aBit> Garbled_Circuit_Serializer<aBit>::deserialize_dabit_from_binary(const string &dabit)
{
    stringstream input_dabit(dabit);
    DabitDTO<aBitDTO> dabit_to_rebuild;
    {
        cereal::BinaryInputArchive ar(input_dabit);

        ar(dabit_to_rebuild);
    }
    return DabitDTO<aBitDTO>::to_domain_model(dabit_to_rebuild);
}

template <> Dabit<Share2> Garbled_Circuit_Serializer<Share2>::deserialize_dabit_from_binary(const string &dabit)
{
    stringstream input_dabit(dabit);
    DabitDTO<Share2DTO> dabit_to_rebuild;
    {
        cereal::BinaryInputArchive ar(input_dabit);
        ar(dabit_to_rebuild);
    }
    return DabitDTO<Share2DTO>::to_domain_model(dabit_to_rebuild);
}

template <class SBit> string Garbled_Circuit_Serializer<SBit>::serialize_dabit_to_base64(const Dabit<SBit> &dabit)
{
    stringstream output;
    dabit.bp_.output(output, true);
    string a = Base64_Encoder::encode(serialize_dabit_to_binary(dabit));
    Dabit<SBit> b = deserialize_dabit_from_binary(Base64_Encoder::decode(a));

    stringstream output2;
    b.bp_.output(output2, true);

    return Base64_Encoder::encode(serialize_dabit_to_binary(dabit));
}

template <class SBit> Dabit<SBit> Garbled_Circuit_Serializer<SBit>::deserialize_dabit_from_base64(const string &dabit)
{
    return deserialize_dabit_from_binary(Base64_Encoder::decode(dabit));
}

template class Garbled_Circuit_Serializer<aBit>;
template class Garbled_Circuit_Serializer<Share2>;

template class DabitDTO<aBitDTO>;
template class DabitDTO<Share2DTO>;
