#include <iostream>
#include <tuple>
#include <vector>
#include <list>
#include <array> // used by CS509 related question
#include <string>


// ---------------- WRITE YOUR CODE HERE (BEGIN)
using namespace std;

/*template<typename T>
struct TypeDisplayer;*/

// Q1
template<typename Function, typename... Rest>
auto bind(Function&& func, const Rest& ... rest) {
    if constexpr (is_invocable_v<Function, Rest...>){
        return func(rest...);
    }
    else {
        return [=](auto... args) {
            return bind(func, rest..., args...);
        };
    }
}

// Q2
struct Animal {
    using function = void (*)(const void*);
    const void* animalPtr;
    function speakFunction;

    template <typename T>
    Animal(const T&& animal) : animalPtr(&animal){
        speakFunction = [](const void* self) {
            static_cast<const T*>(self)->speak();
        };
    }
    void speak() const {
        speakFunction(animalPtr);
    }
};

// Q3
template<typename T>      struct is_tuple                 { static const bool value = false; };
template<typename ... Ts> struct is_tuple<tuple<Ts...>>   { static const bool value = true; };

template<typename T>
const bool is_tuple_v = is_tuple<T>::value;

template <typename Tuple, typename Function, size_t index=0>
void for_each_tuple_item(const Tuple& tuple, Function&& func) requires is_tuple_v<Tuple>
{
    if constexpr (index < std::tuple_size_v<std::remove_reference_t<Tuple>>) {      // compile time tuple_size_v is used
        func(std::get<index>(tuple));
        for_each_tuple_item<Tuple, Function, index + 1>(tuple, std::forward<Function>(func));
    }
}

// Q4
template <typename Tuple, typename Function, int index = 0>
auto filter_tuple(const Tuple& tuple, Function func) requires is_tuple_v<Tuple>
{
    constexpr auto size = std::tuple_size_v<Tuple>;
    if constexpr(index == size)
        return std::tuple<>{};
    else {
        const auto item = std::get<index>(tuple);
        if constexpr(func(item)) {
            return std::tuple_cat(std::tuple(item), filter_tuple<Tuple, Function, 1+index>(tuple, func));
        } else {
            return filter_tuple<Tuple, Function, 1+index>(tuple, func);
        }
    }
}
// Q5
template<typename T>
concept IsContainer = requires(T t) {
    t.begin();
    t.end();
    t.size();
    t.begin()++;
};

template <typename Container, typename Func>
auto filter(const Container& container, Func&& func)
{
    if constexpr (IsContainer<Container>) {
        using ContainerType = std::remove_cvref_t<decltype(container)>;
        ContainerType newContainer;
        for (const auto& item : container) {
            if (func(item))
                newContainer.push_back(item);
        }
        return newContainer;
    } else {
        return filter_tuple(container, func);
    }
}
//Q6
template <typename Tuple, size_t Index = 0>
void printTuple(const Tuple& tuple)
{
    if constexpr (std::tuple_size_v<Tuple> > Index) {
        const auto& item = std::get<Index>(tuple);
        std::cout << item << " ";
        printTuple<Tuple, Index + 1>(tuple);
    }
}

template <typename First, typename Second, typename ... Rest>
auto print(const First& str, const Second& container, const Rest& ... rest){

    if constexpr (sizeof...(rest) > 0) {
        std::cout << "\"" << str << "\" = { ";
        if constexpr (IsContainer<Second>){
            for (auto &item: container) {
                cout << item <<" ";
            }
        }else if constexpr (is_tuple_v<Second>){
            printTuple(container);
        }else{
            cout << container << " ";
        }
        std::cout << "}" << std::endl;
        print(rest...);
    }else{                                   // it has 2 argument like string and print argument.
        std::cout << "\"" << str << "\" = { ";
        if constexpr (IsContainer<Second>){
            for (auto &item: container) {
                cout << item <<" ";
            }
        }else if constexpr (is_tuple_v<Second>){
            printTuple(container);
        }else{
            cout << container << " " ;
        }
        std::cout << "}" << std::endl;
    }
}
// ---------------- WRITE YOUR CODE HERE (END)

int main() {
    {
        // Q1) 20 pts
        // Write a bind function that starts binding to the parameters of an already available callable
        // Completing all the necessary parameters makes bind function call the original function with all params
        // and return the result.
        // Any incomplete bind function returns a lambda that waits for extra parameters to be added by calling it
        // This goes on until all the parameters are completed, then it calls the original function and returns the result if any
        auto add = [](int x, int y, int z) { return x + y + z; };
        const auto val1 = bind(add, 1, 2, 3); // 1+2+3=6
        const auto val2 = bind(add, 1, 2)(3); // 1+2+3=6
        const auto val3 = bind(add, 1)(2)(3); // 1+2+3=6
        const auto val4 = bind(add)(1)(2)(3); // 1+2+3=6
        cout<< val1 <<"," << val2 << "," << val3 << "," << val4 << endl;
    }
    {
        // Q2) 20 pts
        // We know that there is a "virtual function" support in core C++ language.
        // We also know that there is type-erasure support by means of std::function in C++ standard library.
        // Without using virtual or std::function; write an Animal struct that makes the below code work properly.
        struct Cow   { void speak() const { cout << "Moo" << endl; } };
        struct Sheep { void speak() const { cout << "Mee" << endl; } };

        auto do_animal_things = [](Animal animal) { animal.speak(); };
        do_animal_things(Animal{Cow{}});   // writes Moo
        do_animal_things(Cow{});           // writes Moo
        do_animal_things(Animal{Sheep{}}); // writes Mee
        do_animal_things(Sheep{});         // writes Mee
    }

    {
        // Q3) 10 pts
        // A std::tuple is a heterogenous container that can store arbitrary number of types and their corresponding values.
        // auto tpl = make_tuple(1, string("hi"), 3.14) is a 3 dimensional compile-time heterogenous vector
        // it's 1st dimension's type is int, 2nd string, 3rd double
        // they have respective values 1, "hi", 3.14.
        // Write for_each_tuple_item function that traverses the elements/dimensions of a given tuple and passes it to
        // a supplied function "func".
        // This function must be protected by a "requires" clause so that it will be only available for tuple params
        // "concept" core language keyword usage is FORBIDDEN.
        // You must use inline requires expressions and clauses.
    }
        // Q4) 20 pts
        // Write "filter_tuple" algorithm that takes a std::tuple and a function that returns true/false on a dimension of tuple.
        // This algorithm must filter the tuple and return a new tuple with filtered out dimensions and their respective values
        // This function must be protected by a "requires" clause so that it will be only available for tuple params
        // "concept" core language keyword usage is FORBIDDEN.
        // You must use inline requires expressions and clauses.
        // You can write your own is_tuple/is_tuple_v structs if necessary

        // Q5) 20 pts
        // Write a generic filter function that can filter tuples, and containers
        // CS409: (12 pts) tuple, (5 pts) vector, (3 pts) list supports are sufficient
        // CS509: (10 pts) tuple, (1 pts) vector, (1 pts) list, (8 pts) array supports are sufficient

        // Q6) 10 pts
        // Write a generic print function that can print containers, tuples, or any other primitives that are sendable to std::cout
        // Your generic print function must also support variadic number of arguments as illustrated in the below code.

        // DO NOT EVER CHANGE THE CODE IN MAIN
        // THERE IS ONE EXCEPTION FOR CS509 students where they must comment out the respective question below
    {
        auto tpl = make_tuple(
                [](auto a, auto b) { return a + b; },
                10,
                25,
                42.42,
                "Hello",
                string("there")
        );
        auto tpl_type_filtered = filter_tuple(tpl, []<typename T>(const T&) { return is_integral_v<T>; } );
        for_each_tuple_item(tpl, []<typename T>(const T& item) {
            if constexpr(is_integral_v<T>)
                cout << item << endl;
        });

        auto v = vector{10, 25, 101, 1001};
        auto v_filtered = filter(v, [](int val) { return val > 100; } );
        auto l = list{10.10, 25.25, 101.101, 1001.1001};
        auto l_filtered = filter(l, [](int val) { return val > 100.0; } );

        // CS509 students can uncomment the below code
        // auto a = array<int, 4>{10, 25, 101, 1001};
        // auto a_filtered = filter(a, [](int val) { return val > 100; } );
        // print("a_filtered", a_filtered); // will print "a_filtered" = { 101 1001 }

        print(
                "v", v,
                "v_filtered", v_filtered,
                "l", l,
                "l_filtered", l_filtered,
                "tpl_type_filtered", tpl_type_filtered
        );
    }
}

// CONSOLE OUTPUT WILL BE AS BELOW FOR CS409 students.
// For CS509 students there will be one extra line "a_filtered" = { 101 1001 } in respective location. Look at above code.

// 6,6,6,6
// Moo
// Moo
// Mee
// Mee
// 10
// 25
// "v" = { 10 25 101 1001 }
// "v_filtered" = { 101 1001 }
// "l" = { 10.1 25.25 101.101 1001.1 }
// "l_filtered" = { 101.101 1001.1 }
// "tpl_type_filtered" = 10 25