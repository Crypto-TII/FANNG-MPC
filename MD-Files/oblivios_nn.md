# Oblivious NNs

Here we detail how to implement an oblivious neural network (ONN) to classify CIFAR-10 using FANNG-MPC libraries. The diagram below shows the NN blocks implemented. Note that the implementation of this networks in PyTorch can be found in [insert]-[link]

<div style = "text-align:center">
    <img src= "./images/CNN_CIFAR10.png" alt = "CNN architecture for CIFAR10" width ="40%" />
</div>

### import required libraries

```http
from Compiler import matrix_lib  
from Compiler import folding_lib   
from Compiler import relu_parallel_lib 
from Compiler import offline_triple_lib
```

The first library is used for convolutional layers, fully connected layers and input formatting. The second library contains folding layers and softmax (if needed). The third library provides ReLU activations.

### implement convolutional layer

The block below implements a convolutional layer with the following blocks:

 - Convolution
 - Batch Normalization
 - Activation (ReLU)
 - Pooling layer (optional block, according to the diagram above)

```http
def convolutional_block(X, K, Biases, Gamma, Beta, pooling, h, w, l, s, s_, kfh, kfw, debug=0):

#   CONVOLUTION:
#    h      -> feature height
#    w      -> feature width
#    l      -> padding
#    (2l+1) -> kernel height
#    (2l+1) -> kernel width
#    s      -> input channels
#    s_     -> output channels

#   FOLDING:
#    kfh   -> folding block height
#    kfw   -> folding block width

    # CONV. TRIPLE GENERATION
    stride = 1

    CONV_TRIPLE_TYPE = offline_triple_lib.NNTripleType(0, w, h, s, ((2*l)+1), ((2*l)+1), s_, stride, l)

    X_convolved = matrix_lib.conv3d_sfix2sfix(X, K, CONV_TRIPLE_TYPE, ((2*l)+1), ((2*l)+1), s, s_, h, w, 1, l)
    X_biased = matrix_lib.add_matrices(X_convolved, Biases)
    X_normalized = folding_lib.batch_normalization(X_biased, Gamma, Beta)
    X_activated = relu_parallel_lib.relu_2d_parallel(X_normalized)

    if pooling == "yes":
        stride = 2
        X_folded = folding_lib.folding(X_activated, kfh, kfw, stride, "avg_pool", h, w)
        return X_folded

    return X_activated
```

In our implementation, features are assumed to be 3D tensors, parametrized by channels (s), height (h) and width (w). Kernels are 4D tensors, parametrized by output channels (s_), input channels (s_), kernel hight (kh) and kernel width (kw).

In the above block, the Features and Kernels are passed as arguments in a matrix format (i.e. with only two dimensions). Specifically, the Feature matrix has dimension (h $\cdot$ w, s), hence each column contains one channel. The Kernel matrix has dimension (kh $\cdot$ kw, s $\cdot$ s_). Note that *matrix_lib* provides functions to reformat data in this fashion from 3D feature and 4D kernel tensors, with dimensions (s, h, w) and (s_, s, kh, kw) respectively:

```http
features2d = matrix_lib.rearrange_3d_features_into_2d_matrix(features3d)
kernels2d = matrix_lib.rearrange_4d_kernels_into_2d_matrix(kernels4d)
```

In this NN, the convolution applies padding according to the kernel size, so the output features keep the same h and w as the input features. Also, the kernel size is adjusted according to the padding: kh = kw = (2*padding)+1. 

The convolution 'Bias' is a 2D matrix of the same size as the output features, i.e. dimension (h $\cdot$ w, s_). It is worth clarifying that, usually, the same value is applied to each channel. In that case just replicate the value in all elements of each column.

The batch normalization factors 'Gamma' and 'Beta' follow the same approach as the Bias, they are 2D matrixes where each column replicates the same value in all elements. Note that Batch Normalization can come as Gamma, Beta, Mean, Variance and epsilon (numerical stability), in that case use the "less efficient" function *batch_normalization_trn* (we don't recommend this).

Finally, the folding layer needs the type "avg_pool" or "max_pool" (average pooling is faster since it does not require comparisons). The window size for pooling is defined by 'kfh' and 'kfw', height and width respectively.

Since this implementation is only for testing, no real convolutional triples are generated. We use the testing functionality to create convolutional triples types on demand "offline_triple_lib.NNTripleType()" and set the config.h accordingly to return matrix triples full of zeroes.


### implement faster convolutional layer with batched truncations

In the implementation above, after each convolution or folding layer there is a probabilistic truncation. The truncation resets the number of fractional bits after a multiplication.

Alternatively, we can skip truncation after convolution and folding, and do a larger truncation after both blocks are executed. Additionally, we can make truncation in parallel with the activation layer to save communication rounds.

```http
def convolutional_block_fast(X, K, Biases, Gamma, Beta, pooling, h, w, l, s, s_, kfh, kfw, scaling=0):

#   CONVOLUTION:
#    h      -> feature height
#    w      -> feature width
#    l      -> padding
#    (2l+1) -> kernel height
#    (2l+1) -> kernel width
#    s      -> input channels
#    s_     -> output channels

#   FOLDING:
#    kfh   -> folding block height
#    kfw   -> folding block width

    mode = matrix_lib.Trunc_Mode.OFF
    stride = 1

    CONV_TRIPLE_TYPE = offline_triple_lib.NNTripleType(0, w, h, s, ((2*l)+1), ((2*l)+1), s_, stride, l)

    scaling = scaling + 1 # convolution
    X_convolved = matrix_lib.conv3d_sfix2sfix(X, K, CONV_TRIPLE_TYPE, ((2*l)+1), ((2*l)+1), s, s_, h, w, stride, l, mode)
    Biases = matrix_lib.scale_matrix(Biases, scaling)
    X_biased = matrix_lib.add_matrices(X_convolved, Biases)
    scaling = scaling + 1 # multiplication by gamma
    Beta = matrix_lib.scale_matrix(Beta, scaling)
    X_normalized = folding_lib.batch_normalization(X_biased, Gamma, Beta, mode)
    X_activated = matrix_lib.truncate_sfix_matrix_plus_ReLU(X_normalized, scaling)

    if pooling == "yes":
        stride = 2
        padding = 0
        X_folded = folding_lib.folding(X_activated, kfh, kfw, stride, "avg_pool", h, w, padding, mode)
        return X_folded

    return X_activated
```

In the code above there are three main differences with respect to the previous definition of a convolutional block:

1) The functions "conv3d_sfix2sfix" and "folding" are called with mode = matrix_lib.Trunc_Mode.OFF. This mode excludes truncation.
2) Since the outputs of "conv3d_sfix2sfix" and "folding" are not truncated, the additions of Biases (after convolution) and Beta (after normalization) need to be scaled with function "scale_matrix"
3) The activation function also truncates as many levels as the variable "scaling" dictates.

This fast version of the convolutional block makes the ONN twice as fast when compared with the above block.

### implement the whole ONN:

The code for the whole ONN is implemented below, and it has the following arguments:

| arguments | description |
|--|--|
| X | input features - 3d tensor (type SFIX) |
| Y | input kernels/neurons - array of 4d tensors for convolutions and 2d matrices for FC layers (type SFIX) |
| Biases | input biases - array of 2d matrixes (type SFIX) |
| Gamma | input Gamma values for BN blocks - array of 2d matrixes (type SFIX) |
| Biases | input Beta values for BN blocks - array of 2d matrixes (type SFIX) |
| Triple | array with convolutional triples and matrix triples for convolutions and fc layers respectively matrixes (type SINT) |



```http
def oblivious_cnn_cifar10(X, Y, Biases, Gamma, Beta, Triple):  
  
    #####  REFORMAT INPUT FEATURES  
    X1_layer = matrix_lib.rearrange_3d_features_into_2d_matrix(X)  
  
    #####  REFORMAT KERNELS FOR CONVOLUTIONAL LAYERS
    K1_layer = matrix_lib.rearrange_4d_kernels_into_2d_matrix(Y[0])  
    K2_layer = matrix_lib.rearrange_4d_kernels_into_2d_matrix(Y[1])  
    K3_layer = matrix_lib.rearrange_4d_kernels_into_2d_matrix(Y[2])  
    K4_layer = matrix_lib.rearrange_4d_kernels_into_2d_matrix(Y[3])   
    K5_layer = matrix_lib.rearrange_4d_kernels_into_2d_matrix(Y[4])  
    K6_layer = matrix_lib.rearrange_4d_kernels_into_2d_matrix(Y[5])  
  
    #####  CONVOLUTIONAL LAYERS
    # def convolutional_block_fast(X, K, Biases, Gamma, Beta, pooling, h, w, l, s, s_, kfh, kfw, scaling=0)
  
    X2_layer = convolutional_block(X1_layer, K1_layer, Biases[0], Gamma[0], Beta[0], "no", 32, 32,  1,  3,  32,   0,   0)  
    X3_layer = convolutional_block(X2_layer, K2_layer, Biases[1], Gamma[1], Beta[1],"yes", 32, 32,  1, 32,  64,   2,   2)  
    X4_layer = convolutional_block(X3_layer, K3_layer, Biases[2], Gamma[2], Beta[2], "no", 16, 16,  1, 64, 128,   0,   0)  
    X5_layer = convolutional_block(X4_layer, K4_layer, Biases[3], Gamma[3], Beta[3],"yes", 16, 16,  1,128, 128,   2,   2)  
    X6_layer = convolutional_block(X5_layer, K5_layer, Biases[4], Gamma[4], Beta[4], "no",  8,  8,  1,128, 256,   0,   0)  
    X7_layer = convolutional_block(X6_layer, K6_layer, Biases[5], Gamma[5], Beta[5],"yes",  8,  8,  1,256, 256,   2,   2)  
    
    #####  NEURON WEIGHTS for FC LAYERS
    FC1_layer = Y[6]
    FC2_layer = Y[7]

    #####  MATRIX TRIPLES FOR FC LAYERS
    M_TRIPLE_TYPE_L7 = offline_triple_lib.TripleType(0, 1, len(FC1_layer), len(FC1_layer), len(FC1_layer[0]), 1, len(FC1_layer[0]))
    M_TRIPLE_TYPE_L8 = offline_triple_lib.TripleType(0, 1, len(FC2_layer), len(FC2_layer), len(FC2_layer[0]), 1, len(FC2_layer[0]))
    
    #####  REFORMAT INPUT FOR FULLY CONNECTED LAYER
    X7_flattened = matrix_lib.flatten_to_rowmatrix(X7_layer)
    
    #####  FC LAYER 1     
    X7_fully_connected = matrix_lib.multmat_sfix2sfix(X7_flattened, FC1_layer, M_TRIPLE_TYPE_L7, matrix_lib.Trunc_Mode.ON)
    B7 = matrix_lib.scale_matrix(Biases[6], scaling)
    X7_biased = matrix_lib.add_matrices(X7_fully_connected, B7)
    X8_layer = matrix_lib.relu_parallel_lib.relu_2d_parallel(X7_biased)

    #####  FC LAYER 2
    X8_fully_connected = matrix_lib.multmat_sfix2sfix(X8_layer, FC2_layer, M_TRIPLE_TYPE_L8, matrix_lib.Trunc_Mode.ON)
    X9_layer = matrix_lib.add_matrices(X8_fully_connected, Biases[7])

    ### SOFTMAX
    output = folding_lib.softmax_scaled(X9_layer, -15, True)

    return output
```

If we implement the ONN using the fast convolutional block, then we have:

```http
def oblivious_cnn_cifar10(X, Y, Biases, Gamma, Beta, Triple):  
  
    #####  REFORMAT INPUT FEATURES  
    X1_layer = matrix_lib.rearrange_3d_features_into_2d_matrix(X)  
  
    #####  REFORMAT KERNELS FOR CONVOLUTIONAL LAYERS
    K1_layer = matrix_lib.rearrange_4d_kernels_into_2d_matrix(Y[0])  
    K2_layer = matrix_lib.rearrange_4d_kernels_into_2d_matrix(Y[1])  
    K3_layer = matrix_lib.rearrange_4d_kernels_into_2d_matrix(Y[2])  
    K4_layer = matrix_lib.rearrange_4d_kernels_into_2d_matrix(Y[3])   
    K5_layer = matrix_lib.rearrange_4d_kernels_into_2d_matrix(Y[4])  
    K6_layer = matrix_lib.rearrange_4d_kernels_into_2d_matrix(Y[5])  
  
    #####  CONVOLUTIONAL LAYERS
    # def convolutional_block_fast(X, K, Biases, Gamma, Beta, pooling, h, w, l, s, s_, kfh, kfw, scaling=0)
    
    X2_layer = convolutional_block_fast(X1_layer, K1_layer, Biases[0], Gamma[0], Beta[0], "no",  32, 32,  1,   3,  32,   0,   0)
    X3_layer = convolutional_block_fast(X2_layer, K2_layer, Biases[1], Gamma[1], Beta[1], "yes", 32, 32,  1,  32,  64,   2,   2)
    X4_layer = convolutional_block_fast(X3_layer, K3_layer, Biases[2], Gamma[2], Beta[2], "no",  16, 16,  1,  64, 128,   0,   0, scale_up)
    X5_layer = convolutional_block_fast(X4_layer, K4_layer, Biases[3], Gamma[3], Beta[3], "yes", 16, 16,  1, 128, 128,   2,   2)
    X6_layer = convolutional_block_fast(X5_layer, K5_layer, Biases[4], Gamma[4], Beta[4], "no",   8,  8,  1, 128, 256,   0,   0, scale_up)
    X7_layer = convolutional_block_fast(X6_layer, K6_layer, Biases[5], Gamma[5], Beta[5], "yes",  8,  8,  1, 256, 256,   2,   2)
    
    #####  NEURON WEIGHTS for FC LAYERS
    FC1_layer = Y[6]
    FC2_layer = Y[7]

    #####  MATRIX TRIPLES FOR FC LAYERS
    M_TRIPLE_TYPE_L7 = offline_triple_lib.TripleType(0, 1, len(FC1_layer), len(FC1_layer), len(FC1_layer[0]), 1, len(FC1_layer[0]))
    M_TRIPLE_TYPE_L8 = offline_triple_lib.TripleType(0, 1, len(FC2_layer), len(FC2_layer), len(FC2_layer[0]), 1, len(FC2_layer[0]))
    
    #####  REFORMAT INPUT FOR FULLY CONNECTED LAYER
    X7_flattened = matrix_lib.flatten_to_rowmatrix(X7_layer)
    
    #####  FC LAYER 1     
    X7_fully_connected = matrix_lib.multmat_sfix2sfix(X7_flattened, FC1_layer, M_TRIPLE_TYPE_L7, matrix_lib.Trunc_Mode.OFF)
    scaling = 2 # [ previous folding + multmat ]
    B7 = matrix_lib.scale_matrix(Biases[6], scaling)
    X7_biased = matrix_lib.add_matrices(X7_fully_connected, B7)
    X8_layer = matrix_lib.truncate_sfix_matrix_plus_ReLU(X7_biased, scaling)

    #####  FC LAYER 2
    X8_fully_connected = matrix_lib.multmat_sfix2sfix(X8_layer, FC2_layer, M_TRIPLE_TYPE_L8, matrix_lib.Trunc_Mode.ON)
    X9_layer = matrix_lib.add_matrices(X8_fully_connected, Biases[7])

    ### SOFTMAX
    output = folding_lib.softmax_scaled(X9_layer, -15, True)

    return output
```

Note that in the code above, also the FC layers use the functionality of batching several truncation in the activation layer.


### importing features and parameters:

The NN parameters (kernels, biases, etc.) can be stored in a database, and imported using I/O functionality as described below. Note that, the values are imported as SINT and stored as SFIX form, so we need to be sure that the values in the database follow the same format as our SFIX configuration regarding the number of bits representing fractional and integer parts.

[insert]-[explanation]

```http 
def read_values(number_of_values, channel):  

    open_channel_with_return(channel)  
    a = sint.get_private_input_from(0, channel, size=number_of_values)  
    close_channel(channel)  
    
    A = Array(number_of_values, sfix)  
    vstms(number_of_values, a, A.address)  
      
    return A
```

Values are read as an Array. Formatting has to be done accordingly. The code below reads kernels as 4d tensors with dimension (s_, s, kh, kw). (Note that the values could be read other formats to avoid reformatting, e.g. *rearrange_4d_kernels_into_2d_matrix*).

```http 
def get_kernels(kw, kh, s, s_, layer, channel):  
    Kernels = []  
  
    number_of_values = kh * kw * s * s_    
    A = read_values(number_of_values, channel)  
    index_of_A = 0  
  
    for k_ in range(s_):  
        out_channel = []  
        Kernels.append(out_channel)  
        for k in range(s):  
            Kernel = sfix.Matrix(kh, kw)  
            for i in range(kh):  
                for j in range(kw):  
                    Kernel[i][j] = A[index_of_A]  
                    index_of_A += 1  
            Kernels[k_].append(Kernel)  
  
    return Kernels
```

The parameters for the Fully Connected layer are obtained in a similar fashion. It is worth mentioning that we train our CNN with PyTorch, and the parameters are sometimes transposed with respect to our implementation: 

```http 
def get_fc_matrix(h, w, layer, channel):  
    FC = sfix.Matrix(w, h)  
  
    fc_layer_weights = h * w   
    FC_array = read_values(fc_layer_weights, channel)  
    array_index = 0  
  
    for i in range(w):  
        for j in range(h):  
            FC[i][j] = FC_array[array_index]  
            array_index += 1  
  
    FC_tr = matrix_lib.traspose(FC)  
  
    return FC_tr
```

To call the function implementation the ONN, we need to pass as argument an array with the kernels of all layers. Note that we read the parameters using a different channels per layer for convenience:

```http 
def generate_neurons_for_pruned_resnet():  
    K = []  
  
    K1 = get_kernels(3, 3, 3, 32, 1, 100040)  
    K2 = get_kernels(3, 3, 32, 64, 2, 100041)  
    K3 = get_kernels(3, 3, 64, 128, 3, 100042)  
    K4 = get_kernels(3, 3, 128, 128, 4, 100043)  
    K5 = get_kernels(3, 3, 128, 256, 5, 100044)  
    K6 = get_kernels(3, 3, 256, 256, 6, 100045)  
  
    FC1 = get_fc_matrix(4096, 32, 7, 100046)  
    FC2 = get_fc_matrix(32, 10, 8, 100047)  
  
    K.append(K1)  
    K.append(K2)  
    K.append(K3)  
    K.append(K4)  
    K.append(K5)  
    K.append(K6)  
    K.append(FC1)  
    K.append(FC2)  
  
    return K
```

Similarly, values for biases are read and formatted from an Array. Note that each output channel is applied the same bias value, so we must replicate the value in all rows of each column:

```http 
def generate_biases_for_cnn_cifar10():  
    B = []  
  
    B1 = sfix.Matrix(32 * 32, 32)  
    B2 = sfix.Matrix(32 * 32, 64)  
    B3 = sfix.Matrix(16 * 16, 128)  
    B4 = sfix.Matrix(16 * 16, 128)  
    B5 = sfix.Matrix(8 * 8, 256)  
    B6 = sfix.Matrix(8 * 8, 256)  
    B7 = sfix.Matrix(1, 32)  
    B8 = sfix.Matrix(1, 10)  
  
    biases = read_values(32, 100020)  
    fill_matrix_with_value_per_column(B1, biases)  
  
    biases = read_values(64, 100021)  
    fill_matrix_with_value_per_column(B2, biases)  
  
    biases = read_values(128, 100022)  
    fill_matrix_with_value_per_column(B3, biases)  
  
    biases = read_values(128, 100023)  
    fill_matrix_with_value_per_column(B4, biases)  
  
    biases = read_values(256, 100024)  
    fill_matrix_with_value_per_column(B5, biases)  
  
    biases = read_values(256, 100025)  
    fill_matrix_with_value_per_column(B6, biases)  
  
    ### FC LAYERS  
  
    biases = read_values(32, 100026)  
    fill_matrix_with_value_per_column(B7, biases)  
  
    biases = read_values(10, 100027)  
    fill_matrix_with_value_per_column(B8, biases)  
  
    B.append(B1)  
    B.append(B2)  
    B.append(B3)  
    B.append(B4)  
    B.append(B5)  
    B.append(B6)  
    B.append(B7)  
    B.append(B8)  
  
    return B
```

Values for Gamma and Beta are similar to Bias.

### testing outputs

The correctness of the outputs of our ONN can be tested. For that we compare with the results obtained in PyTorch. The function *matrix_lib.compare_with_matrix_from_file* lets you compare the output of each layer with the outputs of PyTorch, provided they were previously stored in a file. In the code below we compare the output of each layer in our ONN with the reference values we obtained in PyTorch:

```http 
def test_all_layers_for_cnn_cifar10(layers_execution_result, execution_name):  
    X2_layer, X3_layer, X4_layer, X5_layer, X6_layer, X7_layer, X8_layer, X9_layer = layers_execution_result  
  
  
    X2_transpose = matrix_lib.traspose(X2_layer) ## or maybe X_biased  
    comparison_result = matrix_lib.compare_with_matrix_from_file(X2_transpose, "Data/Data/Outputs/" + execution_name + "/P0-ReLU_layer_3.output.txt", 64)  
    test(comparison_result, 1)  
  
    X3_transpose = matrix_lib.traspose(X3_layer)  
    comparison_result = matrix_lib.compare_with_matrix_from_file(X3_transpose, "Data/Data/Outputs/" + execution_name + "/P0-Avg_pool_layer_8.output.txt", 64)  
    test(comparison_result, 1)  
  
    X4_transpose = matrix_lib.traspose(X4_layer)  
    comparison_result = matrix_lib.compare_with_matrix_from_file(X4_transpose, "Data/Data/Outputs/" + execution_name + "/P0-ReLU_layer_11.output.txt", 64)  
    test(comparison_result, 1)  
  
    X5_transpose = matrix_lib.traspose(X5_layer)  
    comparison_result = matrix_lib.compare_with_matrix_from_file(X5_transpose, "Data/Data/Outputs/" + execution_name + "/P0-Avg_pool_layer_16.output.txt", 64)  
    test(comparison_result, 1)  
  
    X6_transpose = matrix_lib.traspose(X6_layer)  
    comparison_result = matrix_lib.compare_with_matrix_from_file(X6_transpose, "Data/Data/Outputs/" + execution_name + "/P0-ReLU_layer_19.output.txt", 64)    
    test(comparison_result, 1)  
  
    X7_transpose = matrix_lib.traspose(X7_layer)  
    comparison_result = matrix_lib.compare_with_matrix_from_file(X7_transpose, "Data/Data/Outputs/" + execution_name + "/P0-Avg_pool_layer_24.output.txt", 64)   
    test(comparison_result, 1)  
  
    X8_transpose = matrix_lib.traspose(X8_layer)  
    comparison_result = matrix_lib.compare_with_matrix_from_file(X8_transpose, "Data/Data/Outputs/" + execution_name + "/P0-ReLU_layer_28.output.txt", 128)  
    test(comparison_result, 1)  
  
    X9_transpose = matrix_lib.traspose(X9_layer)  
    comparison_result = matrix_lib.compare_with_matrix_from_file(X9_transpose, "Data/Data/Outputs/" + execution_name + "/P0-Linear_layer_29.output.txt", 512)  
    test(comparison_result, 1)
```

The values '64', '128' and '512' is the 'tolerance' required in the internal call to *matrix_lib.sfix_matrix_equals*. It sets a tolerance in the precession offset when comparing SFIX values. It can be set different in each layer. Our SFIX configuration has 20bits precision by default, so a tolerance of 64 n the value of the mantissa (i.e. 6 bits) is quite relaxed. This tolerance is high due to the approximated values used for Batch Normalization (i.e. the Gamma and Beta factors already include the variance, mean and numerical stability).

### How to compile and execute:

To compile you can run the script below from your FANNG-MPC main folder (so use the correct path). Point to the folder of the mpc program but not to the mpc file itself. Don't forget the "-O1" flag or it will never end.

```bash
./compile.sh -O1 [path]/test_obliv_nn_pruned_resnet
```

Then to execute the line below. The multiple "i i i ..." are an indicator to the script that the multiple channels opened are for "input":

```bash
./Scripts/run-online-interactive-with-answers.sh "i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i" [path]/test_obliv_nn_pruned_resnet
```

We also have scripts to do this faster: 

```bash
./run-lenet.sh
./run-pruned-resnet.sh
```

These scripts require having the data in the folder Data/PrunedResnetData and Data/LenetData. Inside those folders you should have, three folders containing "Inputs", "Outputs" and "all_parameters".
