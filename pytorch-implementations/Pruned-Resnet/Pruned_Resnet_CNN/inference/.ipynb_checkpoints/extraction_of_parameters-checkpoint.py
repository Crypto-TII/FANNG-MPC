# Imports
import torch
import torch.nn as nn
import matplotlib
import matplotlib.pyplot as plt
import os
import numpy as np
get_ipython().run_line_magic('matplotlib', 'inline')


# CNN architecture
class Cifar10CnnModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.network = nn.Sequential(
            
            # Conv-1
            nn.Conv2d(3, 32, kernel_size=3, padding=1),
            nn.BatchNorm2d(32),
            nn.ReLU(),
            nn.Dropout(0.25),
            
            # Conv-2
            nn.Conv2d(32, 64, kernel_size=3, stride=1, padding=1),
            nn.BatchNorm2d(64),
            nn.ReLU(),
            nn.AvgPool2d(2, 2), # output: 64 x 16 x 16
            
            # Conv-3
            nn.Conv2d(64, 128, kernel_size=3, stride=1, padding=1),
            nn.BatchNorm2d(128),
            nn.ReLU(),
            nn.Dropout(0.25),

            # Conv-4
            nn.Conv2d(128, 128, kernel_size=3, stride=1, padding=1),
            nn.BatchNorm2d(128),
            nn.ReLU(),
            nn.AvgPool2d(2, 2), # output: 128 x 8 x 8

            # Conv-5
            nn.Conv2d(128, 256, kernel_size=3, stride=1, padding=1),
            nn.BatchNorm2d(256),
            nn.ReLU(),
            nn.Dropout(0.25),            
            
            # Conv-6
            nn.Conv2d(256, 256, kernel_size=3, stride=1, padding=1),
            nn.BatchNorm2d(256),           
            nn.ReLU(),
            nn.AvgPool2d(2, 2), # output: 256 x 4 x 4

            nn.Flatten(), 
            nn.Linear(256*4*4, 32),
            nn.Dropout(0.25),            
            nn.ReLU(),
            nn.Linear(32, 10))
        
    def forward(self, xb):
        return self.network(xb)


# Loading saved model
saved_model = Cifar10CnnModel()
saved_model.load_state_dict(torch.load('../saved_model/cifar10-cnn_with_bn_and_avg_pool.pth', map_location='cpu'))
saved_model.eval()


# Helper functions
def create_if_not_exists(base_path, name_of_folder=None):
    path = base_path + "/" + name_of_folder + "/"
    try:
        if name_of_folder != None:
            os.makedirs(base_path + "/{}".format(name_of_folder))
        else:
            os.makedirs(base_path)
    except FileExistsError:
        pass
    
    return path

def write_tensor_to_file(tensor, file_path, name_of_file):
    text_file = open(file_path + "/{}".format(name_of_file), "w+")
    tensor.data.numpy().tofile(text_file, sep=" ", format="%.8f")
    text_file.close()


# Convolutional layers

### Collecting all convolutional layers parameters (weight and bias)

#### Storing each kernel weight
conv_weight_list = []

for i in range(0, 24, 4):
    conv_weight_list.append("################# LAYER #################")
    conv_weight_list.append(saved_model.network[i].weight)


#### Storing each kernel bias
conv_bias_list = []

for i in range(0, 24, 4):
    conv_bias_list.append("################# LAYER #################")
    conv_bias_list.append(saved_model.network[i].bias)


### Creating a directory to store weights and biases parameters
path_where_all_parameters_be_stored = "./Data/all_parameters"

create_if_not_exists("./Data", "all_parameters")


## Creating a directory to store weights
weight_path = create_if_not_exists(path_where_all_parameters_be_stored, "weights")


weight_path


### Saving convolutional layer's kernel weights
layer = 1
for i in range(0,len(conv_weight_list)):
    if type(conv_weight_list[i]) != str:
        conv_weight_tensor = conv_weight_list[i]
        write_tensor_to_file(conv_weight_tensor, weight_path, "P0-conv_layer_{}_weight.txt".format(layer))
        layer += 1


## Creating a directory to store biases
bias_path = create_if_not_exists(path_where_all_parameters_be_stored, "bias")

bias_path


### Saving convolutional layer's biases
layer = 1
for i in range(0,len(conv_bias_list)):
    if type(conv_bias_list[i]) != str:
        conv_bias_tensor = conv_bias_list[i]
        write_tensor_to_file(conv_bias_tensor, bias_path, "P0-conv_layer_{}_bias.txt".format(layer))
        layer += 1


# Batch normalization layers

## Getting mean
mean_list = []

layer = 1
for i in range (1, 7):
    print("Layer: ", layer)
    mean = saved_model.network[layer].running_mean
    print("Mean: \n", mean)
    mean_list.append(mean.tolist())
    layer += 4


## Getting variance
var_list = []

layer = 1
for i in range (1, 7):
    print("Layer: ", layer)
    var = saved_model.network[layer].running_var
    print("Variance: \n", var)
    var_list.append(var.tolist())
    layer += 4


## Getting old gamma
old_gamma_list = []

layer = 1
for i in range (1, 7):
    print("Layer: ", layer)
    old_gamma = saved_model.network[layer].weight
    print("Gamma: \n", old_gamma)
    old_gamma_list.append(old_gamma.tolist())
    layer += 4


## Getting old beta
old_beta_list = []

layer = 1
for i in range (1, 7):
    print("Layer: ", layer)
    old_beta = saved_model.network[layer].bias
    print("Beta: \n", old_beta)
    old_beta_list.append(old_beta.tolist())
    layer += 4


## Getting modified parameters

### Formula to calculate new gamma and new beta
def new_gamma_formula(old_gamma, var):
    std_dev = var ** 0.5
    new_gamma = old_gamma / std_dev
    return new_gamma

def new_beta_formula(old_gamma, mean, var, beta):
    std_dev = var ** 0.5
    new_beta = -((old_gamma * mean) / (std_dev)) + beta
    return new_beta


### New gamma
new_gamma_list = []

for i in range(0,6):
    old_gamma_whole_list = old_gamma_list[i]
    var_whole_list = var_list[i]
    # print(old_gamma_whole_list)
    # print("\n",var_whole_list)
    # break
    temp = []
    if len(old_gamma_whole_list) != len(var_whole_list):
        print("Different length!")
    else:
        for j in range(0,len(old_gamma_whole_list)):
            old_gamma = old_gamma_whole_list[j]
            var = var_whole_list[j]
            # print("old_gamma: ", old_gamma)
            # print("var: ",var)
            # break
            new_gamma = new_gamma_formula(old_gamma, var)
            temp.append(new_gamma)
        new_gamma_list.append(temp)


### New beta
new_beta_list = []

for i in range(0,6):
    old_gamma_whole_list = old_gamma_list[i]
    var_whole_list = var_list[i]
    mean_whole_list = mean_list[i]
    old_beta_whole_list = old_beta_list[i]
    # print(old_gamma_whole_list)
    # print("\n",var_whole_list)
    # break
    temp = []
    if len(old_gamma_whole_list) != len(var_whole_list):
        print("Different length!")
    else:
        for j in range(0,len(old_gamma_whole_list)):
            old_gamma = old_gamma_whole_list[j]
            var = var_whole_list[j]
            mean = mean_whole_list[j]
            beta = old_beta_whole_list[j]
            # print("old_gamma: ", old_gamma)
            # print("var: ",var)
            # break
            new_beta = new_beta_formula(old_gamma, mean, var, beta)
            temp.append(new_beta)
        new_beta_list.append(temp)


## Function to save new gamma and new beta
def write_to_text_file(arr, path, file_name):
    # Converting to 1 dimensional array
    elements_of_new_arr = []
    for i in range (0, len(arr)):
        elements_of_new_arr.append(arr[i])
        
    # Converting to tensor
    torch_tensor = torch.tensor(elements_of_new_arr)
    
    # Writing to text file
    write_tensor_to_file(torch_tensor, path, file_name)


### Creating a directory to store new gamma 
new_gamma_path = create_if_not_exists(path_where_all_parameters_be_stored, "new_gamma")

new_gamma_path


## Saving new gamma
layer = 1
for i in range(0,6):
    write_to_text_file(new_gamma_list[i], new_gamma_path, "P0-bn_layer_{}_new_gamma.txt".format(layer))
    layer += 1


### Creating a directory to store new beta
new_beta_path = create_if_not_exists(path_where_all_parameters_be_stored, "new_beta")


new_beta_path


## Saving new beta
layer = 1
for i in range(0,6):
    write_to_text_file(new_beta_list[i], new_beta_path, "P0-bn_layer_{}_new_beta.txt".format(layer))
    layer += 1


# Linear layers (fully connected layers)

### Getting weights of the linear layer
linear_weight_list = []

# 1st linear layer
linear_weight_list.append("################# LAYER #################")
linear_weight_list.append(saved_model.network[25].weight)

# 2nd linear layer
linear_weight_list.append("################# LAYER #################")
linear_weight_list.append(saved_model.network[28].weight)


### Getting bias of the linear layer
linear_bias_list = []

# 1st linear layer
linear_bias_list.append("################# LAYER #################")
linear_bias_list.append(saved_model.network[25].bias)

# 2nd linear layer
linear_bias_list.append("################# LAYER #################")
linear_bias_list.append(saved_model.network[28].bias)


### Saving weights to this path
weight_path

layer = 1
for i in range(0,len(linear_weight_list)):
    if type(linear_weight_list[i]) != str:
        linear_weight_tensor = linear_weight_list[i]
        write_tensor_to_file(linear_weight_tensor, weight_path, "P0-linear_layer_{}_weight.txt".format(layer))
        layer += 1


### Saving bias to this path
layer = 1
for i in range(0,len(linear_bias_list)):
    if type(linear_bias_list[i]) != str:
        linear_bias_tensor = linear_bias_list[i]
        write_tensor_to_file(linear_bias_tensor, bias_path, "P0-linear_layer_{}_bias.txt".format(layer))
        layer += 1