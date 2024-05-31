/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#ifndef GC_TEST_UTILS_H
#define GC_TEST_UTILS_H

gf2n create_gf2n()
{
    auto value= gf2n();
    value.assign_one();
    return value;
}

aBit create_aBit()
{
    auto value= aBit();
    value.assign_zero();
    return value;
}

Base_Garbled_Circuit create_garbled_circuit()
{
    vector<aBit> lambda= {create_aBit()};
    vector<vector<gf2n>> k= {{create_gf2n()}};
    vector<vector<vector<vector<gf2n>>>> gab= {{{{create_gf2n()}}}};
    vector<gf2n> one_label= {create_gf2n()};
    Base_Garbled_Circuit base_garbled_circuit;
    base_garbled_circuit.set_mmo(MMO());
    base_garbled_circuit.set_lambda(lambda);
    base_garbled_circuit.set_k(k);
    base_garbled_circuit.set_gab(gab);
    base_garbled_circuit.set_one_label(one_label);
    return base_garbled_circuit;
}

#endif //GC_TEST_UTILS_H
