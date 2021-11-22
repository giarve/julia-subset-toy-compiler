function params_ret_a(a::Float64)::Float64
    asd = 15.0
    return a + asd
end

print(params_ret_a(2.0))

# WARNING: THESE ARE NOT GLOBAL VARIABLES AND THEY'RE STORED INSIDE THE MAIN FUNCTION
# This means that they won't be accessible from other functions scope
a = 5
b = 2
print(b)
if b >= 2
    b = a
else
    b = b * 2
end
while a <= 20
    a = a * 2
    print(a)
end
print(a)
print(b)

if 12317294
    a = a * 2
else
    a = a + 1
    b = 6
end

if true
    a = a + 2
else
    a = a + 1
end


if true
    a = a + 2
end


function no_params_no_return_type()
    a = 15 + 1
end

function no_params_no_return_type_print()
    # the following line errors because there is no checking (`no_params_no_return_type` returns nothing)
    # but it is not handled properly by the compiler and segfaults
    # b = no_params_no_return_type() 
end

function no_body_param_no_ret(a::Float64)
end

function no_params()::Int64
    no_params_no_return_type()
    b = params_ret_a(1.0)
    c = b + 2.0

    x_stop_val = 0
    for x in 1:6
        if (x == 2 || x == 4 || x == 6)
            print(x)
        else
            if x != 3
                print(x + 100)
            end
        end
        x_stop_val = x_stop_val + 1
    end
    return x_stop_val
end

if no_params() == 6
    print(999)
end

function params_abc_ret_a(a::Float64, b::Int64, c::Int64)::Int64
    c = b * 2 * c
    return c
end

print(params_abc_ret_a(5 * 5.0^3.0, params_abc_ret_a(0.0, 1, 2), 2))

no_params()

i = 10
r = 0.5 
bool = true && true || false && false
print(bool)

d = div(i, 2)
print(d)

