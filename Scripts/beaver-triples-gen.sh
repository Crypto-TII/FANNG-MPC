#!/bin/bash

# Portably find the directory containing this script.
HERE=$(cd `dirname $0`; pwd)
# Assuming it lives in Scripts/, set ROOT to the root directory of the
# project.
ROOT=$HERE/../

echo "===== Welcome to the Beaver Triples Generation program =====";
echo "";
echo "This program does a cool thing!"; 
echo "It helps you generate secret shares of random Beaver Triples.";
echo "You get to choose how many triples you want to generate.";
echo "These secret shares are super secure and get stored in each party's private database.";
echo "Neat, right? Let's get started ...";
echo "";

#Decides to compile or not
echo "What would you like to do?";
echo "--> 1. Instantiate 1 Party";
echo "--> 2. Run all Parties";
read -p "Please Choose [Default 2]?: " party_flag;
party_flag=${party_flag:-2};
echo "The selected option is: $party_flag";  
echo "";

compile_flag="n";

if [ $party_flag -eq 1 ];
then
    read -p "What party are you? [Default 0]: " party_number;
    party_number=${party_number:-0};
    echo "The selected party is: $party_number";  
    echo "";

    if [ $party_number -eq 0 ];
    then
        compile_flag="y";
    else
        # Decides whether to compile or not
        read -p "Would you like to compile (y/n)? [Default n]: " compile_flag;
        compile_flag=${compile_flag:-"n"};
        echo "The selected option is: $compile_flag";
        echo "";
    fi;
else
    compile_flag="y";
fi;

cd $ROOT
if [ $compile_flag = "y" ];
then
    echo "How many random Beaver triples you wanna generate?";
    echo "Each triple is of the form (a,b,c) with c = a * b.";
    echo "The most common choices is 100.";
    echo "";

    while true; do
        # Selects How Many 
        read -p "How many random Beaver triples needs to be generated [Default 100]?: " total;
        total=${total:-100};
        echo "The number of random Beaver triples generated will be: $total"; 
        echo "";
        
        # Client confirmation 
        echo "So, you want to generate $total random Beaver triples"; 
        read -p "Is this correct? (y/n): " confirm_msg;
        echo "";
        if [[ "$confirm_msg" == "y" || "$confirm_msg" == "Y" ]]; then
            break;
        else
            echo "No problem. Please enter the values once again.";
            echo "";
        fi
    done
    
    # Edits the python file
    echo "Changing and Compiling the Files now!";    
    sed -i "s/total_ = .*/total_ = $total/" Programs/beaver_triples_gen/beaver_triples_gen.mpc     
    ./compile.sh Programs/beaver_triples_gen;
fi;

if [ $party_flag -eq 2 ]; 
then 
   echo "Executing in a Single Machine:";
   ./Scripts/run-online.sh Programs/beaver_triples_gen; 
else
   echo "Executing Party $party_number:";
   ./Player.x $party_number Programs/beaver_triples_gen;
fi;

echo "";
echo "Thanks and Have a Nice Day!";