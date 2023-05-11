#!/bin/bash

# Portably find the directory containing this script.
HERE=$(cd `dirname $0`; pwd)
# Assuming it lives in Scripts/, set ROOT to the root directory of the
# project.
ROOT=$HERE/../

OPTIONS="ONLY_ANDS ANDS_XORS";
echo "===== Welcome to the Circuit Generation for Offline GC =====";

#Decides to compile or not
echo "What would you like to do?";
echo "--> 1. Instantiate 1 Party";
echo "--> 2. Run all Parties";
read -p "Please Choose [Default 2]?: " party_flag;
party_flag=${party_flag:-2};
echo "The selected option is: $party_flag";  

compile_flag="n";

if [ $party_flag -eq 1 ];
then
    read -p "What party are you? [Default 0]: " party_number
    party_number=${party_number:-0}
    echo "The selected party is: $party_number";  

    if [ $party_number -eq 0 ];
    then
        compile_flag="y";
    fi;
else
    compile_flag="y";
fi;

cd $ROOT
if [ $compile_flag = "y" ];
then
    # Selects the circuit
    echo "What Circuit are you interested on using?";
    echo "We support";
    echo "--> ONLY_ANDS";
    echo "--> ANDS_XORS";
    read -p "Enter the name [Default ONLY_ANDS]: " circuit_id;
    circuit_id=${circuit_id:-ONLY_ANDS};
    echo "The selected option is: $circuit_id";  

    # Selects How Many 
    read -p "How Many Circuits [Default 100]?: " total;
    total=${total:-100}
    echo "The number of circuits is: $total"; 
    
    # Edits the python file
    echo "Changing and Compiling the Files now!";  
    sed -i "s/circuit_id_ = .*/circuit_id_ = Circuit.$circuit_id/" Programs/gc_gen/gc_gen.mpc   
    sed -i "s/total_ = .*/total_ = $total/" Programs/gc_gen/gc_gen.mpc     
    ./compile.sh Programs/gc_gen;
fi;

if [ $party_flag -eq 2 ]; 
then 
   echo "Executing in a Single Machine:";
   ./Scripts/run-online.sh Programs/gc_gen 
else
   echo "Executing Party $party_number:";
   ./Player.x $party_number Programs/gc_gen
fi;

echo "Thanks and Have a Nice Day!"