#include <iostream>

#include "Application.h"
#include "util/Search.h"

// struct A {
//     double state;
//     double cost;
// };

// template<>
// struct std::hash<A>
// {
//     std::size_t operator()(const A &a) const {
//         return std::hash<double>{}(a.state) ^ (std::hash<double>{}(a.cost) << 1);
//     }
// };

int main(int argc, char **argv)
{
    // auto successors = [](const A &s) -> std::vector<A> {
    //     return {A{s.state + 2, s.cost + 0.1}, A{s.state + 1, s.cost + 1}};
    // };

    // auto is_goal = [](const A &s) {
    //     return s.state == 10.0;
    // };

    // auto x = BFS<A>(A{0, 0}, successors, is_goal);

    // auto solution = x.perform();

    // if (solution) {
    //     for (auto &x : *solution)
    //         std::cout << x->state.state << ',';
    // }
    // else {
    //     std::cout << "no solution";
    // }

    Application app;
    app.main();
    return 0;
}
