# Folding_lib

This library contains functionalities to perform: i) folding layers (average/max pooling); ii) output layers (i.e. softmax); iii) batch normalization; and iv) input processing (standadization and normalization).

## folding layers

```http
folding
```

| arguments |  description ||
|--|--|--|
|X  |  Input features – 2d matrix or 3d tensor (type SFIX) |
|kh | Kernel height |
|kw | Kernel width |
|stride | stride |
|type |'max_pool' or 'avg_pool'|
|h  | feature height (default = -1) |
|w |  feature width (default = -1) |
|padding | padding (default = 0)|

OUTPUT:  Performs pooling layer. It only implements ‘max_pool’ or ‘avg_pool’.

NOTE:  It accepts two input formats, 2d matrix or 3d tensor. If h = w = -1 (default values) then the input features must be of dimension (channels, h, w) . If the input is in 2D matrix format with dimension (h$\cdot$w, channels), then the values ‘h’ and ‘w’ must be passed as arguments.

```http
avg_pool
```

| arguments |  description ||
|--|--|--|
|X  |  Input features – 2d matrix or 3d tensor (type SFIX) |
|kh | Kernel height |
|kw | Kernel width |
|stride | stride |
|h  | feature height (default = -1) |
|w |  feature width (default = -1) |

OUTPUT:  Performs avgerage pooling.

NOTE:  This function is called by *folding*, don't call this function directly.

```http
max_pool
```

| arguments |  description ||
|--|--|--|
|X  |  Input features – 2d matrix or 3d tensor (type SFIX) |
|kh | Kernel height |
|kw | Kernel width |
|stride | stride |
|h  | feature height (default = -1) |
|w |  feature width (default = -1) |

OUTPUT:  Performs max pooling.

NOTE:  This function is called by *folding*, don't call this function directly.

```http
scale_all_values
```

| arguments |  description ||
|--|--|--|
|X  |  Input features – 2d matrix or 3d tensor (type SFIX) |
|scale | weight (type CFIX) |
|h  | matrix height |
|w |  matrix width |

OUTPUT:  multiples all vales by *scale*. This function is called by *avg_pool*.

 
## output layers

```http
softmax_scaled
```

| arguments |  description ||
|--|--|--|
|X  |  Input features – single column matrix (type SFIX) |
|neg_bound | lowest negative value allowed |
|complete  | 'true' or 'false' |

OUTPUT:  Performs softmax. If flag is ‘true’ it outputs a probability distribution. If the flag is ‘false’ it outputs the index with maximum probability. *neg_bound* is the lowest value ‘x’ such hat e^(x) can be represented. In the configuration by default with 20bits for the integer part *neg_bound=-15*.

## batch and input processing

```http
input_normalization
```

| arguments |  description ||
|--|--|--|
|X  |  3d tensor (type SFIX) |
|max | maximum value (type SFIX) |
|min  | minimum value (type SFIX) |

OUTPUT:  Outputs a normalized 3D tensor. Applies ((x – min) / (max - min)) to all values in X.

```http
input_standardization
```

| arguments |  description ||
|--|--|--|
|X  |  3d tensor (type SFIX) |
|mean | mean value (type SFIX) |
|var  | variance value (type SFIX) |

OUTPUT:  Outputs a standardized 3D tensor. Applies ((x – mean) / var) to all values in X.

```http
batch_normalization
```

| arguments |  description ||
|--|--|--|
|X  | 2d matrix (type SFIX) |
|Gp | 2d matrix (type SFIX) |
|Bp | 2d matrix (type SFIX) |

OUTPUT:  Performs the element-wise operation X $\cdot$ Gp + Bp

```http
batch_normalization_trn
```

| arguments |  description ||
|--|--|--|
|X  | Features, 2d matrix (type SFIX) |
|G | Gamma, 2d matrix (type SFIX) |
|B | Beta, 2d matrix (type SFIX) |
|M | Mean, 2d matrix (type SFIX) |
|V | Variance, 2d matrix (type SFIX) |
|e | numerical stability, 1 value (type SFIX) |

OUTPUT:  Performs the element-wise operation ((X - M) / sqrt(V+e)) $\cdot$ G + B

```http
batch_normalization_process_minibatch
```

| arguments |  description ||
|--|--|--|
|X  | Features, 3d tensor (type SFIX) |
|G | Gamma, 2d matrix (type SFIX) |
|B | Beta, 2d matrix (type SFIX) |
|M | Mean, 2d matrix (type SFIX) |
|V | Variance, 2d matrix (type SFIX) |
|e | numerical stability, 1 value (type SFIX) |

OUTPUT:  Performs the element-wise operation ((X - M) / sqrt(V+e)) $\cdot$ G + B

## helper functions

```http
get_max_with_binary_search
```

| arguments |  description ||
|--|--|--|
|X  | Array (type SFIX) |

OUTPUT:  Gets maximum element in an Array of SFIX values. All comparisons are sequential, there is no parallelization.

```http
binary_search_one_round
```

| arguments |  description ||
|--|--|--|
|X  | Array (type SFIX) |

OUTPUT:  Gets and array and compares adjacent elements in pairs. All comparisons are sequential, there is no parallelization. This function is called by *get_max_with_binary_search*

```http
get_mean
```

| arguments |  description ||
|--|--|--|
|X  | 3d tensor (type SFIX) |

OUTPUT:  Gets mean value of all elements

```http
get_variance
```

| arguments |  description ||
|--|--|--|
|X  | 3d tensor (type SFIX) |

OUTPUT:  Gets variance value of all elements

```http
get_mean_and_max
```

| arguments |  description ||
|--|--|--|
|X  | 3d tensor (type SFIX) |

OUTPUT:  Gets maximum and minimum values of all elements [max, min] (type SFIX)


