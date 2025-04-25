#!/usr/bin/env python3

N = 50

for x in range(N, 0, -1):
    strange = [str(t) for t in range(1, x + 1)]
    print("// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)")
    print("#define STRUCT_VISITABLE{}(S, F{}) \\".format(x, ", F".join(strange)))
    print("    STRUCT_VISITABLE_START(S) \\")
    print("    STRUCT_VISITABLE_FIELDNAMES(#F{}) \\".format(", #F".join(strange)))
    print("    STRUCT_VISITABLE_FIELDS(s.F{}) \\".format(", s.F".join(strange)))
    print("    STRUCT_VISITABLE_END(S)")
    print("")

strange = [str(t) for t in range(1, N + 1)]
print("// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)")
print("#define GET_NTH_ARG(_{}, N, ...) N".format(", _".join(strange)))
print("// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)")
print("#define COUNT_VARARGS(...) GET_NTH_ARG(__VA_ARGS__, {}, 0)".format(", ".join(strange[::-1])))
print("// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)")
print("#define DDAD_CONCATENATE2(A, B) A##B")
print("// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)")
print("#define DDAD_CONCATENATE(A, B) DDAD_CONCATENATE2(A, B)")
print("")
print("// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)")
print("#define STRUCT_VISITABLE(S, ...) DDAD_CONCATENATE(STRUCT_VISITABLE, COUNT_VARARGS(__VA_ARGS__))(S, __VA_ARGS__)")
