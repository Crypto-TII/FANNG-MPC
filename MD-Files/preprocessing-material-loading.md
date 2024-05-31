# Loading Preprocessed Materials from a DB.
Introducing our efficient Preprocessing Material Loading feature! 

This capability allows you to efficiently pre-compute and store sensitive data in your designated database, ensuring enhanced security and ease of use. By loading specific materials into RAM selectively, you'll experience remarkable performance gains that optimize your computing resources. 

Our feature currently supports preprocessing the following materials:
 
* Beaver Triples
* Matrix Triples
* Bounded Randomness

Moreover, we continuously expand our collection of functions, seamlessly aligning with the preprocessing paradigm to offload resource-intensive tasks. 

## __Beaver Triples__
A Beaver triple is a set of three values: two random values, `a` and `b`, and their product `c = ab`. Now, generating this triple can be pretty pricey in MPC. But here's the cool part - these triples don't care about your input; they're independent! So we can preprocess them and store their secret-shares in each party's private DB. Smart, right?

Now, when the computation kicks off, we can load the required number of these triples into our RAM and use the Beavers randomization technique to multiply any two values `x` and `y`. It works by first publicly opening `x-a` and `y-b`, which are the differences between the values and the random values. Then, it computes `xy = (x-a)(y-b) + (x-a)b + (y-b)a + c`. 

But wait, we've introduced two new instructions, and we've adapted an oldie to handle the generation of these triples: `OTRIPLE`, `LOADTRIPLE`, and `TRIPLE`. 

You can learn more about our feature to load Beaver triples [here](beaver-triple-loading.md).

## __Matrix Triples__
Matrix triples extend the concept of Beaver triples to matrices. Like Beaver triples, a matrix triple comprises two random matrices, `A` and `B`, and their product `C = AB`. These triples, independent of input data, are preprocessed and stored in private databases. During computation, they're loaded into RAM for efficient matrix multiplication using the Beavers randomization technique: `(X-A)(Y-B) + (X-A)B + (Y-B)A + C`. 

But hold on, we've got three new instructions to generate these matrix triples: `LOADCT`, `CT_DYN`, and `CTRIPLE`. 

Learn more about loading matrix triples [here](matrix-triple-loading.md).

## __Bounded Randomness__
Shared-randomness is a key element in MPC, forming the basis for various protocols. However, generating it can be quite expensive. To tackle this, we can create the shared-randomness beforehand, even before the actual computation starts, among the involved parties. This way, we streamline the MPC process and make computations more efficient and secure. Moreover, in many applications, it is essential for the random values to satisfy certain bounds, depending on the specific context and use case. 

Specifically, given a limit `bound` and the required number of instances `total`, these three instructions—`OSRAND`, `LOADSRAND`, and `SRAND`—help us generate secret-shares of `total` random values less than `2^bound`. We store them in the database and load them into memory when needed during the protocol execution. This approach not only streamlines the MPC process but also ensures that the shared-randomness meets the necessary constraints, making our computations more robust and secure.

You can learn more about our feature to load bounded randomness [here](bounded-randomness-loading.md)

## __DABIT Tuples__ 
So, you may wonder what is a DABIT Tuple, the answer is simple: is a couple of random bits, that are the same, but live in different domains. To be more precise, let us call them `bp` and `b2`. Why?, Well because `bp` lives in modulo `p`, being a large prime, whereas `b2` lives in modulo`2`.  We can generate these using the factory that was already there in `SCALE-MAMBA`. The good news continues, there is no reason why we could not generate them before hand, and store them! 

That is exactly what we achieve here, given that `DABITs` are used everywhere, from comparisons to conversions. To achieve this, and following the results above, we have now included 2 new instructions `ODABIT` and `LOADDABIT`. We also extensively modified the `DABIT` instruction. All of this whilst respecting the template structure that was already present in `SCALE-MAMBA`, and integrating it to the `GC` way of persisting information. 

You can learn more about our feature to load DABITs [here](dabit-tuple-loading.md).