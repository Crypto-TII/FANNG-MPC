#!/bin/bash

# Portably find the directory containing this script.
HERE=$(cd `dirname $0`; pwd)
# Assuming it lives in Scripts/, set ROOT to the root directory of the
# project.
ROOT=$HERE/../

echo "===== Welcome to the Dabits Tuples Generation program =====";
echo "";
echo "This program does a cool thing!"; 
echo "It helps you generate secret shares of random Dabits Tuples.";
echo "You get to choose how many tuples you want to generate.";
echo "Either for Replicated or HSS17.. Whatever you have configured on SM.";
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
    echo "How many random Dabits Tuples you wanna generate?";
    echo "Each Tuple is of the form (b_p,b_2) with both being the same bit in Z_p and Z_2.";
    echo "The most common choices is 100.";
    echo "";

    while true; do
        # Selects How Many 
        read -p "How many random Dabit Tuples needs to be generated [Default 100]?: " total;
        total=${total:-100};
        echo "The number of random Dabits Tuples generated will be: $total"; 
        echo "";
        
        # Client confirmation
        echo "So, you want to generate $total random Dabit Tuples"; 
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
    sed -i "s/total_ = .*/total_ = $total/" Programs/dabits_tuples_gen/dabits_tuples_gen.mpc     
    ./compile.sh Programs/beaver_tuples_gen;
fi;

if [ $party_flag -eq 2 ]; 
then 
   echo "Executing in a Single Machine:";
   ./Scripts/run-online.sh Programs/dabits_tuples_gen; 
else
   echo "Executing Party $party_number:";
   ./Player.x $party_number Programs/dabits_tuples_gen;
fi;

echo "";
echo "Thanks and Have a Nice Day!";