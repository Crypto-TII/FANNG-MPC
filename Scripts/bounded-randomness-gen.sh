#!/bin/bash

# Portably find the directory containing this script.
HERE=$(cd `dirname $0`; pwd)
# Assuming it lives in Scripts/, set ROOT to the root directory of the
# project.
ROOT=$HERE/../

echo "===== Welcome to the Bounded Randomness Generation program =====";
echo "";
echo "This program does a cool thing!"; 
echo "It helps you generate secret shares of random values.";
echo "You get to choose how many values you want and set a limit for them.";
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
    echo "What's the bound for the random value you wanna generate?";
    echo "The value generated will be smaller than 2 to the power of the bound.";
    echo "The most common choices is 40.";
    echo "";

    while true; do
        # Selects the bound
        read -p "Enter the bound [Default 40]: " bound;
        bound=${bound:-40};
        echo "The selected bound is: $bound";  
        echo "";

        # Selects How Many 
        read -p "How many random values needs to be generated [Default 100]?: " total;
        total=${total:-100};
        echo "The number of random values generated will be: $total"; 
        echo "";
        
        # Client confirmation 
        echo "So, you want to generate $total random values with the bound $bound"; 
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
    sed -i "s/bound_ = .*/bound_ = $bound/" Programs/bounded_randomness_gen/bounded_randomness_gen.mpc   
    sed -i "s/total_ = .*/total_ = $total/" Programs/bounded_randomness_gen/bounded_randomness_gen.mpc     
    ./compile.sh Programs/bounded_randomness_gen;
fi;

if [ $party_flag -eq 2 ]; 
then 
   echo "Executing in a Single Machine:";
   ./Scripts/run-online.sh Programs/bounded_randomness_gen; 
else
   echo "Executing Party $party_number:";
   ./Player.x $party_number Programs/bounded_randomness_gen;
fi;

echo "";
echo "Thanks and Have a Nice Day!";