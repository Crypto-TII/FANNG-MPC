# TII_ObliviousNN

This README would normally document whatever steps are necessary to get your application up and running.

## Matrix triples
By default, these sample programs use generated convolutional triples with 0 as values for simplicity. A program called `matrix_triples_generation.mpc` is provided as an example of
how real triples can be generated and stored using the [Input/Output system](../MD-Files/input-output.md).

## Running the Oblivious Neural Networks  
Lenet and Pruned Resnet can be executed in the traditional FANNG-MPC way and using any available Input/Output system. 
However, some scripts have been created to make them easier to run using the Input/Output filesystem without having to answer each question about file paths manually.


Lenet and Pruned Resnet can be easily executed using the `run-lenet.sh` and `run-pruned-resnet.sh` in this way:

* `./run-lenet <FANNG-MPC main folder path> <path_to_lenet_relative_to_FANNG-MPC>`. 
    
    Example: `./run-lenet FANNG-MPC ../test_obliv_nn_lenet`

The files generated from the pytorch code with the parameters, input and output files must be located inside the FANNG-MPC Data folder with **LenetData** and **PrunedResnetData** names respectively.
`generate-lenet-answers-for-io-fs.sh` and `generate-pruned-resnet-answers-for-io-fs.sh` scripts are responsible for generating responses 
necessary to use the input/output filesystem when LeNet and Pruned Resnet are respectively executed using those files. 
If you want to use a custom data structure for these files, you have to change the paths of them inside these scripts.


## Communications channels in use

#### Parameters

Range of channels reserved: **100000 - 100019**

Channels in use:

- 100000 → LenetData/Parameters/Default-parameters/P0-classifier.0.bias.txt
- 100001 → LenetData/Parameters/Default-parameters/P0-classifier.0.weight.txt
- 100002 → LenetData/Parameters/Default-parameters/P0-classifier.2.bias.txt
- 100003 → LenetData/Parameters/Default-parameters/P0-classifier.2.weight.txt
- 100004 → LenetData/Parameters/Default-parameters/P0-feature_extractor.0.bias.txt
- 100005 → LenetData/Parameters/Default-parameters/P0-feature_extractor.0.weight.txt
- 100006 → LenetData/Parameters/Default-parameters/P0-feature_extractor.3.bias.txt
- 100007 → LenetData/Parameters/Default-parameters/P0-feature_extractor.3.weight.txt
- 100008 → LenetData/Parameters/Default-parameters/P0-feature_extractor.6.bias.txt
- 100009 → LenetData/Parameters/Default-parameters/P0-feature_extractor.6.weight.txt

#### First layer input

Range of channels reserved: **90000 - 90049**

- 90000 → execution 1 ./LenetData/Inputs/Execution_0/P0-feature_extractor_0.input.txt
- 90001 → execution 2 ./LenetData/Inputs/Execution_1/P0-feature_extractor_0.input.txt
- 90002 → execution 3 ./LenetData/Inputs/Execution_2/P0-feature_extractor_0.input.txt


#### Parameters
Range of channels reserved: **100020 - 100049**

Channels in use:

##### Bias
- 100020 → PrunedResnetData/all_parameters/bias/P0-conv_layer_1_bias.txt
- 100021 → PrunedResnetData/all_parameters/bias/P0-conv_layer_2_bias.txt
- 100022 → PrunedResnetData/all_parameters/bias/P0-conv_layer_3_bias.txt
- 100023 → PrunedResnetData/all_parameters/bias/P0-conv_layer_4_bias.txt
- 100024 → PrunedResnetData/all_parameters/bias/P0-conv_layer_5_bias.txt
- 100025 → PrunedResnetData/all_parameters/bias/P0-conv_layer_6_bias.txt
- 100026 → PrunedResnetData/all_parameters/bias/P0-linear_layer_1_bias.txt
- 100027 → PrunedResnetData/all_parameters/bias/P0-linear_layer_2_bias.txt
##### Beta
- 100028 → PrunedResnetData/all_parameters/new_beta/P0-bn_layer_1_new_beta.txt
- 100029 → PrunedResnetData/all_parameters/new_beta/P0-bn_layer_2_new_beta.txt
- 100030 → PrunedResnetData/all_parameters/new_beta/P0-bn_layer_3_new_beta.txt
- 100031 → PrunedResnetData/all_parameters/new_beta/P0-bn_layer_4_new_beta.txt
- 100032 → PrunedResnetData/all_parameters/new_beta/P0-bn_layer_5_new_beta.txt
- 100033 → PrunedResnetData/all_parameters/new_beta/P0-bn_layer_6_new_beta.txt
##### Gamma
- 100034 → PrunedResnetData/all_parameters/new_gamma/P0-bn_layer_1_new_gamma.txt
- 100035 → PrunedResnetData/all_parameters/new_gamma/P0-bn_layer_2_new_gamma.txt
- 100036 → PrunedResnetData/all_parameters/new_gamma/P0-bn_layer_3_new_gamma.txt
- 100037 → PrunedResnetData/all_parameters/new_gamma/P0-bn_layer_4_new_gamma.txt
- 100038 → PrunedResnetData/all_parameters/new_gamma/P0-bn_layer_5_new_gamma.txt
- 100039 → PrunedResnetData/all_parameters/new_gamma/P0-bn_layer_6_new_gamma.txt
##### Weights
- 100040 → PrunedResnetData/all_parameters/weights/P0-conv_layer_1_weight.txt
- 100041 → PrunedResnetData/all_parameters/weights/P0-conv_layer_2_weight.txt
- 100042 → PrunedResnetData/all_parameters/weights/P0-conv_layer_3_weight.txt
- 100043 → PrunedResnetData/all_parameters/weights/P0-conv_layer_4_weight.txt
- 100044 → PrunedResnetData/all_parameters/weights/P0-conv_layer_5_weight.txt
- 100045 → PrunedResnetData/all_parameters/weights/P0-conv_layer_6_weight.txt
- 100046 → PrunedResnetData/all_parameters/weights/P0-linear_layer_1_weight.txt
- 100047 → PrunedResnetData/all_parameters/weights/P0-linear_layer_2_weight.txt
#### First layer input
Range of channels reserved: **90050 - 90059**
- 90050 → execution 1 ./PrunedResnetData/Inputs/Execution_0/P0-Conv_layer_1.input.txt
