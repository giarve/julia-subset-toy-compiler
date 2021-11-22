; ModuleID = 'root_module'
source_filename = "example.jl"

@printf_format = private unnamed_addr constant [34 x i8] c"expr %d (decimal) or %f (double)\0A\00", align 1

define void @main() {
entry:
  %d = alloca i64, align 8
  %bool = alloca i1, align 1
  %r = alloca double, align 8
  %i = alloca i64, align 8
  %b = alloca i64, align 8
  %a = alloca i64, align 8
  %params_ret_a_CALL = call double @params_ret_a(double 2.000000e+00)
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([34 x i8], [34 x i8]* @printf_format, i32 0, i32 0), double %params_ret_a_CALL, double %params_ret_a_CALL)
  store i64 5, i64* %a, align 4
  store i64 2, i64* %b, align 4
  %b1 = load i64, i64* %b, align 4
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([34 x i8], [34 x i8]* @printf_format, i32 0, i32 0), i64 %b1, i64 %b1)
  %b2 = load i64, i64* %b, align 4
  %icmpsge = icmp sge i64 %b2, 2
  %ifcond = icmp ne i1 %icmpsge, false
  br i1 %ifcond, label %ifthen, label %ifelse

ifthen:                                           ; preds = %entry
  %a3 = load i64, i64* %a, align 4
  store i64 %a3, i64* %b, align 4
  br label %ifjoin

ifelse:                                           ; preds = %entry
  %b4 = load i64, i64* %b, align 4
  %mul = mul i64 %b4, 2
  store i64 %mul, i64* %b, align 4
  br label %ifjoin

ifjoin:                                           ; preds = %ifelse, %ifthen
  %a5 = load i64, i64* %a, align 4
  %icmpsle = icmp sle i64 %a5, 20
  %whilecond = icmp ne i1 %icmpsle, false
  br i1 %whilecond, label %while, label %whilebreak

while:                                            ; preds = %while, %ifjoin
  %a6 = load i64, i64* %a, align 4
  %mul7 = mul i64 %a6, 2
  store i64 %mul7, i64* %a, align 4
  %a8 = load i64, i64* %a, align 4
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([34 x i8], [34 x i8]* @printf_format, i32 0, i32 0), i64 %a8, i64 %a8)
  %a9 = load i64, i64* %a, align 4
  %icmpsle10 = icmp sle i64 %a9, 20
  %whileitcond = icmp ne i1 %icmpsle10, false
  br i1 %whileitcond, label %while, label %whilebreak

whilebreak:                                       ; preds = %while, %ifjoin
  %a11 = load i64, i64* %a, align 4
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([34 x i8], [34 x i8]* @printf_format, i32 0, i32 0), i64 %a11, i64 %a11)
  %b12 = load i64, i64* %b, align 4
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([34 x i8], [34 x i8]* @printf_format, i32 0, i32 0), i64 %b12, i64 %b12)
  br i1 true, label %ifthen13, label %ifelse16

ifthen13:                                         ; preds = %whilebreak
  %a14 = load i64, i64* %a, align 4
  %mul15 = mul i64 %a14, 2
  store i64 %mul15, i64* %a, align 4
  br label %ifjoin18

ifelse16:                                         ; preds = %whilebreak
  %a17 = load i64, i64* %a, align 4
  %add = add i64 %a17, 1
  store i64 %add, i64* %a, align 4
  store i64 6, i64* %b, align 4
  br label %ifjoin18

ifjoin18:                                         ; preds = %ifelse16, %ifthen13
  br i1 true, label %ifthen19, label %ifelse22

ifthen19:                                         ; preds = %ifjoin18
  %a20 = load i64, i64* %a, align 4
  %add21 = add i64 %a20, 2
  store i64 %add21, i64* %a, align 4
  br label %ifjoin25

ifelse22:                                         ; preds = %ifjoin18
  %a23 = load i64, i64* %a, align 4
  %add24 = add i64 %a23, 1
  store i64 %add24, i64* %a, align 4
  br label %ifjoin25

ifjoin25:                                         ; preds = %ifelse22, %ifthen19
  br i1 true, label %ifthen26, label %ifjoin29

ifthen26:                                         ; preds = %ifjoin25
  %a27 = load i64, i64* %a, align 4
  %add28 = add i64 %a27, 2
  store i64 %add28, i64* %a, align 4
  br label %ifjoin29

ifjoin29:                                         ; preds = %ifthen26, %ifjoin25
  %no_params_CALL = call i64 @no_params()
  %icmpeq = icmp eq i64 %no_params_CALL, 6
  %ifcond30 = icmp ne i1 %icmpeq, false
  br i1 %ifcond30, label %ifthen31, label %ifjoin32

ifthen31:                                         ; preds = %ifjoin29
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([34 x i8], [34 x i8]* @printf_format, i32 0, i32 0), i64 999, i64 999)
  br label %ifjoin32

ifjoin32:                                         ; preds = %ifthen31, %ifjoin29
  %powi = call double @llvm.pow.f64(double 5.000000e+00, double 3.000000e+00)
  %fmul = fmul double 5.000000e+00, %powi
  %params_abc_ret_a_CALL = call i64 @params_abc_ret_a(double 0.000000e+00, i64 1, i64 2)
  %params_abc_ret_a_CALL33 = call i64 @params_abc_ret_a(double %fmul, i64 %params_abc_ret_a_CALL, i64 2)
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([34 x i8], [34 x i8]* @printf_format, i32 0, i32 0), i64 %params_abc_ret_a_CALL33, i64 %params_abc_ret_a_CALL33)
  %no_params_CALL34 = call i64 @no_params()
  store i64 10, i64* %i, align 4
  store double 5.000000e-01, double* %r, align 8
  %and = and i1 true, true
  %and35 = and i1 false, false
  %or = or i1 %and, %and35
  store i1 %or, i1* %bool, align 1
  %bool36 = load i1, i1* %bool, align 1
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([34 x i8], [34 x i8]* @printf_format, i32 0, i32 0), i1 %bool36, i1 %bool36)
  %i37 = load i64, i64* %i, align 4
  %sdiv = sdiv i64 %i37, 2
  store i64 %sdiv, i64* %d, align 4
  %d38 = load i64, i64* %d, align 4
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([34 x i8], [34 x i8]* @printf_format, i32 0, i32 0), i64 %d38, i64 %d38)
  ret void
}

define double @params_ret_a(double %a) {
entry:
  %asd = alloca double, align 8
  %a1 = alloca double, align 8
  store double %a, double* %a1, align 8
  store double 1.500000e+01, double* %asd, align 8
  %a2 = load double, double* %a1, align 8
  %asd3 = load double, double* %asd, align 8
  %fadd = fadd double %a2, %asd3
  ret double %fadd
}

declare i32 @printf(i8*, ...)

define void @no_params_no_return_type() {
entry:
  %a = alloca i64, align 8
  %add = add i64 15, 1
  store i64 %add, i64* %a, align 4
  ret void
}

define void @no_params_no_return_type_print() {
entry:
  ret void
}

define void @no_body_param_no_ret(double %a) {
entry:
  %a1 = alloca double, align 8
  store double %a, double* %a1, align 8
  ret void
}

define i64 @no_params() {
entry:
  %x = alloca i64, align 8
  %x_stop_val = alloca i64, align 8
  %c = alloca double, align 8
  %b = alloca double, align 8
  call void @no_params_no_return_type()
  %params_ret_a_CALL = call double @params_ret_a(double 1.000000e+00)
  store double %params_ret_a_CALL, double* %b, align 8
  %b1 = load double, double* %b, align 8
  %fadd = fadd double %b1, 2.000000e+00
  store double %fadd, double* %c, align 8
  store i64 0, i64* %x_stop_val, align 4
  store i64 1, i64* %x, align 4
  br label %loop

loop:                                             ; preds = %ifjoin14, %entry
  %x2 = load i64, i64* %x, align 4
  %x3 = load i64, i64* %x, align 4
  %icmpeq = icmp eq i64 %x3, 2
  %x4 = load i64, i64* %x, align 4
  %icmpeq5 = icmp eq i64 %x4, 4
  %or = or i1 %icmpeq, %icmpeq5
  %x6 = load i64, i64* %x, align 4
  %icmpeq7 = icmp eq i64 %x6, 6
  %or8 = or i1 %or, %icmpeq7
  %ifcond = icmp ne i1 %or8, false
  br i1 %ifcond, label %ifthen, label %ifelse

ifthen:                                           ; preds = %loop
  %x9 = load i64, i64* %x, align 4
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([34 x i8], [34 x i8]* @printf_format, i32 0, i32 0), i64 %x9, i64 %x9)
  br label %ifjoin14

ifelse:                                           ; preds = %loop
  %x10 = load i64, i64* %x, align 4
  %icmpne = icmp ne i64 %x10, 3
  %ifcond11 = icmp ne i1 %icmpne, false
  br i1 %ifcond11, label %ifthen12, label %ifjoin

ifthen12:                                         ; preds = %ifelse
  %x13 = load i64, i64* %x, align 4
  %add = add i64 %x13, 100
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([34 x i8], [34 x i8]* @printf_format, i32 0, i32 0), i64 %add, i64 %add)
  br label %ifjoin

ifjoin:                                           ; preds = %ifthen12, %ifelse
  br label %ifjoin14

ifjoin14:                                         ; preds = %ifjoin, %ifthen
  %x_stop_val15 = load i64, i64* %x_stop_val, align 4
  %add16 = add i64 %x_stop_val15, 1
  store i64 %add16, i64* %x_stop_val, align 4
  %stepadd = add i64 %x2, 1
  store i64 %stepadd, i64* %x, align 4
  %forcond = icmp ult i64 %x2, 6
  br i1 %forcond, label %loop, label %afterloop

afterloop:                                        ; preds = %ifjoin14
  %x_stop_val17 = load i64, i64* %x_stop_val, align 4
  ret i64 %x_stop_val17
}

define i64 @params_abc_ret_a(double %a, i64 %b, i64 %c) {
entry:
  %a1 = alloca double, align 8
  store double %a, double* %a1, align 8
  %b2 = alloca i64, align 8
  store i64 %b, i64* %b2, align 4
  %c3 = alloca i64, align 8
  store i64 %c, i64* %c3, align 4
  %b4 = load i64, i64* %b2, align 4
  %mul = mul i64 %b4, 2
  %c5 = load i64, i64* %c3, align 4
  %mul6 = mul i64 %mul, %c5
  store i64 %mul6, i64* %c3, align 4
  %c7 = load i64, i64* %c3, align 4
  ret i64 %c7
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare double @llvm.pow.f64(double, double) #0

attributes #0 = { nofree nosync nounwind readnone speculatable willreturn }
