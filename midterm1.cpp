// Ozyegin University 2022-23 Spring Term CS409/509 Takehome Exam 1
// This exam is exactly the same for CS409 and CS509 students
// This exam is the same, but, Takehome 2 will have different questions tailored for
// undergrad and grad students.

// !!!!!!!!!! FILL-IN BELOW TWO LINES !!!!!!!!!!!!!!
// STUDENT NUMBER: S021308
// STUDENT NAME: Ezgi Nur Alışan

// All the takehome exam 1 is in this file. There is no extra file for the exam.
// !!!!!!!!!! READ THIS FILE FULLY AND VERY CAREFULLY BEFORE STARTING YOUR SOLUTIONS. !!!!!!!!!!

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// THIS IS AN OPEN EXAM. YOU CAN USE GPT-4, ALL AVAILABLE LITERATURE, ETC.
// BUT YOU NEED TO SOLVE THE EXAM PERSONALLY YOURSELF.
// YOU CANNOT GET HELP FROM REAL PERSONS.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// YOU MIGHT BE SELECTED FOR AN ORAL EXAM AND BE WANTED TO MANIPULATE YOUR OWN SUBMITTED CODE.
// THIS IS PART OF THE SYLLABUS.
// ANY MAJOR FAILURE IN THE ORAL EXAM WILL CAUSE A DISCIPLINARY ACTION.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// In this exam, you will code a Regular Expression Matcher
// A regular expression (regex) is a sequence of characters that define a
// search pattern used to match and manipulate text.

// You are already provided a sample codebase below that supports basic RegexMatcher that supports:
// - single fixed character matching
// - single any character matching
//     (DOT) . character means match any character
// - character classes support:
//     [aDf] -> means match either a, D or f
//     [c-y] -> means match any character between c to y alpabetically: c, d, e, ..., x, y
//     [c-yA-C] -> means match any character between c to y or A to C: c, ..., x, y, A, B, C
//     [12c-e] -> 1, 2, c, d, e

// Now have a look at the code first, then start solving all the questions provided at the end.

#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <stack>
using namespace std;

struct RxResult{
    bool matched;
    string input;
    vector<pair<int,int>> regions;

    RxResult(bool matched,vector<pair<int,int>> regions, string input) : matched{matched}, regions{regions}, input{input}{ }
    RxResult() { };

    RxResult(const RxResult& other): matched(other.matched), regions{other.regions}, input{other.input}{ }
    RxResult(RxResult&& other): matched(other.matched),regions{other.regions}, input{other.input}{ }

    RxResult& operator=(const RxResult& other){
        matched=other.matched;
        regions =other.regions;
        input =other.input;
        return (*this);
    }
    RxResult& operator=(RxResult&& other) {
        matched = other.matched;
        regions =other.regions;
        input =other.input;
        return (*this);
    }

    auto sortResult(const auto& regions) const{
        auto sortedRegions = regions;
        auto size = regions.size();
        for (auto& item : sortedRegions){
            for(auto item1 = 1  ; item1 < size ; item1++){
                if((sortedRegions[item1].first<= item.first || sortedRegions[item1].second>=item.second) ){
                    auto t = move(sortedRegions[item1]);
                    sortedRegions[item1] = move(item);
                    item = move(t);
                }
            }
        }
        return sortedRegions;
    }
    void printResult() const {
        cout << "Input: \"" << input << "\""<< endl;
        cout << "Matched: " << matched ;
        auto sortedRegions = sortResult(regions);
        for (const auto& region : sortedRegions) {
            cout << " (" << region.first << "," << region.second << ")=";
            cout << input.substr(region.first, region.second - region.first) << " ";
        }
        cout << endl;
    }
};
template<typename T>
struct RxConfig{
    constexpr static const T DOT = (T) 10000 ;
    constexpr static const T CCbegin = (T) 10001;
    constexpr static const T CCend = (T) 10002;
    constexpr static const T DASH = (T) 10003;
};

template<typename ...>
struct TypeDisplayer;

template< typename T > struct remove_const_ref                    { using type = T; };
template< typename T > struct remove_const_ref<const T&>          { using type =  T; };

template<typename T>
struct RxMatcher{
    T pattern = 0;
    explicit RxMatcher(const string& pattern) : pattern{pattern} { }

    template<template<typename> typename Container,typename K>
    explicit RxMatcher(const Container<K>& pattern) : pattern{pattern} { }

    template<typename K>
    auto parseCharacterClass(std::size_t start) const -> std::pair<set<K>, std::size_t> {
        set<K> general_class;
        std::size_t i = start;

        while (i < pattern.size() && (is_same_v<K,char> && pattern[i] != ']') || (!is_same_v<K,char> && pattern[i] != RxConfig<K>::CCend)) {
            K c1 = pattern[i++];
            if (i < pattern.size() && (is_same_v<K,char> &&pattern[i] == '-') || (!is_same_v<K,char> &&pattern[i] == RxConfig<K>::DASH) && i + 1 < pattern.size()) {
                K c2 = pattern[i + 1];
                i += 2;
                for (K c = c1; c <= c2; ++c) {
                    general_class.insert(c);
                }
            } else {
                general_class.insert(c1);
            }
        }
        return {general_class, i + 1};
    }

    RxResult findIn(const T& input) const {
        auto st = stack<pair<char,int>>{};
        auto rx = RxResult{};
        return findIn(input, 0, 0, st,rx);
    }

    RxResult findIn(const T& input, size_t i, size_t j, stack<pair<char, int>>& st, RxResult& result) const {
        result.matched = false;
        using type = typename remove_const_ref<decltype(input[0])>::type;
        auto vec = vector<pair<int,int>>{};
        if constexpr(is_same_v<type,char>){
            result.input=input;
        }
        if (j == pattern.size()) // if we're at the end of pattern's characters
            result.matched= (i == input.size()); // true if input is also finished else false

        if(is_same_v<type,char> && pattern[j] == '(') {
            auto p = pair<char,int>{pattern[j],i};
            st.push(p);
            return forward<RxResult>(findIn(input,i,j+1,st,result));
        }

        if(is_same_v<type,char> && pattern[j]==')'){
            auto x = st.top();
            st.pop();
            auto p = pair<int,int>{x.second,i};
            result.regions.push_back(p);
            return forward<RxResult>(findIn(input, i, j+1,st,result));
        }
        if ( (is_same_v<type,char> && pattern[j] == '[') || (!is_same_v<type,char> && pattern[j] == RxConfig<type>::CCbegin) ) {
            if((pattern[j+2] == '[' || pattern[j+2] == RxConfig<type>::DASH) && !is_same_v<type,char>){
                type initial = pattern[j+1];
                type last = pattern[j+3];
                if(pattern[j+1]<= input[i] && pattern[j+3]>= input[i] ){
                    result.matched = true;
                    return forward<RxResult>(findIn(input, i + 1, j+5, st, result));
                }
            }
            auto[general_class, next_j] = parseCharacterClass<type>(j + 1);
            if (i < input.size() && general_class.count((input[i])) > 0) {
                result.matched = true;
                return forward<RxResult>(findIn(input, i + 1, next_j, st, result));
            }
        } else
            // still have input and
            // either the pattern is a dot that matches to everything or the character exactly matches
            // then divide the problem into a smaller problem because this part fits to the rules
            // solve the rest of the problem similarly
        if ((is_same_v<type,char> && (i < input.size() && (pattern[j] == '.' || input[i] == pattern[j]))) || (!is_same_v<type,char> && i < input.size() && (pattern[j]==RxConfig<type>::DOT || input[i] == pattern[j]))){
            result.matched=true;
            return forward<RxResult>(findIn(input, i + 1, j + 1,st,result));
        }
        if (is_same_v<type,char> && i < input.size() && input[i] != pattern[j] ) {
            if(!st.empty()){
                auto p = st.top();
                st.pop();
                auto nPair = pair<char,int>{p.first,i+1};
                st.push(nPair);
            }
            return forward<RxResult>(findIn(input, i + 1, j,st,result));
        }
        if (result.matched==false)
            result.regions=vector<pair<int,int>>{};
        return forward<RxResult>(result);
    }
};

// generic print that only works with RxResult struct
template<typename First, typename ...Rest>
void print(const First& first, const Rest& ... rest){
    static_assert((std::is_same_v<First, RxResult>), "Argument type should be RxResult");
    static_assert((std::is_same_v<First, Rest> && ...), "All arguments must be of the same type");

    if constexpr(std::is_same<First,RxResult>::value){
        first.printResult();
        (rest.printResult(), ...);
    }
}
template<template<typename> typename Container,typename K>
RxMatcher(Container<K>) -> RxMatcher<Container<K>>;
RxMatcher(const string&) ->RxMatcher<string>;

int main() {
    cout << boolalpha;
    {
        cout << "STRING matcher" << endl;
        auto matcher = RxMatcher("h((.[a-z])(lo))");
        print(
                matcher.findIn("hello "),
                matcher.findIn("hallo"),
                matcher.findIn("_hEblo!"),
                matcher.findIn("heako")
        );
        {
            cout << "STRING matcher" << endl;
            auto matcher2 = RxMatcher("he[lL][lL]o");
            print(matcher2.findIn("!_helLo_!"));
            auto matcher = RxMatcher("(h((.[a-z])(lo)))");
            print(
                    matcher.findIn("!_hello_!"),//true
                    matcher.findIn("hello "), //true
                    matcher.findIn("hallo"), //true
                    matcher.findIn("_hEblo!"),  //true
                    matcher.findIn("AhalloA"), //true
                    matcher.findIn("heako") //false
            );
        }
        {
            auto matcher = RxMatcher("h.[a-zA-C]lo");
            print (matcher.findIn("hello"), // true
                   matcher.findIn("hAllo"),  // true
                   matcher.findIn("hillo"),// true
                   matcher.findIn("heALo"), //false
                   matcher.findIn("!_hello_!")); // true
        }
    }
    cout << endl;

    {
        cout << "INT matcher" << endl;
        using CFG = RxConfig<int>;
        auto matcher1 = RxMatcher(vector{1, CFG::DOT, CFG::CCbegin, 10, 20, CFG::CCend, 4});
        cout << matcher1.findIn(vector{1, 2, 20, 4}).matched << endl;
        cout << matcher1.findIn(vector{1, 2, 15, 4}).matched << endl;

        auto matcher2 = RxMatcher(vector{1, CFG::DOT, CFG::CCbegin, 10, CFG::DASH, 20, CFG::CCend, 4});
        cout << matcher2.findIn(vector{1, 2, 15, 4}).matched << endl;
    }
    cout << endl;

    {
        cout << "DOUBLE matcher" << endl;
        using CFG = RxConfig<double>;
        auto matcher1 = RxMatcher(vector{1.1, CFG::DOT, CFG::CCbegin, 10.5, CFG::DASH, 20.8, CFG::CCend, 4.0});
        cout << matcher1.findIn(vector{1.1, 2.5, 10.6, 4.0}).matched << endl;
    }
    {
        cout << "FLOAT matcher" << endl;
        using CFG = RxConfig<float>;
        auto matcher1 = RxMatcher(vector{1.1f, CFG::DOT, CFG::CCbegin, 10.0f, CFG::DASH, 20.0f, CFG::CCend, CFG::CCbegin, 17.5f, CFG::DASH, 21.4f, CFG::CCend, 4.0f});
        cout << matcher1.findIn(vector{1.1f, 2.5f, 15.5f, 17.51f, 4.0f}).matched << endl;  //true
    }
    return 0;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// In this takehome;
// You can only use the C++ stuff that we covered during our lectures.
// You will be penalized by -10 points per individual usage out of our lecture's content.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Valid content is ANYTHING that can be found in our lecture videos, lecture's GitHub Repo,
// or anything provided in this sample code.
// If you are not sure, you must ask us (both TA and instructor) in the same email
// and get a consent to use the asked core language or standard library feature in your solution.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Consent emails that don't include both the TA and the instructor WILL NOT BE ANSWERED.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// First day of the exam, no questions will be answered until it is 15:00.
// Email responses will be given at worst by 21:00 in the same day.
// You asked at 13:45, you are answered by 21:00 that day.
// You asked at 21:15, you are answered by 21:00 the next day.
// You cannot ask questions about the exam to anybody else.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// You can #include files only from the C/C++ standard libraries.
// You cannot use #include <regex> of course
// You will be uploading a SINGLE file as a solution to the LMS.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// Either by improving the template provided for you or writing everything from scratch;
// Write a regular expression matcher class that can do below:

// Q1) 10 points
// Write a struct that can hold the result of a match operation (i.e. RxResult?).
// The stored information will cover:
// - if a match has been found or not ("matched" attribute maybe?)
// - the regions of the input that aligns with the paranthesis in the search pattern

// Q2) 10 points
// Write a generic print function that only matches with RxResult struct(s) and
// prints its/tehir contents by using fold expressions
// exactly as indicated in the console output at the very bottom of this file.

// Q3) 15 points
// Current code exactly matches the pattern and input from its beginining to end fully.
// Any extra characters in the front of or at the end of the matched portion aren't allowed.
// Make the code flexible so that below code will match the "helLo" within "!_helLo_!"
// in spite of the additional character(s) around the searched pattern.
// RxMatcher("he[lL][lL]o").findIn("!_helLo_!");

// Q4) 25 points
// Current code only supports string containers. Improve the code so that it supports any container.
// Ensure that it works by enabling the commented out code in the main() function for
// containers storing INT and DOUBLE. You need to allow customization of your generic RegexMatcher class
// so that its pattern characters are customizable for different item typenames of a container.
// (look at the sample code in commented out main() function

// Q5) 20 points
// Add support for paranthesis characters that indicates certain regions of the pattern
// so that matcher returns a feedback on what a specific (...) region matched what.
// Adding support for paranthesis is only required for RegexMatch<string>.
// You need to support paranthesis within paranthesis as well
// Look at the commented out main() function and its console output provided for you
// so that you can understand how it will work

// Q6) 20 points
// Improve character class support in such a way that
//   auto matcher = RxMatcher(vector{1.0, CFG::DOT, CFG::CCbegin, 10.0, CFG::DASH, 20.0, CFG::CCend, 4.0});
//   matcher.findIn(vector{1.0, 2.0, 15.5, 4.0}).matched; // --> returns true
// lines return true. This means that character-class 10.0 DASH 20.0 also supports 15.5 as valid.

// this is the main() function that needs to work correctly after you completed the takehome fully



// Correct OUTPUT that will be exactly written to the console is below:

// STRING matcher
// Input: "hello "
// Matched: true  (1,5)=ello (1,3)=el (3,5)=lo
// Input: "hallo"
// Matched: true  (1,5)=allo (1,3)=al (3,5)=lo
// Input: "_hEblo!"
// Matched: true  (2,6)=Eblo (2,4)=Eb (4,6)=lo
// Input: "heako"
// Matched: false
//
// INT matcher
// true
// false
// true
//
// DOUBLE matcher
// true