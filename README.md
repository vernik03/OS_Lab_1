# Communication between tasks
The aim ofthis lab is to correctly organize computation using multiple processes
or threads (scheduling entities). Solution should compute expression f(x) X g(x),
where X is a binary operation, integer value x is scanned from the input, computations f and g are parameters and specified independently. The schematic ofthe
system is given in figure 1. The major requirement of the exercise is that computations f and g should run in parallel to main component, manager. Manager
initializes computation process (effectively starting f and g for the scanned x),
computes the final result and organizes cancellation. Depending on variant manager can be single- or multi-threaded. Multithreaded manager can summarize the
result in the main thread (centralized approach) or any manager's thread is allowed to finalize the expression evaluation. Which approach must be used is also
stated in the variant. Manager should make no assumptions about computational
specifics of f and g. But it is assumed that function result could be undefined
for some inputs. System should make allowance for this (see Cancellation subsection). In some cases knowing the return ofsingle computation (either J' or g)
could be enough to provide final result ofthe whole computation. The reason for
this is failure ofindividual computation. See particulars in further description and
variant formulation.
