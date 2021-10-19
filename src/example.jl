# Identifiers, variables and types
i = 10
r = 0.5 
s = "Hello"
b = true

# Arithmetic expressions
5 + i * 5^3
i / 2
s * ", " * s

# m_i * (v_i - 5 * v_r)
# 3 * ones(Int64, 2, 2)
# transpose(v_i) + m_r
# transpose(v_r) * v_r

# Boolean expressions
i == 10 && (i + 10) >= 20
v_i[1] < v_i[2] && false
s * s == "HelloHello"
b || !b
(b || !b) && (b && !b)
b || !b && b && !b


# Sentences: the combination of any of the above
id = b || !b && b && !(5 + i * 5^3)


# Arrays and vectors

#i + A[3, 2] / v_i[1]
#m_i[i - 8, m_i[1, 3]]
#-7 - (i + 7.0)
#
v_i = [10; 20; 30]
#v_r = [1.0, 2.0, 3.0]
#m_i = [1 2 3; 4 5 6]
#m_r = [1 2.2 3]
#A = [1.1 1.2; 2.1 2.2; 3.1 3.2]
#
#v_i[2]
#m_r[1, 3]
#A[3, 1]