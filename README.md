[Build status](https://github.com/johnjohnlin/namedtuple/actions/workflows/unittest.yaml/badge.svg)

# What is namedtuple?
`namdetuple` is a super fast C++ type-reflection library (5x `BOOST_HANA_DEFINE_STRUCT`),
and you can insert the `namedtuple` macro into your existing structure without modification.
It's based on `boost.preprocessor`, and the core of our macro is only roughly 10 lines.


# Idea behind namedtuple
In Python, there is a `namedtuple`

```python
Point = namedtuple('Point', ['x', 'y'])
p = Point(11, y=22)
p[0] + p[1] # 33
p.x + p.y # 33
```

In C++, we have `tuple` and struct, but we cannot connect them easily.

```cpp
struct S { int x; float y; string z; };
S s_struct;
tuple<int, float, string> s_tuple;
s_struct.y; // float
s_tuple.get<1>(); // float
s_tuple.y; // ERROR
s_struct.get<1>(); // ERROR
```

We provide a macro for converting your struct to a `namedtuple` in C++ easily.

```cpp
struct S2 {
	int    x;
	float  y;
	string z;
	// NOTE: no semicolon here
	MAKE_NAMEDTUPLE(x, y, z)
};
S2 s_namedtuple;
s_namedtuple.y; // float
s_namedtuple.get<1>(); // float, the reference of y
static_assert(sizeof(S2) == sizeof(S)); // namedtuple does not add extra members!
static_assert(sizeof(S2::num_members) == 3u); // namedtuple also provides ::num_members
S2::get_name(1); // const char* = "y"
```

# nametuple: why and how?
`namedtuple` provides similar functionalities as [boost::hana](https://boostorg.github.io/hana/index.html) for struct meta-programming (specifically, `BOOST_HANA_DEFINE_STRUCT`).
You can loop over the member of a struct in compile time, so you can implement JSON-like-`cout` struct without repeatedly writing boring code.
Besides, the core of `namedtuple` is very simple (roughly 10 lines of macro!), so it has super fast compilation time.
Next we shall show some `namedtuple` usages and review the `namedtuple` performance.

## Sample usage
With the `get()` and its overloaded functions, we can make use of parameter pack (C++11) of fold expression (C++17).
For example, if we want to compare two namedtuples, but ignore members started with an underscore:

```cpp
struct S2 {
	int    x;
	// will not compare member started with "_"
	float  _y;
	string z;
	MAKE_NAMEDTUPLE(x, y, z)
};

template<typename NT, unsigned...indices>
bool compare_impl(const NT& lhs, const NT& rhs, integer_sequence<unsigned, indices...>) {
	return (
		(
			NT::template get_name<indices>()[0] == '_' or
			lhs.template get<indices>() == rhs.template get<indices>()
		)
		and ...
	);
}

template<typename NT>
bool compare(const NT& lhs, const NT& rhs) {
	return compare_impl(lhs, rhs, make_integer_sequence<unsigned, NT::num_members>{});
}

S2 a{1, 2, "3"};
S2 b{1, 3, "3"};
compare(a, b); // true
b.z = "";
compare(a, b); // false
```

With `boost::hana`, you can achieve the same thing, but `namedtuple` is much faster.
We shall show this in the following section.

## Compile time performance compared with boost::hana

`boost::hana` is one of the most efficient template library as shown [here](https://boostorg.github.io/hana/index.html#tutorial-performance).
`namedtuple` is much faster than `boost::hana` due to its simplicity.
We compare three versions of implementation of `ostream<<` with tens of classes in one file:
* Use `for_each` in `hana` to implement generic `ostream<<`
* Use `foreach` in `namedtuple` to implement generic `ostream<<`
* Use code generator to generate plain `ostream<<`

In this specific scenario, we consistently save time/memory by a factor of 5 compared with `boost::hana`, only costing 11-50% overhead compared to non-templated C++ code with code-gen.

Testing scenario:

* Randomly generate tens of deeply nested struct
* ArchLinux 202301
* AMD 3700X
* gcc 12.2.1
* No extra compile flag
* Benchmark under `benchmarks/benchmark1`

### Peak memory usage (KB)
| #structs | Hana | namedtuple | Code-gen | Improvement vs hana |
| -: | -: | -: | -: | -: |
| 20 | 499960 | 125364 | 80528 | 3.99x |
| 25 | 573604 | 138156 | 84188 | 4.15x |
| 30 | 720840 | 152108 | 88040 | 4.74x |
| 35 | 826164 | 165412 | 91512 | 4.99x |
| 40 | 831900 | 179880 | 96728 | 4.62x |

### Compile time (second)
| #structs | Hana | namedtuple | Code-gen | Improvement vs hana |
| -: | -: | -: | -: | -: |
| 20 | 6.81  | 1.02 | 0.67 | 6.67x |
| 25 | 12.11 | 2.12 | 1.74 | 5.71x |
| 30 | 20.59 | 4.10 | 3.55 | 5.02x |
| 35 | 25.96 | 5.47 | 4.92 | 5.27x |
| 40 | 37.02 | 8.31 | 7.44 | 4.45x |

# Implementations

The idea is simple. If we implement `get(X)`, where X is overloaded with different `integral_constant`, then we can implement `get<unsigned>()` easily. This is made possible by `boost.preprocessor`.

```cpp
struct S2 {
	int    x;
	float  y;
	string z;
	// extra code inserted by MAKE_NAMEDTUPLE(x, y, z)
	// implemented by boost.preprocessor
	int    get(integral_constant<unsigned, 0>) { return x; }
	float  get(integral_constant<unsigned, 1>) { return y; }
	string get(integral_constant<unsigned, 2>) { return z; }
	template<unsigned x> auto get() { return get(integral_constant<unsigned, x>()); }
};
```

# Requirements
Just copy everything under `include/` to anywhere you can include.
The core `namedtuple` is macro based on `boost.preprocessor` (you don't need the entire boost), so it only requires C++11.
However, `util/` might require C++14 or C++17 features, or specifically, auto template, `integer_sequence` and fold expressions.
