# Matrix_lib

This library contains functionalities to perform triple-assisted convolutions and fully connected layers. 

## convolution and fully connected layers

#### convolution

```bash
conv3d_sfix2sfix
```


Performs a convolution of 3d features (rearranged as 2d matrixes). It accepts strides 1 and 2. It accepts any padding. The input features and kernels must be type SFIX. The convolutional triple must be type SINT.

| arguments |  description ||
|--|--|--|
|X_sfix  |  Input features – 2d matrix with dimension (w $\cdot$ h, s) (type SFIX) |
|Y_sfix  |  Kernels – 2d matrix with dimension (kw $\cdot$ kh $\cdot$ s, s_)
 (type SFIX) |
|A  | Convolutional triple element A |
|B  | Convolutional triple element B |
|C  | Convolutional triple element C |
|kh | Kernel height |
|kw | Kernel width |
|s  | Input channels |
|s_ | Output channels |
|h  | Feature height |
|w |  Feature width |
|stride | stride (1 or 2)|
|padding | padding (any value)|

OUTPUT: A matrix Z with dimension (w_out $\cdot$ h_out, s_) where 'w_out' and 'h_out' are width and hight of the output features, which depend on the stride and the padding. 

Both w' and h' are internally calculated as:

```http
if padding > 0:  
    h = (h + (2 * padding))  
    w = (w + (2 * padding))  

if stride == 2:  
    w_out = (w - kw + 1)  
    h_out = (h - kh + 1)  
    w_out = (w_out // stride)  
    h_out = (h_out // stride)  

    if (w % stride) > 0:  
        w_out += 1  
    if (h % stride) > 0:  
        h_out += 1  
```

NOTE 1 :This function calls *conv3d_sint2sint*, after converting the input matrixes into SINT type. Then it performs probabilistic truncation on the output matrix.

NOTE 2: Use a convolutional triple generator with the same convolution parameters to obtain a triple A,B,C of correct size.

Size of A = Size of X_sfix + 2  $\cdot$ padding 
Size of B = Size of Y_sfix
Size of C =  (w_out $\cdot$ h_out, s_)


#### matrix multiplication

```http
multmat_sfix2sfix
```

Performs a matrix multiplication using matrix triples. Returns X $\cdot$ Y. The matrixes must be type SFIX. The input matrix triple must be type SINT.

| arguments |  description ||
|--|--|--|
|X_sfix  |  Input features – 2d matrix with dimension (w $\cdot$ h, s) (type SFIX) |
|Y_sfix  |  Kernels – 2d matrix with dimension (kw $\cdot$ kh $\cdot$ s, s_) (type SFIX) |
|A  | Convolutional triple element A |
|B  | Convolutional triple element B |
|C  | Convolutional triple element C |

OUTPUT: Z = X $\cdot$ Y. 

NOTE 1: This function calls *multmat_sint2sint*, after converting the input matrixes into SINT type. Then it performs probabilistic truncation on the output matrix.

NOTE 2: Use a matrix triple generator with the same convolution parameters to obtain a triple A,B,C of correct size, i.e. same as X and Y.


## vectorized probabilistic truncantion 

```http
truncate_sfix_matrix
```

| arguments |  description ||
|--|--|--|
|X  | 2d matrix (type SFIX) |
|value  | SFIX value with correct fractional part |

OUTPUT: X with correct fractional size. Takes a Matrix that has undergone a multiplication / convolution, and truncates the all values. This function is required to correct the offset of the fractional part. Second argument is used as a reference.

NOTE 1: This function equals the functionality of performing truncation individually in each matrix value, but it is more efficient. The process is vectorized, thus it renders less lines of code in the MPC program, and it also ensures that the number of communication rounds is minimized even when the compiler uses the -O1 flag.

NOTE 2: This function is called in *conv3d_sfix2sfix* and *mulmat_sfix2sfix* 

## data formatting 

```http
rearrange_3d_features_into_2d_matrix
```

| arguments |  description ||
|--|--|--|
|X  | 3d tensor with dimension (s, h, w), (any type) |

OUTPUT: Outputs a 2d matrix with dimension (h $\cdot$ w, s)

```http
rearrange_2d_matrix_into_3d_features
```

| arguments |  description ||
|--|--|--|
|X  | 2d matrix (r, s), (any type) |
|h  | height |
|w  | width |

OUTPUT: Outputs a 3d tensor with dimension (s, h, w).

NOTE: h and w must fulfil h$\cdot$w = r

```http
rearrange_4d_kernels_into_2d_matrix
```

| arguments |  description ||
|--|--|--|
|X  | 4d tensor with dimension (s_, s, kh, kw), (any type) |

OUTPUT: Outputs a 2d matrix with dimension (kh $\cdot$ kw, s $\cdot$ s_).

```http
rearrange_2d_matrix_into_4d_kernels
```

| arguments |  description ||
|--|--|--|
|X  | 2d matrix (r, s), (any type) |
|l  | padding |
|s  | input channels |
|s_  | output channels |

OUTPUT: Outputs a 4d tensor with dimension (s_, s,  kh, kw); where kh = kw = 2 $\cdot$l + 1  

## support functions

```http
conv3d_sint2sint
```

| arguments |  description ||
|--|--|--|
|X_sfix  |  Input features – 2d matrix with dimension (w $\cdot$ h, s) (type SINT) |
|Y_sfix  |  Kernels – 2d matrix with dimension (kw $\cdot$ kh $\cdot$ s, s_) (type SINT) |
|A  | Convolutional triple element A |
|B  | Convolutional triple element B |
|C  | Convolutional triple element C |
|kh | Kernel height |
|kw | Kernel width |
|s  | Input channels |
|s_ | Output channels |
|h  | Feature height |
|w |  Feature width |
|stride | stride (1 or 2)|
|padding | padding (any value)|

OUTPUT: Same as *conv3d_sfix2sfix* but for SINT type. This function is called by *conv3d_sfix2sfix*. 

#### matrix multiplication

```http
multmat_sint2sint
```

| arguments |  description ||
|--|--|--|
|X_sfix  |  Input features – 2d matrix with dimension (w $\cdot$ h, s) (type SINT) |
|Y_sfix  |  Kernels – 2d matrix with dimension (kw $\cdot$ kh $\cdot$ s, s_) (type SINT) |
|A  | Convolutional triple element A |
|B  | Convolutional triple element B |
|C  | Convolutional triple element C |

OUTPUT: Same as *multmat_sfix2sfix* but for SINT type. This functions is called by *multmat_sfix2sfix*

```http
multmat_sint2cint
```

| arguments |  description ||
|--|--|--|
|A  | 2d matrix (type SINT) |
|B  | 2d matrix (type CINT) |

```http
multmat_cint2sint
```

| arguments |  description ||
|--|--|--|
|A  | 2d matrix (type CINT) |
|B  | 2d matrix (type SINT) |

```http
multmat_cint2cint
```

| arguments |  description ||
|--|--|--|
|A  | 2d matrix (type CINT) |
|B  | 2d matrix (type CINT) |

OUTPUT: C = A $\cdot$ B

NOTE: Multiplies A and B (with different types when at least one is CINT). These operations do not require communication rounds and triples. This functions are required in: i) *conv3d_sint2sint*; and ii) *multmat_sint2sint*.

```http
traspose
```

| arguments |  description ||
|--|--|--|
|X  | 2d matrix (any type) |

OUTPUT: transpose of a matrix. Any type (SFIX, SINT, CINT).

```http
add_matrices
```

| arguments |  description ||
|--|--|--|
|X  | 2d matrix (any type) |
|Y  | 2d matrix (any type) |

OUTPUT: X + Y. Any type (SFIX, SINT, CINT).

```http
flatten_to_array
```

| arguments |  description ||
|--|--|--|
|X  | 2d matrix (any type) |

OUTPUT: X as an Array type. Any type (SFIX, SINT, CINT). Ordered by input rows.

```http
flatten_to_rowmatrix
```

| arguments |  description ||
|--|--|--|
|X  | 2d matrix (any type) |

OUTPUT: A as single row Matrix. Any type (SFIX, SINT, CINT). Ordered by input columns.

```http
sfix_matrix_equals
```

| arguments |  description ||
|--|--|--|
|X  | 2d matrix (type SFIX) |
|Y  | 2d matrix (type SFIX) |
|tolerance|allowed difference in the mantissa

OUTPUT: 1 if matrixes are equal, 0 otherwise.

NOTE: Tests equality of two matrixes of type SFIX. Each value is compared individually and some difference (‘tolerance’) is allowed in the mantissa. This function is NOT PRIVATE. Used for testing only. 

## transformations for convolution

The below transformations allow performing a convolution as a matrix multiplication. 

```http
transform_input_features
```

| arguments |  description ||
|--|--|--|
|X  | 2d matrix with dimension (h$\cdot$w,s) (type any) |
|h  | height|
|w  | width |
|s  | channels |
|kh  | kernel height |
|kw  | kernel width |
|stride  | stride (1 or 2)|

OUTPUT: Transforms X into another 2d matrix with dimension (w’ $\cdot$ h’ , kh $\cdot$ kw $\cdot$ s). Stride supported 1 and 2. Function used in: *conv3d_sint2sint*.

```http
transform_kernels
```

| arguments |  description ||
|--|--|--|
|K  | 2d matrix with dimension (h$\cdot$w,s) (type any) |
|kh  | height|
|kw  | width |
|s  | input channels |
|s_ | output channels |

OUTPUT: Transforms the Kernels K from a 2D matrix (kh $\cdot$ kw, s $\cdot$ s_) into another 2d matrix (kh $\cdot$ kw $\cdot$ s, s_). Function used in: *conv3d_sint2sint*.

```http
prune_matrix_rows_according_to_stride
```

| arguments |  description ||
|--|--|--|
|X  | 2d matrix with dimension (h$\cdot$w,s) (type any) |
|h  | height|
|w  | width |
|stride  | stride|

OUTPUT: Takes a set of 3D features, represented as 2D matrix, and deletes rows according to the stride. This function is used inside *conv3d_sfix2sfix->conv3d_sint2sint->transform_input_features*.

```http
pad_feature_with_zeros_2d(X, h, w, l)
```

| arguments |  description ||
|--|--|--|
|X  | 2d matrix with dimension (h$\cdot$w,s) (type any) |
|h  | height|
|w  | width |
|l  | padding|

OUTPUT: Takes a set of 3D features, represented as 2D matrix, pads 'l' zeros in the edges. This function is used inside *conv3d_sfix2sfix->conv3d_sint2sint*.

## input / output functions

```http
input_to_matrix
```

| arguments |  description ||
|--|--|--|
|input  | List |
|n  | number of rows|
|m  | number of columns |

OUTPUT: Converts input list to a matrix (n, m).

```http
input_matrix_triple
```

| arguments |  description ||
|--|--|--|
|rowsA | number of rows of A|
|colsA | number of columns of A|
|colsB | number of columns of B|
|channel | input channel (default=0)|

OUTPUT: Inputs a matrix triple A,B,C from channel

```http
output_matrix_triple
```

| arguments |  description ||
|--|--|--|
|triple | Tuple [A,B,C] |
|channel | output channel (default=0)|

OUTPUT: Outputs a matrix triple A,B,C to channel

```http
compare__matrix_from_file
```

| arguments |  description ||
|--|--|--|
|A | matrix (type SFIX) |
|path | file path|
|tolerance | allowed difference in the mantissa|

OUTPUT: 1 if equal, 0 otherwise. Compares a SFIX matrix with another one stored in a file. It uses a tolerance and calls function *sfix_matrix_equals*. This function is NOT PRIVATE, used for testing.

## *faster* helper functions

These functions user register memory, and are faster than their counterparts. Only compatible with SINT type.

```http
traspose_sint_reg
```

| arguments |  description ||
|--|--|--|
|A | matrix (type SINT) |
|h | height (rows)|
|w | width (columns)|

OUTPUT: Outputs transpose of a matrix. Only SINT type. Same functionality as *traspose*.

```http
flatten_to_array
```

| arguments |  description ||
|--|--|--|
|A | matrix (type SINT) |

OUTPUT: Transforms a Matrix type into an Array type. Only SINT type. Same functionality as *flatten_to_array*.

## legacy functions

These functions have not been properly tested. We recommend using conv3d_sfix2sfix*, *conv3d_sint2sint*, *transform_kernels*, *transform_input_features* instead. They operate like the previous functions but are optimised for padding equal to 'l' where the kernels have size ((2l+1) , (2l+1)).

```http
conv3d_sfix2sfix_adjusted_padding
```
```http
conv3d_sint2sint_adjusted_padding
```
```http
transform_kernels_adjusted_padding
```
```http
transform_input_features_adjusted_padding
```

## new byte codes in SCALE compiler

The following bytecodes have been added to the compiler to extend its functionality. These byte codes perform a summation of an array of SINT or CINT values.

Both bytecodes have the following syntax:

```http
sums(output, input_array, size)
sumc(output, input_array, size)
```

for example:

```http
sint_array1 = sint(size=100) # array of 100 elements  
summation = sint() # a single sint 
sums(summation, aint_array, 100) # the variable 'summation' stores the additon of 100 elements  
```
