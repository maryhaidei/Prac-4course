#include <vector>
#include <iostream>

#include "functions.cpp"



int
main()
{
    // пример работы с библиотекой, схематично:
    std::vector<TFunction_ptr> cont;
    auto factory = Factory();
    auto f = factory.create("power", 2); // PowerFunc x^2
    cont.push_back(f);
    auto g = factory.create("polynomial", {7, 0, 3, 15}); // TPolynomial 7 + 3*x^2 + 15*x^3
    cont.push_back(g);
    for (const auto ptr : cont) {
        std::cout << ptr->ToString() << " for x = 10 is " << (*ptr)(10) << std::endl;
    }
    auto p = f + g;
    p->derivate(1); // 53
    std::cout << p->ToString() << " derivate for x = 1 is " << p->derivate(1) << std::endl;
    f->derivate(3); // 6
    std::cout << f->ToString() << " derivate for x = 3 is " << f->derivate(3) << std::endl;

    auto y = factory.create("polynomial", {1, -2, 1});
    std::cout << "Root for " << y->ToString() << " is in x = " << find_equation_root(y) << std::endl;
    return 0;
}