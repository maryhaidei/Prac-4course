#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "functions.cpp"

using ::testing::HasSubstr;

//Тесты фабрики 

TEST(FactoryTest, CreatesBasicFunctionsByTypeAndParam) {
    Factory factory;

    auto ident = factory.create("ident");
    auto c     = factory.create("const", 5.0);
    auto p     = factory.create("power", 2.0);
    auto e     = factory.create("exp");

    EXPECT_DOUBLE_EQ(ident->evaluate(10.0), 10.0);   // f(x) = x
    EXPECT_DOUBLE_EQ(c->evaluate(123.0), 5.0);       // f(x) = 5
    EXPECT_DOUBLE_EQ(p->evaluate(3.0), 9.0);         // f(x) = x^2
    EXPECT_NEAR(e->evaluate(0.0), 1.0, 1e-12);       // e^0 = 1
}

TEST(FactoryTest, CreatesPolynomial) {
    Factory factory;

    std::vector<double> coefs{7, 0, 3, 15}; // 7 + 0x + 3x^2 + 15x^3
    auto poly = factory.create("polynomial", coefs);

    // f(1) = 7 + 0 + 3 + 15 = 25
    EXPECT_DOUBLE_EQ(poly->evaluate(1.0), 25.0);
}

TEST(FactoryTest, UnknownTypeThrowsLogicError) {
    Factory factory;

    EXPECT_THROW(factory.create("unknown", 1.0), std::logic_error);
    EXPECT_THROW(factory.create("unknown", std::vector<double>{1.0, 2.0}),
                 std::logic_error);
}

TEST(FactoryTest, WrongOverloadThrowsLogicError) {
    Factory factory;

    EXPECT_THROW(factory.create("polynomial", 1.0), std::logic_error);
    EXPECT_THROW(factory.create("ident", std::vector<double>{1.0, 2.0}),
                 std::logic_error);
}

//базовые функции

TEST(BasicFunctionsTest, IdentityEvaluateAndDerivative) {
    Identity f;

    EXPECT_DOUBLE_EQ(f.evaluate(5.0), 5.0);
    EXPECT_DOUBLE_EQ(f.derivate(0.0), 1.0);
    EXPECT_DOUBLE_EQ(f.derivate(10.0), 1.0);
}

TEST(BasicFunctionsTest, ConstEvaluateAndDerivative) {
    Const c(3.14);

    EXPECT_DOUBLE_EQ(c.evaluate(-100.0), 3.14);
    EXPECT_DOUBLE_EQ(c.derivate(0.0), 0.0);
    EXPECT_DOUBLE_EQ(c.derivate(10.0), 0.0);
}

TEST(BasicFunctionsTest, PowerEvaluateAndDerivative) {
    Power p2(2.0); 

    EXPECT_DOUBLE_EQ(p2.evaluate(3.0), 9.0); 
    EXPECT_DOUBLE_EQ(p2.derivate(3.0), 6.0); 

    Power pMinusOne(-1.0);
    EXPECT_THROW(pMinusOne.evaluate(0.0), std::invalid_argument);
}

TEST(BasicFunctionsTest, ExponentEvaluateAndDerivative) {
    Exponent e;

    EXPECT_NEAR(e.evaluate(0.0), 1.0, 1e-12);
    EXPECT_NEAR(e.evaluate(1.0), std::exp(1.0), 1e-12);

    EXPECT_NEAR(e.derivate(0.0), 1.0, 1e-12);
    EXPECT_NEAR(e.derivate(1.0), std::exp(1.0), 1e-12);
}

TEST(BasicFunctionsTest, PolynomialEvaluateAndDerivative) {
    // 7 + 0x + 3x^2 + 15x^3
    Polynomial poly({7, 0, 3, 15});

    // f(1) = 7 + 0 + 3 + 15 = 25
    EXPECT_DOUBLE_EQ(poly.evaluate(1.0), 25.0);

    // f'(x) = 6x + 45x^2, f'(1) = 6 + 45 = 51
    EXPECT_DOUBLE_EQ(poly.derivate(1.0), 51.0);
}

//Тесты ToString

TEST(ToStringTest, IdentityToString) {
    Identity f;
    EXPECT_EQ(f.ToString(), "x");
}

TEST(ToStringTest, ConstToString) {
    Const c(5.0);
    auto s = c.ToString();
    EXPECT_THAT(s, HasSubstr("5"));
}

TEST(ToStringTest, PowerToString) {
    Power p(2.0);
    EXPECT_EQ(p.ToString(), "x^2.000000");
}

TEST(ToStringTest, ExponentToString) {
    Exponent e;
    EXPECT_EQ(e.ToString(), "e^x");
}

TEST(ToStringTest, PolynomialToStringContainsExpectedPowers) {
    Polynomial poly({7, 0, 3, 15}); // 7 + 0x + 3x^2 + 15x^3
    auto s = poly.ToString();
    EXPECT_THAT(s, HasSubstr("x^3"));
    EXPECT_THAT(s, HasSubstr("x^2"));
    EXPECT_THAT(s, HasSubstr("7"));
}

//Тесты арифметических операций

TEST(ArithmeticTest, AdditionEvaluateAndDerivative) {
    Factory factory;
    auto f = factory.create("power", 2.0);               
    auto g = factory.create("const", 3.0);               
    auto h = f + g;                                      

    EXPECT_DOUBLE_EQ(h->evaluate(2.0), 7.0);

    EXPECT_DOUBLE_EQ(h->derivate(2.0), 4.0);
}

TEST(ArithmeticTest, SubtractionEvaluateAndDerivative) {
    Factory factory;
    auto f = factory.create("power", 2.0); 
    auto g = factory.create("const", 5.0);  
    auto h = f - g;                         // x^2 - 5

    EXPECT_DOUBLE_EQ(h->evaluate(3.0), 9.0 - 5.0); 
    EXPECT_DOUBLE_EQ(h->derivate(3.0), 6.0);      
}

TEST(ArithmeticTest, MultiplicationEvaluateAndDerivative) {
    Factory factory;
    auto f = factory.create("power", 2.0);  // x^2
    auto g = factory.create("const", 3.0);  // 3
    auto h = f * g;                         // 3x^2

    // h(2) = 3 * 4 = 12
    EXPECT_DOUBLE_EQ(h->evaluate(2.0), 12.0);

    // h'(x) = 6x, h'(2) = 12
    EXPECT_DOUBLE_EQ(h->derivate(2.0), 12.0);
}

TEST(ArithmeticTest, DivisionEvaluateAndDerivative) {
    Factory factory;
    auto f = factory.create("power", 2.0);  // x^2
    auto g = factory.create("const", 2.0);  // 2
    auto h = f / g;                         // x^2 / 2

    // h(2) = 4 / 2 = 2
    EXPECT_DOUBLE_EQ(h->evaluate(2.0), 2.0);

    // h'(x) = x, h'(2) = 2
    EXPECT_DOUBLE_EQ(h->derivate(2.0), 2.0);
}

TEST(ArithmeticTest, DivisionByZeroThrowsInvalidArgument) {
    Factory factory;
    auto f = factory.create("const", 1.0);
    auto g = factory.create("const", 0.0);
    auto h = f / g;

    EXPECT_THROW(h->evaluate(1.0), std::invalid_argument);
    EXPECT_THROW(h->derivate(1.0), std::invalid_argument);
}

TEST(CompositionTest, ComplexExpressionEvaluateAndDerivative) {
    Factory factory;
    auto f = factory.create("power", 2.0);               
    auto g = factory.create("polynomial", std::vector<double>{7, 0, 3, 15}); // 7 + 3x^2 + 15x^3
    auto expr = f + g;                                   // x^2 + (7 + 3x^2 + 15x^3)

    // expr(1) = 1 + 25 = 26
    EXPECT_DOUBLE_EQ(expr->evaluate(1.0), 26.0);

    // f'(1) = 2; g'(1) = 51; сумма = 53
    EXPECT_DOUBLE_EQ(expr->derivate(1.0), 53.0);
}


TEST(RootFindingTest, FindsRootForSimpleLinearEquation) {
    Factory factory;
    // f(x) = x - 5
    auto x  = factory.create("ident");
    auto c5 = factory.create("const", 5.0);
    auto eq = x - c5;

    double root = find_equation_root(eq, 0.0, 20000, 1e-3);


    EXPECT_NEAR(root, 5.0, 1e-2);
}

TEST(RootFindingTest, FindsRootNearExpectedForQuadratic) {
    Factory factory;
        
    auto x2 = factory.create("power", 2.0);        
    auto c4 = factory.create("const", 4.0);        
    auto eq = x2 - c4;                            

    double root = find_equation_root(eq, 3.0, 50000, 1e-3);

    EXPECT_NEAR(root, 2.0, 1e-1); 
}


int main(int argc, char** argv) {
    //::testing::InitGoogleTest(&argc, argv);
    //return RUN_ALL_TESTS();
    Factory factory; 
    auto p1 = factory.create("polynomial", {0, -1, 1}); 
    auto p2 = factory.create("const", -6.0) ; 
    auto sum = p1 +p2 ; 
    std::cout<<sum->ToString()<<std::endl; 
    std::cout<<sum->evaluate(1.0)<<std::endl;
    std::cout<<(*sum)(6)<<std::endl;
    std::cout<<sum->derivate(10.0)<<std::endl;
    std::cout<<find_equation_root(sum, 10.0, 100000, 1e-2)<<std::endl; 

}
