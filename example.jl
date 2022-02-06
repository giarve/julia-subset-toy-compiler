# Identifiers, variables and types

i = 10
r = 0.5 
s = "Hello"
b = true

# Arithmetic expressions
5 + i * 5^3
i / 2
s * ", " * s
5 % 4

# Arrays and vectors
v_i = [10; 20; 30]
v_r = [1.0; 2.0; 3.0]
m_i = [1 2 3; 4 5 6]
m_r = [1 2.2 3]
A = [1.1 1.2; 2.1 2.2; 3.1 3.2]

i + A[3, 2] / v_i[1]
m_i[i - 8, m_i[1, 3]]
-7 - (i + 7.0)

v_i[2]
m_r[1, 3]
A[3, 1]

m_i * (v_i - 5 * v_r)
3 * ones(Int32, 2, 2)
transpose(v_i) + m_r
transpose(v_r) * v_r

# Boolean expressions
i == 10 && (i + 10) >= 20
v_i[1] < v_i[2] && false
s * s == "HelloHello"
b || !b
(b || !b) && (b && !b)
b || !b && b && !b

# Sentences: the combination of any of the above
id = b || !b && b
# && !(5 + i * 5^3)
#= ^^^^^
Julia has short-circuit evaluation, but we dont

julia> b = true
true

julia> id = b || !b && b && !(5 + i * 5^3)
true

julia> id = !(5 + i * 5^3)
ERROR: MethodError: no method matching !(::Int32)
Closest candidates are:
  !(::Bool) at bool.jl:33
  !(::Function) at operators.jl:968
  !(::Missing) at missing.jl:101
Stacktrace:
 [1] top-level scope
   @ REPL[31]:1
=#

#= multi line in single line =#

#= multi
line
comment #  =#

[1 2; 3 4]
a = [1 2; 3 4]
sizea = size(a)
len = length(a)
ones(Int32, 1, 4)
ones(Bool, 2, 4)
zeros(Float64, 2,2)

division = div(3,2)

transpose([1;2])
transpose([4 5 1])

0.5
.5
00.5
0.6
.55
0.7
0.3e+12
0.4e-12

# Errors, keep compiling!
div(3,0)
123 % 12.4

tuple = (1,2,3,4)
1*tuple
tuple_sum = +(tuple)
tuple - (1,2)

(1, pi)

(1, )

sin(90)
sin(45.0)
asin(0.5)

cos(pi/3)

tan(pi/4)

acos(cos(pi/3))

"atan(tan(pi/4)) == pi/4:"
atan(tan(pi/4)) == pi/4
