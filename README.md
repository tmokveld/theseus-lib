# Theseus-lib

## 1. Introduction

### 1.1. What is Theseus?
Theseus is a fast, optimal and affine-gap Sequence-to-Graph aligner [1]. It leverages the expected high similarity in the aligned data to accelerate computation and reduce the search space compared to other alternatives. Theseus is a general purpose aligner, providing two main functionalities:
1. Multiple Sequence Alignment (MSA): Theseus performs MSA of a set of N sequences using the Partial Order Alignment (POA) [2] approach. That is, it progressively builds a partial order graph representing an MSA of a set of given sequences, adding one more sequence to the graph at each iteration.
2. Aligning to a graph: Alternatively, Theseus can align one or several sequences to a reference graph. The user has to provide an initial position for the alignment to start.

<p align = "center">
<img src = "img/Theseus_green.png" width="300px">
</p>

Theseus extends the proposal from the Wavefront Alignment algorithm (WFA) [3, 4], originally devised for pairwise sequence alignment, to the context of sequence-to-graph alignment.

### 1.2. Getting started
Git clone and compile the library, tools, and examples (by default, use `cmake` for the library and benchmark build).

```
git clone https://github.com/albertjimenezbl/theseus-lib
cd theseus-lib
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```


## 2. Using Theseus

### 2.1. Multiple Sequence Alignment (MSA) example

This example illustrates how to use Theseus as a Multiple Sequence Aligner. First, include the msa and general headers:
```
#include "theseus/alignment.h"
#include "theseus/penalties.h"
#include "theseus/theseus_msa_aligner.h"
```

Then, create and configure a MSA aligner object. This object is defined by two parameters: a set of penalties and an initial sequence behaving as the starting point of the alignment. An example on how to set such parameters and create an MSA object is found in the next code snippet:
```
theseus::Penalties penalties(match, mismatch, gap_open, gap_extend);
theseus::TheseusMSA aligner(penalties, initial_sequence);
```

Once this is done, we can start adding sequences to our POA graph using the align functionality, that returns an Alignment object with CIGAR, path and score information:
```
theseus::Alignment alignment_object = aligner.align(sequence);
```

Each time a new sequence is added to the POA graph, the graph is updated with the newly found variation (all the insertions, deletions and mismatches of the resulting alignment object).

Finally, we can output the result in three different formats: a graph in .gfa format, a multiple sequence alignment and a consensus sequence:
```
aligner.output_as_gfa(output_file);             // Output the compacted POA graph in .gfa format
aligner.output_as_msa(output_file);             // Output as a Multiple Sequence Alignment
sequence = aligner.get_consensus_sequence();    // Find the consensus sequence of the alignment
```


### 2.2. Mapping a sequence to a graph

This example illustrates how to use Theseus as a general Sequence-to-Graph Aligner. First, include the alignemnt and general headers:
```
#include "theseus/alignment.h"
#include "theseus/penalties.h"
#include "theseus/theseus_aligner.h"
```

Then, create and configure an aligner object. This object is defined by two parameters: a set of penalties and file stream containing a reference graph in .gfa format. An example on how to set such parameters and create an aligner is found in the next code snippet:
```
theseus::Penalties penalties(match, mismatch, gap_open, gap_extend);
theseus::TheseusAligner aligner(penalties, gfa_file_stream);
```

Once this is done, we can start aligning sequences to the reference graph using the align functionality. **Importantly**, a call to the align function consists of three arguments: the **sequence** to be aligned, the **starting vertex** for the alignment and **starting offset** in that starting vertex. The result of the alignment is an Alignment object with CIGAR, path and score information:
```
theseus::Alignment alignment_object = aligner.align(sequence, start_vertex, start_offset);
```


## <a name="theseus.other.notes"></a> 3. Some technical notes

TODO: Change this?
- Thanks to Eizenga's formulation, Theseus-lib can operate with any match score. In practice, M=0 is often the most efficient choice.


## <a name="theseus.bugs"></a> 4. REPORTING BUGS AND FEATURE REQUEST

Feedback and bug reporting is highly appreciated. Please report any issue or suggestion on github or email to the main developer (albert.jimenez1@bsc.es). Don't hesitate to contact us if:
  - You experience any bug or crash.
  - You want to request a feature or have any suggestion.
  - Your application using the library is running slower than it should or you expected.
  - Need help integrating the library into your tool.

## <a name="theseus.licence"></a> 5. LICENSE

Theseus-lib is distributed under MIT licence.

## <a name="theseus.authors"></a> 6. AUTHORS

Albert Jimenez Blanco (albert.jimenez1@bsc.es) is the main developer and the person you should address your complaints.

Lorién López-Villellas has had major contributions both in the technical implementation of Theseus and the final structure of the library.

Santiago Marco-Sola and Juan Carlos Moure have contributed with fruitful theoretical discussions, helping shape the final version of the algorithm.

## <a name="theseus.ack"></a> 7. ACKNOWLEDGEMENTS

(TODO:)


## <a name="theseus.ref"></a> 8. REFERENCES

1. TODO:

2. **Christopher Lee, Catherine Grasso, and Mark F. Sharlow**. ["Multiple sequence alignment using partial order graphs."](https://doi.org/10.1093/bioinformatics/18.3.452). Bioinformatics, 2002.

3. **Santiago Marco-Sola, Juan Carlos Moure, Miquel Moreto, Antonio Espinosa**. ["Fast gap-affine pairwise alignment using the wavefront algorithm."](https://doi.org/10.1093/bioinformatics/btaa777). Bioinformatics, 2020.

4. **Santiago Marco-Sola, Jordan M Eizenga, Andrea Guarracino, Benedict Paten, Erik Garrison, Miquel Moreto**. ["Optimal gap-affine alignment in O(s) space"](https://doi.org/10.1093/bioinformatics/btad074). Bioinformatics, 2023.

<!--
## <a name="theseus.cite"></a> 9. CITATION

**Albert Jimenez-Blanco, Lorien Lopez-Villellas, Juan Carlos Moure, Miquel Moreto, Santiago Marco-Sola**. ["Theseus: Fast and Optimal Affine-Gap Sequence-to-Graph Alignment"](). Bioinformatics, 2025. -->

