function no_params()::Int64
end

function no_params_no_return_type()
    a = 1
end

function no_body_no_params_no_return_type()
end

function params_ret_a(a::Float64)::Int64
    return a
end

function params_ret_a(a::Float64)::Int64
    return a + 1
end

function params_abc_ret_a(a, b, c::Int64)::Int64
      
         
    a
end

noparams()
params_ret_a(a)
params_abc_ret_a(params_ret_a(a), 2, 3)


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

#= multi line in single line =#

#= multi
line
comment #  =#

[1 2; 3 4]
a = [1 2; 3 4]

0.5
.5
00.5
0.6
.55
0.7
0.3e+12
  0.4e-12   
  
     
                     
