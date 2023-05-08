# What is this repository for? #

This repository contains a LeNet implementation with ReLUs (instead of Tanh) and two types of pooling layers.
The code is a modification of the one published here: https://towardsdatascience.com/implementing-yann-lecuns-lenet-5-in-pytorch-5e05a0911320

# Requirements #
All the requirements to get the project running have been mentioned in the `Lenet/requirements.txt` file. The Python version required is `3.8.15`.

# How to run the project? #

## Following are the steps to run the project (perform training and inference using PyTorch): ###

* Open the Jupyter notebook `Lenet/LeNet_jupiter.ipynb`.
* Make sure you have installed all the requirements from `Lenet/requirements.txt` and you have Python version `3.8.15` or newer.
* Run the cells of the Jupyter notebook one by one.
<br>

## Parameters

Range of channels reserved: **100000 - 100019** 

Channels in use:

- LenetData/Parameters/Default-parameters/P0-classifier.0.bias.txt
- LenetData/Parameters/Default-parameters/P0-classifier.0.weight.txt
- LenetData/Parameters/Default-parameters/P0-classifier.2.bias.txt
- LenetData/Parameters/Default-parameters/P0-classifier.2.weight.txt
- LenetData/Parameters/Default-parameters/P0-feature_extractor.0.bias.txt
- LenetData/Parameters/Default-parameters/P0-feature_extractor.0.weight.txt
- LenetData/Parameters/Default-parameters/P0-feature_extractor.3.bias.txt
- LenetData/Parameters/Default-parameters/P0-feature_extractor.3.weight.txt
- LenetData/Parameters/Default-parameters/P0-feature_extractor.6.bias.txt
- LenetData/Parameters/Default-parameters/P0-feature_extractor.6.weight.txt

## First layer inputs
Range of channels reserved: **90000 - 90049** 

- 90000 → execution 1 ./LenetData/Inputs/Execution_0/P0-feature_extractor_0.input.txt
- 90001 → execution 2 ./LenetData/Inputs/Execution_1/P0-feature_extractor_0.input.txt
- 90002 → execution 3 ./LenetData/Inputs/Execution_2/P0-feature_extractor_0.input.txt

## How to insert neural network parameters in database
To insert neural network parameters, a script can be found in Scripts directory `insert-parameters-and-inputs.sh`. 
Mysql client is required to be installed in your machine and setting up the option to share files with 
the server by using *SET GLOBAL local_infile = 1*;