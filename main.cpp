#include <iostream>


template<typename T, T...>
struct List {};

template<typename T, T>
struct Single {};


template<typename T, T... xs, T... ys>
consteval List<T, xs..., ys...> operator+(List<T, xs...>, List<T, ys...>)
{ return {}; }


template<typename T, unsigned index>
consteval auto at(Single<unsigned, index>, List<T>)
{ return 0; }

template<typename T, T value, T... rest>
consteval auto at(Single<unsigned, 0>, List<T, value, rest...>)
{ return value; }

template<typename T, unsigned index, T value, T... rest>
consteval auto at(Single<unsigned, index>, List<T, value, rest...>)
{ return at(Single<unsigned, index - 1>{}, List<T, rest...>{}); }


template<typename T, T first, T... rest>
consteval auto skip(Single<unsigned, 1>, List<T, first, rest...>)
{ return List<T, rest...>{}; }

template<typename T, unsigned to_skip, T value, T... rest>
consteval auto skip(Single<unsigned, to_skip>, List<T, value, rest...>)
{ return skip(Single<unsigned, to_skip - 1>{}, List<T, rest...>{}); }


template<typename T, T first, T... rest>
consteval auto take(Single<unsigned, 1>, List<T, first, rest...>)
{ return List<T, first>{}; }

template<typename T, unsigned to_take, T first, T... rest>
consteval auto take(Single<unsigned, to_take>, List<T, first, rest...>)
{ return List<T, first>{} + take(Single<unsigned, to_take - 1>{}, List<T, rest...>{}); }


template<unsigned index, char... ops, unsigned... stack>
consteval auto find_matching_open_paren(Single<unsigned, index>, List<char, '[', ops...>, List<unsigned, stack...>)
{ return find_matching_open_paren(Single<unsigned, index + 1>{}, List<char, ops...>{}, List<unsigned, index, stack...>{}); }

template<unsigned index, char... ops, unsigned stack_top, unsigned... stack>
consteval auto find_matching_open_paren(Single<unsigned, index>, List<char, ']', ops...>, List<unsigned, stack_top, stack...>)
{ return find_matching_open_paren(Single<unsigned, index + 1>{}, List<char, ops...>{}, List<unsigned, stack...>{}); }

template<unsigned index, char op, char... ops, unsigned... stack>
consteval auto find_matching_open_paren(Single<unsigned, index>, List<char, op, ops...>, List<unsigned, stack...>)
{ return find_matching_open_paren(Single<unsigned, index + 1>{}, List<char, ops...>{}, List<unsigned, stack...>{}); }

template<unsigned index, unsigned stack_top, unsigned... stack>
consteval auto find_matching_open_paren(Single<unsigned, index>, List<char>, List<unsigned, stack_top, stack...>)
{ return stack_top; }


template<unsigned index, char... ops>
consteval auto find_matching_close_paren(Single<unsigned, index>, List<char, ']', ops...>, Single<unsigned, 1>)
{ return index; }

template<unsigned index, char... ops, unsigned depth>
consteval auto find_matching_close_paren(Single<unsigned, index>, List<char, '[', ops...>, Single<unsigned, depth>)
{ return find_matching_close_paren(Single<unsigned, index + 1>{}, List<char, ops...>{}, Single<unsigned, depth + 1>{}); }

template<unsigned index, char... ops, unsigned depth>
consteval auto find_matching_close_paren(Single<unsigned, index>, List<char, ']', ops...>, Single<unsigned, depth>)
{ return find_matching_close_paren(Single<unsigned, index + 1>{}, List<char, ops...>{}, Single<unsigned, depth - 1>{}); }

template<unsigned index, char op, char... ops, unsigned depth>
consteval auto find_matching_close_paren(Single<unsigned, index>, List<char, op, ops...>, Single<unsigned, depth>)
{ return find_matching_close_paren(Single<unsigned, index + 1>{}, List<char, ops...>{}, Single<unsigned, depth>{}); }


template<char value, char... rest>
consteval auto increment(Single<unsigned, 0>, List<char, value, rest...>)
{ return List<char, value + 1, rest...>{}; }

template<unsigned index>
consteval auto increment(Single<unsigned, index>, List<char>)
{ return increment(Single<unsigned, index>{}, List<char, 0>{}); }

template<unsigned index, char value, char... rest>
consteval auto increment(Single<unsigned, index>, List<char, value, rest...>)
{ return List<char, value>{} + increment(Single<unsigned, index - 1>{}, List<char, rest...>{}); }


template<char value, char... rest>
consteval auto decrement(Single<unsigned, 0>, List<char, value, rest...>)
{ return List<char, value - 1, rest...>{}; }

template<unsigned index>
consteval auto decrement(Single<unsigned, index>, List<char>)
{ return decrement(Single<unsigned, index>{}, List<char, 0>{}); }

template<unsigned index, char value, char... rest>
consteval auto decrement(Single<unsigned, index>, List<char, value, rest...>)
{ return List<char, value>{} + decrement(Single<unsigned, index - 1>{}, List<char, rest...>{}); }


template<char... ops, unsigned op_index, unsigned ptr, char... tape, char... output>
consteval auto run(List<char, ops...>, Single<unsigned, op_index>, Single<unsigned, ptr>, List<char, tape...>, List<char, output...>) {
    return run_op(
        Single<char, at(Single<unsigned, op_index>{}, List<char, ops...>{})>{},
        List<char, ops...>{},
        Single<unsigned, op_index>{},
        Single<unsigned, ptr>{},
        List<char, tape...>{},
        List<char, output...>{}
    );
}

template<char... ops, unsigned op_index, unsigned ptr, char... tape, char... output>
consteval auto run_op(Single<char, '+'>, List<char, ops...>, Single<unsigned, op_index>, Single<unsigned, ptr>, List<char, tape...>, List<char, output...>) {
    return run(
        List<char, ops...>{},
        Single<unsigned, op_index + 1>{},
        Single<unsigned, ptr>{},
        increment(Single<unsigned, ptr>{}, List<char, tape...>{}),
        List<char, output...>{}
    );
}

template<char... ops, unsigned op_index, unsigned ptr, char... tape, char... output>
consteval auto run_op(Single<char, '-'>, List<char, ops...>, Single<unsigned, op_index>, Single<unsigned, ptr>, List<char, tape...>, List<char, output...>) {
    return run(
        List<char, ops...>{},
        Single<unsigned, op_index + 1>{},
        Single<unsigned, ptr>{},
        decrement(Single<unsigned, ptr>{}, List<char, tape...>{}),
        List<char, output...>{}
    );
}

template<char... ops, unsigned op_index, unsigned ptr, char... tape, char... output>
consteval auto run_op(Single<char, '>'>, List<char, ops...>, Single<unsigned, op_index>, Single<unsigned, ptr>, List<char, tape...>, List<char, output...>) {
    return run(
        List<char, ops...>{},
        Single<unsigned, op_index + 1>{},
        Single<unsigned, ptr + 1>{},
        List<char, tape...>{},
        List<char, output...>{}
    );
}

template<char... ops, unsigned op_index, unsigned ptr, char... tape, char... output>
consteval auto run_op(Single<char, '<'>, List<char, ops...>, Single<unsigned, op_index>, Single<unsigned, ptr>, List<char, tape...>, List<char, output...>) {
    return run(
        List<char, ops...>{},
        Single<unsigned, op_index + 1>{},
        Single<unsigned, ptr - 1>{},
        List<char, tape...>{},
        List<char, output...>{}
    );
}

template<char... ops, unsigned op_index, unsigned ptr, char... tape, char... output>
consteval auto conditional_run_op(Single<bool, true>, List<char, ops...>, Single<unsigned, op_index>, Single<unsigned, ptr>, List<char, tape...>, List<char, output...>) {
    return run(
        List<char, ops...>{},
        Single<unsigned, 1 + find_matching_close_paren(Single<unsigned, op_index+1>{}, skip(Single<unsigned, op_index + 1>{}, List<char, ops...>{}), Single<unsigned, 1>{})>{},
        Single<unsigned, ptr>{},
        List<char, tape...>{},
        List<char, output...>{}
    );
}

template<char... ops, unsigned op_index, unsigned ptr, char... tape, char... output>
consteval auto conditional_run_op(Single<bool, false>, List<char, ops...>, Single<unsigned, op_index>, Single<unsigned, ptr>, List<char, tape...>, List<char, output...>) {
    return run(
        List<char, ops...>{},
        Single<unsigned, op_index>{},
        Single<unsigned, ptr>{},
        List<char, tape...>{},
        List<char, output...>{}
    );
}

template<char... ops, unsigned op_index, unsigned ptr, char... tape, char... output>
consteval auto run_op(Single<char, '['>, List<char, ops...>, Single<unsigned, op_index>, Single<unsigned, ptr>, List<char, tape...>, List<char, output...>) {
    return conditional_run_op(
        Single<bool, at(Single<unsigned, ptr>{}, List<char, tape...>{}) == 0>{},
        List<char, ops...>{},
        Single<unsigned, op_index + 1>{},
        Single<unsigned, ptr>{},
        List<char, tape...>{},
        List<char, output...>{}
    );
}


template<char... ops, unsigned op_index, unsigned ptr, char... tape, char... output>
consteval auto run_op(Single<char, ']'>, List<char, ops...>, Single<unsigned, op_index>, Single<unsigned, ptr>, List<char, tape...>, List<char, output...>) {
    return run(
        List<char, ops...>{},
        Single<unsigned, find_matching_open_paren(Single<unsigned, 0>{}, take(Single<unsigned, op_index>{}, List<char, ops...>{}), List<unsigned>{})>{},
        Single<unsigned, ptr>{},
        List<char, tape...>{},
        List<char, output...>{}
    );
}


template<char... ops, unsigned op_index, unsigned ptr, char... tape, char... output>
consteval auto run_op(Single<char, '.'>, List<char, ops...>, Single<unsigned, op_index>, Single<unsigned, ptr>, List<char, tape...>, List<char, output...>) {
    return run(
        List<char, ops...>{},
        Single<unsigned, op_index + 1>{},
        Single<unsigned, ptr>{},
        List<char, tape...>{},
        List<char, output..., at(Single<unsigned, ptr>{}, List<char, tape...>{})>{}
    );
}


template<char... ops, unsigned op_index, unsigned ptr, char... tape, char... output>
consteval auto run_op(Single<char, 0>, List<char, ops...>, Single<unsigned, op_index>, Single<unsigned, ptr>, List<char, tape...>, List<char, output...>) {
    return List<char, output...>{};
}

template<char op, char... ops, unsigned op_index, unsigned ptr, char... tape, char... output>
consteval auto run_op(Single<char, op>, List<char, ops...>, Single<unsigned, op_index>, Single<unsigned, ptr>, List<char, tape...>, List<char, output...>) {
    return run(
        List<char, ops...>{},
        Single<unsigned, op_index + 1>{},
        Single<unsigned, ptr>{},
        List<char, tape...>{},
        List<char, output...>{}
    );
}


template<char... ops>
consteval auto interpret(List<char, ops...>) {
    return run(
        List<char, ops...>{},
        Single<unsigned, 0>{},
        Single<unsigned, 0>{},
        List<char>{},
        List<char>{}
    );
}


template<char... cs>
std::ostream& operator<<(std::ostream& stream, List<char, cs...>) {
    for (char c : {cs...}) {
        stream << c;
    }

    return stream;
}

auto main() -> int {
    auto result = interpret(List<char, '+', '+', '+', '+', '+', '+', '+', '+', '[', '>', '+', '+', '+', '+', '[', '>', '+', '+', '>', '+', '+', '+', '>', '+', '+', '+', '>', '+', '<', '<', '<', '<', '-', ']', '>', '+', '>', '+', '>', '-', '>', '>', '+', '[', '<', ']', '<', '-', ']', '>', '>', '.', '>', '-', '-', '-', '.', '+', '+', '+', '+', '+', '+', '+', '.', '.', '+', '+', '+', '.', '>', '>', '.', '<', '-', '.', '<', '.', '+', '+', '+', '.', '-', '-', '-', '-', '-', '-', '.', '-', '-', '-', '-', '-', '-', '-', '-', '.', '>', '>', '+', '.', '>', '+', '+', '.'>{});
    std::cout << result << std::endl;
}
