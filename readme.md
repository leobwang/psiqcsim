# psiqcsim

**A quantum-computing simulator written from scratch in C++, plus a
brute-force gate-synthesis search.** The synthesis question: *given a set of
building-block gates and a target gate, can the target be constructed by
composing the building blocks?* If yes, the search prints a shortest gate
sequence; if not, it says so.

(`psi` = Ψ, the wave function.)

## Contents

| File | Role |
|---|---|
| `psi.h` / `psi.cpp` | the `psi` library: `State` (n-qubit state vector) and `Gate` (2ⁿ×2ⁿ unitary matrix), with composition and application operators |
| `search_gate.cpp` | BFS gate-synthesis driver built on the library |

## The `psi` library

### `psi::State`
- n-qubit state vector of `std::complex<double>` amplitudes (`dim = 2^n`)
- construct from a raw amplitude array, or by name for the common
  single-qubit states: `"0"`, `"1"`, `"+"`, `"-"`, `"i"`, `"-i"`
- `==` / `!=` compare amplitudes with a `1e-8` tolerance

### `psi::Gate`
- 2ⁿ×2ⁿ complex matrix; construct from a raw matrix or by name:

| Qubits | Gates |
|---|---|
| 1 | `i` (identity), `x`, `y`, `z`, `s`, `t`, `h` |
| 2 | `cx`, `cy`, `cz`, `ch` |
| 3 | `ccx` (Toffoli) |

- `Gate * Gate` composes gates; `Gate * State` applies a gate to a state
- **Global-phase normalization**: after a gate is applied, the resulting state
  is divided by the phase of its first nonzero amplitude, so states that
  differ only by a global phase compare as equal — exactly the equivalence
  relation the synthesis search needs.

## Gate synthesis (`search_gate.cpp`)

A target gate is specified by its action on the states {|+⟩, |i⟩, |0⟩} —
three states whose images pin down a single-qubit unitary up to global phase.
The search runs breadth-first over sequences of the building-block gates
({S, H} in the included examples), so the first hit is a shortest
decomposition. Search depth is capped at 16.

`main()` runs five target mappings; actual output:

```
mapping: |+> -> |0>, |i> -> |+>, |0> -> |i>   →  Gate found: h-s
mapping: |+> -> |+>, |i> -> |-i>, |0> -> |1>  →  Gate found: h-s-s-h
mapping: |+> -> |0>, |i> -> |i>, |0> -> |+>   →  Max depth reached. Gate not found.
mapping: |+> -> |1>, |i> -> |i>, |0> -> |+>   →  Gate found: s-s-h
mapping: |+> -> |1>, |i> -> |->, |0> -> |i>   →  Gate found: s-s-h-s
```

(The third mapping is genuinely unreachable: it fixes |i⟩ while swapping
|0⟩ ↔ |+⟩, which no unitary composition of S and H realizes — the search
reporting "not found" is the correct answer, not a failure.)

## Build & run

```bash
g++ search_gate.cpp psi.h psi.cpp -o search_gate && ./search_gate
```

No dependencies beyond a C++ compiler.

## Possible extensions

- Read the building-block set and target mapping from the command line
- Deduplicate BFS nodes that reach an already-visited unitary
- Synthesize multi-qubit targets using the library's 2- and 3-qubit gates
